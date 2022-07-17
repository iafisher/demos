#include <cassert>
#include <cstdlib>
#include <iostream>

#include "unixbuild/common.h"

void test_trim_whitespace() {
  std::string s = " abc ";
  unixbuild::trim_whitespace(s);
  assert(s == "abc");
}

void test_split_string() {
  std::vector<std::string> vec;
  unixbuild::split_string("  ab cdef g ", vec, ' ');

  assert(vec.size() == 3);
  assert(vec[0] == "ab");
  assert(vec[1] == "cdef");
  assert(vec[2] == "g");

  vec.clear();
  unixbuild::split_string("abc", vec, ' ');
  assert(vec.size() == 1);
  assert(vec[0] == "abc");

  vec.clear();
  unixbuild::split_string("     ", vec, ' ');
  assert(vec.size() == 0);
}

void test_read_lines() {
  std::vector<std::string> lines =
      unixbuild::read_lines("test/resources/bigfile.txt");
  assert(lines.size() == 100);

  for (size_t i = 0; i < 100; i++) {
    assert(lines[i] == std::string(i + 1, 'a').append("\n"));
  }

  lines = unixbuild::read_lines("test/resources/no_newline.txt");
  assert(lines.size() == 1);
  assert(lines[0] == "no newline");

  lines = unixbuild::read_lines("test/resources/10000.txt");
  assert(lines.size() == 1);
  assert(lines[0] == std::string(10000, 'a').append("\n"));
}

int main(int argc, char* argv[]) {
  if (argc > 1) {
    std::cerr << argv[0] << ": error: test binary takes no arguments"
              << std::endl;
    exit(1);
  }

  try {
    test_trim_whitespace();
    test_split_string();
    test_read_lines();
  } catch (unixbuild::ExitException& e) {
    std::cerr << "Exception caught while running tests: " << e.message_
              << std::endl;
    return e.returncode_;
  }

  std::cout << "All tests passed." << std::endl;
  return 0;
}
