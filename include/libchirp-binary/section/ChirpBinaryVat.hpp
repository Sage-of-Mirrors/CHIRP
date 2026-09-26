#pragma once

#include <libchirp-common/ChirpCommon.hpp>

#include "ChirpBinarySection.hpp"

namespace chirp::binary
{
	class ChirpBinaryVat : public ChirpBinarySection
	{
    public:
        ChirpBinaryVat();
        ChirpBinaryVat(u32 size, u32 elementCount);

		bool Import(athena::io::IStreamReader& inStream) override;
        bool Export(athena::io::IStreamWriter &outStream) override;

    private:
	};
} // namespace chirp::binary
