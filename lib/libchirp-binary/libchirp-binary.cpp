#include "libchirp-binary/libchirp-binary.hpp"

#include "libchirp-binary/ChirpBinarySectionManager.hpp"
#include "libchirp-binary/section/ChirpBinaryGeo.hpp"
#include "libchirp-binary/section/ChirpBinaryVat.hpp"

void chirp::binary::Init() {
  ChirpBinarySectionManager& sectionMgr = ChirpBinarySectionManager::Instance();

  sectionMgr.RegisterSection(FourCC("CVAT"), ChirpBinaryVat::CreateVATSection,
                             ChirpBinaryVat::CreateVATSectionWithOffset);
  sectionMgr.RegisterSection(chirp::FourCC("CGEO"), ChirpBinaryGeo::CreateGEOSection,
                             ChirpBinaryGeo::CreateGEOSectionWithOffset);
}
