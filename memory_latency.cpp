// OS 24 EX1

#include "memory_latency.h"
#include "measure.h"
#include <stdint.h>

#define GALOIS_POLYNOMIAL ((1ULL << 63) | (1ULL << 62) | (1ULL << 60) | (1ULL << 59))

#define MAX_SIZE_ARG 1
#define FACTOR_ARG 2
#define REPEAT_ARG 3

/**
 * Converts the struct timespec to time in nano-seconds.
 * @param t - the struct timespec to convert.
 * @return - the value of time in nano-seconds.
 */
uint64_t nanosectime(struct timespec t)
{
    return t.tv_sec * 1000000000LL + t.tv_nsec
}

/**
* Measures the average latency of accessing a given array in a sequential order.
* @param repeat - the number of times to repeat the measurement for and average on.
* @param arr - an allocated (not empty) array to preform measurement on.
* @param arr_size - the length of the array arr.
* @param zero - a variable containing zero in a way that the compiler doesn't "know" it in compilation time.
* @return struct measurement containing the measurement with the following fields:
*      double baseline - the average time (ns) taken to preform the measured operation without memory access.
*      double access_time - the average time (ns) taken to preform the measured operation with memory access.
*      uint64_t rnd - the variable used to randomly access the array, returned to prevent compiler optimizations.
*/
struct measurement measure_sequential_latency(uint64_t repeat, array_element_t* arr, uint64_t arr_size, uint64_t zero)
{
    repeat = arr_size > repeat ? arr_size:repeat; // Make sure repeat >= arr_size
    register uint64_t num=12345;

    // Baseline measurement:
    struct timespec t0;
    timespec_get(&t0, TIME_UTC);
    for (register uint64_t i = 0; i < repeat; i++)
    {
        for (register uint64_t index = 0; i < arr_size; i++)
            num ^= index & zero;
    }
    struct timespec t1;
    timespec_get(&t1, TIME_UTC);

    // Memory access measurement:
    struct timespec t2;
    timespec_get(&t2, TIME_UTC);
    for (register uint64_t i = 0; i < repeat; i++)
    {
        for (register uint64_t index = 0; i < arr_size; i++)
            num ^= arr[index] & zero;
    }
    struct timespec t3;
    timespec_get(&t3, TIME_UTC);

    // Calculate baseline and memory access times:
    double baseline_per_cycle=(double)(nanosectime(t1)- nanosectime(t0))/(repeat);
    double memory_per_cycle=(double)(nanosectime(t3)- nanosectime(t2))/(repeat);
    struct measurement result;

    result.baseline = baseline_per_cycle;
    result.access_time = memory_per_cycle;
    result.rnd = rnd;
    return result;

}

/**
 * Runs the logic of the memory_latency program. Measures the access latency for random and sequential memory access
 * patterns.
 * Usage: './memory_latency max_size factor repeat' where:
 *      - max_size - the maximum size in bytes of the array to measure access latency for.
 *      - factor - the factor in the geometric series representing the array sizes to check.
 *      - repeat - the number of times each measurement should be repeated for and averaged on.
 * The program will print output to stdout in the following format:
 *      mem_size_1,offset_1,offset_sequential_1
 *      mem_size_2,offset_2,offset_sequential_2
 *              ...
 *              ...
 *              ...
 */
int main(int argc, char* argv[])
{
    // zero==0, but the compiler doesn't know it. Use as the zero arg of measure_latency and measure_sequential_latency.
    struct timespec t_dummy;
    timespec_get(&t_dummy, TIME_UTC);
    const uint64_t zero = nanosectime(t_dummy)>1000000000ull?0:nanosectime(t_dummy);

    // Your code here
    const uint64_t MAX_SIZE = strtoull(argv[MAX_SIZE_ARG], NULL, 10);
    const double FACTOR = strtod(argv[FACTOR_ARG], NULL);
    const uint64_t REPEAT = strtoull(argv[REPEAT_ARG], NULL, 10);

    array_element_t *arr = calloc(MAX_SIZE, sizeof(array_element_t));

    for (register uint64_t mem_size = 100; mem_size < MAX_SIZE;
         mem_size = (uint64_t) (mem_size * FACTOR)) {
        struct measurement rnd_meas =
            measure_latency(REPEAT, arr, mem_size, zero);
        struct measurement seq_meas =
            measure_sequential_latency(REPEAT, arr, mem_size, zero);
        double rnd_offset = rnd_meas.access_time - rnd_meas.baseline;
        double seq_offset = seq_meas.access_time - seq_meas.baseline;

        printf("%" PRIu64 ",%f,%f\n", mem_size, rnd_offset, seq_offset);
    }
}
