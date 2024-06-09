// json2bin.cpp
/// json2bin takes in a usage mode of either mapgrid, metatiles, or metatile_attributes and then a list of 
///     json files. The bin files will be output to the same paths as passed in except the extension will 
///     change. The mode to use are detailed below:
/// mapgrid:             used for data/layouts/*/border.json and data/layouts/*/map.json
/// metatiles:           used for data/tilesets/*/*/metatiles.json
/// metatile_attributes: used for data/tilesets/*/*/metatile_attributes.json 

#include "jsonbinconverter.h"
#include "commonutils.h"

#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

constexpr auto Usage{"USAGE: json2bin <mapgrid|metatiles|metatile_attributes> ...\n"};

enum class UsageMode : unsigned{
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

void convertMapgridJsonToBin(const std::vector<std::string>& file_names) {
    for(auto file_name: file_names) {
        // Deserialize json
        std::filesystem::path path{file_name};
        std::ifstream mapgrid_json_stream(path);
        nlohmann::ordered_json mapgrid_json = nlohmann::json::parse(mapgrid_json_stream);

        // Setup file to write to
        path.replace_extension("bin");
        std::ofstream mapgrid_bin{path, std::ios::binary | std::ios::out};

        // Get each of the masks from which the key should be the name of the mask that you'll find in the
        //  mapgrid data, and a string containing a hexidecimal number we can convert to an int to mask against 
        std::map<std::string, uint16_t> masks;
        for (const auto& [key, value]: mapgrid_json["mapgridMasks"].items()) {
            masks.insert(std::make_pair(key, hexToInt<uint16_t>(value.template get<std::string>())));
        }

        std::vector<nlohmann::ordered_json> mapgrid = mapgrid_json["mapgrid"];
        std::vector<uint16_t> mapgrid_tiles;

        // Iterate through each of the masks that each element of the mapgrid data should have, the data then
        //  gets bit shifted to the start of the mask, and then finally masked to make sure we don't overflow
        //  into another masked section
        for (auto tile_data: mapgrid) {
            uint16_t temp_tile{0};
            for (const auto& [key, value]: masks) {
                temp_tile |= static_cast<uint16_t>(tile_data[key].template get<uint16_t>() << firstBitOffset(value) & value);
            }
            mapgrid_tiles.push_back(temp_tile);
        }

        // Write our bin file
        for (const uint16_t& tile: mapgrid_tiles) {
            mapgrid_bin.write(reinterpret_cast<const char *>(&tile), sizeof(tile));
        }
    }
}

void convertMetatilesJsonToBin(const std::vector<std::string>& file_names) {
    for(auto file: file_names) {
        // Deserialize json

        // Read json data
        
        // Serialize as bin
    }
}

void convertMetatileAttributesJsonToBin(const std::vector<std::string>& file_names) {
    for(auto file: file_names) {
        // Deserialize json

        // Read json data
        
        // Serialize as bin
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

    std::vector<std::string> file_names;

    for (int i = 2; i < argc; i++) {
        file_names.emplace_back(argv[i]);
    }

    switch(mode) {
        case UsageMode::Mapgrid:
            convertMapgridJsonToBin(file_names);
            break;
        case UsageMode::Metatiles:
            convertMetatilesJsonToBin(file_names);
            break;
        case UsageMode::MetatileAttributes:
            convertMetatileAttributesJsonToBin(file_names);
            break;
        case UsageMode::Error:
            FATAL_ERROR(Usage);
            break;
    }

    return 0;
}
