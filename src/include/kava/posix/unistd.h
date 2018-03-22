#ifndef POSIX_UNISTD_H
#define POSIX_UNISTD_H

#include <config.h>

#if HAVE_UNISTD_H

#include <unistd.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
  
#define kava_unlink(filename)   unlink (filename)

static inline size_t
kava_read (FILE *file, void *buffer, off_t buffer_length)
{
  return (size_t) read (fileno (file), buffer, buffer_length);
}

#ifdef __cplusplus
}
#endif

#else

#error /* Not yet implemented */

#endif /* HAVE_UNISTD_H */

#endif
