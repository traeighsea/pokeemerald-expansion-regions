#pragma once

#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

/// Converts a number of any type, into the hex representation in a string
template <typename T>
std::string intToHexStd(T num)
{
    std::stringstream stream;
    stream << "0x" 
            << std::setfill ('0') << std::setw(sizeof(T)*2) 
            << std::hex << num;
    return stream.str();
}

/// Uses some C++ shenanigans to get the num of bits represented in the variable
template <typename T>
constexpr std::size_t sizeInBits(T var) noexcept
{
    return sizeof(typeid(T).name()) * CHAR_BIT;
}

/// Keep bitshifting to the left until we get to the first set bit
template <typename T>
constexpr unsigned firstBitOffset(T var) noexcept
{
    unsigned max_size = sizeInBits(var);
    unsigned first_bit = 0;
    while (first_bit < max_size && (var | 0x1) != 1) {
        var = var >> 1;
        first_bit++;
    }
    return first_bit;
}

/// Takes in a file path that will be read from as a binary file, then outputs this data into a collection
///     of bytes to be read by something else
inline std::vector<std::byte> readBinFileIntoBuffer(const std::filesystem::path& file_path) {
    std::ifstream bin_file(file_path, std::ios_base::binary);

    bin_file.seekg(0, std::ios_base::end);
    auto length = bin_file.tellg();
    bin_file.seekg(0, std::ios_base::beg);

    std::vector<std::byte> buffer(length);
    bin_file.read(reinterpret_cast<char*>(buffer.data()), length);

    // Wooo copy elision :sparkles:
    return buffer;
}

#endif // COMMONUTILS_H