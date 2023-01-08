// Copyright (C) 2022 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.

#pragma once

#include "fast_matrix_market.hpp"

#include "write_body_threads.hpp"

namespace fast_matrix_market {
    /**
     * Get header field type based on the C++ type of the values to be written.
     */
    struct get_field_type {
        template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
        static field_type value() {
            return integer;
        }

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
        static field_type value() {
            return real;
        }

        template <typename T, typename std::enable_if<is_complex<T>::value, int>::type = 0>
        static field_type value() {
            return complex;
        }
    };

    /**
     * Write Matrix Market body sequentially.
     *
     * Chunks are computed and written sequentially.
     */
    template <typename FORMATTER>
    void write_body_sequential(std::ostream& os,
                               FORMATTER& formatter, const write_options& options = {}) {

        while (formatter.has_next()) {
            std::string chunk = formatter.next_chunk(options)();

            os.write(chunk.c_str(), (std::streamsize)chunk.size());
        }
    }

    /**
     * Write Matrix Market body.
     *
     * @tparam FORMATTER implementation class that writes chunks.
     */
    template <typename FORMATTER>
    void write_body(std::ostream& os,
                    FORMATTER& formatter, const write_options& options = {}) {
        if (options.parallel_ok && options.num_threads != 1) {
            write_body_threads(os, formatter, options);
            return;
        }
        write_body_sequential(os, formatter, options);
    }
}