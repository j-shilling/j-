#include "utf8-stream.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <uniconv.h>

#include <kava/posix/unistd.h>

static const char test_string[] =
        "#include <stdio.h>\n"
        "\n"
        "int\n"
        "main (int argc, char *argv[])\n"
        "  {\n"
        "    printf (Hello, World\\n);\n"
        "    return 0;\n"
        "  }\n"
        "\n";

static int
write_file (const char *path, void *data, size_t size, size_t nmemb)
{
  FILE *file = fopen (path, "w");
  if (!file)
    {
      fprintf (stderr, "Could not open %s: %s\n", path, strerror (errno));
      return -1;
    }

  if (nmemb != fwrite (data, size, nmemb, file))
    {
      fprintf (stderr, "Error writing data to %s: %s\n", path, strerror (errno));
      fclose (file);

      if (-1 == kava_unlink (path))
        {
          fprintf (stderr, "Error deleting %s: %s\n", path, strerror (errno));
        }

      return -1;
    }

  return 0;
}

int
main (int argc, char *argv[])
{
  const size_t str_len = strlen (test_string);
  const char *encoding = locale_charset ();

  /* Write test files */
  size_t nmemb;
  uint8_t *utf8 = u8_conv_from_encoding (encoding,
                                         iconveh_question_mark,
                                         test_string,
                                         str_len,
                                         NULL,
                                         NULL,
                                         &nmemb);
  if (!utf8)
    {
      fprintf (stderr, "Conversiont error: %s\n", strerror (errno));
      return EXIT_FAILURE;
    }

  if (write_file ("utf8.txt", utf8, sizeof (uint8_t), nmemb))
    {
      return EXIT_FAILURE;
    }

  free (utf8);

  uint16_t *utf16 = u16_conv_from_encoding (encoding,
                                            iconveh_question_mark,
                                            test_string,
                                            str_len,
                                            NULL,
                                            NULL,
                                            &nmemb);

  if (!utf16)
    {
      fprintf (stderr, "Conversiont error: %s\n", strerror (errno));
      return EXIT_FAILURE;
    }

  if (write_file ("utf16.txt", utf16, sizeof (uint16_t), nmemb))
    {
      return EXIT_FAILURE;
    }

  free (utf16);

  uint32_t *utf32 = u32_conv_from_encoding (encoding,
                                            iconveh_question_mark,
                                            test_string,
                                            str_len,
                                            NULL,
                                            NULL,
                                            &nmemb);

  if (!utf32)
    {
      fprintf (stderr, "Conversiont error: %s\n", strerror (errno));
      return EXIT_FAILURE;
    }

  if (write_file ("utf32.txt", utf16, sizeof (uint16_t), nmemb))
    {
      return EXIT_FAILURE;
    }

  free (utf32);
  
  utf8_stream u8, u16, u32;
  if (utf8_stream_from_path (&u8, "utf8.txt"))
    return EXIT_FAILURE;
  if (utf8_stream_from_path (&u16, "utf16.txt"))
    return EXIT_FAILURE;
  if (utf8_stream_from_path (&u32, "utf32.txt"))
    return EXIT_FAILURE;
  
  for (;;)
    {
      int u8err, u16err, u32err;
      utf8_string u8str, u16str, u32str;
      
      u8str = utf8_stream_readline (&u8, &u8err);
      u16str = utf8_stream_readline (&u16, &u16err);
      u32str = utf8_stream_readline (&u32, &u32err);
      
      if (u8err == EOF)
        {
          if (u16err != EOF || u32err != EOF)
            return EXIT_FAILURE;
          
          break;
        }
      else if (u8err || u16err || u32err)
        {
          return EXIT_FAILURE;
        }
      
      if (u8str.len != u16str.len 
          || strncmp (u8str.bytes, u16str.bytes, u8str.len))
        {
          return EXIT_FAILURE;
        }
      
      if (u8str.len != u32str.len 
          || strncmp (u8str.bytes, u32str.bytes, u8str.len))
        {
          return EXIT_FAILURE;
        }
      
      free (u8str.bytes);
      free (u16str.bytes);
      free (u32str.bytes);
    }
  
  if (utf8_stream_close (&u8))
    return EXIT_FAILURE;
  if (utf8_stream_close (&u16))
    return EXIT_FAILURE;
  if (utf8_stream_close (&u32))
    return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
