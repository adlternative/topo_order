#ifndef ADL_SHA256_H
#define ADL_SHA256_H
#include <cstring>
#include <openssl/sha.h>
#include <string>

class Sha256General {
  SHA256_CTX hash;

public:
  Sha256General() {}
  void init() { SHA256_Init(&hash); }
  void setFile(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
      perror("fopen");
      exit(1);
    }
    size_t size;
    char buf[4096];
    while ((size = fread(buf, 1, 4096, file)) != 0) {
      set(buf, size);
    }
    fclose(file);
  }
  void set(char *buf, size_t size) { SHA256_Update(&hash, buf, size); }
  void set(std::string buf, size_t size) {
    SHA256_Update(&hash, buf.c_str(), size);
  }
  std::string getString() {
    unsigned char result[SHA256_DIGEST_LENGTH];
    SHA256_Final(result, &hash);
    char ss[65];
    memset(ss, 0, sizeof(ss));
    char s[3];
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      snprintf(s, 3, "%02x", result[i]);
      strncat(ss, s, 2);
    }
    return ss;
  }
  char *getarr() {
    unsigned char result[SHA256_DIGEST_LENGTH];
    SHA256_Final(result, &hash);
    char ss[65];
    memset(ss, 0, sizeof(ss));
    char s[3];
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      snprintf(s, 3, "%02x", result[i]);
      strncat(ss, s, 2);
    }
    return strdup(ss);
  }
};
#endif