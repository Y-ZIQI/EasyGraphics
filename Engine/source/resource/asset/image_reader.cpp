#include "resource/asset/image_reader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <iostream>

#define MAKE_FOUR_CHARACTER_CODE(char1, char2, char3, char4)                                                                               \
    static_cast<uint32_t>(char1) | (static_cast<uint32_t>(char2) << 8) | (static_cast<uint32_t>(char3) << 16) |                            \
        (static_cast<uint32_t>(char4) << 24)

#define UNDO_FOUR_CHARACTER_CODE(x, name)                                                                                                  \
    std::string name;                                                                                                                      \
    name.reserve(4);                                                                                                                       \
    name.push_back(x >> 0);                                                                                                                \
    name.push_back(x >> 8);                                                                                                                \
    name.push_back(x >> 16);                                                                                                               \
    name.push_back(x >> 24);

namespace Eagle
{
	ImageRes ImageReader::readImage(const std::string& file, bool is_srgb, bool vflip) {
		ImageRes image{};

		std::string extname = file.substr(file.rfind('.', file.size()) + 1);
        if (extname == "png") {
			int iw, ih, n;
			stbi_set_flip_vertically_on_load(vflip);
			image.data = stbi_load(file.c_str(), &iw, &ih, &n, 4);
			if (image.data) {
                image.dataSize = iw * ih * 4;
				image.width = iw;
				image.height = ih;
				image.depth = 1;
				image.arraySize = 1;
				image.numMips = 1;
				image.type = EAGLE_IMAGE_TYPE_2D;
				image.supportsAlpha = true;
				image.format = (is_srgb) ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
			}
		}
		else if (extname == "dds") {
            if (readDDSImage(file, image, is_srgb) == ReadResult::Success) {

            }
            else {
                throw("DDS image read failed!");
            }
		}

		return image;
	}

    enum DDSMagicNumber {
        DDS = MAKE_FOUR_CHARACTER_CODE('D', 'D', 'S', ' '),

        DXT1 = MAKE_FOUR_CHARACTER_CODE('D', 'X', 'T', '1'), // BC1_UNORM
        DXT2 = MAKE_FOUR_CHARACTER_CODE('D', 'X', 'T', '2'), // BC2_UNORM
        DXT3 = MAKE_FOUR_CHARACTER_CODE('D', 'X', 'T', '3'), // BC2_UNORM
        DXT4 = MAKE_FOUR_CHARACTER_CODE('D', 'X', 'T', '4'), // BC3_UNORM
        DXT5 = MAKE_FOUR_CHARACTER_CODE('D', 'X', 'T', '5'), // BC3_UNORM
        ATI1 = MAKE_FOUR_CHARACTER_CODE('A', 'T', 'I', '1'), // BC4_UNORM
        BC4U = MAKE_FOUR_CHARACTER_CODE('B', 'C', '4', 'U'), // BC4_UNORM
        BC4S = MAKE_FOUR_CHARACTER_CODE('B', 'C', '4', 'S'), // BC4_SNORM
        ATI2 = MAKE_FOUR_CHARACTER_CODE('A', 'T', 'I', '2'), // BC5_UNORM
        BC5U = MAKE_FOUR_CHARACTER_CODE('B', 'C', '5', 'U'), // BC5_UNORM
        BC5S = MAKE_FOUR_CHARACTER_CODE('B', 'C', '5', 'S'), // BC5_SNORM
        RGBG = MAKE_FOUR_CHARACTER_CODE('R', 'G', 'B', 'G'), // R8G8_B8G8_UNORM
        GRBG = MAKE_FOUR_CHARACTER_CODE('G', 'R', 'B', 'G'), // G8R8_G8B8_UNORM
        YUY2 = MAKE_FOUR_CHARACTER_CODE('Y', 'U', 'Y', '2'), // YUY2
        UYVY = MAKE_FOUR_CHARACTER_CODE('U', 'Y', 'V', 'Y'),

