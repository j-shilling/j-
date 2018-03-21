#include "utf8-stream.h"

#include <kava/posix/unistd.h>
#include <kava/posix/thread.h>
#include <kava/posix/sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

typedef struct _char_node {
  unichar c;
  struct _char_node *next;
} char_node;

struct _utf8_stream {
  kava_thread thread;
  kava_mutex mutex;
  kava_cond cond; 
  FILE *file;
  volatile int err;
  volatile int open;

  char_node *head;
  char_node *tail;
};

_Noreturn static void *
read_file (void *arg)
{
  utf8_stream *stream = arg;
  /* stop until an error is found or until the stream is closed */
  while (stream->open && !stream->err)
    {
      uint8_t byte;
      if (1 != fread (&byte, 1, 1, stream->file))
	{
	  if (feof (stream->file))
	    stream->open = 0;
	  else
	    stream->err = ferror (stream->file);
	  fclose (stream->file);
	  stream->file = NULL;
	  kava_thread_exit (NULL);
	}

      size_t width;
      uint8_t width_mask = byte & 0xF0;
      switch (width_mask)
	{
	case 0xF0: width = 4; break;
	case 0xE0: width = 3; break;
	case 0xC0: width = 2; break;
	default:   width = 1; break;
	}

      unichar c = 0;
      uint8_t *bytes = (uint8_t *)(&c);
      bytes[0] = byte;

      for (int i = 1; i < width; i++)
	{
	  if (1 != fread (&byte, 1, 1, stream->file))
	    {
	      if (feof (stream->file))
		stream->open = 0;
	      else
		stream->err = ferror (stream->file);
	      fclose (stream->file);
	      kava_thread_exit (NULL);
	    }

	  if (byte & 0xC0 != 0x80)
	    {
	      stream->err = EENCODING;
	      fclose (stream->file);
	      stream->file = NULL;
	      kava_thread_exit (NULL);
	    }

	  bytes[i] = byte;
	}

      char_node *node = malloc (sizeof (char_node));
      node->c = c;
      node->next = NULL;

      kava_mutex_lock (&stream->mutex);
      if (!stream->head)
	{
	  stream->head = node;
	  stream->tail = node;
	}
      else
	{
	  stream->tail->next = node;
	  stream->tail = node;
	}
      kava_cond_signal (&stream->cond);
      kava_mutex_unlock (&stream->mutex);
    }

  kava_mutex_lock (&stream->mutex);
  if (stream->file)
    fclose (stream->file);
  kava_cond_signal (&stream->cond);
  kava_mutex_unlock (&stream->mutex);
  kava_thread_exit (NULL);
}

utf8_stream *
utf8_stream_from_path (const char *path, int *err)
{
  utf8_stream *ret = malloc (sizeof (utf8_stream));
  ret->file = fopen (path, "r");
  if (!ret->file)
    {
      *err = errno;
      free (ret);
      return NULL;
    }

  kava_mutex_init (&ret->mutex);
  kava_cond_init (&ret->cond);
  ret->head = NULL;
  ret->tail = NULL;
  ret->open = 1;

  ret->err = kava_thread_create (&ret->thread, read_file, ret);
  if (ret->err)
    {
      *err = ret->err;
      free (ret);
      return NULL;
    }

  return ret;
}

unichar
utf8_stream_next (utf8_stream *const restrict stream, int *const restrict err)
{
  if (!stream)
    {
      *err = EINVAL;
      return 0;
    }

  kava_mutex_lock (&stream->mutex);

  if (stream->err)
    {
      *err = stream->err;
      kava_mutex_unlock (&stream->mutex);
      return 0;
    }

  if (!stream->head && stream->open)
    /* stream is empty, but data is still being read */
    kava_cond_wait (&stream->cond, &stream->mutex);

  if (stream->head)
    {
      /* stream is not empty */
      char_node *node = stream->head;
      stream->head = stream->head->next;
      if (!stream->head)
	stream->tail = NULL;

      unichar c = node->c;
      free (node);

      kava_mutex_unlock (&stream->mutex);
      return c;
    }
  else
    {
      /* stream has been closed */
      kava_mutex_unlock (&stream->mutex);
      *err = EOF;
      return 0;
    }
}

void
utf8_stream_close (utf8_stream *restrict stream, int *const restrict err)
{
  if (!stream)
    return;

  stream->open = 0;

  *err = kava_thread_join (stream->thread, NULL);
  if (*err)
    return;

  char_node *node = stream->head;
  while (node)
    {
      char_node *next = node->next;
      free (node);
      node = next;
    }

  free (stream);
}
