#pragma once

#include <filesystem>

namespace athena::io {
class IStreamReader;
class IStreamWriter;
} // namespace athena::io

namespace chirp::text {
class ChirpText {
public:
  ChirpText() {}

  bool Import(const std::filesystem::path& inFilepath);
  bool Export(const std::filesystem::path& inFilepath);

  bool Import(athena::io::IStreamReader& inStream);
  bool Export(athena::io::IStreamWriter& outStream);

private:
};
} // namespace chirp::text
