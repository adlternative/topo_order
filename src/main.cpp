// #include "parse_options.h"
#include "object.h"
#include "util.h"
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <zconf.h>
#include <zlib.h>

const char *usage = {"[usage]:myg"
                     "[add <file>]"
                     "[init]"
                     "[--help]"};
void myg_init(int argc, const char **argv) {
  std::string Repo = ".";
  if (argc == 2) {
    Repo = argv[1];
    if (Repo.back() != '/')
      Repo.push_back('/');
  }
  std::string GIT = Repo + ".git/";
  std::string HEAD = GIT + "HEAD";
  // std::string HEAD = "HEAD";
  std::vector<std::string> GITDIR{"objects/", "refs/", "refs/heads/"};
  mkdir(Repo.c_str(), 0755);
  mkdir(GIT.c_str(), 0755);
  for (auto &&dirName : GITDIR) {
    mkdir((GIT + dirName).c_str(), 0755);
  }
  int fd = open(HEAD.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644);
  std::string write_to_HEAD = "ref: refs/heads/master\n";
  write(fd, write_to_HEAD.c_str(), write_to_HEAD.size());
  close(fd);
}

void hash_object() {}

void myg_add(int argc, const char **argv) {}

void handler_args(int argc, const char **argv) {

  if (strcmp(argv[0], "add") == 0)
    myg_add(argc, argv);
  if (strcmp(argv[0], "init") == 0)
    myg_init(argc, argv);
}
int main(int argc, char const *argv[]) {
  // ArgumentParser parser = ArgumentParser(argc, argv);
  // SubParsers sub_parsers = parser.addSubParsers();
  // SubParser sub_parser = sub_parsers.add_parser("add", "add file(s) to
  // index");
  if (argc == 1) {
    die(usage);
  }
  argc--;
  argv = static_cast<char const **>(&argv[1]);
  handler_args(argc, argv);
  return 0;
}
