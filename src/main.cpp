#include <libchirp/ChirpBinary.hpp>
#include <libchirp/libchirp.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>

struct ChirpArgs {
  std::filesystem::path m_inFilepath;
  std::filesystem::path m_outFilepath;
  athena::Endian m_gxEndianness{athena::Endian::Big};
  chirp::binary::ChirpOptLevel m_optLevel{chirp::binary::ChirpOptLevel::o0};
};

void PrintTitle() {
  std::cout << "Character Intermediate Representation Pipeline (CHIRP) for "
               "GX/Flipper."
            << std::endl;
  std::cout << "Designed & written by Antidote, Gamma, et al." << std::endl;
  std::cout << std::endl;
}

void PrintUsage() {
  std::cout << "Usage:" << std::endl;
  std::cout << "  chirp.exe [input filepath] [output filepath] <arguments>" << std::endl;
  std::cout << "      OR" << std::endl;
  std::cout << "  chirp.exe [arguments]" << std::endl << std::endl;
}

void PrintArgument(std::string_view longName, std::string_view shortName,
                   std::string_view valueType, std::string_view desc) {
  std::cout << "  " << longName << "/" << shortName;
  if (!valueType.empty()) {
    std::cout << " [" << valueType << "]";
  }
  std::cout << std::endl << "      " << desc << std::endl;
}

void PrintArguments() {
  std::cout << "Arguments:" << std::endl;

  PrintArgument("--input", "-i", "string", "Filepath to *.chirm/*.chirb file to convert.");
  PrintArgument("--output", "-o", "string",
                "Filepath to where the converted *.chirb/*.chirm file will be saved.");
  PrintArgument("--endiangx", "-e", "string",
                "Endianness of raw GX data (vertices, textures, mesh primitives):\n\t  * big: use "
                "big endian for GX data.\n\t  * little: Use little endian for GX data.");
  PrintArgument(
      "--opt", "-p", "int",
      "Optimization level for *.chirm to *.chirb conversion:\n\t  * 0: No "
      "optimization.\n\t  * 1: Material and texture data de-duplicated.\n\t  * 2: Material, "
      "texture, and vertex data de-duplicated.");
  PrintArgument("--help", "-h", "", "Display usage information.");

  std::cout << std::endl;
}

bool ParseArgParameterValue(int& argParamIndex, int argCount, char* argList[],
                            std::string& outArgValue) {
  int argValueIndex = argParamIndex + 1;
  if (argValueIndex >= argCount || argList[argValueIndex][0] == '-') {
    std::cout << "Argument parameter \"" << argList[argParamIndex]
              << "\" is missing required value." << std::endl;
    return false;
  }

  size_t argValueLength = std::strlen(argList[argValueIndex]);
  if (!argValueLength) {
    std::cout << "Value for argument parameter \"" << argList[argParamIndex] << "\" is empty."
              << std::endl;
    return false;
  }

  outArgValue = std::string(argList[argValueIndex], argValueLength);
  argParamIndex++;

  return true;
}

