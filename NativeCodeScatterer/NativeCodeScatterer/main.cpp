#include <fstream>
#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <Windows.h>
#include <filesystem>

#include "../Release/stub.h"
typedef unsigned char BYTE;

std::vector<BYTE> readFile(const char* filename)
{
    // open the file:
    std::streampos fileSize;
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    std::vector<BYTE> fileData(fileSize);
    file.read((char*)&fileData[0], fileSize);
    return fileData;
}

void createFile(const char* filename, std::vector<BYTE> content) {
    std::ofstream out(filename, std::fstream::app | std::fstream::binary);
    for (auto b : content)
        out << b;
}

int GetHash()
{

    // Get the string
    // to get its hash value
    std::string hashing1;
    HW_PROFILE_INFO hwProfileInfo;
    GetCurrentHwProfile(&hwProfileInfo);
    hashing1.append(hwProfileInfo.szHwProfileName);
    hashing1.append(hwProfileInfo.szHwProfileGuid);
    srand(time(0));
    hashing1.append(std::to_string(rand() + rand()));

    // Instantiation of Object
    std::hash<std::string> mystdhash;
    return mystdhash(hashing1);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Too few arguments provided!";
        return -1;
    }

    std::vector<BYTE> filecontent = readFile(argv[2]);
    int hash;
    std::cout << "Do you have a hash? y/n\n";
    char inp;
    std::cin >> inp;
    if (std::tolower(inp) == 'y') {
        std::cout << "Enter hash: ";
        std::cin >> hash;
    }
    else {
        hash = GetHash();
        std::cout << "[*] Generating Hash: " << hash << std::endl;
    }

    // seed our pseudo rng
    srand(hash);

    // mess up bytes in exe
    if (argv[1][0] == 'e') 
        std::cout << "[*] Scrambling bytes...\n";
    else
        std::cout << "[*] Unscrambling bytes...\n";

    for (auto& b : filecontent) {
        b = rand() % 256 - b;
    }

    // load entire filecontent into an array in stub.h
    std::cout << "[*] Creating stub.h...\n";
    std::ofstream stubout("stub.h");
    stubout << "#pragma once\nint hash = " << hash << ";\nunsigned char file[] = {";
    for (auto b : filecontent) {
        stubout << (int)b << ",";
    }
    stubout << "};";
    stubout.close();

    if (argv[1][0] == 'e')
        createFile("encoded.exe", filecontent);
    else
        createFile("decoded.exe", filecontent);

    system("pause");
}