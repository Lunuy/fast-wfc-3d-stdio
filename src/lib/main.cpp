#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include "time.h"

#include "fastwfc/overlapping_wfc.hpp"
#include "fastwfc/utils/array3D.hpp"
#include "fastwfc/wfc.hpp"
#include <unordered_set>

using namespace std;

/**
 * Get a random seed.
 * This function use random_device on linux, but use the C rand function for
 * other targets. This is because, for instance, windows don't implement
 * random_device non-deterministically.
 */
int get_random_seed() {
  #ifdef __linux__
    return random_device()();
  #else
    return rand();
  #endif
}


int main() {
    srand(time(NULL));

    int size_z, size_y, size_x;
    bool periodic_input_z, periodic_input_y, periodic_input_x;
    bool periodic_output_z, periodic_output_y, periodic_output_x;
    int output_size_z, output_size_y, output_size_x;
    int N;
    cin >> size_z >> size_y >> size_x;
    cin >> periodic_input_z >> periodic_input_y >> periodic_input_x;
    cin >> periodic_output_z >> periodic_output_y >> periodic_output_x;
    cin >> output_size_z >> output_size_y >> output_size_x;
    cin >> N;

    // fixed blocks
    int fixed_numbers;
    cin >> fixed_numbers;
    std::vector<std::tuple<int, int, int, int>> fixed_blocks;

    for (int i = 0; i < fixed_numbers; i++) {
        int z, y, x, block;
        cin >> z >> y >> x >> block;
        fixed_blocks.emplace_back(z, y, x, block);
    }

    // get input
    Array3D<int> source(size_z, size_y, size_x);
    for (int z = 0; z < size_z; z++) {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                int block;
                cin >> block;
                source.get(z, y, x) = block;
            }
        }
    }

    // WFC
    OverlappingWFCOptions options = {
        periodic_input_z, periodic_input_y, periodic_input_x,
        periodic_output_z, periodic_output_y, periodic_output_x,
        output_size_z, output_size_y, output_size_x,
        0, false, N };

    int seed = get_random_seed();
    OverlappingWFC<int> wfc(source, options, seed);
    for (auto& fixed_block : fixed_blocks) {
        int z = get<0>(fixed_block);
        int y = get<1>(fixed_block);
        int x = get<2>(fixed_block);
        int block = get<3>(fixed_block);
        wfc.set_block(block, z, y, x);
    }
    wfc.void_propagate();

    OverlappingWFC<int> temp_wfc = wfc;
    OverlappingWFC<int> prev_wfc = wfc;
    OverlappingWFC<int> pprev_wfc = wfc;
    OverlappingWFC<int> ppprev_wfc = wfc;

    while (true) {
        WFC::ObserveStatus status;
        std::optional<std::vector<std::tuple<unsigned, unsigned, unsigned, Array3D<int>>>> maybeChanges;
        std::tie(status, maybeChanges) = wfc.runOnce();
        if (status == WFC::ObserveStatus::success) {
            Array3D<int> result = *wfc.run();
            for (int z = 0; z < output_size_z; z++) {
                for (int y = 0; y < output_size_y; y++) {
                    for (int x = 0; x < output_size_x; x++) {
                        cout << "FINISHSET " << z << " " << y << " " << x << " " << result.get(z, y, x) << endl;
                    }
                }
            }
            cout << "FINISH" << endl;

            break;
        }
        else if (status == WFC::ObserveStatus::failure) {
            pprev_wfc = ppprev_wfc;
            prev_wfc = ppprev_wfc;
            temp_wfc = ppprev_wfc;
            wfc = ppprev_wfc;
            wfc.set_seed(get_random_seed());
            cout << "UNDO" << endl;
        }
        else {
            ppprev_wfc = pprev_wfc;
            pprev_wfc = prev_wfc;
            prev_wfc = temp_wfc;
            temp_wfc = wfc;

            cout << "ING " << (*maybeChanges).size() << endl;
            for (auto& change : *maybeChanges) {
                Array3D<int> pattern = get<3>(change);
                for (int z = 0; z < pattern.size_z; z++) {
                    for (int y = 0; y < pattern.size_y; y++) {
                        for (int x = 0; x < pattern.size_x; x++) {
                            int realZ = (get<0>(change) + z) % output_size_z;
                            int realY = (get<1>(change) + y) % output_size_y;
                            int realX = (get<2>(change) + x) % output_size_x;
                            cout << "SET " << realZ << " " << realY << " " << realX << " " << pattern.get(z, y, x) << endl;
                        }
                    }
                }
            }
        }
    }
}