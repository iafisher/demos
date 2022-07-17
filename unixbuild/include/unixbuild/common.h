#ifndef UNIXBUILD_COMMON_H_
#define UNIXBUILD_COMMON_H

#include <string>
#include <vector>

namespace unixbuild {

class ExitException : public std::exception {
public:
  explicit ExitException(int returncode)
      : message_(""), returncode_(returncode) {}
  explicit ExitException(std::string message, int returncode)
      : message_(message), returncode_(returncode) {}

  std::string message_;
  int returncode_;
};

class ParseException : public ExitException {
public:
  explicit ParseException(size_t lineno, std::string message)
      : ExitException(std::string("could not parse line ")
                          .append(std::to_string(lineno))
                          .append(": ")
                          .append(message),
                      2) {}
};

// Reads the lines of the file into a vector of strings.
//
// Each string includes the trailing newline, except the last one may not if the
// file is missing a final newline.
std::vector<std::string> read_lines(const char* path);

// Removes whitespace characters from the beginning and end of `s` in-place.
void trim_whitespace(std::string& s);

// Splits the string around occurrences of `ch` and places the resulting
// fragments into the `out` vector.
//
// Multiple consecutive instances of `ch` are treated the same as a single
// instance; this means that an empty string will never be pushed onto `out`.
void split_string(const std::string& s, std::vector<std::string>& out, char ch);

} // namespace unixbuild

#endif
