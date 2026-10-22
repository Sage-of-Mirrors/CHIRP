#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

#include <unordered_map>

namespace athena::io {
class IStreamReader;
class IStreamWriter;
} // namespace athena::io

namespace chirp::binary {
using ChirpBinarySectionMap =
    std::unordered_map< FourCC, std::unique_ptr< ChirpBinarySection >, FourCCHash >;

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
  bool HasSectionType(FourCC type) const { return m_sections.contains(type); }

  // Attempts to retrieve a section of the given type from this ChirpBinary instance.
  // Returns nullptr if an invalid type is given or if the instance does not contain
  // a section of that type.
  template < typename T,
             typename = typename std::enable_if< std::is_base_of_v< ChirpBinarySection, T > > >
  T* GetSection(FourCC type) {
    if (auto itr = m_sections.find(type); itr != m_sections.end()) {
      return static_cast< T* >(itr->second.get());
    }

    std::cerr << "ChirpBinary has no section with FourCC \"" << type.toString() << "\"."
              << std::endl;
    return nullptr;
  }

  // Attempts to create a section of the given type for this ChirpBinary instance.
  // Returns nullptr if the creation fails due to an invalid type being passed,
  // or if this instance already contains a section of the given type.
  template < typename T,
             typename = typename std::enable_if< std::is_base_of_v< ChirpBinarySection, T > > >
  T* CreateSection(FourCC type) {
    if (HasSectionType(type)) {
      std::cerr << "ChirpBinary already has a section with FourCC \"" << type.toString() << "\"."
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
