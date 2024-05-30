// json2bin.cpp
/// json2bin takes in a usage mode of either mapgrid, metatiles, or metatile_attributes and then a list of 
///     json files. The bin files will be output to the same paths as passed in except the extension will 
///     change. The mode to use are detailed below:
/// mapgrid:             used for data/layouts/*/border.json and data/layouts/*/map.json
/// metatiles:           used for data/tilesets/*/*/metatiles.json
/// metatile_attributes: used for data/tilesets/*/*/metatile_attributes.json 

#include "jsonbinconverter.h"

#include <string>
#include <vector>
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
    for(auto file: file_names) {
        // Deserialize json

        // Read json data

        // Serialize as bin
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
