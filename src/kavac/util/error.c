#include <kavac/error.h>

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

typedef struct {
  unsigned int line;
  unsigned int column;
  unsigned int message;
} kavac_error_header;

void
kavac_set_error (kavac_error **error,
                 const char * filename,
                 const unsigned int line,
                 const unsigned int column,
                 const char *format,
                 ...) 
{
  if (!error)
    return;
  
  if (*error)
    kavac_free_error (*error);
  
  /* Get length of filename */
  size_t filename_len = strlen (filename);
  
  /* Get length of message */
  va_list ap;
  va_start (ap, format);
  
  va_list apc;
  va_copy (apc, ap);
  size_t message_len = vsnprintf (NULL, 0, format, apc);
  va_end (apc);
  
  
  /* Allocate memory block */
  kavac_error_header *header = 
          malloc (sizeof (kavac_error_header) + filename_len + message_len + 2);
  if (!header)
    {
      perror ("Could not allocate space for an error object");
      exit (EXIT_FAILURE);
    }
  kavac_error *err =
          (kavac_error *) (header + 1);
  
  /* Set header values */
  header->line = line;
  header->column = column;
  header->message = filename_len + 1;
  
  memcpy (err, filename, filename_len + 1);
  vsnprintf (err + header->message, message_len + 1, format, ap);
  
  /* Cleanup and return */
  va_end (ap);
  *error = err;
}

void
kavac_propagate_error (kavac_error **dest, kavac_error *src) 
{
  if (!dest || !src)
    return;
  
  if (*dest)
    kavac_free_error (*dest);
  
  *dest = src;
}

void
kavac_free_error (kavac_error *error) 
{
  if (error)
    {
      kavac_error_header *header = ((kavac_error_header *)error) - 1;
      free (header);
    }
}

void 
kavac_perror (const char *msg, kavac_error *error)
{
  if (!error)
    return;
  
  kavac_error_header *header = ((kavac_error_header *)error) - 1;
  fprintf (stderr, "%s:%d,%d ", error, header->line, header->column);
  
  if (msg)
    fprintf (stderr, "%s: ", msg);
  
  fprintf (stderr, "%s\n", error + header->message);
}