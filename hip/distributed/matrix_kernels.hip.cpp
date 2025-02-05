// SPDX-FileCopyrightText: 2017-2023 The Ginkgo authors
//
// SPDX-License-Identifier: BSD-3-Clause

#include "core/distributed/matrix_kernels.hpp"


#include <thrust/binary_search.h>
#include <thrust/copy.h>
#include <thrust/distance.h>
#include <thrust/execution_policy.h>
#include <thrust/for_each.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/sequence.h>
#include <thrust/sort.h>
#include <thrust/transform_reduce.h>
#include <thrust/unique.h>


#include <ginkgo/core/base/exception_helpers.hpp>


#include "hip/base/thrust.hip.hpp"
#include "hip/components/atomic.hip.hpp"


namespace gko {
namespace kernels {
namespace hip {
namespace distributed_matrix {


#include "common/cuda_hip/distributed/matrix_kernels.hpp.inc"


}  // namespace distributed_matrix
}  // namespace hip
}  // namespace kernels
}  // namespace gko
