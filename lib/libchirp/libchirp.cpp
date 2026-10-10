#include "libchirp/libchirp.hpp"
#include "libchirp/ChirpBinarySectionManager.hpp"
#include "libchirp/section/ChirpBinaryGeo.hpp"
#include "libchirp/section/ChirpBinaryMat.hpp"
#include "libchirp/section/ChirpBinaryScene.hpp"
#include "libchirp/section/ChirpBinaryVat.hpp"

void chirp::Init() {
  binary::ChirpBinarySectionManager& sectionMgr = binary::ChirpBinarySectionManager::Instance();

  sectionMgr.RegisterSection(FourCC("CVAT"), binary::ChirpBinaryVat::CreateVATSection,
                             binary::ChirpBinaryVat::CreateVATSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CGEO"), binary::ChirpBinaryGeo::CreateGEOSection,
                             binary::ChirpBinaryGeo::CreateGEOSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CSCN"), binary::ChirpBinaryScene::CreateSCNSection,
                             binary::ChirpBinaryScene::CreateSCNSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CMAT"), binary::ChirpBinaryMaterials::CreateMATSection,
                             binary::ChirpBinaryMaterials::CreateMATSectionWithOffset);
}
