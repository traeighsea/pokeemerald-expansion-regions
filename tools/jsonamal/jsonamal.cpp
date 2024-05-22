#include "jsonamal.h"

#include <string>
#include <fstream>
#include <ostream>
#include <nlohmann/json.hpp>

namespace {
    enum class Mode : unsigned {
        Simple,
        Encounters,

        Error
    };
    constexpr auto NumModes = static_cast<unsigned>(Mode::Error);
    const char* ModeStr[NumModes] = {"simple", "encounters"};
    Mode StrToMode(const char* str) {
        for(unsigned i = 0; i < NumModes - 1; i++) {
            if (str == ModeStr[i]) {
                return static_cast<Mode>(i);
            }
        }
        return Mode::Error;
    }

    constexpr auto UsageError{"USAGE: jsonamal <simple | encounters> <dest_amalgamated_json_file> ...\n"};
}

void ProcessSimple(nlohmann::ordered_json& combined_jsons, std::vector<const std::string>& jsonpaths) {
    for (const auto& jsonpath: jsonpaths) {
        std::ifstream individual_json_stream(jsonpath);
        nlohmann::ordered_json individual_json;
        individual_json_stream >> individual_json;

        combined_jsons.update(individual_json, true);
    }
}

void ProcessEncounters(nlohmann::ordered_json& combined_jsons, std::vector<const std::string>& jsonpaths) {
    // TODO(@traeighsea): need to do a special merge of some of the data based on the label :/
    for (const auto& jsonpath: jsonpaths) {
        std::ifstream individual_json_stream(jsonpath);
        nlohmann::ordered_json individual_json;
        individual_json_stream >> individual_json;

        combined_jsons.update(individual_json, true);
    }
}


/// This tool amalgamates a collection of json files into a single "master" json file. The first parameter
///   is the location of where the "master" json file will be output, from there every other json passed in
///   will be merged into this one file.
///
/// This should work generically for any list of json files, though is intended to compile encounter data.
int main(int argc, char *argv[])
{
    if (argc <= 3) {
        FATAL_ERROR(UsageError);
    }

    Mode mode(StrToMode(argv[1]));
    if (mode == Mode::Error) {
        FATAL_ERROR(UsageError);
    }

    std::string amal_filepath(argv[2]);

    // grab all the paths passed in
    std::vector<const std::string> jsonpaths;
    for (int i = 2; i < argc; i++) {
        jsonpaths.push_back(argv[i]);
    }

    // create an empty json to be filled with whatever mode is chosen
    nlohmann::ordered_json combined_jsons;
    switch(mode) {
        case Mode::Simple:
            ProcessSimple(combined_jsons, jsonpaths);
            break;
        case Mode::Encounters:
            ProcessEncounters(combined_jsons, jsonpaths);
            break;
        case Mode::Error:
            FATAL_ERROR(UsageError);
    }

    // output our final json
    std::ofstream json_out(amal_filepath);
    json_out << std::setw(2) << combined_jsons << std::endl;

    return 0;
}
