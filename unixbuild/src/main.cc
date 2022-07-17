#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <optional>
#include <string>
#include <unistd.h>
#include <vector>

#include "unixbuild/common.h"

struct CommandLine {
  std::string build_path;
  std::string target;
  std::string output_path;
};

struct Rule {
  std::string output;
  std::vector<std::string> deps;
};

struct BuildFile {
  std::vector<Rule> rules;
};

BuildFile parse_build_file(const std::string& path);
std::optional<Rule> parse_line(std::string& line, size_t lineno);

CommandLine parse_args(int argc, char* argv[]);
void print_help(void);
void print_usage(void);

int main(int argc, char* argv[]) {
  try {
    CommandLine cmdline = parse_args(argc, argv);
    BuildFile build_file = parse_build_file(cmdline.build_path);
  } catch (unixbuild::ExitException& e) {
    std::cerr << "error: " << e.message_ << std::endl;
    return e.returncode_;
  }
  return 0;
}

BuildFile parse_build_file(const std::string& path) {
  std::vector<std::string> lines = unixbuild::read_lines(path.c_str());
  BuildFile build_file;

  size_t lineno = 1;
  for (std::string& line : lines) {
    std::optional<Rule> optional_rule = parse_line(line, lineno);
    if (optional_rule.has_value()) {
      build_file.rules.push_back(optional_rule.value());
    }
    lineno++;
  }

  return build_file;
}

std::optional<Rule> parse_line(std::string& line, size_t lineno) {
  Rule rule;
  unixbuild::trim_whitespace(line);
  if (line.empty() or line[0] == '#') {
    return {};
  }

  auto colon_pos = line.find(':');
  if (colon_pos == std::string::npos) {
    throw unixbuild::ParseException(lineno, "no colon");
  }

  rule.output = line.substr(0, colon_pos);
  unixbuild::trim_whitespace(rule.output);
  unixbuild::split_string(line.substr(colon_pos + 1), rule.deps, ' ');

  if (rule.deps.size() == 0) {
    throw unixbuild::ParseException(lineno, "no deps");
  }

  return rule;
}

CommandLine parse_args(int argc, char* argv[]) {
  CommandLine cmdline;

  if (argc < 2) {
    puts("error: too few arguments\n");
    print_usage();
    exit(1);
  }

  bool seen_arg_separator = false;
  char** argp = argv + 1;
  while (*argp != NULL) {
    char* arg = *argp;
    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      print_help();
      exit(0);
    } else if (strcmp(arg, "--out") == 0) {
      argp++;
      arg = *argp;
      if (arg == NULL || *arg == '-') {
        puts("error: expected argument to --out\n");
        print_usage();
        exit(1);
      } else {
        cmdline.output_path = arg;
      }
    } else if (strcmp(arg, "--") == 0) {
      seen_arg_separator = true;
    } else if (!seen_arg_separator && *arg == '-') {
      printf("error: unknown flag %s\n\n", arg);
      print_usage();
      exit(1);
    } else {
      if (cmdline.build_path.empty()) {
        cmdline.build_path = arg;
      } else if (cmdline.target.empty()) {
        cmdline.target = arg;
      } else {
        puts("error: too many arguments\n");
        print_usage();
        exit(1);
      }
    }

    // The last element of argv is guaranteed to be a null pointer, so as long
    // as we check it isn't NULL in the loop condition, we can continue to
    // increment it to get the next argument.
    argp++;
  }

  return cmdline;
}

void print_usage() { puts("usage: unixbuild <build file> <target>"); }

void print_help() {
  print_usage();
  puts(
      "\n"
      "Arguments:\n"
      "  <build file>        Path to the BUILD.uxb file. Mandatory.\n"
      "  <target>            Target to build. Defaults to first target listed\n"
      "                      in the build file.\n"
      "  --out <directory>   Directory in which to place output files.\n"
      "                      Defaults to current directory.");
}
