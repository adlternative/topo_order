// #ifndef PARSE_OPTIONS_H
// #define PARSE_OPTIONS_H
// #include <string>

// class SubParsers;
// class ArgumentParser {
// public:
//   ArgumentParser(int argc, const char **argv);
//   SubParsers addSubParsers(/* const char dest */);

// private:
//   std::string parent_command;
//   int _argc;
//   const char **_argv;
//   // std::string sub_command;
// };
// class SubParsers {
// public:
//   SubParsers(int argc, const char **argv) : _argc(argc), _argv(argv) {
//     if (_argc == 0) {
//       sub_command = "help";
//     } else {
//       sub_command = _argv[0];
//     }
//   }

// private:
//   std::string sub_command;
//   int _argc;
//   const char **_argv;
// };

// #endif