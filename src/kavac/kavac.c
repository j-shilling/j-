#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "parser.h"

static const int EOPEN = -1;
static const int EREAD = -2;
static const int EPARSE = -3;

extern FILE *yyin;

int
kavac (const char *path)
{
  yyin = fopen (path, "r");
  if (!yyin)
    {
      char buf[100];
      strerror_r (errno, buf, 100);
      fprintf (stderr, "Could not open %s: %s\n", path, buf);
      return EOPEN;
    }

  if (yyparse ())
        {
          return EPARSE;
        }
      
  return 0;
}