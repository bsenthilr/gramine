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

void unsealFilePath(const fs::path& path, bool isDirectory) {
  try {
    if (fs::exists(path)) {
      std::cout<<  path.string() << " is un sealed" << std::endl;
      return;
    } else {
      fs::create_directories(path.string());
    }
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
      } else {
        if (!std::filesystem::remove(path.string(), errc)) {
          auto errMsg = "failed to remove unsealable file - " + path.string() + " " + errc.message();
          std::cout << errMsg << std::endl;
          throw std::runtime_error(errMsg);
        }
      }
      fs::create_directories(path.string());
    } else {
      auto errMsg = "unknown error opening the file: " + path.string() + " " + ex.code().message();
      std::cout << errMsg << std::endl;
      throw std::runtime_error(errMsg);
    }
  }
}

int main(void) {
    std::cout << "dsfasdfsdump dir ####@@@@####\n";
    if (fs::exists("/bft")) {
      std::cout<< "dir is un sealed" << std::endl;
    }
    if (fs::exists("/bft/test_data")) {
      std::cout<< "dir is un sealed" << std::endl;
    }
    unsealFilePath("/bft", true);
    
    std::filesystem::copy("test_data", "/bft/test_data", std::filesystem::copy_options::overwrite_existing);

    std::cout << "muck up measurement..";
    return 0;
}
