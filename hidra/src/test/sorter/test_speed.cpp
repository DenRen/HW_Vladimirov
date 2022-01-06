#include <random>
#include <iomanip>

#include "test_speed.hpp"
#include "../../bin/other_func.hpp"

namespace test_speed {

void
testSpeedSorter (unsigned pow2_begin, unsigned pow2_end) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t min_size_arr = 8 * 1 << pow2_begin;
    const size_t max_size_arr = 8 * 1 << pow2_end;
    const size_t repeat = 10;

    for (std::size_t size_arr = min_size_arr; size_arr <= max_size_arr; size_arr *= 2) {
        // Generate vectors
        std::vector <std::vector <int>> vecs;
        vecs.reserve (repeat);
        for (std::size_t i = 0; i < repeat; ++i) {
            vecs.push_back (getRandFillVector <int> (size_arr, mersenne));
        }
        auto copy_vecs = vecs;

        // Test GPU
        auto gpu_begin = std::chrono::high_resolution_clock::now ();
        decltype (sorter.sort (vecs[0])) gpu_kernel_time = 0;
        for (auto& vec : vecs) {
            gpu_kernel_time += sorter.sort (vec);
        }
        auto gpu_end = std::chrono::high_resolution_clock::now ();

        // Test CPU
        auto cpu_begin = std::chrono::high_resolution_clock::now ();
        for (auto& vec : copy_vecs) {
            std::sort (vec.begin (), vec.end ());
        }
        auto cpu_end = std::chrono::high_resolution_clock::now ();

        for (std::size_t i = 0; i < repeat; ++i) {
            if (copy_vecs[i] != vecs[i]) {
                std::cerr << "Error! copy_vecs_cpu[i] != vecs[i]" << std::endl;
            }
        }

        auto to_ns = [] (auto time) {
            return std::chrono::duration_cast <std::chrono::nanoseconds> (time);
        };

        auto time_coef = repeat * 1000;
        auto gpu_time = to_ns (gpu_end - gpu_begin) / time_coef;
        auto cpu_time = to_ns (cpu_end - cpu_begin) / time_coef;
        gpu_kernel_time /= time_coef;

        std::cout.setf (std::ios::fixed);
        std::cout.precision (2);

        std::cout << "Size array: " << std::setw (10) << size_arr << " ints"
                  << std::setw (14) << "C/G: " << 100.0 * cpu_time.count () / gpu_time.count () << "%" << std::endl
                  << tab << "time GPU:    " <<  std::setw (10) << gpu_time.count () << " mks" << std::endl
                  << tab << "time Kernel: " <<  std::setw (10) << gpu_kernel_time   << " mks"
                  << " K/G: " << 100.0 * gpu_kernel_time / gpu_time.count () << "%" << std::endl
                  << tab << "time CPU:    " <<  std::setw (10) << cpu_time.count () << " mks" << std::endl << std::endl;
    }
} // testSpeedSorter (unsigned pow2_begin, unsigned pow2_end)

} // namespace test_speed