#pragma once

#include "libchirp/ChirpCommon.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

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

class SectionReader : public athena::io::IStreamReader {
public:
  SectionReader(athena::io::IStreamReader& inStream, u64 start, u64 end)
  : m_inStream(inStream), m_sectionStart(start), m_sectionEnd(end) {
    setEndian(athena::Endian::Little);
  }

  u64 position() const override { return m_inStream.position(); }
  u64 length() const override { return m_sectionEnd - m_sectionStart; }
  void seek(s64 offset, athena::SeekOrigin origin = athena::SeekOrigin::Current) override {
    const s64 base = origin == athena::SeekOrigin::Begin ? 0
                     : origin == athena::SeekOrigin::End ? static_cast< s64 >(m_sectionEnd)
                                                         : static_cast< s64 >(position());
    if (offset < static_cast< s64 >(m_sectionStart) - base ||
        offset > static_cast< s64 >(m_sectionEnd) - base) {
      setError();
      return;
    }
    m_inStream.seek(base + offset, athena::SeekOrigin::Begin);
    if (m_inStream.hasError()) {
      setError();
    }
  }
  u64 readUBytesToBuf(void* buffer, u64 count) override {
    if (position() < m_sectionStart || position() > m_sectionEnd ||
        count > m_sectionEnd - position()) {
      setError();
      return 0;
    }
    const u64 read = m_inStream.readUBytesToBuf(buffer, count);
    if (read != count || m_inStream.hasError()) {
      setError();
    }
    return read;
  }

private:
  athena::io::IStreamReader& m_inStream;
  u64 m_sectionStart;
  u64 m_sectionEnd;
};

class SectionWriter : athena::io::IStreamWriter {
public:
  SectionWriter(athena::io::IStreamWriter& outStream, u64 start, u64 end)
  : m_outStream(outStream), m_sectionStart(start) {
    setEndian(athena::Endian::Little);
  }

  u64 position() const override { return m_outStream.position(); }
  u64 length() const override { return m_outStream.length(); }
  void seek(s64 offset, athena::SeekOrigin origin = athena::SeekOrigin::Current) override {
    const s64 curLength = length();
    const s64 base = origin == athena::SeekOrigin::Begin ? 0
                     : origin == athena::SeekOrigin::End ? static_cast< s64 >(curLength)
                                                         : static_cast< s64 >(position());
    if (offset < static_cast< s64 >(m_sectionStart) - base ||
        offset > static_cast< s64 >(curLength) - base) {
      setError();
      return;
    }
    m_outStream.seek(base + offset, athena::SeekOrigin::Begin);
    if (m_outStream.hasError()) {
      setError();
    }
  }

  void writeUBytes(const uint8_t* data, uint64_t length) override {
    m_outStream.writeUBytes(data, length);
  }

  void writeAlign4() { writeZeroTo(ROUND_UP_4(position())); }
  void writeAlign8() { writeZeroTo(ROUND_UP_8(position())); }
  void writeAlign16() { writeZeroTo(ROUND_UP_16(position())); }

  void writeOffset(s64 pos) {
    s64 curPos = position();
    seek(m_sectionStart + pos, athena::SeekOrigin::Begin);
    writeUint32(static_cast< u32 >(curPos));
  }

private:
  athena::io::IStreamWriter& m_outStream;
  u64 m_sectionStart;
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

  virtual bool Import(SectionReader& inStream) = 0;
  virtual bool Export(SectionWriter& outStream) = 0;

  ChirpSectionType GetType() const { return m_type; }
  u16 GetMajorVersion() const { return m_versionMajor; }
  u16 GetMinorVersion() const { return m_versionMinor; }
  std::string GetVersionString() const {
    return std::format("{}.{}", m_versionMajor, m_versionMinor);
  }
  u32 GetSize() const { return m_size; }
  u32 GetElementCount() const { return m_elementCount; }

protected:
  virtual s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) = 0;
  ChirpSectionType m_type{ChirpSectionType::Unknown};

  u16 m_versionMajor{0};
  u16 m_versionMinor{0};

  u64 m_offset{0};
  u32 m_size{0};
  u32 m_elementCount{0};
};

} // namespace chirp::binary
