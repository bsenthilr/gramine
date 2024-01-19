#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <random>
#include <filesystem>

const std::string dek = "/key/dek";
const std::string dataSealKeyPath = "/dev/attestation/keys/dek";
const std::string dataPath = "/data";

void dumpFile(const std::filesystem::path &path) {
    std::cout << "file: " << path << std::endl;
    std::ifstream ifs;
    ifs.open(path, std::ifstream::in);
    char c = ifs.get();
    while (ifs.good()) {
        std::cout << c << " ";
        c = ifs.get();
    }
    std::cout << std::endl;
    ifs.close();
}

// DEK sealed by mrsigner
void generateDEK() {
    std::random_device r;
    std::default_random_engine randomEngine(r());
    std::uniform_int_distribution<int> uniformDist(0, 127);
    std::array<uint8_t, 32> data;
    
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

    // ensure the attestation key path exists
    const std::filesystem::path  fsPath{dataSealKeyPath};
    //std::cout << fsPath << std::endl;
    std::cout << fsPath << " exist: " << std::filesystem::exists(fsPath) << std::endl;
    dumpFile(fsPath);
}

// create data
void generateData() {
  std::ofstream dataFs;
  const std::filesystem::path fsPath{dataPath + "/trialdata.txt"};
  dataFs.open(fsPath, std::ofstream::out);
  dataFs << "dsafkj;sdjfl;asjdfsadlfjsadljfl;asd" << std::endl;
  dataFs.close();
  dumpFile(fsPath);
}

int main(void) {
    std::cout << "user provided dek...\n";
    generateDEK();
    // Toss data to data and see if its encrypted..
    generateData();
    return 0;
}
