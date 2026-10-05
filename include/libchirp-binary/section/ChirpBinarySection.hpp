#pragma once

#include <libchirp-common/ChirpCommon.hpp>

namespace athena::io {
class IStreamReader;
class IStreamWriter;
} // namespace athena::io

namespace chirp::binary {
enum class ChirpSectionType : u8 {
  Unknown,

  Vertex,            // CVAT
  Skinning,          // CSKN
  Scenegraph,        // CSCN
  Geometry,          // CGEO
  Materials,         // CMAT
  Textures,          // CTEX
  Userdata,          // CUSR
  KeyAnimation,      // CKAN
  MaterialAnimation, // CMAN
  TextureAnimation,  // CTAN

  SectionTypeMax
};

class ChirpBinarySection {
public:
  virtual ~ChirpBinarySection() = default;
  ChirpBinarySection() = default;
  ChirpBinarySection(ChirpSectionType type, u32 size, u32 elementCount, u16 versionMajor,
                     u16 versionMinor)
  : m_type(type)
  , m_versionMajor(versionMajor)
  , m_versionMinor(versionMinor)
  , m_size(size)
  , m_elementCount(elementCount) {}
  ChirpBinarySection(ChirpSectionType type, u64 offset, u32 size, u32 elementCount,
                     u16 versionMajor, u16 versionMinor)
  : m_type(type)
  , m_versionMajor(versionMajor)
  , m_versionMinor(versionMinor)
  , m_offset(offset)
  , m_size(size)
  , m_elementCount(elementCount) {}

  virtual bool Import(athena::io::IStreamReader& inStream) = 0;
  virtual bool Export(athena::io::IStreamWriter& outStream) = 0;

  ChirpSectionType GetType() const { return m_type; }
  u16 GetMajorVersion() const { return m_versionMajor; }
  u16 GetMinorVersion() const { return m_versionMinor; }
  std::string GetVersionString() const {
    return std::format("{}.{}", m_versionMajor, m_versionMinor);
  }
  u32 GetSize() const { return m_size; }
  u32 GetElementCount() const { return m_elementCount; }

protected:
  ChirpSectionType m_type{ChirpSectionType::Unknown};

  u16 m_versionMajor{0};
  u16 m_versionMinor{0};

  u64 m_offset{0};
  u32 m_size{0};
  u32 m_elementCount{0};
};

} // namespace chirp::binary
