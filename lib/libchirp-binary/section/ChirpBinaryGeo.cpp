#include "libchirp-binary/section/ChirpBinaryGeo.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>
#include <cmath>

namespace chirp::binary {
ChirpMesh::ChirpMesh(std::string_view name) : m_name(name) {}

ChirpBinaryGeo::ChirpBinaryGeo() : ChirpBinarySection() {}

ChirpBinaryGeo::ChirpBinaryGeo(u32 size, u32 elementCount)
: ChirpBinarySection(ChirpSectionType::Geometry, size, elementCount) {}

ChirpBinaryGeo::ChirpBinaryGeo(u64 offset, u32 size, u32 elementCount)
: ChirpBinarySection(ChirpSectionType::Geometry, offset, size, elementCount) {}

bool ChirpBinaryGeo::Import(athena::io::IStreamReader& inStream) { return true; }
bool ChirpBinaryGeo::Export(athena::io::IStreamWriter& outStream) { return true; }

ChirpMesh* ChirpBinaryGeo::CreateMesh(std::string_view name) {
  auto newMesh = std::make_unique< ChirpMesh >(name);
  m_meshes.push_back(std::move(newMesh));

  return m_meshes.back().get();
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryGeo::CreateGEOSection(const u32 size,
                                                                       const u32 elementCount) {
  return std::make_unique< ChirpBinaryGeo >(ChirpBinaryGeo(size, elementCount));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryGeo::CreateGEOSectionWithOffset(const u64 offset, const u32 size,
                                           const u32 elementCount) {
  return std::make_unique< ChirpBinaryGeo >(ChirpBinaryGeo(offset, size, elementCount));
}
} // namespace chirp::binary
