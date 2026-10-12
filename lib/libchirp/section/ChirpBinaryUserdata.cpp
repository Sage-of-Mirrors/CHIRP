#include "libchirp/section/ChirpBinaryUserdata.hpp"

#include <athena/IStreamWriter.hpp>

namespace chirp::binary {
std::optional< const ChirpKeyValue > ChirpKeyValue::GetTupleValue(std::string_view key) const {
  const ChirpKeyValueTuple* tupleVal = std::get_if< ChirpKeyValueTuple >(&m_value);
  if (!tupleVal) {
    return std::nullopt;
  }

  auto findTupleKeyFunc = [key](const ChirpKeyValue& value) { return value.GetKey() == key; };

  auto findResult = std::find_if(tupleVal->begin(), tupleVal->end(), findTupleKeyFunc);
  return findResult != tupleVal->end() ? std::make_optional< ChirpKeyValue >(*findResult)
                                       : std::make_optional< ChirpKeyValue >();
}

std::optional< const ChirpKeyValue > ChirpKeyValue::operator[](std::string_view key) const {
  return GetTupleValue(key);
}

std::optional< const ChirpKeyValue > ChirpUserData::operator[](std::string_view key) const {
  return std::make_optional< ChirpKeyValue >();
}

ChirpBinaryUserdata::ChirpBinaryUserdata() : ChirpBinarySection() {}
ChirpBinaryUserdata::ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Userdata, size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryUserdata::ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Userdata, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinaryUserdata::Import(SectionReader& inStream) { return true; }
bool ChirpBinaryUserdata::Export(athena::io::IStreamWriter& outStream) { return true; }

s64 ChirpBinaryUserdata::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  return 0;
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryUserdata::CreateUSRSection(const u32 size,
                                                                            const u32 elementCount,
                                                                            u16 versionMajor,
                                                                            u16 versionMinor) {
  return std::make_unique< ChirpBinaryUserdata >(
      ChirpBinaryUserdata(size, elementCount, versionMajor, versionMinor));
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryUserdata::CreateUSRSectionWithOffset(
    const u64 offset, const u32 size, const u32 elementCount, u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryUserdata >(
      ChirpBinaryUserdata(offset, size, elementCount, versionMajor, versionMinor));
}
} // namespace chirp::binary
