#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <random>

auto dek = "/key/dek";
auto dataSealPath = "/dev/attestation/keys/dek";

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
    attestKeyFs.open(dataSealPath, std::ofstream::out);
    for (auto i : data) {
        //std::cout << i << " ";
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
  dataFs.open("/data/trialdata.txt", std::ofstream::out);
  dataFs << "dsafkj;sdjfl;asjdfsadlfjsadljfl;asd" << std::endl;
  dataFs.close();
}

int main(void) {
    std::cout << "user provided dek...\n";
    // std::cout << "Read secret file: " << secret << std::endl;
    // std::ifstream ifs;
    // ifs.open(secret, std::ifstream::in);
    // char c = ifs.get();
    // while (ifs.good()) {
    //     std::cout << c << " ";
    //     c = ifs.get();
    // }
    // std::cout << std::endl;
    // ifs.close();

    generateDEK();

    // Toss data to data and see if its encrypted..
    generateData();

    return 0;
}
