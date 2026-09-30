#include <libchirp-binary/libchirp-binary.hpp>
#include <libchirp-binary/ChirpBinary.hpp>

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

  chirp::binary::Init();

  chirp::binary::ChirpBinary bin;
  bin.Import(R"(C:\Git\CHIRP\examples\test.chirb)");

  return 0;
}
