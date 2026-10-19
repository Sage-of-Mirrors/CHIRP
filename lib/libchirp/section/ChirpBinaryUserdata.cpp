#include "libchirp/section/ChirpBinaryUserdata.hpp"

#include <athena/IStreamWriter.hpp>

namespace {
struct UserDataInfo {
  /*0x00*/ u16 m_nameSize;
  /*0x02*/ u16 m_fieldCount;
  /*0x04*/ u16 m_childCount;

  /*0x06*/ u16 m_pad0;

  /*0x08*/ u32 m_nameOffset;
  /*0x0C*/ u32 m_firstFieldOffset;
};

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

void ReadUserDataInfo(chirp::binary::SectionReader& sectionReader, UserDataInfo& outData) {
  outData.m_nameSize = sectionReader.readUint16();
  outData.m_fieldCount = sectionReader.readUint16();
  outData.m_childCount = sectionReader.readUint16();

  sectionReader.seekAlign4();

  outData.m_nameOffset = sectionReader.readUint32();
  outData.m_firstFieldOffset = sectionReader.readUint32();
}

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
const ChirpKeyValue& ChirpKeyValue::GetValue(std::string_view key) {
  ChirpUserTuple* tuplePtr = std::get_if< ChirpUserTuple >(&m_value);
  if (!tuplePtr) {
    throw new std::invalid_argument("UserTuple value was empty.");
  }

  std::string keyAsStr(key);
  if (!tuplePtr->contains(keyAsStr)) {
    throw new std::invalid_argument("UserTuple key not found.");
  }

  return tuplePtr->at(keyAsStr);
}

const ChirpKeyValue& ChirpKeyValue::operator[](std::string_view key) { return GetValue(key); }

const ChirpKeyValue& ChirpUserData::GetField(std::string_view key) {
  if (!HasField(key)) {
    throw new std::invalid_argument("KeyValue key not found.");
  }

  return *m_fields[std::string(key)].get();
}

ChirpUserData* ChirpUserData::CreateChild(std::string_view name) {
  std::string nameAsStr(name);
  if (m_children.contains(nameAsStr)) {
    return nullptr;
  }

  m_children[nameAsStr] = std::make_unique< ChirpUserData >(name);
  return m_children[nameAsStr].get();
}

ChirpKeyValue* ChirpUserData::CreateField(std::string_view name) {
  if (HasField(name)) {
    return nullptr;
  }

  std::string nameAsStr(name);
  m_fields[nameAsStr] = std::make_unique< ChirpKeyValue >(name);
  return m_fields[nameAsStr].get();
}

const ChirpKeyValue& ChirpUserData::operator[](std::string_view key) { return GetField(key); }

ChirpBinaryUserdata::ChirpBinaryUserdata() : ChirpBinarySection() {}
ChirpBinaryUserdata::ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Userdata, size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryUserdata::ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Userdata, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinaryUserdata::Import(SectionReader& inStream) {
  ReadOffsets(inStream);

  for (u32 i = 0; i < m_elementCount; i++) {
    ReadUserData(inStream, nullptr);
  }

  return true;
}

bool ChirpBinaryUserdata::Export(athena::io::IStreamWriter& outStream) { return true; }

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
  case ChunkId::Tuples:
    chunkOffset = m_tuplesOffset;
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
  m_tuplesOffset = sectionReader.readUint32();

  sectionReader.seekAlign16();
}

void ChirpBinaryUserdata::ReadUserData(SectionReader& sectionReader,
                                       ChirpUserData* parentUserData) {
  UserDataInfo userDataInfo;
  ReadUserDataInfo(sectionReader, userDataInfo);

  s64 returnPos =
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), userDataInfo.m_nameOffset);
  std::string name = sectionReader.readString(userDataInfo.m_nameSize);
  ChirpUserData* newUserData =
      parentUserData ? parentUserData->CreateChild(name) : CreateUserData(name);

  SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::KeyValues),
              userDataInfo.m_firstFieldOffset);
  for (u32 f = 0; f < userDataInfo.m_fieldCount; f++) {
    ReadKeyValue(sectionReader, newUserData);
  }

  sectionReader.seek(returnPos, athena::SeekOrigin::Begin);

  for (u32 c = 0; c < userDataInfo.m_childCount; c++) {
    ReadUserData(sectionReader, newUserData);
  }
}

void ChirpBinaryUserdata::ReadKeyValue(SectionReader& sectionReader, ChirpUserData* ownerUserData) {
  KeyValueInfo keyValueInfo;
  ReadKeyValueInfo(sectionReader, keyValueInfo);

  s64 nextKeyPos =
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_keyOffset);
  ChirpKeyValue* newKeyValue =
      ownerUserData->CreateField(sectionReader.readString(keyValueInfo.m_keySize));

  switch (static_cast< ValueType >(keyValueInfo.m_type)) {
  case ValueType::Bool:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Bools), keyValueInfo.m_valueOffset);
    newKeyValue->SetValue< bool >(sectionReader.readBool());
    break;
  case ValueType::S64:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::S64s), keyValueInfo.m_valueOffset);
    newKeyValue->SetValue< s64 >(sectionReader.readInt64());
    break;
  case ValueType::Double:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Doubles), keyValueInfo.m_valueOffset);
    newKeyValue->SetValue< double >(sectionReader.readDouble());
    break;
  case ValueType::String:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_valueOffset);
    newKeyValue->SetValue< std::string >(
        sectionReader.readString(keyValueInfo.m_tupleCountStringSize));
    break;
  case ValueType::Tuple:
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::KeyValues), keyValueInfo.m_valueOffset);
    ReadTuples(sectionReader, newKeyValue, keyValueInfo.m_tupleCountStringSize);
    break;
  case ValueType::Null:
  default:
    break;
  }

  sectionReader.seek(nextKeyPos, athena::SeekOrigin::Begin);
}

void ChirpBinaryUserdata::ReadTuples(SectionReader& sectionReader, ChirpKeyValue* parentKeyValue,
                                     u16 count) {
  ChirpUserTuple tuples;

  for (u32 t = 0; t < count; t++) {
    KeyValueInfo keyValueInfo;
    ReadKeyValueInfo(sectionReader, keyValueInfo);

    s64 nextKeyPos =
        SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_keyOffset);
    std::string name = sectionReader.readString(keyValueInfo.m_keySize);
    tuples[name] = ChirpKeyValue(name);

    switch (static_cast< ValueType >(keyValueInfo.m_type)) {
    case ValueType::Bool:
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Bools), keyValueInfo.m_valueOffset);
      tuples[name].SetValue< bool >(sectionReader.readBool());
      break;
    case ValueType::S64:
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::S64s), keyValueInfo.m_valueOffset);
      tuples[name].SetValue< s64 >(sectionReader.readInt64());
      break;
    case ValueType::Double:
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Doubles), keyValueInfo.m_valueOffset);
      tuples[name].SetValue< double >(sectionReader.readDouble());
      break;
    case ValueType::String:
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Strings), keyValueInfo.m_valueOffset);
      tuples[name].SetValue< std::string >(
          sectionReader.readString(keyValueInfo.m_tupleCountStringSize));
      break;
    case ValueType::Tuple:
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::KeyValues), keyValueInfo.m_valueOffset);
      ReadTuples(sectionReader, &tuples[name], keyValueInfo.m_tupleCountStringSize);
      break;
    case ValueType::Null:
    default:
      break;
    }

    sectionReader.seek(nextKeyPos, athena::SeekOrigin::Begin);
  }

  parentKeyValue->SetValue(std::move(tuples));
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
