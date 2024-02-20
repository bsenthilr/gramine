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
// #include <openssl/aes.h>

#ifndef SEMANTIC_VERSION
#define SEMANTIC_VERSION "0.0.0"
#endif
const std::string dek             = "/key/dek";
const std::string dataSealKeyPath = "/dev/attestation/keys/dek";
const std::string dataPath        = "/data";
const std::string bkup_dek        = "bkup_dek";

void dumpFile(const std::filesystem::path& path) {
    std::cout << "file: " << path << std::endl;
    std::ifstream ifs;
    ifs.open(path, std::ifstream::in);
    char c = ifs.get();
    while (ifs.good()) {
        // xxd -ps test_dek
        // https://stackoverflow.com/questions/71346656/how-can-i-perform-a-hex-memory-dump-on-an-address-in-memory
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)(unsigned char)c;
        c = ifs.get();
    }
    std::cout << std::endl;
    ifs.close();
}

void setupDEK() {
    std::cout << "copying: " << dek << " over to : " << dataSealKeyPath << std::endl;
    std::filesystem::copy(dek, dataSealKeyPath, std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy(dek, bkup_dek, std::filesystem::copy_options::overwrite_existing);
}

// DEK sealed by mrsigner
void generateDEK() {
    // gramine-sgx-pf-crypt gen-key  -w /tmp/pfkey
    std::random_device r;
    std::default_random_engine randomEngine(r());
    std::uniform_int_distribution<int> uniformDist(0, 127);
    // Size of the AES-GCM encryption key
    std::array<uint8_t, 16> data;

    std::generate(data.begin(), data.end(),
                  [&uniformDist, &randomEngine]() { return (uint8_t)uniformDist(randomEngine); });

    std::cout << "generating secret file...\n";
    std::ofstream ofs, attestKeyFs;
    ofs.open(dek, std::ofstream::out);
    // attestKeyFs.open(dataSealKeyPath, std::ofstream::out);
    for (auto i : data) {
        std::cout << i << " ";
        ofs << i;
        // attestKeyFs << i;
    }
    // std::cout << std::endl;
    ofs.close();
    // attestKeyFs.close();
}

// create data
void generateDataFile(const std::filesystem::path& fsPath) {
    std::ofstream dataFs;
    dataFs.open(fsPath, std::ofstream::out | std::ofstream::app);
    dataFs << "353063396666383133333765333033386539373064396537633038353538383065633533326536623237"
              "366539303138656531356"
           << std::endl;
    dataFs.close();
    dumpFile(fsPath);
}

// bool trialEncrypt(key std::string_view) {
//     auto ctx = EVP_CIPHER_CTX_new();
//     auto std::string plaintext = "test try encryption";
//     auto ciphertext = std::make_unique<unsigned char[]>(plaintext.size() + 16);
//   int c_len{0};
//   int f_len{0};
//   if (EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL) != 1) {
//     throw std::exception("failed to init context");
//   }
//   if (EVP_EncryptInit_ex(ctx, NULL, NULL, key.data(), NULL) != 1) {
//     throw std::exception("failed to init context");
//   }
//   EVP_EncryptUpdate(ctx, ciphertext.get(), &c_len, plaintext.get(), input.size())
//   return true;
// }

int main(void) {
    try {
        if (!std::filesystem::exists(dek)) {
            std::cout << "DEK not found generating dek...\n";
            generateDEK();
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Cannot unseal the file => " << ex.what() << " Error code : " << ex.code() << std::endl;
        // If permission denied it means the old secret was sealed by another mrenclave so not
        // accessilbe any more so unlink it
        std::cout << "Eperm - " << EPERM << " Eaccess- " << EACCES << std::endl;
        if (ex.code().value() == EACCES) { //std::errc::operation_not_permitted) {
             std::error_code errc;
            if (!std::filesystem::remove(dek, errc)) {
                std::cout << "failed to remove: " << dek << " error code: " << errc << std::endl;
                auto stat = unlink(dek.c_str());
                std::cout << "unlink stat - " << stat << " err: " << errno << " str: " << std::strerror(errno) << std::endl;
                // dont catch die..
                exit(1);                
            }
            

            std::cout << "Removed old sealed data \n";
            if (!std::filesystem::exists(bkup_dek)) {
                std::cout << " no backup kek: " << bkup_dek << std::endl;
                // dont catch die..
                exit(1);
            } else {
                std::cout << "using backup kek: " << bkup_dek << std::endl;
                std::filesystem::copy(bkup_dek, dek,
                                      std::filesystem::copy_options::overwrite_existing);
            }
        } else {
            std::cout << " cannot recover the old sealed info " << std::endl;
            // dont catch die..
            exit(1);
        }
    }

    std::cout << "Semantic version: " << SEMANTIC_VERSION << std::endl;
    std::cout << "users provided dek...\n";
    setupDEK();
    // generateDEK();

    // ensure the attestation key path exists
    std::filesystem::path fsPath{dataSealKeyPath};
    std::cout << fsPath << " exist: " << std::filesystem::exists(fsPath) << std::endl;
    dumpFile(fsPath);

    // Toss data to data and see if its encrypted..
    fsPath = dataPath + "/trialdata.txt";
    generateDataFile(fsPath);
    std::filesystem::copy("test_data", "/data/", std::filesystem::copy_options::overwrite_existing);
    dumpFile(fsPath);
    return 0;
}
