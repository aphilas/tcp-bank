
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vector.h"

#define BASE_SIZE 10

Vector* create(int unitSize) {
  Vector* vector = malloc(sizeof(Vector));

  if (vector == NULL) {
    printf("malloc failed\n");
    exit(1);
  } 

  vector->unitSize = unitSize;
  vector->size = BASE_SIZE;
  vector->used = 0;
  vector->data = malloc(unitSize * BASE_SIZE);

  if (vector->data == NULL) {
    free(vector);
    vector = NULL;
    printf("malloc failed\n");
    exit(1);
  }

  return vector;
}

int push(Vector* vector, void *data) {
  if (vector->used == vector->size) {
  //  vector->data = realloc(vector->data, vector->unitSize * (vector->size * 2));
   void* temp = realloc(vector->data, vector->unitSize * (vector->size * 2));

   if (vector->data == NULL) {
      printf("Realloc failed\n");
      exit(1);
    }

    // memcpy(vector->data + (vector->size) * (vector->unitSize), data, vector->unitSize);
    vector->size *= 2;
    // printf("Vector resized\n");
  }

  // SO - https://stackoverflow.com/questions/37496282/c-generic-array-implementation
  memcpy(vector->data + (vector->used) * (vector->unitSize), data, vector->unitSize);

  // erroneous result
  // vector->data[vector->used] = data;
  vector->used += 1;

  // printf("Data added\n");
  return 0;
}

void* get(Vector* vector, int index) {
  assert(vector);
  if (index < 0 || index >= vector->size) {
    printf("Out of index\n");
    exit(1);
  }

  return vector->data + index * (vector->unitSize);
}

// not tested yet
int delete(Vector* vector) {
  for (int i = 0; i < vector->used; i++) {
    free(vector->data[i]);
  }

  free(vector->data);  
  vector->data = NULL;
  vector->used = vector->size = 0;
  free(vector); // order?

  return 0;
}