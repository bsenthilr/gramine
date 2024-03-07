#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <system_error>
#include <unistd.h>
#include <cerrno>
#include <clocale>
#include <cmath>
#include <cstring>
#include <iostream>
#include <filesystem>
// #include <openssl/aes.h>

#ifndef SEMANTIC_VERSION
#define SEMANTIC_VERSION "0.0.0"
#endif

const std::string dek             = "/key/dek";
const std::string dataSealKeyPath = "/dev/attestation/keys/dek";
const std::string dataPath        = "/data";
const std::string bkup_dek        = "bkup_dek";

const std::string blah = "dogsfadsfsadfs";

namespace fs = std::filesystem;

// void removeunsealdir(const fs::path& path) {
//   try {
//   auto retval = std::filesystem::remove_all(path.string(), errc);
//   if ( retval == static_cast<std::uintmax_t>(-1)) {
//   }
//   } catch (std::filesystem::filesystem_error const& ex) {
//   }
// }

void unsealFilePath(const fs::path& path, bool isDirectory=false) {
  try {
    if (fs::exists(path)) {
      std::cout<<  path.string() << " is un sealed" << std::endl;
    } else {
      if (isDirectory) {
        fs::create_directories(path.string());
      }
    }
    return;
  } catch (std::filesystem::filesystem_error const& ex) {
    if (ex.code().value() == EACCES) {
     std::cout<<  path.string() << " is not unsealable, remove and recreate" << std::endl;
      std::error_code errc;
      if (isDirectory) {
        auto retval = std::filesystem::remove_all(path.string(), errc);
        if ( retval == static_cast<std::uintmax_t>(-1)) {
          auto errMsg = "failed to remove unsealable directory - " + path.string() + " " + errc.message();
          std::cout << errMsg << std::endl;
          throw std::runtime_error(errMsg);
        } else {
          std::cout << "number of files deleted - " << retval << std::endl;
        }
        fs::create_directories(path.string());
      } else {
        if (!std::filesystem::remove(path.string(), errc)) {
          auto errMsg = "failed to remove unsealable file - " + path.string() + " " + errc.message();
          std::cout << errMsg << std::endl;
          throw std::runtime_error(errMsg);
        }
      }
    } else {
      auto errMsg = "unknown error opening the file: " + path.string() + " " + ex.code().message();
      std::cout << errMsg << std::endl;
      throw std::runtime_error(errMsg);
    }
  }
}

int main(void) {
    std::cout << "test dump dir \n";
     std::error_code errc;
    if (!fs::exists("/bft")) {
      auto retval = std::filesystem::remove_all("/bft", errc);
      if ( retval != static_cast<std::uintmax_t>(-1)) {
        std::cout<< "failed to remove " << errc.message() << std::endl;
      }
      std::cout<< "deleted and recreated" << std::endl;
    }

    unsealFilePath("/bft", true);
    
    unsealFilePath("/bft/test_data");
    std::filesystem::copy("test_data", "/bft/test_data", std::filesystem::copy_options::overwrite_existing);
    unsealFilePath("/bft/test_data1");
    std::filesystem::copy("test_data", "/bft/test_data1", std::filesystem::copy_options::overwrite_existing);
    fs::create_directories("/bft/test1");
    unsealFilePath("/bft/test1/test_data1");
    std::filesystem::copy("test_data", "/bft/test1/test_data1", std::filesystem::copy_options::overwrite_existing);

    std::cout << "muck up measurement..";
    return 0;
}
