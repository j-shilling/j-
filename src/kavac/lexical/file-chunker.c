#include "file-chunker.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <kava/posix/thread.h>

#include <kavac/error.h>

#define HEADER(x)               (x ? (((chunk_header *)x) - 1) : (chunk_header *)NULL)
#define CHUNK(x)                (x ? ((chunk *)(x + 1)) : (chunk *)NULL)

#define BUF_SIZE                512

typedef enum
{
  KEYWORD_OR_IDENTIFIER,
  PUNCTUATION,
  STRING_LITERAL,
  HEX_INT_LITERAL,
  DEC_INT_LITERAL,
  FLOAT_LITERAL
} chunk_tag;

typedef struct _chunk_header
{
  chunk_tag tag;
  size_t len;
  unsigned int line;
  unsigned int column;
  struct _chunk_header *next;
} chunk_header;

_Noreturn static void *
read_file (void *data)
{
  file_chunker *stream = data;

  FILE *file = fopen (stream->path, "r");
  if (!file)
    {
      kavac_set_error (&stream->err,
                       stream->path,
                       0, 0,
                       "Could not open file. %s",
                       strerror (errno));
      stream->closed = 1;
      kava_thread_exit (NULL);
    }

  char buf[BUF_SIZE];
  int index = 0;
  unsigned int line = 1;
  unsigned int column = 1;

  /* Get initial character */
  char byte;
  if (1 != fread (&byte, 1, 1, file))
    {
      /* Maybe we are done */
      int ferr = ferror (file);
      if (ferr)
        {
          kavac_set_error (&stream->err,
                           stream->path,
                           line, column,
                           "Could not read initial character. %s",
                           strerror (ferr));
        }

      stream->closed = 1;
    }

  while (!stream->closed)
    {
      if (byte & 0x80)
        {
          kavac_set_error (&stream->err,
                           stream->path,
                           line, column,
                           "Multibyte characters are not allowed "
                           "outside of string literals");

          stream->closed = 1;
          break;
        }

      chunk_tag tag;
      /* Ignore whitespace */
      if (byte == '\n')
        {
          line++;
          column = 0;
        }

      /* key word or identifier [_a-zA-Z][_a-zA-Z0-9]* */
      else if (byte == '_' || isalpha (byte))
        {
          tag = KEYWORD_OR_IDENTIFIER;
          while (byte == '_' || isalnum (byte))
            {
              if (index < BUF_SIZE)
                {
                  buf[index++] = byte;
                }
              else
                {
                  kavac_set_error (&stream->err,
                                   stream->path,
                                   line, column + index,
                                   "Chunk buffer overflow");
                  stream->closed = 1;
                  break;
                }

              if (1 != fread (&byte, 1, 1, file))
                {
                  /* Maybe we are done */
                  int ferr = ferror (file);
                  if (ferr)
                    {
                      kavac_set_error (&stream->err,
                                       stream->path,
                                       line, column,
                                       "Could not read next character. %s",
                                       strerror (ferr));
                    }

                  stream->closed = 1;
                  break;
                }
            }
        }
      
      /* anything else is an error */
      else
        {
          kavac_set_error (&stream->err,
                           stream->path,
                           line, column,
                           "Illegal character \'%c\'",
                           byte);
          stream->closed = 1;
          break;
        }
      
      /* Save buffer on chunk stack */
      if (index > 0)
        {
          chunk_header *header = malloc (sizeof (chunk_header) + index);
          if (!header)
            {
              kavac_set_error (&stream->err,
                               stream->path,
                               line, column,
                               "Could not allocate another chunck. %s",
                               strerror (errno));
              stream->closed = 1;
              break;
            }
          
          header->tag = tag;
          header->line = line;
          header->column = column;
          header->len = index;
          header->next = NULL;
          
          chunk *data = CHUNK(header);
          memcpy (data, buf, header->len);
          
          column += index;
          index = 0;
          
          kava_mutex_lock (&stream->mutex);
          if (stream->tail)
            {
              HEADER(stream->tail)->next = header;
              stream->tail = CHUNK(header);
            }
          else
            {
              stream->head = CHUNK(header);
              stream->tail = CHUNK(header);
            }
          kava_cond_signal (&stream->cond);
          kava_mutex_unlock (&stream->mutex);
        }

      /* Read next byte */
      if (1 != fread (&byte, 1, 1, file))
        {
          /* Maybe we are done */
          int ferr = ferror (file);
          if (ferr)
            {
              kavac_set_error (&stream->err,
                               stream->path,
                               line, column,
                               "Could not read initial character. %s",
                               strerror (ferr));
            }

          stream->closed = 1;
        }
      else
        {
          column ++;
        }
    }

  fclose (file);
  kava_mutex_lock (&stream->mutex);
  kava_cond_signal (&stream->cond);
  kava_mutex_unlock (&stream->mutex);
  kava_thread_exit (NULL);
}

int
file_chunker_open (file_chunker * const stream,
                   const char *path,
                   kavac_error **err)
{
  kava_mutex_init (&stream->mutex);
  kava_cond_init (&stream->cond);
  stream->closed = 0;
  stream->err = NULL;
  stream->eof = 0;
  stream->head = NULL;
  stream->tail = NULL;

  size_t path_len = strlen (path);
  stream->path = malloc (path_len + 1);
  if (!stream->path)
    {
      kavac_set_error (err,
                       path,
                       0, 0,
                       "Could not allocate memory to store file path");
      return -1;
    }
  memcpy (stream->path, path, path_len + 1);

  int res = kava_thread_create (&stream->thread, read_file, stream);
  if (res)
    {
      kavac_set_error (err,
                       path,
                       0, 0,
                       "Could not start thread to read from file. %s",
                       strerror (res));
      return -1;
    }

  return 0;
}

chunk *
file_chunker_next (file_chunker * const stream,
                   kavac_error **err)
{
  if (!stream)
    return NULL;

  chunk *ret;
  kava_mutex_lock (&stream->mutex);

  if (!stream->head && !stream->closed)
    kava_cond_wait (&stream->cond, &stream->mutex);

  if (stream->head)
    {
      ret = stream->head;
      chunk_header *header = HEADER (stream->head);
      if (header->next)
        {
          stream->head = CHUNK (header->next);
        }
      else
        {
          stream->head = NULL;
          stream->tail = NULL;
        }
    }
  else
    {
      kavac_propagate_error (err, stream->err);
      ret = NULL;
    }

  kava_mutex_unlock (&stream->mutex);
  return ret;
}

void
file_chunker_close (file_chunker *stream)
{
  if (!stream)
    return;

  stream->closed = 1;
  kava_thread_join (stream->thread, NULL);

  chunk_header *cur = HEADER (stream->head);
  while (cur)
    {
      chunk_header *next = cur->next;
      free (cur);
      cur = next;
    }

  if (stream->path)
    free (stream->path);
}

size_t 
file_chunk_len (chunk *filechunk)
{
  if (filechunk)
    return HEADER(filechunk)->len;
  else
    return 0;
}

void
file_chunk_free (chunk *filechunk)
{
  if (filechunk)
    free (HEADER (filechunk));
}