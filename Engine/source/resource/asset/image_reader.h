#pragma once

#include "resource/res_type/formats.h"

#include <vector>
#include <string>

namespace Eagle
{
    enum ReadResult {
        Success = 0,
        Failure = -1,
        UnsupportedFormat = -2,
        NoDx10Header = -3,
        InvalidSize = -4,
    };

	struct ImageRes {
        uint32_t numMips = 0;
        uint32_t arraySize = 1;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        EAGLE_IMAGE_TYPE type = EAGLE_IMAGE_TYPE_UNKNOWM;
        bool supportsAlpha = false;
        EAGLE_DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        void* data = nullptr;
        uint32_t dataSize = 0;
	};

	class ImageReader
	{
	public:
		ImageReader() {};
		~ImageReader() {};

        ImageRes readImage(const std::string& file, bool is_srgb = true, bool vflip = true);

    private:
        ReadResult readDDSImage(const std::string& file, ImageRes& image, bool is_srgb);
	};
}