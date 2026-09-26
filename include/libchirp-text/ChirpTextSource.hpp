#pragma once
#include <cstddef>
#include <string>


struct SourceLocation {
  std::string mFile;
  std::size_t mLine = 1;
  std::size_t mColumn = 1;
};

struct ChirpTextSourceSpan {
  SourceLocation mBegin;
  SourceLocation mEnd;
};
