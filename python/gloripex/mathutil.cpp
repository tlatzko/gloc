#define NO_IMPORT_ARRAY

#include "gloripex.hpp"
#include "mathutil.hpp"
#include "array.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>

void
gloripex::mathutil::goertzel(Array<double, 2> data,
                             Array<size_t, 1> indices,
                             Array<complex128, 2> result, int num_threads)
{
  // Number of input arrays
  const size_t Npx = data.shape(0);

  // Number of samples
  const size_t N = data.shape(1);

  // Frequency bins to calculate
  const size_t M = indices.shape(0);

  // Coefficients for each frequency
  std::vector<double> c(M);
  std::vector<double> s(M);

  for (size_t i = 0; i < M; ++ i) {
    const double omega = 2 * M_PI * double(indices(i)) / double(N);
    c[i] = cos(omega);
    s[i] = sin(omega);
  }

#ifdef _OPENMP
  omp_set_num_threads(num_threads);
#pragma omp parallel for
#endif
  for (size_t i = 0; i < Npx; ++ i) {
    for (size_t k = 0; k < M; ++ k) {
      result(i, k) = complex128(0, 0);
    }
    for (size_t j = 0; j < N; ++ j) {
      for (size_t k = 0; k < M; ++ k) {
        // use result array for calculation
        // sprev -> imag
        // sprev2 -> real
        const double s = data(i, j) + 2 * c[k] * result(i, k).real() - result(i, k).imag();
        result(i, k) = complex128(s, result(i, k).real());

      }
    }
    for (size_t k = 0; k < M; ++ k) {
      result(i, k) = complex128(c[k] * result(i, k).real() - result(i, k).imag(),
                                s[k] * result(i, k).real());
    }
  }
}

template<typename T>
void
gloripex::mathutil::linreg_equidist(const T *data_y, size_t n, double &slope, double &offset) {

  slope = 0;
  offset = 0;

  double sy = 0;
  const double sx = 0.5 * n * (n - 1);
  const double sxoss = sx / n;

  double st2 = 0;

  for (size_t i = 0; i < n; ++ i) {
    const double t = double(i) - sxoss;
    slope += t * data_y[i];
    st2 += t * t;
    sy += data_y[i];
  }

  slope /= st2;
  offset = (sy - slope * sx) / n;

}

template<typename T>
void
gloripex::mathutil::detrend_cub(Array<T, 2> data, Array<double, 2> dest,
                                int nthreads) {

#ifdef OPENMP
  omp_set_num_threads(nthreads);
#pragma omp parallel for
#endif
  for (size_t i = 0; i < data.shape(0); ++ i) {
    double slope = 0;
    double offset = 0;
    linreg_equidist(&data(i, 0), data.shape(1), slope, offset);
    for (size_t j = 0; j < data.shape(1); ++ j) {
      dest(i, j) = double(data(i, j)) - (slope * double(j) + offset);
    }
  }
}

template<typename T>
void
gloripex::mathutil::detrend_and_normalize_cub(Array<T, 2> data, Array<double, 2> dest,
                                              int nthreads) {
#ifdef OPENMP
  omp_set_num_threads(nthreads);
#pragma omp parallel for
#endif
  for (size_t i = 0; i < data.shape(0); ++ i) {
    double nrm = 0;
    double slope = 0;
    double offset = 0;
    linreg_equidist(&data(i, 0), data.shape(1), slope, offset);
    for (size_t j = 0; j < data.shape(1); ++ j) {
      dest(i, j) = double(data(i, j)) - (slope * double(j) + offset);
      nrm += dest(i, j) * dest(i, j);
    }
    nrm = sqrt(nrm);
    for (size_t j = 0; j < data.shape(1); ++ j) {
      dest(i, j) /= nrm;
    }
  }
}

template<typename T>
void
gloripex::mathutil::detrend_and_normalize_cub_stats(
    Array<T, 2> data, Array<double, 1> mean, Array<double, 1> std)    
{
    auto n = data.shape(0);
    auto m = data.shape(1);

    for (size_t j = 0; j < m; ++ j) {
        mean(j) = 0;
        std(j) = 0;
    }

    double cnt = 0;
    for (size_t i = 0; i < n; ++ i) {
        double slope = 0;
        double offset = 0;
        linreg_equidist(&data(i, 0), data.shape(1), slope, offset);

        double norm = 0;
        for (size_t j = 0; j < m; ++ j) {
            double temp = static_cast<double>(data(i, j)) - (slope * static_cast<double>(j) + offset);
            norm += temp * temp;
        }
        norm = sqrt(norm);
        if (norm > 0) {
            cnt += 1;
            double cnt_inv = 1. / n;
            for (size_t j = 0; j < m; ++ j) {
                double temp = (static_cast<double>(data(i, j)) - (slope * static_cast<double>(j) + offset)) / norm - mean(j);
                mean(j) += temp * cnt_inv;
                std(j) += temp * temp * (cnt - 1) * cnt_inv;
            }
                
        }
    }
    for (size_t j = 0; j < m; ++ j) {
        std(j) = sqrt(std(j) / cnt);
    }
}


template void gloripex::mathutil::detrend_and_normalize_cub_stats(
    Array<double, 2>, Array<double, 1>, Array<double, 1>);
template void gloripex::mathutil::detrend_and_normalize_cub_stats(
    Array<cub_t, 2>, Array<double, 1>, Array<double, 1>);
template void gloripex::mathutil::detrend_and_normalize_cub(Array<cub_t, 2>, Array<double, 2>, int);
template void gloripex::mathutil::detrend_cub(Array<cub_t, 2>, Array<double, 2>, int);
template void gloripex::mathutil::detrend_and_normalize_cub(Array<double, 2>, Array<double, 2>, int);
template void gloripex::mathutil::detrend_cub(Array<double, 2>, Array<double, 2>, int);