        DX10 = MAKE_FOUR_CHARACTER_CODE('D', 'X', '1', '0'), // Any DXGI format
    };
    enum class DDSPixelFormatFlags : uint32_t {
        AlphaPixels = 0x1,
        Alpha = 0x2,
        FourCC = 0x4,
        PAL8 = 0x20,
        RGB = 0x40,
        RGBA = RGB | AlphaPixels,
        YUV = 0x200,
        Luminance = 0x20000,
        LuminanceA = Luminance | AlphaPixels,
    };
    inline DDSPixelFormatFlags operator&(DDSPixelFormatFlags a, DDSPixelFormatFlags b) {
        //return static_cast<DDSPixelFormatFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        return static_cast<DDSPixelFormatFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    enum DDSHeaderFlags : uint32_t {
        Caps = 0x1,
        Height = 0x2,
        Width = 0x4,
        Pitch = 0x8,
        PixelFormat = 0x1000,
        Texture = Caps | Height | Width | PixelFormat,
        Mipmap = 0x20000,
        Volume = 0x800000,
        LinearSize = 0x00080000,
    };
    struct DDSFilePixelFormat {
        uint32_t size;
        DDSPixelFormatFlags flags;
        uint32_t fourCC;
        uint32_t bitCount;
        uint32_t rBitMask;
        uint32_t gBitMask;
        uint32_t bBitMask;
        uint32_t aBitMask;
    };
    struct DDSFileHeader {
        uint32_t size;
        DDSHeaderFlags flags;
        uint32_t height;
        uint32_t width;
        uint32_t pitch;
        uint32_t depth;
        uint32_t mipmapCount;
        uint32_t reserved[11];
        DDSFilePixelFormat pixelFormat;
        uint32_t caps1;
        uint32_t caps2;
        uint32_t caps3;
        uint32_t caps4;
        uint32_t reserved2;

        bool hasAlphaFlag() const;
    };
    inline bool DDSFileHeader::hasAlphaFlag() const {
        return (pixelFormat.flags & DDSPixelFormatFlags::AlphaPixels) == DDSPixelFormatFlags::AlphaPixels;
    }
    struct Dx10Header {
        EAGLE_DXGI_FORMAT dxgiFormat;
        EAGLE_IMAGE_TYPE resourceDimension;
        uint32_t miscFlags;
        uint32_t arraySize;
        uint32_t miscFlags2;
    };
    enum Caps2Flags : uint32_t {
        Cubemap = 0x200,
    };

    template <typename T>
    inline constexpr const T& DDSmin(const T& a, const T& b) {
        return a > b ? b : a;
    }

    template <typename T>
    inline constexpr const T& DDSmax(const T& a, const T& b) {
        return a > b ? a : b;
    }

    template <typename T>
    inline constexpr bool DDShasBit(T value, T bit) {
        return (value & bit) == bit;
    }

