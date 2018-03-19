#ifndef POSIX_SYS_STAT_H
#define POSIX_SYS_STAT_H

#include <config.h>

#if HAVE_SYS_STAT_H

#include <sys/stat.h>

#define kava_stat struct stat
#define kava_read_stat(pathname,statbuf) \
  stat (pathname, statbuf)

#else

#error /* Not yet implemented */

#endif /* HAVE_SYS_STAT_H */

#endif 
