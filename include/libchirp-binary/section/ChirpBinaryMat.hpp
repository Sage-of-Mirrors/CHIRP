#pragma once

#include <libchirp-common/ChirpCommon.hpp>

#include "libchirp-binary/section/ChirpBinarySection.hpp"

#include <libchirp-common/gx/Material.hpp>

namespace chirp::binary {
class ChirpBinaryMaterials : public ChirpBinarySection {
public:
  ChirpBinaryMaterials();
  ChirpBinaryMaterials(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(athena::io::IStreamReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  static std::unique_ptr< ChirpBinarySection > CreateMATSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateMATSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

private:
};
} // namespace chirp::binary
