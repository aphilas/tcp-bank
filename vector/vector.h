#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
  char **data; // for pointer arithmetic? - SO, void**?
  int unitSize;
  int size;
  int used;
} Vector;

Vector* create(int unitSize);
int push(Vector* vector, void* data);
void* get(Vector* vector, int index);
int delete(Vector* vector);

#endif