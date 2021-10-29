#include <stdio.h>

int foo() {
  printf("OK\n");
}

int bar(int x, int y) {
  printf("%d\n", x + y);
}

int baz(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}
