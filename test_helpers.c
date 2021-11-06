#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int foo() {
  printf("OK\n");

  return 0;
}

int bar(int x, int y) {
  printf("%d\n", x + y);

  return 0;
}

int baz(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}

void alloc4(int **ptr, int a, int b, int c, int d) {
  *ptr = (int *)malloc(4 * sizeof(int));

  int arr[4] = { a, b, c, d };

  memcpy(*ptr, arr, 4 * sizeof(arr));
}
