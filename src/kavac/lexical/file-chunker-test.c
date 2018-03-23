#include "file-chunker.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <uniconv.h>

#include <kava/posix/unistd.h>
#include <kavac/error.h>

#define PATH "file-chunker-test.txt"

static const char test_string[] =
        "\n"
        "#include <stdio.h>\n"
        "\n"
        "int\n"
        "main (int argc, char *argv[])\n"
        "  {\n"
        "    printf (Hello, World\\n);\n"
        "    return 0;\n"
        "  }\n"
        "\n";

int
main (int argc, char *argv[])
{
  const size_t str_len = strlen (test_string);

  FILE *file = fopen (PATH, "w");
  if (!file)
    {
      perror ("Could not open " PATH);
      return EXIT_FAILURE;
    }

  if (str_len != fwrite (test_string, 1, str_len, file))
    {
      perror ("Could not write " PATH);
      fclose (file);

      if (-1 == kava_unlink (PATH))
        {
          perror ("Could not delete " PATH);
        }

      return EXIT_FAILURE;
    }
  
  fclose (file);

  kavac_error *err = NULL;
  file_chunker chunker;
  if (file_chunker_open (&chunker, PATH, &err))
    {
      kavac_perror (NULL, err);
      return EXIT_FAILURE;
    }

  chunk *ch = file_chunker_next (&chunker, &err);
  while (ch)
    {
      size_t len = file_chunk_len (ch);
      char *str = malloc (len + 1);

      snprintf (str, len + 1, "%s", ch);
      printf ("%s", str);

      ch = file_chunker_next (&chunker, &err);
    }

  if (err)
    {
      kavac_perror (NULL, err);

      file_chunker_close (&chunker);
      if (-1 == kava_unlink (PATH))
        {
          perror ("Could not delete " PATH);
        }
      
      kavac_free_error (err);

      return EXIT_FAILURE;
    }

  file_chunker_close (&chunker);
  if (-1 == kava_unlink (PATH))
    {
      perror ("Could not delete " PATH);
    }
  return EXIT_SUCCESS;
}
