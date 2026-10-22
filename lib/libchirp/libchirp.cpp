#include "libchirp/libchirp.hpp"
#include "libchirp/ChirpBinarySectionManager.hpp"
#include "libchirp/section/ChirpBinaryGeo.hpp"
#include "libchirp/section/ChirpBinaryJoints.hpp"
#include "libchirp/section/ChirpBinaryMat.hpp"
#include "libchirp/section/ChirpBinarySkin.hpp"
#include "libchirp/section/ChirpBinaryTextures.hpp"
#include "libchirp/section/ChirpBinaryUserdata.hpp"
#include "libchirp/section/ChirpBinaryVat.hpp"

void chirp::Init() {
  binary::ChirpBinarySectionManager& sectionMgr = binary::ChirpBinarySectionManager::Instance();

  sectionMgr.RegisterSection(FourCC("CVAT"), binary::ChirpBinaryVat::CreateVATSection,
                             binary::ChirpBinaryVat::CreateVATSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CGEO"), binary::ChirpBinaryGeo::CreateGEOSection,
                             binary::ChirpBinaryGeo::CreateGEOSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CJNT"), binary::ChirpBinaryJoints::CreateJNTSection,
                             binary::ChirpBinaryJoints::CreateJNTSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CMAT"), binary::ChirpBinaryMaterials::CreateMATSection,
                             binary::ChirpBinaryMaterials::CreateMATSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CUSR"), binary::ChirpBinaryUserdata::CreateUSRSection,
                             binary::ChirpBinaryUserdata::CreateUSRSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CTEX"), binary::ChirpBinaryTextures::CreateTEXSection,
                             binary::ChirpBinaryTextures::CreateTEXSectionWithOffset);
  sectionMgr.RegisterSection(FourCC("CSKN"), binary::ChirpBinarySkin::CreateSKNSection,
                             binary::ChirpBinarySkin::CreateSKNSectionWithOffset);
}
