#ifndef ERROR_H
#define ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

  typedef char kavac_error;
  
  void kavac_set_error (kavac_error **error,
                        const char * filename,
                        const unsigned int line,
                        const unsigned int column,
                        const char *format,
                        ...);
  
  void kavac_propagate_error (kavac_error **dest, kavac_error *src);
  
  void kavac_free_error (kavac_error *error);
  
  void kavac_perror (const char *msg, kavac_error *error);

#ifdef __cplusplus
}
#endif

#endif /* ERROR_H */

