#ifndef KAVAC_LEXICAL_UTF8_STREAM_H
#define KAVAC_LEXICAL_UTF8_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <kava/posix/thread.h>

#define EENCODING	    101

typedef struct _utf8_string {
  size_t len;
  uint8_t *bytes;
} utf8_string;

typedef struct _string_node {
  utf8_string string;
  struct _string_node *next;
} string_node;

typedef struct _utf8_stream {
  kava_thread thread;
  kava_mutex mutex;
  kava_cond cond;
  
  FILE *file;
  size_t file_size;
  
  volatile int err;
  volatile int open;

  string_node *head;
  string_node *tail;
} utf8_stream;

int          utf8_stream_from_path (utf8_stream *const restrict stream, const char *path);
utf8_string  utf8_stream_readline  (utf8_stream *const restrict stream, int *const restrict err);
int          utf8_stream_close     (utf8_stream *restrict stream);

#endif /* KAVAC_LEXICAL_UTF8_STREAM_H */
