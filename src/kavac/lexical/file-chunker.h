#ifndef KAVAC_LEXICAL_UTF8_STREAM_H
#define KAVAC_LEXICAL_UTF8_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kava/posix/thread.h>

#include <kavac/error.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef char chunk;

  /* Stream which reads a file and returns it as meaningful chuncks of strings */
  typedef struct
  {
    /* Thread Safty */
    kava_thread thread;
    kava_mutex mutex;
    kava_cond cond;
    volatile int closed;

    /* File infomation */
    char *path;
    kavac_error *err;
    int eof;

    /* Stack of output chunks */
    chunk *head;
    chunk *tail;
  } file_chunker;

  int file_chunker_open (file_chunker * const stream,
                         const char *path,
                         kavac_error **err);

  chunk *file_chunker_next (file_chunker * const stream,
                            kavac_error **err);

  void file_chunker_close (file_chunker *stream);

  size_t file_chunk_len (chunk *filechunk);
  
  void file_chunk_free (chunk *filechunk);

#ifdef __cplusplus
}
#endif

#endif /* KAVAC_LEXICAL_UTF8_STREAM_H */
