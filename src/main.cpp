#include "libchirp-binary/ChirpBinarySectionManager.hpp"
#include "libchirp-binary/section/ChirpBinaryVat.hpp"

#include <filesystem>
#include <iostream>

void PrintTitle() {
  std::cout << "Character Intermediate Representation Pipeline (CHIRP) for "
               "GX/Flipper."
            << std::endl;
  std::cout << "Designed & written by Antidote, Gamma, et al." << std::endl;
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  PrintTitle();
  chirp::binary::ChirpBinarySectionManager::Instance().RegisterSection(
    chirp::FourCC("CVAT"), chirp::binary::ChirpBinaryVat::CreateVATSection,
    chirp::binary::ChirpBinaryVat::CreateVATSectionWithOffset);
  const auto section = chirp::binary::ChirpBinarySectionManager::Instance().NewSection("CVAT", 4 * 128, 128);
  return 0;
}
