#include "libchirp/section/ChirpBinaryUserdata.hpp"

#include <athena/VectorWriter.hpp>

#include <functional>

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

struct KeyValueWriteVisitor {
  void operator()(bool val) {
    m_sectionWriter.writeUint32(static_cast< u32 >(m_boolsWriter.position()));
    m_boolsWriter.writeBool(val);
  }
  void operator()(s64 val) {
    m_sectionWriter.writeUint32(static_cast< u32 >(m_s64sWriter.position()));
    m_s64sWriter.writeInt64(val);
  }
  void operator()(double val) {
    m_sectionWriter.writeUint32(static_cast< u32 >(m_doublesWriter.position()));
    m_doublesWriter.writeDouble(val);
  }
  void operator()(const std::string& val) {
    m_sectionWriter.writeUint32(static_cast< u32 >(m_stringsWriter.position()));
    m_stringsWriter.writeString(val);
  }
  // Catches std::nullopt_t and std::shared_ptr<ChirpKeyValueTuple>. nullopt_t has no data to write
  // and tuples will be handled by the main export function.
  void operator()(auto val) { m_sectionWriter.writeUint32(0); }

  chirp::binary::SectionWriter& m_sectionWriter;
  athena::io::VectorWriter& m_boolsWriter;
  athena::io::VectorWriter& m_s64sWriter;
  athena::io::VectorWriter& m_doublesWriter;
  athena::io::VectorWriter& m_stringsWriter;
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
  m_userDataRoot.Set< SharedChirpKeyValueTuple >(std::make_shared< ChirpKeyValueTuple >());
}
ChirpBinaryUserdata::ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor), m_userDataRoot("root") {
  m_userDataRoot.Set< SharedChirpKeyValueTuple >(std::make_shared< ChirpKeyValueTuple >());
}
ChirpBinaryUserdata::ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor)
, m_userDataRoot("root") {
  m_userDataRoot.Set< SharedChirpKeyValueTuple >(std::make_shared< ChirpKeyValueTuple >());
}

bool ChirpBinaryUserdata::Import(SectionReader& inStream) {
  ReadOffsets(inStream);

  SharedChirpKeyValueTuple newRootMap = m_userDataRoot.Get< SharedChirpKeyValueTuple >().value();
  for (u32 i = 0; i < m_elementCount; i++) {
    ReadKeyValue(inStream, newRootMap);
  }

  return true;
}

