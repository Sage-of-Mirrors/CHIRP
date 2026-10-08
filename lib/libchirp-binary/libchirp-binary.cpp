#include "libchirp-binary/libchirp-binary.hpp"

#include "libchirp-binary/ChirpBinarySectionManager.hpp"
#include "libchirp-binary/section/ChirpBinaryGeo.hpp"
#include "libchirp-binary/section/ChirpBinaryMat.hpp"
#include "libchirp-binary/section/ChirpBinaryScene.hpp"
#include "libchirp-binary/section/ChirpBinaryVat.hpp"

void chirp::binary::Init() {
  ChirpBinarySectionManager& sectionMgr = ChirpBinarySectionManager::Instance();

  sectionMgr.RegisterSection(FourCC("CVAT"), ChirpBinaryVat::CreateVATSection,
                             ChirpBinaryVat::CreateVATSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CGEO"), ChirpBinaryGeo::CreateGEOSection,
                             ChirpBinaryGeo::CreateGEOSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CSCN"), ChirpBinaryScene::CreateSCNSection,
                             ChirpBinaryScene::CreateSCNSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CMAT"), ChirpBinaryMaterials::CreateMATSection,
                             ChirpBinaryMaterials::CreateMATSectionWithOffset);
}
