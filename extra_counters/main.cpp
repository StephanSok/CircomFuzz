#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstddef>

using json = nlohmann::json;


#ifdef __linux__
__attribute__((section("__libfuzzer_extra_counters")))
#endif
static uint8_t extra_counters[256];

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data) {
    extra_counters[Data[0]] = 1;
    std::cout << static_cast<int>(Data[0]) << std::endl;
    return 0;
}