bool ChirpBinaryUserdata::Export(SectionWriter& outStream) {
  // Write section header.
  outStream.writeUint32(FourCC("CUSR").toUint32());
  outStream.writeUint16(m_versionMajor);
  outStream.writeUint16(m_versionMinor);
  outStream.writeUint32(0); // Placeholder for section size.

  SharedChirpKeyValueTuple rootTuple = m_userDataRoot.Get< SharedChirpKeyValueTuple >().value();

  outStream.writeUint32(static_cast< u32 >(rootTuple->size()));
  outStream.writeUint32(0x30); // KeyValues are always at 0x30.
  outStream.writeUint32(0);    // Placeholder for bools offset.
  outStream.writeUint32(0);    // Placeholder for s64s offset.
  outStream.writeUint32(0);    // Placeholder for doubles offset.
  outStream.writeUint32(0);    // Placeholder for strings offset.
  outStream.writeAlign16();

  athena::io::VectorWriter boolsWriter;
  athena::io::VectorWriter s64sWriter;
  s64sWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter doublesWriter;
  doublesWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter stringsWriter;
  stringsWriter.setEndian(athena::Endian::Little);

  KeyValueWriteVisitor writeVisitor{outStream, boolsWriter, s64sWriter, doublesWriter,
                                    stringsWriter};

  // I wouldn't normally make this a lambda, but we need access to all of the value writers...
  // so a lambda is just the most convenient solution I guess.
  std::function< void(ChirpKeyValue&) > writeKeyValueFunc = [&](ChirpKeyValue& keyValue) {
    std::string key = keyValue.GetKey();
    ChirpKeyValueType valueType = keyValue.GetType();

    // Write KeyValue entry.
    outStream.writeUint16(static_cast< u16 >(key.size() + 1));
    outStream.writeUint16(keyValue.GetSize());
    outStream.writeUByte(static_cast< u8 >(valueType));
    outStream.writeAlign4();
    outStream.writeUint32(static_cast< u32 >(stringsWriter.position()));

    stringsWriter.writeString(key);

    // Use visitation to write the last u32 (offset to whatever value the KeyValue is holding).
    std::visit(writeVisitor, keyValue.GetVariant());

    // Recurse into this KeyValue's tuple if needed.
    if (valueType == ChirpKeyValueType::Tuple) {
      SharedChirpKeyValueTuple tuplePtr = keyValue.Get< SharedChirpKeyValueTuple >().value();
      if (!tuplePtr) {
        throw std::invalid_argument(
            "Encountered KeyValue tuple with invalid value while exporting ChirpBinaryUserData.");
      }

      for (auto& keyValue : *tuplePtr) {
        writeKeyValueFunc(keyValue.second);
      }
    }
  };

  for (auto& keyValue : *rootTuple) {
    writeKeyValueFunc(keyValue.second);
  }

  // Write bools table.
  outStream.writeAlign16();
  outStream.writeOffset(kBoolsOffsetPos);
  outStream.writeUBytes(boolsWriter.data().data(), boolsWriter.length());

  // Write s64s table.
  outStream.writeAlign16();
  outStream.writeOffset(kS64sOffsetPos);
  outStream.writeUBytes(s64sWriter.data().data(), s64sWriter.length());

  // Write doubles table.
  outStream.writeAlign16();
  outStream.writeOffset(kDoublesOffsetPos);
  outStream.writeUBytes(doublesWriter.data().data(), doublesWriter.length());

  // Write strings table.
  outStream.writeAlign16();
  outStream.writeOffset(kStringsOffsetPos);
  outStream.writeUBytes(stringsWriter.data().data(), stringsWriter.length());

  outStream.writeAlign16();
  outStream.writeSectionSize();

  return true;
}

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
                                       SharedChirpKeyValueTuple parentTuple) {
  KeyValueInfo keyValueInfo;
  ReadKeyValueInfo(sectionReader, keyValueInfo);

  s64 nextKeyPos =
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_keyOffset);

  std::string newKey = sectionReader.readString(keyValueInfo.m_keySize);
  if (parentTuple->contains(newKey)) {
    std::cout << "Encountered duplicate key \"" << newKey << "\" while parsing user data."
              << std::endl;
    return;
  }

  parentTuple->emplace(newKey, newKey);

  switch (static_cast< ChirpKeyValueType >(keyValueInfo.m_type)) {
  case ChirpKeyValueType::Bool:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Bools), keyValueInfo.m_valueOffset);
    (*parentTuple)[newKey].Set< bool >(sectionReader.readBool());
    break;
  case ChirpKeyValueType::S64:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::S64s), keyValueInfo.m_valueOffset);
    (*parentTuple)[newKey].Set< s64 >(sectionReader.readInt64());
    break;
  case ChirpKeyValueType::Double:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Doubles), keyValueInfo.m_valueOffset);
    (*parentTuple)[newKey].Set< double >(sectionReader.readDouble());
    break;
  case ChirpKeyValueType::String:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_valueOffset);
    (*parentTuple)[newKey].Set< std::string >(
        sectionReader.readString(keyValueInfo.m_tupleCountStringSize));
    break;
  case ChirpKeyValueType::Tuple: {
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::KeyValues), keyValueInfo.m_valueOffset);
    (*parentTuple)[newKey].Set< SharedChirpKeyValueTuple >(
        std::make_shared< ChirpKeyValueTuple >());

    SharedChirpKeyValueTuple newTuple =
        (*parentTuple)[newKey].Get< SharedChirpKeyValueTuple >().value();
    for (u16 i = 0; i < keyValueInfo.m_tupleCountStringSize; i++) {
      ReadKeyValue(sectionReader, newTuple);
    }
    break;
  }
  case ChirpKeyValueType::Null:
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
