#ifndef KAVAC_LEXICAL_UTF8_STREAM_H
#define KAVAC_LEXICAL_UTF8_STREAM_H

#include <stdint.h>

#define EENCODING	    101

typedef uint32_t unichar;
typedef struct _utf8_stream utf8_stream;

utf8_stream *utf8_stream_from_path (const char *path, int *err);
unichar      utf8_stream_next      (utf8_stream *const restrict stream, int *const restrict err);
void         utf8_stream_close     (utf8_stream *restrict stream, int *const restrict err);

#endif /* KAVAC_LEXICAL_UTF8_STREAM_H */
