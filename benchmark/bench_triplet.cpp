// Copyright (C) 2022 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.

#include <sstream>

#include "fmm_bench.hpp"


/**
 * Read triplets.
 */
static void triplet_read(benchmark::State& state) {
    // Generate a big matrix in-memory.
    auto triplet = construct_triplet<int64_t, double>(kInMemoryByteTargetRead);

    std::ostringstream oss;
    fast_matrix_market::matrix_market_header header(triplet.nrows, triplet.ncols);
    fast_matrix_market::write_matrix_market_triplet(oss, header, triplet.rows, triplet.cols, triplet.vals);
    std::string mm = oss.str();

    // read options
    fast_matrix_market::read_options options{};
    options.parallel_ok = true;
    options.num_threads = (int)state.range(0);

//    options.generalize_symmetry = false;
//    options.chunk_size_bytes = 2 * 2 << 20;

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        triplet.rows.resize(0);
        triplet.cols.resize(0);
        triplet.vals.resize(0);

        std::istringstream iss(mm);
        fast_matrix_market::read_matrix_market_triplet(iss, header, triplet.rows, triplet.cols, triplet.vals, options);
        num_bytes += mm.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(triplet_read)->Name("Triplet/read")->Apply(NumThreadsArgument);


/**
 * Write triplets.
 */
static void triplet_write(benchmark::State& state) {
    auto triplet = construct_triplet<int64_t, double>(kInMemoryByteTargetWrite);

    std::size_t num_bytes = 0;

    fast_matrix_market::write_options options;
    options.parallel_ok = true;
    options.num_threads = (int)state.range(0);

    for ([[maybe_unused]] auto _ : state) {

        std::ostringstream oss;

        fast_matrix_market::matrix_market_header header(triplet.nrows, triplet.ncols);
        fast_matrix_market::write_matrix_market_triplet(oss, header,
                                                        triplet.rows, triplet.cols, triplet.vals,
                                                        options);

        num_bytes += oss.str().size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(triplet_write)->Name("Triplet/write")->Apply(NumThreadsArgument);