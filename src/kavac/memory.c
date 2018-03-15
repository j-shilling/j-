/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memory.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void *kavac_alloc (size_t size) {
  void *ret = malloc (size);
  if (ret)
    return ret;
  
  if (errno == ENOMEM)
    fprintf (stderr, "Out of memory. Shutting down.\n");
  else
    fprintf (stderr, "Failed to allocate memory for an unkown reason. Shutting down.\n");
  
  exit (EXIT_FAILURE);
}

void *kavac_alloc0 (size_t size) {
  void *ret = calloc (1, size);
  if (ret)
    return ret;
  
  if (errno == ENOMEM)
    fprintf (stderr, "Out of memory. Shutting down.\n");
  else
    fprintf (stderr, "Failed to allocate memory for an unkown reason. Shutting down.\n");
  
  exit (EXIT_FAILURE);
}

void kavac_free (void *ptr) {
  if (ptr)
    free (ptr);
}