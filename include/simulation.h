//https://codeforces.com/blog/entry/96344
//https://codeforces.com/blog/entry/66279
//https://faculty.cc.gatech.edu/~echow/ipcc/hpc-course/08_simd.pdf
#include <inttypes.h>

namespace simulation {
	typedef uint8_t io_vector_t __attribute__ ((vector_size (32)));
	
	union io_t {
		io_vector_t vec;
		uint8_t byte[32];
	};

}