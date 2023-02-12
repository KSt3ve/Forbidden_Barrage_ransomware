# ForbiddenBarrage

## Overview

ForbiddenBarrage is a ransomware program used to learn/understand encryption with the C++ cryptopp library.

## Version and Status
- Version: 1.0
- Update: 12/02/2023
- Status: stable

## Features
- Targeted operating system: Windows (an update will be done to add other OS)
- Encryption: Random AES encryption
- Discord: Send the decryption key to your personal Discord
- Speed: Encryption and fast folder browsing

## What is a Ransomware?

A ransomware is a type of malware that prevents legitimate users from accessing their device or data and asks for a payment in exchange for the stolen functionality. They have been used for mass extortion in various forms, but the most successful one seems to be encrypting ransomware: most of the user data are encrypted and the key can be obtained paying the attacker.

## Disclaimer

This ransomware must not be used to harm, threaten, or hurt other person's computer. Its purpose is only to share knowledge and awareness about malware, cryptography, operating systems, and programming. ForbiddenBarrage is an academic ransomware made for learning and awareness about security/cryptography.

## Installation

To use this program, we must use **Microsoft Visual Studio**.

**1. Add the libraries in the properties:**
  - Go to the setting of the project (click on the project name "Forbidden_Barrage_ransomware" and Alt + Enter)
  - Make sure you are doing your modification for the following configuration: Debug - x64
  - Go to C/C++ -> General -> Additional include directories
    - Add the following paths:
      - [Path_From_Your_Computer]\Forbidden_Barrage_ransomware\include
      - [Path_From_Your_Computer]\Forbidden_Barrage_ransomware\cryptopp
  - Go to Linker -> General -> Additional Includes Directory
    - [Path_From_Your_Computer]\Forbidden_Barrage_ransomware\cryptopp\x64\Output\Debug
  - Check that you have:
    - C/C++ -> Code generation -> Runtime library -> DLL multithread (/MD)
    - C/C++ -> Language -> C++ language standard -> ISO C++17 standard (/std:c++17)
    - Linker -> General -> Additional Depedencies -> cryptlib.lib
  - For help, check this video: https://www.youtube.com/watch?v=5XE4zEN-WKg

**2. Put your Discord Webhook in the code:**
  - Put your Discord Webhook in the `DiscordWebHook` definition:
    ```
    using namespace CryptoPP;
    namespace fs = std::filesystem;
    using json = nlohmann::json;
    
    ///Discord Webhook value
    #define DiscordWebHook "----- Put Your Discord Webhook Here ! -----"
    ```

**3. Create the executable:**
  - Build your project
  - Go to: [Path_From_Your_Computer]\Forbidden_Barrage_ransomware\x64\Debug
  - You should find the .exe file
  - If you want to move it and use it in another folder, make sure you move

###
project made by KSt3ve