bool ParseArguments(int argCount, char* args[], ChirpArgs& outChirpArgs) {
  for (int i = 1; i < argCount; i++) {
    // Input path parameter.
    if (std::strcmp(args[i], "-i") == 0 || std::strcmp(args[i], "--input") == 0) {
      std::string inPath;
      if (!ParseArgParameterValue(i, argCount, args, inPath)) {
        std::cout << "Failed to parse value for input argument." << std::endl;
        return false;
      }

      outChirpArgs.m_inFilepath = inPath;
    }
    // Output path parameter.
    else if (std::strcmp(args[i], "-o") == 0 || std::strcmp(args[i], "--output") == 0) {
      std::string outPath;
      if (!ParseArgParameterValue(i, argCount, args, outPath)) {
        std::cout << "Failed to parse value for output argument." << std::endl;
        return false;
      }

      outChirpArgs.m_outFilepath = outPath;
    }
    // GX endianness parameter.
    else if (std::strcmp(args[i], "-e") == 0 || std::strcmp(args[i], "--endiangx") == 0) {
      std::string endianArgValue;
      if (!ParseArgParameterValue(i, argCount, args, endianArgValue)) {
        std::cout << "Failed to parse value for GX endianness argument." << std::endl;
        return false;
      }

      std::transform(endianArgValue.begin(), endianArgValue.end(), endianArgValue.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      if (endianArgValue == "big") {
        outChirpArgs.m_gxEndianness = athena::Endian::Big;
      } else if (endianArgValue == "little") {
        outChirpArgs.m_gxEndianness = athena::Endian::Little;
      } else {
        std::cout << "Failed to parse GX endianness argument: invalid endian type \""
                  << endianArgValue << "\"." << std::endl;
        return false;
      }
    }
    // Optimization level parameter.
    else if (std::strcmp(args[i], "-p") == 0 || std::strcmp(args[i], "--opt") == 0) {
      std::string optArgValue;
      if (!ParseArgParameterValue(i, argCount, args, optArgValue)) {
        std::cout << "Failed to parse value for optimization argument." << std::endl;
        return false;
      }

      try {
        u32 optLevel = std::stoul(optArgValue);
        outChirpArgs.m_optLevel = static_cast< chirp::binary::ChirpOptLevel >(optLevel);
      } catch (std::invalid_argument& e) {
        std::cout << "Failed to parse value for optimization level. Error: " << e.what()
                  << std::endl;
        return false;
      } catch (std::out_of_range& e) {
        std::cout << "Failed to parse value for optimization level. Error: " << e.what()
                  << std::endl;
        return false;
      }
    }

    // Input path without parameter.
    else if (i == 1) {
      size_t inPathLength = std::strlen(args[i]);
      if (!inPathLength) {
        return false;
      }

      outChirpArgs.m_inFilepath = std::string(args[i], inPathLength);
    }
    // Output path without parameter.
    else if (i == 2) {
      size_t outPathLength = std::strlen(args[i]);
      if (!outPathLength) {
        return false;
      }

      outChirpArgs.m_outFilepath = std::string(args[i], outPathLength);
    }
  }

  return true;
}

bool ValidateArguments(const ChirpArgs& chirpArgs) {
  // Input file must exist.
  if (chirpArgs.m_inFilepath.empty() || !std::filesystem::exists(chirpArgs.m_inFilepath)) {
    std::cout << "Argument error: input filepath does not exist." << std::endl;
    return false;
  }
  // Input file must have an extension.
  if (!chirpArgs.m_inFilepath.has_extension()) {
    std::cout << "Argument error: input filepath does not have a file extension." << std::endl;
    return false;
  }
  // Input file must have either .chirb or .chirm as its extension.
  if (chirpArgs.m_inFilepath.extension() != ".chirb" &&
      chirpArgs.m_inFilepath.extension() != ".chirm") {
    std::cout << "Argument error: input file is not a valid filetype (*.chirb or *.chirm)."
              << std::endl;
    return false;
  }

  // Output file must have an extension.
  if (!chirpArgs.m_outFilepath.has_extension()) {
    std::cout << "Argument error: output filepath does not have a file extension." << std::endl;
    return false;
  }
  // Output file must have either .chirb or .chirm as its extension.
  if (chirpArgs.m_outFilepath.extension() != ".chirb" &&
      chirpArgs.m_outFilepath.extension() != ".chirm") {
    std::cout << "Argument error: output file is not a valid filetype (*.chirb or *.chirm)."
              << std::endl;
    return false;
  }

  // Input and output files cannot be the same type (.chirm to .chirm and .chirb to .chirb are not
  // supported).
  if (chirpArgs.m_inFilepath.extension() == chirpArgs.m_outFilepath.extension()) {
    std::cout << "Argument error: input and output files cannot be the same filetype." << std::endl;
    return false;
  }

  return true;
}

int main(int argc, char* argv[]) {
  PrintTitle();

  if (argc <= 1 || std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
    PrintUsage();
    PrintArguments();
    return 0;
  }

  ChirpArgs args;
  if (!ParseArguments(argc, argv, args)) {
    return -1;
  }
  if (!ValidateArguments(args)) {
    return -2;
  }

  chirp::Init();

  // chirp::binary::ChirpBinary bin;
  // bin.Import(R"(../../examples/test.chirb)");
  // bin.Export(R"(../../examples/exported.chirb)");

  return 0;
}
