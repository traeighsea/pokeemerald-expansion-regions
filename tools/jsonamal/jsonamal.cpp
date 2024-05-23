#include "jsonamal.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ostream>
#include <nlohmann/json.hpp>

constexpr auto UsageError{"USAGE: jsonamal <simple | encounters> <dest_amalgamated_json_file> ...\n"};

enum class Mode : unsigned {
    Simple,
    Encounters,

    Error
};
constexpr auto NumModes = static_cast<unsigned>(Mode::Error);
const char* ModeStr[NumModes] = {"simple", "encounters"};
Mode StrToMode(const char* str) {
    for(unsigned i = 0; i < NumModes; i++) {
        if (strcmp(str, ModeStr[i]) == 0) {
            return static_cast<Mode>(i);
        }
    }
    return Mode::Error;
}

void ProcessSimple(nlohmann::ordered_json& combined_jsons, std::vector<const std::string>& jsonpaths) {
    for (const auto& jsonpath: jsonpaths) {
        std::ifstream individual_json_stream(jsonpath);
        nlohmann::ordered_json individual_json;
        individual_json_stream >> individual_json;

        combined_jsons.update(individual_json, true);
    }
}


void MergeEncounters(nlohmann::ordered_json& combined_jsons, nlohmann::ordered_json& individual_json) {
    for (auto& individual_encounter: individual_json["encounters"]) {
        std::string map = individual_encounter["map"].template get<std::string>();
        std::string base_label = individual_encounter["base_label"].template get<std::string>();
        bool found = false;
        for (auto &combined_encounter: combined_jsons["encounters"]) {
            // If the maps match, this is the same encounter, so we should merge the data accordingly
            if (combined_encounter["map"].template get<std::string>() == map && 
                combined_encounter["base_label"].template get<std::string>() == base_label) {
                // Merge the encounters
                combined_encounter.update(individual_encounter, true);
                
                found = true;
                break;
            }
        }
        if (!found) {
            combined_jsons["encounters"].push_back(individual_encounter);
        }
    }
}

void MergeWildEncounterGroups(nlohmann::ordered_json& combined_jsons, nlohmann::ordered_json& individual_json, bool override_keys) {
    for (auto& individual_encounter_group: individual_json["wild_encounter_groups"]) {
        std::string label = individual_encounter_group["label"].template get<std::string>();
        bool found = false;
        for (auto &combined_encounter_group: combined_jsons["wild_encounter_groups"]) {
            // If the labels match, this is the same wild encounter group, so we should merge the data accordingly
            if (combined_encounter_group["label"].template get<std::string>() == label) {
                if (override_keys) {
                    // Overwrite the encounters, completely replacing them
                    combined_encounter_group.update(individual_encounter_group, false);
                } else {
                    // Merge the encounters
                    MergeEncounters(combined_encounter_group, individual_encounter_group);

                    // Merge the rest of the data, but first lets remove the key that we already have set; encounters
                    individual_encounter_group.erase("encounters");
                    combined_encounter_group.update(individual_encounter_group, true);
                }
                
                found = true;
                break;
            }
        }
        if (!found) {
            combined_jsons["wild_encounter_groups"].push_back(individual_encounter_group);
        }
    }
}

void ProcessEncounters(nlohmann::ordered_json& combined_jsons, 
                       std::vector<const std::string>& jsonpaths,
                       bool override_keys = false) {
    // Iterate through our json paths to add each to the combined_jsons
    for (const auto& jsonpath: jsonpaths) {
        std::ifstream individual_json_stream(jsonpath);
        nlohmann::ordered_json individual_json;
        individual_json_stream >> individual_json;

        // If this is our first item, just copy that, otherwise merge based off of the wild_encounter_group label
        if (combined_jsons.empty()) {
            combined_jsons = individual_json;
        } else {
            MergeWildEncounterGroups(combined_jsons, individual_json, override_keys);
        }
    }
}

/// This tool amalgamates a collection of json files into a single "master" json file. The first parameter
///   is the mode, depending what you're combining there may need to be specialized logic such as for encounter
///   data, the second param is the location of where the "master" json file will be output, from there every 
///   other json passed in will be merged into this one file.
///
/// This should work generically for any list of json files, though is intended to compile encounter data which
///     does need special logic
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
    for (int i = 3; i < argc; i++) {
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
