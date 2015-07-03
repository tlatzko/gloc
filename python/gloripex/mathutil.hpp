#ifndef GLORIPEX_MATHUTIL_HPP
#define GLORIPEX_MATHUTIL_HPP

#include "gloripex.hpp"
#include "array.hpp"

namespace gloripex {
  namespace mathutil {
    void goertzel(Array<double, 2> data, Array<size_t, 1> indices,
                  Array<complex128, 2> result, int num_threads);

    /// Perform a simple linear fit, assuming no errors and x=[0, 1, ..., N-1].
    template<typename T>
    void linreg_equidist(const T *data_y, size_t n, double &slope, double &offset);

    /// Detrend 2D array, write to dest.
    template<typename T>
    void detrend_cub(Array<T, 2> data, Array<double, 2> dest, int nthreads=1);

    /// Detrend 2D array and normalize, write to dest.
    template<typename T>
    void detrend_and_normalize_cub(Array<T, 2> data, Array<double, 2> dest, int nthreads=1);

    /// Detrend 2D array and normalize, write mean and std over detector to mean and std.
    template<typename T>
    void detrend_and_normalize_cub_stats(Array<T, 2> data, Array<double, 1> mean, Array<double, 1> std);
  }
}

#endif
