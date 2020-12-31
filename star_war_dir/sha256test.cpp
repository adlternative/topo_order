#include "sha256.h"
#include <cstring>
#include <iostream>
#include <openssl/sha.h>
#include <string>
int main(void) {
  Sha256General g;
  g.init();
  g.setFile("sha256test.cpp");
  std::cout << g.getString() << std::endl;
  g.init();
  g.setFile("sha256test.cpp");
  std::cout << g.getString() << std::endl;
  g.init();
  char s[] = "safgfhg";
  g.set(s, strlen(s));
  std::cout << g.getString() << std::endl;

  return 0;
}