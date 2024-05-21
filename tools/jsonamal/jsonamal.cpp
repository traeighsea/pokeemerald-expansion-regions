#include "jsonamal.h"

#include <string>
#include <fstream>
#include <ostream>
#include <nlohmann/json.hpp>

/// This tool amalgamates a collection of json files into a single "master" json file. The first parameter
///   is the location of where the "master" json file will be output, from there every other json passed in
///   will be merged into this one file.
///
/// This should work generically for any list of json files, though is intended to compile encounter data.
int main(int argc, char *argv[])
{
    if (argc <= 2) {
        FATAL_ERROR("USAGE: amaljsons <dest_amalgamated_json_file> ...\n");
    }

    std::string amal_filepath(argv[1]);
    std::vector<const std::string> jsonpaths;

    for (int i = 2; i < argc; i++) {
        jsonpaths.push_back(argv[i]);
    }

    nlohmann::json combined_jsons;

    for (const auto& jsonpath : jsonpaths) {
        std::ifstream individual_json_stream(jsonpath);
        nlohmann::json individual_json;
        individual_json_stream >> individual_json;

        combined_jsons.update(individual_json, true);
    }

    std::ofstream json_out(amal_filepath);
    json_out << std::setw(2) << combined_jsons << std::endl;

    return 0;
}
