#include "libchirp-text/ChirpText.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace chirp::text {
bool ChirpText::Import(const std::filesystem::path& inFilepath) { return true; }

bool ChirpText::Export(const std::filesystem::path& inFilepath) { return true; }

bool ChirpText::Import(athena::io::IStreamReader& inStream) { return true; }

bool ChirpText::Export(athena::io::IStreamWriter& outStream) { return true; }
} // namespace chirp::text