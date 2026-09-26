#include "libchirp-binary/ChirpBinary.hpp"

#include "libchirp-binary/section/ChirpBinaryVat.hpp"

#include <athena/FileReader.hpp>
#include <athena/FileWriter.hpp>
#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace
{
    enum class ChirpBinaryFourCC : u32
    {
        ChirpBinaryFile = 0x43485242, // 'CHRB'
        VertexAttributes = 0x43564154 // 'CVAT'
    };
} // namespace

bool chirp::binary::ChirpBinary::Import(std::filesystem::path inFilepath)
{
    athena::io::FileReader inStream(inFilepath.generic_string());
    inStream.setEndian(athena::Endian::Little);

    return Import(inStream);
}

bool chirp::binary::ChirpBinary::Export(std::filesystem::path outFilepath) {
    athena::io::FileWriter outStream(outFilepath.generic_string());
    return Export(outStream);
}

bool chirp::binary::ChirpBinary::Import(athena::io::IStreamReader& inStream)
{
    ChirpBinaryFourCC headerFourcc = static_cast<ChirpBinaryFourCC>(inStream.readUint32());
    if (headerFourcc != ChirpBinaryFourCC::ChirpBinaryFile)
    {
        std::cerr << "Import stream for ChirpBinary was not a valid *.chirb file;"
                     " the file magic was incorrect." << std::endl;
        return false;
    }

    m_verMajor = inStream.readUint16();
    m_verMinor = inStream.readUint16();

    u32 m_totalSize = inStream.readUint32();
    u32 m_sectionCount = inStream.readUint32();

    // The header for a CHIRB file currently only uses 16 bytes,
    // but 32 total bytes are reserved for future use.
    inStream.seek(0x10);

    for (u32 i = 0; i < m_sectionCount; i++)
    {
        if (inStream.position() >= m_totalSize)
        {
            std::cerr << "ChirpBinary import stream has gone outside the bounds of"
                         " the file." << std::endl;
            return false;
        }

        u64 sectionStart = inStream.position();

        ChirpBinaryFourCC sectionFourcc = static_cast<ChirpBinaryFourCC>(inStream.readUint32());
        u32 sectionSize = inStream.readUint32();
        u32 sectionElementCount = inStream.readUint32();

        std::unique_ptr<ChirpBinarySection> newSection;
        ChirpSectionType newSectionType;

        switch (sectionFourcc)
        {
            case ChirpBinaryFourCC::VertexAttributes:
                newSectionType = ChirpSectionType::Vertex;
                newSection = std::make_unique<ChirpBinaryVat>(sectionSize,
                                                              sectionElementCount);
                break;
            default:
                std::cerr << "Encountered unknown ChirpSectionType while "
                             "importing from stream. Aborting import." << std::endl;
                return false;
        }

        newSection->Import(inStream);
        m_sections[newSectionType] = std::move(newSection);

        inStream.seek(sectionStart + sectionSize, athena::SeekOrigin::Begin);
    }

    return true;
}

bool chirp::binary::ChirpBinary::Export(athena::io::IStreamWriter& outStream)
{
    return true;
}

