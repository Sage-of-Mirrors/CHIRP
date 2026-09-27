#pragma once
#include "libchirp-common/ChirpCommon.hpp"

#include <functional>
#include <map>

namespace chirp::binary {
class ChirpBinarySection;
class ChirpBinarySectionManager {
public:
  using FSectionFactory = std::function<std::unique_ptr<ChirpBinarySection>(u32 size, u32 elementCount)>;
  ChirpBinarySectionManager(const ChirpBinarySectionManager&) = delete;
  ChirpBinarySectionManager& operator=(const ChirpBinarySectionManager&) = delete;
  ChirpBinarySectionManager(const ChirpBinarySectionManager&&) = delete;
  ChirpBinarySectionManager& operator=(const ChirpBinarySectionManager&&) = delete;

  static ChirpBinarySectionManager& Instance();

  bool RegisterSection(const FourCC& typeId, FSectionFactory factory);

  std::unique_ptr<ChirpBinarySection> NewSection(const FourCC& typeId, u32 size, u32 elementCount);

private:
  ChirpBinarySectionManager() = default;

  std::map<FourCC, FSectionFactory> mSectionFactories;
};
} // namespace chirp::binary