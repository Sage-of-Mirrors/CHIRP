#include <libchirp/libchirp.hpp>
#include <libchirp/ChirpBinary.hpp>

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

  chirp::Init();

  chirp::binary::ChirpBinary bin;
  bin.Import(R"(../../examples/test.chirb)");

  return 0;
}
