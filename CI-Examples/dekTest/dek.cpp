#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <random>
#include <filesystem>

const std::string dek = "/key/dek";
const std::string dataSealKeyPath = "/dev/attestation/keys/dek";
const std::string dataPath = "/data";
const std::string test_dek = "test_dek";

void dumpFile(const std::filesystem::path &path) {
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
    std::cout << "copying over to : " << dek << std::endl;
    std::filesystem::copy(test_dek, dek, std::filesystem::copy_options::overwrite_existing);
    std::cout << "copying over to : " << dataSealKeyPath << std::endl;
    std::filesystem::copy(test_dek, dataSealKeyPath, std::filesystem::copy_options::overwrite_existing);
}

// DEK sealed by mrsigner
void generateDEK() {
    //gramine-sgx-pf-crypt gen-key  -w /tmp/pfkey
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
    attestKeyFs.open(dataSealKeyPath, std::ofstream::out);
    for (auto i : data) {
        std::cout << i << " ";
        ofs << i;
        attestKeyFs << i;
    }
    //std::cout << std::endl;
    ofs.close();
    attestKeyFs.close();
}

// create data
void generateData() {
  std::ofstream dataFs;
  const std::filesystem::path fsPath{dataPath + "/trialdata.txt"};
  dataFs.open(fsPath, std::ofstream::out | std::ofstream::app);
  dataFs << "353063396666383133333765333033386539373064396537633038353538383065633533326536623237366539303138656531356" << std::endl;
  dataFs.close();
  dumpFile(fsPath);
}

int main(void) {
    std::cout << "user provided dek...\n";
    setupDEK();
    // generateDEK();
    
    // ensure the attestation key path exists
    std::filesystem::path  fsPath{dataSealKeyPath};
    std::cout << fsPath << " exist: " << std::filesystem::exists(fsPath) << std::endl;
    dumpFile(fsPath);

    // Toss data to data and see if its encrypted..
    generateData();
    // fsPath = "test_data1";
    // dumpFile(fsPath);
    // std::filesystem::copy("test_data1", "hello", std::filesystem::copy_options::overwrite_existing);
    // std::filesystem::copy("test_data2", "/data/test_data2", std::filesystem::copy_options::overwrite_existing);
    return 0;
}
