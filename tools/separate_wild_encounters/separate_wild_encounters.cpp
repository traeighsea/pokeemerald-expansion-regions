/// This tool can help in project migration from a single master list of wild encounters to
///   individual encounters stored in the map directory. It works by scanning each passed in 
///   map.json and if an id is found for an id in the encounter list, a wild_encounters.json 
///   is generated and output to that map dir and the entry is removed from the master list.
/// Once the script completes there may be some maps not found, the remaining entries in the json
///   then gets stored as wild_encounters_common.json.
/// An optional param can be passed in to keep the ordering of the map indexes instead of arbitrary
///   order. I'd suggest only using this when compiling the default rom

#include "separate_wild_encounters.h"

#include <filesystem>
#include <string>
#include <fstream>
#include <ostream>
#include <map>
#include <nlohmann/json.hpp>

constexpr auto UsageError{"USAGE: separate_wild_encounters [optional: -retainOrder] <all_wild_encounters_json_file> ...\n"};

void SeparateIndividualMaps(nlohmann::ordered_json& all_wild_encounters,
                                  const std::map<std::string, std::string>& map_id_to_path_mapping) {
    // Iterate through all of the maps and grab anything that indexes on that map
    for (auto& map_id: map_id_to_path_mapping) {
        // Create an empty json object and add to it as we go
        nlohmann::ordered_json individual_json;
        individual_json["wild_encounter_groups"] = nlohmann::ordered_json::array();

        for (auto& map_group : all_wild_encounters["wild_encounter_groups"]) {
            // We only want to add a group if we actually find a map within one of those groups, so we
            //      store if we've added this group yet
            bool added_wild_encounter_group = false;

            // We should only have map keys that match if for_maps is set to true
            if (map_group["for_maps"] == true) {
                for (auto encounter : map_group["encounters"]) {
                    // Note that there can be multiple encounters that have the same map key, that differs on the base_label,
                    //      so we want to make a list of all of these
                    if (encounter["map"].template get<std::string>() == map_id.first) {
                        if (!added_wild_encounter_group) {
                            individual_json["wild_encounter_groups"].push_back({
                                                        {"label", map_group["label"]},
                                                        {"encounters", nlohmann::ordered_json::array()}
                                                    });
                        }

                        // Get the end of the list and append the current encounter to it
                        individual_json["wild_encounter_groups"].back()["encounters"].push_back(encounter);
                    }
                }
            }
        }

        // If we have data, serialize, otherwise don't bother
        if (!individual_json["wild_encounter_groups"].empty()) {
            // Set the path to the wild encounters
            std::filesystem::path dir_path(map_id.second);
            dir_path.replace_filename("wild_encounters");
            dir_path.replace_extension("json");

            // Serialize/save the json data to the map directory
            std::ofstream individual_json_stream(dir_path);
            individual_json_stream << std::setw(2) << individual_json << std::endl;
        }
    }
}

void OutputRemainingEncounterData(nlohmann::ordered_json& all_wild_encounters,
                                  const std::map<std::string, std::string>& map_id_to_path_mapping,
                                  const std::string& all_wild_encounters_filepath,
                                  bool retain_order) {
    // Iterate through all of the encounters and build a list of leftovers that couldn't find a home
    for (auto& map_group : all_wild_encounters["wild_encounter_groups"]) {
        if (map_group["for_maps"] == true) {
            nlohmann::ordered_json array_leftovers = nlohmann::ordered_json::array();
            for (auto encounter : map_group["encounters"]) {
                if (auto search = map_id_to_path_mapping.find(encounter["map"]); 
                        search != map_id_to_path_mapping.end()) {
                    if (retain_order) {
                        // We want to keep the position in the encounter array, so we keep an "empty" encounter object
                        nlohmann::ordered_json encounter_placeholder;
                        encounter_placeholder["map"] = encounter["map"];
                        encounter_placeholder["base_label"] = encounter["base_label"];
                        array_leftovers.push_back(encounter_placeholder);
                    }
                } else {
                    array_leftovers.push_back(encounter);
                }
            }
            map_group["encounters"] = array_leftovers;
        }
    }

    // Set the path to the wild encounters
    std::filesystem::path dir_path(all_wild_encounters_filepath);
    dir_path.replace_filename("wild_encounters_common");
    dir_path.replace_extension("json");

    // Serialize/save the json data to the map directory
    std::ofstream json_out(dir_path);
    json_out << std::setw(2) << all_wild_encounters << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc <= 2) {
        FATAL_ERROR(UsageError);
    }

    bool retain_order = std::string(argv[1]) == "-retainOrder";
    if (retain_order && argc == 3) {
        FATAL_ERROR(UsageError);
    }

    std::string all_wild_encounters_filepath(argv[1 + retain_order]);
    std::vector<const std::string> map_jsonpaths;

    // Offset the starting pos by one if we have a retain order flag
    for (int i = 2 + retain_order; i < argc; i++) {
        map_jsonpaths.push_back(argv[i]);
    }

    nlohmann::ordered_json all_wild_encounters;
    std::ifstream all_wild_encounters_json_stream(all_wild_encounters_filepath);
    all_wild_encounters_json_stream >> all_wild_encounters;

    std::map<std::string /* map id */, std::string /* map.json path */> map_id_to_path_mapping;

    // Build a mapping from id to map.json path
    for (const auto& map_jsonpath : map_jsonpaths) {
        std::ifstream map_json_stream(map_jsonpath);
        nlohmann::ordered_json map_json;
        map_json_stream >> map_json;
        map_id_to_path_mapping.insert(std::make_pair(map_json["id"], map_jsonpath));
    }

    SeparateIndividualMaps(all_wild_encounters, map_id_to_path_mapping);

    OutputRemainingEncounterData(all_wild_encounters, map_id_to_path_mapping, all_wild_encounters_filepath, retain_order);

    return 0;
}
