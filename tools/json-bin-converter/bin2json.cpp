// bin2json.cpp
/// bin2json takes in a usage mode of either mapgrid, metatiles, or metatile_attributes, then a version,
///     followed by a list of bin files. The json files will be output to the same paths as passed in except
///     the extension will change. The mode to use are detailed below:
/// mapgrid:             used for data/layouts/*/border.bin and data/layouts/*/map.bin
/// metatiles:           used for data/tilesets/*/*/metatiles.bin
/// metatile_attributes: used for data/tilesets/*/*/metatile_attributes.bin
///
/// The version will determine how to pack the data. For the version the options are rse, frlg, custom
///   For the custom "version" you will have to edit this code to add your own packer which you can find 
///   in the section down below. I'd suggest committing this to your project :)

#include "jsonbinconverter.h"

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

struct MapGridInfo {
   std::map<std::string, uint16_t> mapgrid_masks{};
};

struct MetatilesInfo {
   unsigned num_metatiles{512};
   unsigned num_tiles{512};
   unsigned num_pals{6};
   unsigned num_tiles_in_metatile{8};
   std::map<std::string, uint16_t> tiles_masks{};
};

/// The type should be uint16_t, or uint32_t depending on RSE or FRLG respectively for size 
///    of the attribute
template<class T>
struct MetatileAttributesInfo {
   unsigned num_metatiles{0};
   std::map<std::string, T> attribute_masks{};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///// IF USING CUSTOM, FILL IN YOUR DATA PACKERS HERE /////////////////////////////////////////////
const MapGridInfo CustomMapGridInfo = {
   .mapgrid_masks = {
      {"metatileId", 0x03FF},
      {"collision",  0x0C00},
      {"elevation",  0xF000}
   }
};
const MetatilesInfo CustomMetatilesInfo = {
   .num_metatiles = 512,
   .num_tiles = 512,
   .num_pals = 6,
   .num_tiles_in_metatile = 8,
   .tiles_masks{
      {"tileId",  0x03FF},
      {"xflip",   0x0400},
      {"yflip",   0x0800},
      {"palette", 0xF000}
   }
};
const MetatileAttributesInfo<uint16_t> CustomMetatileAttributesDataPacker = {
   .num_metatiles = 512,
   .attribute_masks = {
      {"behavior", 0x00FF},
      {"layer",    0xF000},
      {"unused",   0x0F00}
   }
};
///////////////////////////////////////////////////////////////////////////////////////////////////


constexpr auto Usage{"USAGE: bin2json <version: rse|frlg> <mode: mapgrid|metatiles|metatile_attributes> ...\n"};

enum class UsageMode : unsigned {
    Mapgrid,
    Metatiles,
    MetatileAttributes,

    Error
};
constexpr auto NumModes = static_cast<unsigned>(UsageMode::Error);
const char* UsageModeStr[NumModes] = {"mapgrid", "metatiles", "metatile_attributes"};
UsageMode StrToMode(const char* str) {
    for(unsigned i = 0; i < NumModes; i++) {
        if (strcmp(str, UsageModeStr[i]) == 0) {
            return static_cast<UsageMode>(i);
        }
    }
    return UsageMode::Error;
}

enum class Version : unsigned {
    RubySapphireEmerald,
    FireRedLeafGreen,
    Custom,

    Error
};
constexpr auto NumVersions = static_cast<unsigned>(Version::Error);
const char* VersionStr[NumVersions] = {"rse", "frlg", "custom"};
Version StrToVersion(const char* str) {
    for(unsigned i = 0; i < NumVersions; i++) {
        if (strcmp(str, VersionStr[i]) == 0) {
            return static_cast<Version>(i);
        }
    }
    return Version::Error;
}

const MapGridInfo MapGridInfoAll{
   .mapgrid_masks = {
      {"metatileId", 0x03FF},
      {"collision",  0x0C00},
      {"elevation",  0xF000}
   }
};

const MetatilesInfo MetatilesInfoRSE{
   .num_metatiles = 512,
   .num_tiles = 512,
   .num_pals = 6,
   .num_tiles_in_metatile = 8,
   .tiles_masks{
      {"tileId",  0x03FF},
      {"xflip",   0x0400},
      {"yflip",   0x0800},
      {"palette", 0xF000}
   }
};
const MetatilesInfo MetatilesInfoFRLG{
   .num_metatiles = 640,
   .num_tiles = 640,
   .num_pals = 7,
   .num_tiles_in_metatile = 8,
   .tiles_masks{
      {"tileId",  0x03FF},
      {"xflip",   0x0400},
      {"yflip",   0x0800},
      {"palette", 0xF000}
   }
};

const MetatileAttributesInfo<uint16_t> MetatileAttributesInfoRSE{
   .num_metatiles = 512,
   .attribute_masks = {
      {"behavior", 0x00FF},
      {"layer",    0xF000},
      {"unused",   0x0F00}
   }
};
const MetatileAttributesInfo<uint32_t> MetatileAttributesInfoFRLG{
   .num_metatiles = 640,
   .attribute_masks = {
      {"behavior",  0x000001FF},
      {"terrain",   0x00003E00},
      {"encounter", 0x07000000},
      {"layer",     0x60000000},
      {"unused",    0x98FFC000},
   }
};

void convertMapgridBinToJson(Version version, const std::vector<std::string>& file_names) {
    for(auto file: file_names) {
        // Deserialize bin

        // Process

        // Serialize as json
    }
}

void convertMetatilesBinToJson(Version version, const std::vector<std::string>& file_names) {
    for(auto file: file_names) {
        // Deserialize bin

        // Process
        
        // Serialize as json
    }
}

void convertMetatileAttributesBinToJson(Version version, const std::vector<std::string>& file_names) {
    for(auto file: file_names) {
        // Deserialize bin

        // Process
        
        // Serialize as json
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        FATAL_ERROR(Usage);
    }
    
    UsageMode mode = StrToMode(argv[1]);
    if (mode == UsageMode::Error) {
        FATAL_ERROR(Usage);
    }

    Version version = StrToVersion(argv[2]);
    if (version == Version::Error) {
        FATAL_ERROR(Usage);
    }

    std::vector<std::string> file_names;
    for (int i = 3; i < argc; i++) {
        file_names.emplace_back(argv[i]);
    }

    switch(mode) {
        case UsageMode::Mapgrid:
            convertMapgridBinToJson(version, file_names);
            break;
        case UsageMode::Metatiles:
            convertMetatilesBinToJson(version, file_names);
            break;
        case UsageMode::MetatileAttributes:
            convertMetatileAttributesBinToJson(version, file_names);
            break;
        case UsageMode::Error:
            FATAL_ERROR(Usage);
            break;
    }

    return 0;
}
