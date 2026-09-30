#include "libchirp-binary/libchirp-binary.hpp"

#include "libchirp-binary/ChirpBinarySectionManager.hpp"
#include "libchirp-binary/section/ChirpBinaryVat.hpp"

void chirp::binary::Init() {
  chirp::binary::ChirpBinarySectionManager& sectionMgr =
      chirp::binary::ChirpBinarySectionManager::Instance();

  sectionMgr.RegisterSection(chirp::FourCC("CVAT"), chirp::binary::ChirpBinaryVat::CreateVATSection,
                             chirp::binary::ChirpBinaryVat::CreateVATSectionWithOffset);
}
