#include "libchirp/section/ChirpBinaryUserdata.hpp"

#include <athena/IStreamWriter.hpp>

namespace {
struct KeyValueInfo {
  /*0x00*/ u16 m_keySize;
  /*0x02*/ u16 m_tupleCountStringSize;
  /*0x04*/ u8 m_type;

  /*0x05*/ u8 m_pad0;
  /*0x06*/ u8 m_pad1;
  /*0x07*/ u8 m_pad2;

  /*0x08*/ u32 m_keyOffset;
  /*0x0C*/ u32 m_valueOffset;
};

void ReadKeyValueInfo(chirp::binary::SectionReader& sectionReader, KeyValueInfo& outData) {
  outData.m_keySize = sectionReader.readUint16();
  outData.m_tupleCountStringSize = sectionReader.readUint16();
  outData.m_type = sectionReader.readUByte();

  sectionReader.seekAlign4();

  outData.m_keyOffset = sectionReader.readUint32();
  outData.m_valueOffset = sectionReader.readUint32();
}
} // namespace

namespace chirp::binary {
ChirpBinaryUserdata::ChirpBinaryUserdata() : ChirpBinarySection(), m_userDataRoot("root") {
  m_userDataRoot.Set< ChirpKeyValueTuple* >(new ChirpKeyValueTuple());
}
ChirpBinaryUserdata::ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor), m_userDataRoot("root") {
  m_userDataRoot.Set< ChirpKeyValueTuple* >(new ChirpKeyValueTuple());
}
ChirpBinaryUserdata::ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor)
, m_userDataRoot("root") {
  m_userDataRoot.Set< ChirpKeyValueTuple* >(new ChirpKeyValueTuple());
}

bool ChirpBinaryUserdata::Import(SectionReader& inStream) {
  ReadOffsets(inStream);

  ChirpKeyValueTuple* newRootMap = m_userDataRoot.Get< ChirpKeyValueTuple* >().value();

  for (u32 i = 0; i < m_elementCount; i++) {
    ReadKeyValue(inStream, *newRootMap);
  }

  return true;
}

bool ChirpBinaryUserdata::Export(SectionWriter& outStream) { return true; }

s64 ChirpBinaryUserdata::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::KeyValues:
    chunkOffset = m_keyValuesOffset;
    break;
  case ChunkId::Bools:
    chunkOffset = m_boolsOffset;
    break;
  case ChunkId::S64s:
    chunkOffset = m_s64sOffset;
    break;
  case ChunkId::Doubles:
    chunkOffset = m_doublesOffset;
    break;
  case ChunkId::Strings:
    chunkOffset = m_stringsOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
}

void ChirpBinaryUserdata::ReadOffsets(SectionReader& sectionReader) {
  m_keyValuesOffset = sectionReader.readUint32();
  m_boolsOffset = sectionReader.readUint32();
  m_s64sOffset = sectionReader.readUint32();
  m_doublesOffset = sectionReader.readUint32();
  m_stringsOffset = sectionReader.readUint32();

  sectionReader.seekAlign16();
}

void ChirpBinaryUserdata::ReadKeyValue(SectionReader& sectionReader,
                                       ChirpKeyValueTuple& keyValueParent) {
  KeyValueInfo keyValueInfo;
  ReadKeyValueInfo(sectionReader, keyValueInfo);

  s64 nextKeyPos =
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_keyOffset);

  std::string newKey = sectionReader.readString(keyValueInfo.m_keySize);
  if (keyValueParent.contains(newKey)) {
    std::cout << "Encountered duplicate key \"" << newKey << "\" while parsing user data."
              << std::endl;
    return;
  }

  keyValueParent.emplace(newKey, newKey);

  switch (static_cast< ValueType >(keyValueInfo.m_type)) {
  case ValueType::Bool:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Bools), keyValueInfo.m_valueOffset);
    keyValueParent[newKey].Set< bool >(sectionReader.readBool());
    break;
  case ValueType::S64:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::S64s), keyValueInfo.m_valueOffset);
    keyValueParent[newKey].Set< s64 >(sectionReader.readInt64());
    break;
  case ValueType::Double:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Doubles), keyValueInfo.m_valueOffset);
    keyValueParent[newKey].Set< double >(sectionReader.readDouble());
    break;
  case ValueType::String:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_valueOffset);
    keyValueParent[newKey].Set< std::string >(
        sectionReader.readString(keyValueInfo.m_tupleCountStringSize));
    break;
  case ValueType::Tuple:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::KeyValues), keyValueInfo.m_valueOffset);
    keyValueParent[newKey].Set< ChirpKeyValueTuple* >(new ChirpKeyValueTuple());
    ReadKeyValue(sectionReader, *keyValueParent[newKey].Get< ChirpKeyValueTuple* >().value());
    break;
  case ValueType::Null:
  default:
    break;
  }

  sectionReader.seek(nextKeyPos, athena::SeekOrigin::Begin);
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryUserdata::CreateUSRSection(const u32 size,
                                                                            const u32 elementCount,
                                                                            u16 versionMajor,
                                                                            u16 versionMinor) {
  return std::make_unique< ChirpBinaryUserdata >(size, elementCount, versionMajor, versionMinor);
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryUserdata::CreateUSRSectionWithOffset(
    const u64 offset, const u32 size, const u32 elementCount, u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryUserdata >(offset, size, elementCount, versionMajor,
                                                 versionMinor);
}
} // namespace chirp::binary
