#pragma once

#include <libchirp-common/ChirpCommon.hpp>

#include "libchirp-binary/section/ChirpBinarySection.hpp"

#include <unordered_map>

namespace athena::io {
class IStreamReader;
class IStreamWriter;
} // namespace athena::io

namespace chirp::binary {
using ChirpBinarySectionMap =
    std::unordered_map< ChirpSectionType, std::unique_ptr< ChirpBinarySection > >;

class ChirpBinaryImportOptions;
class ChirpBinaryExportOptions;

class ChirpBinary {
public:
  ChirpBinary() = default;

  // Attempts to import data into this ChirpBinary instance from the file
  // at the given path.
  // Returns true if the data was successfully imported, false if it was not.
  bool Import(const std::filesystem::path& inFilepath);
  // Attempts to export the data in this ChirpBinary instance to the file
  // at the given path.
  // Returns true if the data was successfully exported, false if it was not.
  bool Export(const std::filesystem::path& outFilepath);

  // Attempts to import data into this ChirpBinary instance from the given stream.
  // Returns true if the data was successfully imported, false if it was not.
  bool Import(athena::io::IStreamReader& inStream);
  // Attempts to export the data in this ChirpBinary instance to the given stream.
  // Returns true if the data was successfully exported, false if it was not.
  bool Export(athena::io::IStreamWriter& outStream);

  // Returns the major version of this ChirpBinary instance.
  u32 GetMajorVersion() const { return m_verMajor; }
  // Returns the minor version of this ChirpBinary instance.
  u32 GetMinorVersion() const { return m_verMinor; }
  // Returns a string representing the full versioning of this ChirpBinary instance,
  // in the format "major.minor".
  std::string GetVersionString() const { return std::format("{}.{}", m_verMajor, m_verMinor); }

  // Returns the number of sections that this ChirpBinary instance contains.
  u32 GetSectionCount() const { return m_sections.size(); }
  // Returns whether this ChirpBinary instance contains a section of the given type.
  bool HasSectionType(ChirpSectionType type) const { return m_sections.contains(type); }

  // Attempts to retrieve a section of the given type from this ChirpBinary instance.
  // Returns nullptr if an invalid type is given or if the instance does not contain
  // a section of that type.
  template < typename T,
             typename = typename std::enable_if< std::is_base_of_v< ChirpBinarySection, T > > >
  T* GetSection(ChirpSectionType type) {
    if (type == ChirpSectionType::Unknown) {
      std::cerr << "Attempted to get a section of an unknown type from "
                   "ChirpBinary."
                << std::endl;
      return nullptr;
    }
    if (type >= ChirpSectionType::SectionTypeMax) {
      std::cerr << "Attempted to get a section of an invalid type from "
                   "ChirpBinary."
                << std::endl;
      return nullptr;
    }

    if (auto itr = m_sections.find(type); itr != m_sections.end()) {
      return static_cast< T* >(itr->second.get());
    }

    std::cerr << "Attempted to get a section from ChirpBinary that it "
                 "doesn't have."
              << std::endl;
    return nullptr;
  }

  // Attempts to create a section of the given type for this ChirpBinary instance.
  // Returns nullptr if the creation fails due to an invalid type being passed,
  // or if this instance already contains a section of the given type.
  template < typename T,
             typename = typename std::enable_if< std::is_base_of_v< ChirpBinarySection, T > > >
  T* CreateSection(ChirpSectionType type) {
    if (type == ChirpSectionType::Unknown) {
      std::cerr << "Attempted to create a section of an unknown type for "
                   "ChirpBinary."
                << std::endl;
      return nullptr;
    }
    if (type >= ChirpSectionType::SectionTypeMax) {
      std::cerr << "Attempted to create a section of an invalid type for "
                   "ChirpBinary."
                << std::endl;
      return nullptr;
    }

    if (HasSectionType(type)) {
      std::cerr << "Attempted to create a section for "
                   "ChirpBinary of a type it already has."
                << std::endl;
      return nullptr;
    }

    m_sections[type] = std::make_unique< T >();
    return static_cast< T* >(m_sections[type].get());
  }

private:
  ChirpBinarySectionMap m_sections;

  u32 m_verMajor{0};
  u32 m_verMinor{0};
};
} // namespace chirp::binary
