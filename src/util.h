#ifndef UTIL_H
#define UTIL_H
#include <iostream>
void die(const char *str) {
  fprintf(stderr, str);
  exit(1);
}
#endif
