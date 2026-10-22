#include "libchirp/ChirpBinary.hpp"
#include "libchirp/ChirpBinarySectionManager.hpp"

#include <athena/FileReader.hpp>
#include <athena/FileWriter.hpp>
#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>
#include <athena/VectorWriter.hpp>

namespace chirp::binary {
bool ChirpBinary::Import(const std::filesystem::path& inFilepath) {
  athena::io::FileReader inStream(inFilepath.generic_string());
  inStream.setEndian(athena::Endian::Little);

  return Import(inStream);
}

bool ChirpBinary::Export(const std::filesystem::path& outFilepath) {
  athena::io::VectorWriter bufferStream;
  bufferStream.setEndian(athena::Endian::Little);
  bool result = Export(bufferStream);

  if (!result) {
    return false;
  }

  athena::io::FileWriter outStream(outFilepath.generic_string());
  outStream.setEndian(athena::Endian::Little);
  outStream.writeUBytes(bufferStream.data().data(), bufferStream.length());

  return true;
}

bool ChirpBinary::Import(athena::io::IStreamReader& inStream) {
  FourCC headerFourcc = inStream.readUint32();
  if (headerFourcc != "CHRB") {
    std::cerr << "Import stream for ChirpBinary was not a valid *.chirb file;"
                 " the file magic was incorrect."
              << std::endl;
    return false;
  }

  m_verMajor = inStream.readUint16();
  m_verMinor = inStream.readUint16();

  u32 totalSize = inStream.readUint32();
  u32 sectionCount = inStream.readUint32();

  // The header for a CHIRB file currently only uses 16 bytes,
  // but 32 total bytes are reserved for future use.
  inStream.seek(0x10);

  for (u32 i = 0; i < sectionCount; i++) {
    if (inStream.position() >= totalSize) {
      std::cerr << "ChirpBinary import stream has gone outside the bounds of"
                   " the file."
                << std::endl;
      return false;
    }

    u64 sectionStart = inStream.position();

    FourCC sectionFourcc = inStream.readUint32();
    u16 versionMajor = inStream.readUint16();
    u16 versionMinor = inStream.readUint16();
    u32 sectionSize = inStream.readUint32();
    u32 sectionElementCount = inStream.readUint32();

    std::unique_ptr< ChirpBinarySection > newSection =
        ChirpBinarySectionManager::Instance().NewSectionWithOffset(sectionFourcc, sectionStart,
                                                                   sectionSize, sectionElementCount,
                                                                   versionMajor, versionMinor);

    if (!newSection) {
      return false;
    }

    SectionReader sectionReader(inStream, sectionStart, sectionStart + sectionSize);
    newSection->Import(sectionReader);
    m_sections[sectionFourcc] = std::move(newSection);

    inStream.seek(static_cast< s64 >(sectionStart + sectionSize), athena::SeekOrigin::Begin);
  }

  return true;
}

bool ChirpBinary::Export(athena::io::IStreamWriter& outStream) {
  outStream.writeUint32(FourCC("CHRB").toUint32());
  outStream.writeUint16(m_verMajor);
  outStream.writeUint16(m_verMinor);
  outStream.writeUint32(0); // Placeholder for total file size.
  outStream.writeUint32(static_cast< u32 >(m_sections.size()));

  outStream.writeZeroTo(outStream.position() + 0x10);

  for (const auto& s : m_sections) {
    SectionWriter sectionWriter(outStream, outStream.position());
    s.second->Export(sectionWriter);
  }

  outStream.seek(0x8, athena::SeekOrigin::Begin);
  outStream.writeUint32(static_cast< u32 >(outStream.length()));
  outStream.seek(0, athena::SeekOrigin::End);

  return true;
}
} // namespace chirp::binary
