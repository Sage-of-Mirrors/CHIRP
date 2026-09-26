#include "libchirp-binary/section/ChirpBinaryVat.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

chirp::binary::ChirpBinaryVat::ChirpBinaryVat() : ChirpBinarySection()
{
}

chirp::binary::ChirpBinaryVat::ChirpBinaryVat(u32 size, u32 elementCount)
    : ChirpBinarySection(ChirpSectionType::Vertex, size, elementCount)
{

}

bool chirp::binary::ChirpBinaryVat::Import(athena::io::IStreamReader& inStream)
{
	return true;
}

bool chirp::binary::ChirpBinaryVat::Export(athena::io::IStreamWriter &outStream)
{
	return true;
}
