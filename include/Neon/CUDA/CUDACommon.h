#pragma once

#include <stdio.h>

#include <algorithm>
#include <cstdlib>

#include <vector>

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include <thrust/iterator/constant_iterator.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/copy.h>
#include <thrust/device_vector.h>
#include <thrust/functional.h>
#include <thrust/generate.h>
#include <thrust/host_vector.h>
#include <thrust/sort.h>
#include <thrust/tuple.h>
#include <thrust/iterator/zip_iterator.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#undef _HAS_STD_BYTE
#define _HAS_STD_BYTE 0
#include <nvtx3/nvToolsExt.h>

#define IntNAN std::numeric_limits<int>::quiet_NaN()
#define RealNAN std::numeric_limits<float>::quiet_NaN()
#define IntInfinity std::numeric_limits<int>::max()

#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

#define CUDA_CHECK_ERROR(err) \
if (err != cudaSuccess && err != cudaErrorUnknown) { \
    fprintf(stderr, "CUDA error: %s at %s:%d\n", cudaGetErrorString(err), __FILE__, __LINE__); \
    exit(err); \
}

namespace NeonCUDA {
	float Trimax(float a, float b, float c);
	float Trimin(float a, float b, float c);
}