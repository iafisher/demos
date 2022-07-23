#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

#include "unixbuild/common.h"

namespace unixbuild {

void trim_whitespace(std::string& s) {
  // Courtesy of https://stackoverflow.com/a/217605/
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));

  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

void split_string(const std::string& s, std::vector<std::string>& out,
                  char ch) {
  size_t outer = 0;
  size_t n = s.size();

  // Each iteration of this loop should consume at most one fragment.
  while (outer < n) {
    // Skip initial whitespace.
    size_t start = outer;
    while (start < n && s[start] == ch) {
      start++;
    }

    // Consume the fragment.
    size_t i = start;
    while (i < n && s[i] != ch) {
      i++;
    }

    // If we consumed anything, push it onto the vector.
    if (i > start) {
      out.push_back(s.substr(start, i - start));
    }

    outer = i;
  }
}

constexpr long PAGE_SIZE_DEFAULT = 4096;

std::vector<std::string> read_lines(const char* path) {
  // This function could be written with higher-level C/C++ file I/O functions,
  // but since the point of this project is to demonstrate the Unix environment,
  // we use the low-level Unix syscalls instead.
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    throw ExitException(std::string("could not open file: ").append(path), 2);
  }

  // `sysconf` will tell us the system's page size so we can efficiently read
  // from the file.
  long page_size = sysconf(_SC_PAGE_SIZE);
  if (page_size == -1) {
    page_size = PAGE_SIZE_DEFAULT;
  }

  // Normally modern C++ code should avoid using `new` explicitly, but the
  // `read` syscall expects a pointer and I don't know of a better way of doing
  // it than this.
  char* page = new char[page_size];

  std::vector<std::string> lines;
  std::string current_line;
  while (true) {
    ssize_t nread = read(fd, page, page_size);
    if (nread < 0) {
      throw ExitException(
          std::string("I/O error while reading file: ").append(path), 2);
    } else if (nread == 0) {
      break;
    } else {
      ssize_t start = 0;
      for (ssize_t i = 0; i < nread; i++) {
        if (page[i] == '\n') {
          current_line.append(page + start, (i - start) + 1);
          lines.push_back(current_line);
          current_line.clear();
          start = i + 1;
        }
      }

      // There may be leftover characters at the end of the block if a line of
      // the file straddles two blocks (or more!).
      if (start < nread) {
        current_line.append(page + start, nread - start);
      }
    }
  }

  // This case is encountered if the last line of the file is missing its
  // terminating newline.
  if (!current_line.empty()) {
    lines.push_back(current_line);
  }

  delete[] page;
  close(fd);
  return lines;
}

} // namespace unixbuild
