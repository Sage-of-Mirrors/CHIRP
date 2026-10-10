#pragma once

#include <cstddef>
#include <string>

namespace chirp {
struct ChirpSourceLocation {
  std::string mFile;
  std::size_t mLine = 1;
  std::size_t mColumn = 1;
};

struct ChirpSourceSpan {
  ChirpSourceLocation mBegin;
  ChirpSourceLocation mEnd;
};
} // namespace chirp
