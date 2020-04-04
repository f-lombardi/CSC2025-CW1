/*
 * Replace the following string of 0s with your student number
 * 160437414
 */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arraylib.h"

/* error message format for fatalerror */
static const char *ERR_FMT = "%s:%d - %s, errno %d: %s\n";

/***************************************************************************/
/***** ARRAYLIB.H IMPLEMENTATIONS ******************************************/
/***************************************************************************/

/* In the following newarray function:
 *  - dynamically allocate an array struct
 *  - allocate an array of given length
 *  - assign length and pointer to fields of array struct
 *  - return pointer to array struct
 * The normal and error condition behaviour of newarray must comply with
 * the function's specification in the comments in arraylib.h
 */

array *newarray(int len) {

  /*Checks if the length is less than 1*/
  if (len < 1){
    errno = EINVAL; /*sets the error*/
    return NULL; /*returns null and exits function*/
  }
  /*dynamically allocate an array struct*/
  array *arr = malloc(sizeof(array));
  /*Assigns the length to the field of array struct*/
  arr->len = len;
  /*assign a pointer to field of an array struct*/
  arr->ai  = calloc(len, sizeof(int));

  if (!arr){ /*Checks if the memory allocation fails*/
    errno = ENOMEM; /*sets the error*/
    delarray(arr); /*frees the memory allocated to the new array*/
    return NULL; /*returns null and exits function*/
  }
  /*Returns the pointer of the array struct*/
  return arr;
}

int get(array *arr, int idx) {
  /*Checks if arr is null, arr->ai is null or if the index is out of bounds*/
  if (!arr || !arr->ai || idx < 0 || idx >= arr->len) {
    errno = EINVAL; /*sets errno to EINVAL*/
    return INT_MIN; /*returns the minimum integer*/
  }
  /*returns the value at the index position idx from the int array arr->ai*/
  return arr->ai[idx];
}

/* see set specification comments in arraylib.h */
void set(array *arr, int idx, int value) {

  /*Checking to see if the array is out of bounds*/
  if (!arr || !arr->ai || idx < 0 || idx >= arr->len){
    errno = EINVAL; /*sets the error*/
    return; /*exits the function*/
  }
  arr->ai[idx] = value;
}

/* see foreach specification comments in arraylib.h */
void foreach(array *arr, applyfunction applyf) {

   /*Checking the different conditions*/
  if (!arr || !arr->ai || arr->len < 1 || !applyf){
    return; /*exits function*/
  }
  for (int i = 0; i < arr->len; i++){ /*for loop to iterate through the array*/
    arr->ai[i] = applyf(arr, i); /*applys the function applyf*/
  }
}

/* see print specification comments in arraylib.h */
void print(FILE *stream, array *arr) {

  if (!arr || !arr->ai || arr->len < 1){
    fprintf(stream, "[ ]"); /*prints an empty pair of square brackets if the conditions are met*/
    return; /*exits function*/
  }
  fprintf(stream,"["); /*if conditions are not met, prints first square bracket*/

  for (int i = 0; i < arr->len; i++){ /*for loop to iterate through the contents of the array*/
      fprintf(stream, " %d,", arr->ai[i]); /*prints out the contents of the array with commas between*/
  }
  fprintf(stream," ]"); /*prints the last square bracket*/
}

/***************************************************************************/
/***** THE FOLLOWING FUNCTIONS ARE IMPLEMENTED FOR YOU - DO NOT CHANGE *****/
/***************************************************************************/

/* see println comments in arraylib.h */
void println(FILE *stream, array *arr) {
    print(stream, arr);
    fprintf(stream, "\n");
}

/* see delearray comments in arraylib.h */
void delarray(array *arr) {
    if (arr) {
        if (arr->ai)
            free(arr->ai);
        free(arr);
    }
}

/* see fatalerror comments in arraylib.h */
void fatalerror(int line, char *msg) {
    fprintf(stderr, ERR_FMT, __FILE__, line, msg, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

/* see newarray_e comments in arraylib.h */
array *newarray_e(int len) {
    array *arr = newarray(len);

    if (!arr)
        fatalerror(__LINE__, "array allocation failed");

    return arr;
}

/* see get_e comments in arraylib.h */
int get_e(array *arr, int idx) {
    int val = get(arr, idx);

    if (val == INT_MIN && errno == EINVAL)
        fatalerror(__LINE__, "null array or index out of bounds");

    return val;
}


/* see set_e comments in arraylib.h */
void set_e(array *arr, int idx, int value) {
    set(arr, idx, value);

    if (errno == EINVAL)
        fatalerror(__LINE__, "null array or index out of bounds");
}

/***************************************************************************/