    inline uint32_t getBlockSize(const EAGLE_DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 8;
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
            return 16;
        default:
            return 0;
        }
    }

    inline uint32_t getBitsPerPixel(const EAGLE_DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 128;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 96;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return 64;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return 32;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return 16;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
            return 8;

        case DXGI_FORMAT_R1_UNORM:
            return 1;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 4;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return 8;
        default:
            return 0;
        }
    }

	ReadResult ImageReader::readDDSImage(const std::string& file, ImageRes& image, bool is_srgb)
	{
		std::ifstream filestream(file, std::ios::binary | std::ios::in);
		if (!filestream.is_open())
			return ReadResult::Failure;

		filestream.seekg(0, std::ios::end);
		auto fileSize = filestream.tellg();
		image.data = new uint8_t[fileSize];
        image.dataSize = fileSize;
		filestream.seekg(0);
		filestream.read(reinterpret_cast<char*>(image.data), fileSize);

		auto* ptr = (uint8_t*)image.data;
		auto* ddsMagic = reinterpret_cast<uint32_t*>(ptr);
		ptr += sizeof(uint32_t);

		if (fileSize < sizeof(DDSFileHeader))
			return ReadResult::Failure;
		const auto* header = reinterpret_cast<const DDSFileHeader*>(ptr);
		ptr += sizeof(DDSFileHeader);

        // Validate header. A DWORD (magic number) containing the four character code value 'DDS '
        // (0x20534444).
        if (*ddsMagic != DDSMagicNumber::DDS)
            return ReadResult::Failure;

        const Dx10Header* additionalHeader = nullptr;
        if (DDShasBit(header->pixelFormat.flags, DDSPixelFormatFlags::FourCC) &&
            DDShasBit(header->pixelFormat.fourCC, static_cast<uint32_t>(DDSMagicNumber::DX10))) {
            additionalHeader = reinterpret_cast<const Dx10Header*>(ptr);
            ptr += sizeof(Dx10Header);

            // "If the DDS_PIXELFORMAT dwFlags is set to DDPF_FOURCC and a dwFourCC is
            // set to "DX10", then the total file size needs to be at least 148
            // bytes."
            if (fileSize < 148)
                return ReadResult::InvalidSize;

            image.arraySize = additionalHeader->arraySize;
            image.format = additionalHeader->dxgiFormat;
            image.type = additionalHeader->resourceDimension;
        }

        // Determine format information
        auto getFormatInfo = [](const DDSFileHeader* header, bool is_srgb) -> EAGLE_DXGI_FORMAT {
            auto& pf = header->pixelFormat;
            if ((pf.flags & DDSPixelFormatFlags::FourCC) == DDSPixelFormatFlags::FourCC) {
                switch (pf.fourCC) {
                    // clang-format off
                case DXT1:            return is_srgb? DXGI_FORMAT_BC1_UNORM_SRGB : DXGI_FORMAT_BC1_UNORM;
                case DXT2: case DXT3: return is_srgb? DXGI_FORMAT_BC2_UNORM_SRGB : DXGI_FORMAT_BC2_UNORM;
                case DXT4: case DXT5: return is_srgb? DXGI_FORMAT_BC3_UNORM_SRGB : DXGI_FORMAT_BC3_UNORM;
                case ATI1: case BC4U: return DXGI_FORMAT_BC4_UNORM;
                case BC4S:            return DXGI_FORMAT_BC4_SNORM;
                case ATI2: case BC5U: return DXGI_FORMAT_BC5_UNORM;
                case BC5S:            return DXGI_FORMAT_BC5_SNORM;
                default:              return DXGI_FORMAT_UNKNOWN;
                    // clang-format on
                }
            }

            // TODO: Write more of this bitmask stuff to determine formats.
            if ((pf.flags & DDSPixelFormatFlags::RGBA) == DDSPixelFormatFlags::RGBA) {
                switch (pf.bitCount) {
                case 32: {
                    if (pf.rBitMask == 0xFF && pf.gBitMask == 0xFF00 && pf.bBitMask == 0xFF0000 && pf.aBitMask == 0xFF000000)
                        return DXGI_FORMAT_R8G8B8A8_UNORM;
                    break;
                }
                }
            }

            return DXGI_FORMAT_UNKNOWN;
        };

        // We'll always trust the DX10 header.
        if (additionalHeader == nullptr) {
            image.format = getFormatInfo(header, is_srgb);
            if (image.format == DXGI_FORMAT_UNKNOWN)
                return ReadResult::UnsupportedFormat;
        }

        if (header->flags & DDSHeaderFlags::Volume || header->caps2 & Caps2Flags::Cubemap) {
            image.type = EAGLE_IMAGE_TYPE_3D;
        }
        else {
            image.type = header->height > 1 ? EAGLE_IMAGE_TYPE_2D : EAGLE_IMAGE_TYPE_1D;
        }

        const auto blockSizeBytes = getBlockSize(image.format);
        const auto bitsPerPixel = getBitsPerPixel(image.format);
        if (!blockSizeBytes && !bitsPerPixel)
            return ReadResult::UnsupportedFormat;

        auto computeMipmapSize = [bitsPerPixel, blockSizeBytes](EAGLE_DXGI_FORMAT dxgiFormat, uint32_t width, uint32_t height) -> uint32_t {
            // Instead of checking each format enum each we'll check for the range in
            // which the BCn compressed formats are.
            if ((dxgiFormat >= DXGI_FORMAT_BC1_TYPELESS && dxgiFormat <= DXGI_FORMAT_BC5_SNORM) ||
                (dxgiFormat >= DXGI_FORMAT_BC6H_TYPELESS && dxgiFormat <= DXGI_FORMAT_BC7_UNORM_SRGB)) {
                auto pitch = DDSmax(1u, (width + 3) / 4) * blockSizeBytes;
                return pitch * DDSmax(1u, (height + 3) / 4);
            }

            // These formats are special
            if (dxgiFormat == DXGI_FORMAT_R8G8_B8G8_UNORM || dxgiFormat == DXGI_FORMAT_G8R8_G8B8_UNORM) {
                return ((width + 1) >> 1) * 4 * height;
            }

            return DDSmax(1u, static_cast<uint32_t>((width * bitsPerPixel + 7) / 8)) * height;
        };

        // TODO: Support files with multiple resources.
        if (image.arraySize > 1)
            return ReadResult::UnsupportedFormat;

        // arraySize is populated with the additional DX10 header.
        uint64_t totalSize = 0;
        for (uint32_t i = 0; i < image.arraySize; ++i) {
            auto mipmaps = DDSmin(header->mipmapCount, 1u);
            auto width = header->width;
            auto height = header->height;

            /*for (uint32_t mip = 0; mip < mipmaps && width != 1; ++mip) {
                uint32_t size = computeMipmapSize(image.format, width, height);
                totalSize += static_cast<uint64_t>(size);

                image.mipmaps.emplace_back(ptr, size);
                ptr += size;

                width = DDSmax(width / 2, 1u);
                height = DDSmax(height / 2, 1u);
            }*/
        }

        image.numMips = header->mipmapCount;
        image.width = header->width;
        image.height = header->height;
        image.supportsAlpha = header->hasAlphaFlag();

        // Close the file and return success
        filestream.close();
        return ReadResult::Success;
	}
}