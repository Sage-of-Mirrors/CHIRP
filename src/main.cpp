#include "libchirp-binary/ChirpBinarySectionManager.hpp"
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
  chirp::binary::ChirpBinarySectionManager::Instance();
  PrintTitle();
  return 0;
}
