#include "separate_wild_encounters.h"

#include <filesystem>
#include <string>
#include <fstream>
#include <ostream>
#include <algorithm>
#include <map>
#include <nlohmann/json.hpp>

/// This tool can help in project migration from a single master list of wild encounters to
///   individual encounters stored in the map directory. It works by scanning each passed in 
///   map.json and if an id is found for an id in the encounter list, a wild_encounters.json 
///   is generated and output to that map dir and the entry is removed from the master list.
/// Once the script completes there may be some maps not found, the remaining entries in the json
///   then gets stored as wild_encounters_header.json.
int main(int argc, char *argv[])
{
    if (argc <= 2) {
        FATAL_ERROR("USAGE: separate_wild_encounters <all_wild_encounters_json_file> ...\n");
    }

    std::string all_wild_encounters_filepath(argv[1]);
    std::vector<const std::string> map_jsonpaths;

    for (int i = 2; i < argc; i++) {
        map_jsonpaths.push_back(argv[i]);
    }

    nlohmann::ordered_json all_wild_encounters;
    std::ifstream all_wild_encounters_json_stream(all_wild_encounters_filepath);
    all_wild_encounters_json_stream >> all_wild_encounters;

    // map<map id, map.json path>
    std::map<std::string, std::string> map_id_to_path_mapping;

    // build a mapping from id to map.json path
    for (const auto& map_jsonpath : map_jsonpaths) {
        std::ifstream map_json_stream(map_jsonpath);
        nlohmann::ordered_json map_json;
        map_json_stream >> map_json;
        map_id_to_path_mapping.insert(std::make_pair(map_json["id"], map_jsonpath));
    }

    // Iterate through all of the encounters for each group
    for (auto& map_group : all_wild_encounters["wild_encounter_groups"]) {
        if (map_group["for_maps"] == true) {
            nlohmann::ordered_json array_leftovers = nlohmann::ordered_json::array();
            for (auto encounter : map_group["encounters"]) {
                if (auto search = map_id_to_path_mapping.find(encounter["map"]); 
                        search != map_id_to_path_mapping.end()) {
                    nlohmann::ordered_json individual_json;
                    individual_json["wild_encounter_groups"] = nlohmann::ordered_json::array({{
                                                                {"label", map_group["label"]}, 
                                                                {"encounters", nlohmann::ordered_json::array({encounter})}
                                                            }});

                    // get dir path
                    std::filesystem::path dir_path(map_id_to_path_mapping[encounter["map"]]);
                    dir_path.replace_filename("wild_encounters");
                    dir_path.replace_extension("json");

                    // save json data to a file
                    std::ofstream individual_json_stream(dir_path);
                    individual_json_stream << std::setw(2) << individual_json << std::endl;
                } else {
                    array_leftovers.push_back(encounter);
                }
            }
            map_group["encounters"] = array_leftovers;
        }
    }

    // Save remaining json
    std::filesystem::path dir_path(all_wild_encounters_filepath);
    dir_path.replace_filename("wild_encounters_header");
    dir_path.replace_extension("json");

    std::ofstream json_out(dir_path);
    json_out << std::setw(2) << all_wild_encounters << std::endl;

    return 0;
}
