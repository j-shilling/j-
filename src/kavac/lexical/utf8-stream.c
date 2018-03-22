#include "utf8-stream.h"

#include <kava/posix/unistd.h>
#include <kava/posix/thread.h>
#include <kava/posix/sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistr.h>
#include <uninorm.h>
#include <unictype.h>

_Noreturn static void *
read_file (void *arg)
{
  utf8_stream *stream = arg;
  
  /* lock while we initialize */
  kava_mutex_lock (&stream->mutex);
  
  /* Get file contents */
  uint8_t *file_buffer = malloc (stream->file_size);
  if (!file_buffer)
    {
      stream->err = errno;
      kava_mutex_unlock (&stream->mutex);
      kava_thread_exit (NULL);
    }
  if (-1 == kava_read (stream->file, file_buffer, stream->file_size))
    {
      stream->err = errno;
      kava_mutex_unlock (&stream->mutex);
      kava_thread_exit (NULL);
    }
  fclose (stream->file);
  
  /* Discover UTF encoding and convert to UTF-8 if necessary */
  uint8_t *utf8_buffer;
  size_t utf8_buffer_size;
  if (u8_check ((const uint8_t *) file_buffer, stream->file_size))
    {
      /* Not UTF-8 */
      if (u16_check ((const uint16_t *) file_buffer, stream->file_size))
        {
          /* Not UTF-16 */
          if (u32_check ((const uint32_t *) file_buffer, stream->file_size))
            {
              /* Unrecognized Encoding */
              stream->err = EENCODING;
              kava_mutex_unlock (&stream->mutex);
              kava_thread_exit (NULL);
            }
          else
            {
              /* File was in UTF-32 */
              utf8_buffer = u32_to_u8 ((const uint32_t *) file_buffer, 
                                       stream->file_size,
                                       NULL, 
                                       &utf8_buffer_size);
              free (file_buffer);
            }
        }
      else
        {
          /* File was in UTF-16 */
          utf8_buffer = u16_to_u8 ((const uint16_t *) file_buffer, 
                                   stream->file_size,
                                   NULL,
                                   &utf8_buffer_size);
          free (file_buffer);
        }
    }
  else
    {
      /* File was already UTF-8 */
      utf8_buffer = file_buffer;
      utf8_buffer_size = stream->file_size;
    }
  
  /* Normalize UTF-8 Buffer*/
  size_t buffer_size;
  uint8_t *buffer = u8_normalize (UNINORM_NFD, utf8_buffer, utf8_buffer_size, NULL, &buffer_size);
  
  /* Initialization is complete */
  kava_mutex_unlock (&stream->mutex);
  free (utf8_buffer);
  
  /* Start to fill the stack */
  int start = 0;
  int end = 0;
  int i = -1;
  while (stream->open && !stream->err)
    {
      /* Make sure there is room for i to grow */
      if (i + 1 >= buffer_size)
        {
          stream->open = 0;
          continue;
        }
      
      /* Get next character */
      ucs4_t uc;
      uint8_t byte = buffer[++i];
      if (byte & 0x80 == 0)
        uc = byte;
      else if (byte & 0xE0 == 0xC0)
        uc = (byte << 8) | buffer[++i];
      else if (byte & 0xF0 == 0xE0)
        uc = ((byte << 16) | (buffer[++i] << 8)) | buffer[++i];
      else
        uc = (((byte << 24) | (buffer[++i] << 16)) | (buffer[++i] << 8)) | buffer[++i];
      
      /* Look for line break */
      if (uc_is_property_line_separator (uc))
        {
          string_node *node = malloc (sizeof (string_node));
          if (!node)
            {
              stream->err = errno;
              continue;
            }
          
          node->next = NULL;
          node->string.len = end - start;
          
          if (node->string.len > 0)
            {
              node->string.bytes = malloc (node->string.len);
              if (!node->string.bytes)
                {
                  free (node);
                  stream->err = errno;
                  continue;
                }
              memcpy (node->string.bytes, buffer + start, node->string.len);
            }
          else
            {
              node->string.bytes = NULL;
            }
          
          /* Insert node into the stack */
          kava_mutex_lock (&stream->mutex);
          if (stream->tail)
            {
              stream->tail->next = node;
              stream->tail = node;
            }
          else
            {
              stream->head = node;
              stream->tail = node;
            }
          kava_cond_signal (&stream->cond);
          kava_mutex_unlock (&stream->mutex);
          
          start = i + 1;
          end = start;
        }
      else
        {
          end = i;
        }
    }
  
  /* The stream was closed  */
  kava_mutex_lock (&stream->mutex);
  kava_cond_signal (&stream->cond);
  kava_mutex_unlock (&stream->mutex);
  
  free (buffer);
  kava_thread_exit (NULL);
}

int
utf8_stream_from_path (utf8_stream *const restrict stream, const char *path)
{
  stream->file = fopen (path, "r");
  if (!stream->file)
    return errno;

  kava_mutex_init (&stream->mutex);
  kava_cond_init (&stream->cond);
  
  kava_stat file_stat;
  kava_read_stat (path, &file_stat);
  stream->file_size = (size_t) file_stat.st_size;
  
  stream->head = NULL;
  stream->tail = NULL;
  stream->open = 1;

  return kava_thread_create (&stream->thread, read_file, stream);
}

utf8_string
utf8_stream_readline (utf8_stream *const restrict stream, int *const restrict err)
{
  utf8_string ret = {
    .bytes = NULL,
    .len = 0
  };
  
  if (!stream)
    {
      *err = EINVAL;
      return ret;
    }

  kava_mutex_lock (&stream->mutex);

  if (stream->err)
    {
      *err = stream->err;
      kava_mutex_unlock (&stream->mutex);
      return ret;
    }

  if (!stream->head && stream->open)
    /* stream is empty, but data is still being read */
    kava_cond_wait (&stream->cond, &stream->mutex);

  if (stream->head)
    {
      /* stream is not empty */
      string_node *node = stream->head;
      stream->head = stream->head->next;
      if (!stream->head)
	stream->tail = NULL;
      kava_mutex_unlock (&stream->mutex);

      ret.len = node->string.len;
      ret.bytes = node->string.bytes;
      free (node);
      return ret;
    }
  else
    {
      /* stream has been closed */
      kava_mutex_unlock (&stream->mutex);
      *err = EOF;
      return ret;
    }
}

int
utf8_stream_close (utf8_stream *restrict stream)
{
  if (!stream)
    return 0;

  stream->open = 0;

  int err = kava_thread_join (stream->thread, NULL);
  if (err)
    return err;

  string_node *node = stream->head;
  while (node)
    {
      free (node->string.bytes);
      string_node *next = node->next;
      free (node);
      node = next;
    }
  
  return 0;
}
