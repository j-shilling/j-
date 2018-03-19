#ifndef KAVAC_LEXICAL_STRING_STREAM_H
#define KAVAC_LEXICAL_STRING_STREAM_H

typedef struct _string_stream string_stream;
typedef struct _string string;
#define EENCODING   1

string_stream * string_stream_from_path (const char *path);
string        * string_stream_next      (restrict string_stream *const stream);
int             string_stream_error     (restrict string_stream const *const stream);

#endif /* KAVAC_LEXICAL_STRING_STREAM_H */
