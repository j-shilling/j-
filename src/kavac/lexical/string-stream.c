#include "string-stream.h"

#include <kava/memory.h>
#include <kava/posix/unistd.h>
#include <kava/posix/thread.h>
#include <kava/posix/sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistr.h>
#include <uninorm.h>
#include <uniwbrk.h>


typedef struct _string_node {
  char *string;
  size_t len;

  struct _string_node *next;
} string_node;

struct _string {
  char *string;
  size_t len;
};

struct _string_stream {
  kava_thread thread;
  kava_mutex mutex;
  kava_cond cond;
  int open;

  char *path;
  int err;

  string_node *head;
  string_node *tail;
};

_Noreturn static void *
read_file (void *arg)
{
  string_stream *stream = arg;

  /* Lock the stream while we read the file to a
     memory buffer */
  kava_mutex_lock (&stream->mutex);
  kava_stat info;
  if (-1 == kava_read_stat (stream->path, &info))
    {
      stream->err = errno;
      kava_mutex_unlock (&stream->mutex);
      kava_thread_exit (NULL);
    }
  FILE *file = fopen (stream->path, "r");
  if (!file)
    {
      stream->err = errno;
      kava_mutex_unlock (&stream->mutex);
      kava_thread_exit (NULL);
    }
  char *file_buffer = kava_alloc (info.st_size);
  int read_status = kava_read (file, file_buffer, info.st_size);

  /* clean up the file read and exit if the read was
     unsuccessful. */
  if (read_status == -1)
    {
      stream->err = errno;
      fclose (file);
      kava_mutex_unlock (&stream->mutex);
      kava_thread_exit (NULL);
    }
  fclose (file);

  /* Check the encoding of file_buffer and converit it
     to UTF-8 if necessary */
  uint8_t *utf8_buffer;
  size_t utf8_buffer_length;
  if (u8_check ((const uint8_t *)file_buffer, (size_t)info.st_size))
    {
      if (u16_check ((const uint16_t *)file_buffer, (size_t)info.st_size))
	{
	  if (u32_check ((const uint32_t *)file_buffer, (size_t)info.st_size))
	    {
	      /* Unkown encoding */
	      stream->err = EENCODING;
	      kava_free (file_buffer);
	      kava_mutex_unlock (&stream->mutex);
	      kava_thread_exit (NULL);
	    }
	  else
	    {
	      /* utf32 encoding */
	      utf8_buffer = u32_to_u8 ((const uint32_t *)file_buffer, (size_t)info.st_size,
				       NULL, &utf8_buffer_length);
	      kava_free (file_buffer);
	    }
	}
      else
	{
	  /* utf16 encoding */
	  utf8_buffer = u16_to_u8 ((const uint16_t *)file_buffer, (size_t)info.st_size,
				   NULL, &utf8_buffer_length);
	  kava_free (file_buffer);
	}
      /* utf8 encoding */
      utf8_buffer = (uint8_t *) file_buffer;
      utf8_buffer_length = (size_t) info.st_size;
    }

  /* Normalize the UTF-8 buffer */
  size_t buffer_length;
  uint8_t *buffer = u8_normalize (UNINORM_NFD, utf8_buffer, utf8_buffer_length, NULL, &buffer_length);

  /* The stream is not initialized; unlock it */
  kava_mutex_unlock (&stream->mutex);
  kava_free (utf8_buffer);

  /* Break the buffer into words and add them to the stream */
  char *p = kava_alloc (buffer_length);
  u8_wordbreaks (buffer, buffer_length, p);

  int start = 0;
  size_t len = 0;
  for (;;)
    {
      while (((start + len) < buffer_length) && !p[start + len])
	len++;

      if (!len)
	{
	  /* We must be out of stuff to read */
	  kava_mutex_lock (&stream->mutex);
	  stream->open = 0;
	  kava_cond_signal (&stream->cond);
	  kava_mutex_unlock (&stream->mutex);

	  kava_free (buffer);
	  kava_thread_exit (NULL);
	}

      /* create the new string node */
      char *word = kava_alloc (len);
      memcpy (word, buffer + start, len);

      string_node *node = kava_alloc (sizeof (string_node));
      node->string = word;
      node->len = len;
      node->next = NULL;

      /* insert into the stack */
      kava_mutex_lock (&stream->mutex);
      if (!stream->tail)
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

      /* update loop vars */
      start += len;
      len = 0;
    }
}

string_stream *
string_stream_from_path (const char *path)
{
  string_stream *ret = kava_alloc (sizeof (string_stream));
  kava_mutex_init (&ret->mutex);
  kava_cond_init (&ret->cond);
  ret->path = strdup (path);
  ret->head = NULL;
  ret->tail = NULL;

  kava_thread thread;
  kava_mutex_lock (&ret->mutex);
  ret->err = kava_thread_create (&thread, read_file, ret);
  if (ret->err)
    ret->thread = 0;
  else
    ret->thread = thread;
  kava_mutex_unlock (&ret->mutex);

  return ret;
}

string *
string_stream_next (restrict string_stream *const stream)
{
  if (!stream)
    return NULL;

  kava_mutex_lock (&stream->mutex);

  if (stream->err)
    return NULL;

  if (!stream->head && stream->open)
    /* stream is empty, but data is still being read */
    kava_cond_wait (&stream->cond, &stream->mutex);

  string *ret = NULL;

  if (stream->head)
    {
      /* stream is not empty */
      string_node *node = stream->head;
      stream->head = stream->head->next;

      ret = kava_alloc (sizeof (string));
      ret->string = node->string;
      ret->len = node->len;

      kava_free (node);
    }

  kava_mutex_unlock (&stream->mutex);
  return ret; /* might be null */
}

int
string_stream_error (restrict string_stream const *const stream)
{
  if (stream)
    return stream->err;
  else
    return 0;
}
