#define NO_IMPORT_ARRAY

#include "gloripex.hpp"
#include "calibration.hpp"
#include "array.hpp"

#include <complex>
#include <numpy/arrayobject.h>
#include <vector>
#include <utility>
#include <iostream>

#ifdef WITH_SSE
#include <ammintrin.h>
#endif

namespace gloripex {
    namespace calibration {
        static void setup_interpolation(
                const Array<float, 1> &x_ar,
                const Array<float, 1> &xp_ar,
                std::vector<std::pair<size_t, size_t> > &positions)
        {
            positions.resize(x_ar.size());

            size_t i = 0;
            for (; x_ar(i) < xp_ar(0); ++ i) {
                gassert(i < x_ar.size(), i << " " << x_ar.size());
                positions[i].first = 0;
                positions[i].second = 0;
            }

            size_t j0 = 0;
            size_t j1 = 0;
            for (; i < x_ar.size(); ++ i) {
                while ((j1 < xp_ar.size()) && (x_ar(i) >= xp_ar(j1))) {
                    j0 = j1 ++;
                }
                if (j1 >= xp_ar.size()) {
                    // this more complicated stuff aligns interpolation with python interp
                    if (j1 == xp_ar.size() && x_ar(i) == xp_ar(j0)) {
                        positions[i].first = j0 - 1;
                        positions[i].second = j1 - 1;
                        ++ i;
                    }
                    break;
                }
                positions[i].first = j0;
                positions[i].second = j1;
            }

            for (; i < x_ar.size(); ++ i) {
                positions[i].first = xp_ar.size() - 1;
                positions[i].second = xp_ar.size() - 1;
            }
        }


        void perform_complex_calibration(
                numpy::ndarray &data,
                const numpy::ndarray &gain,
                const numpy::ndarray &offset,
                const numpy::ndarray &abscissa_python,
                const numpy::ndarray &abscissa_p_python,
                float scale,
                int num_threads)
        {
            Array<float, 1> abscissa(abscissa_python), abscissa_p(abscissa_p_python);
            gassert(numpy::getArrayType(gain) == NPY_CFLOAT, "gain wrong type");
            gassert(numpy::getArrayType(offset) == NPY_CFLOAT, "offset wrong type");
            gassert(numpy::getArrayType(data) == NPY_CDOUBLE, "data wrong type");

            const size_t length = abscissa.shape(0);
            const size_t length_p = abscissa_p.shape(0);

            // Number of pixels = product of all but the last dimension
            size_t num_px = 1;
            for (int i = 0; i < numpy::getArrayNDims(data) - 1; ++ i) {
                num_px *= numpy::getArrayDims(data)[i];
            }

            // Construct interpolation sample positions
            std::vector<std::pair<size_t, size_t> > positions
                = std::vector<std::pair<size_t, size_t> >(length);
            setup_interpolation(abscissa, abscissa_p, positions);

#ifdef _OPENMP
            omp_set_num_threads(num_threads);
#pragma omp parallel for
#endif
            for (size_t n = 0; n < num_px; ++ n) {
                complex64 const *const gain_px = numpy::getArrayDataAs<complex64> (gain) + n * length_p;
                complex64 const *const offset_px = numpy::getArrayDataAs<complex64> (offset) + n * length_p;
                complex128 *data_px = numpy::getArrayDataAs<complex128>(data) + n * length;
                detail::complex_calibration_innerloop(gain_px, offset_px, data_px,
                                                      abscissa, abscissa_p, positions, scale);
            }
        }


      namespace detail {

#ifndef WITH_SSE
        //#warning "NO SSE!"
        void complex_calibration_innerloop(complex64 const *const gain_px,
                                           complex64 const *const offset_px,
                                           complex128 *data_px,
                                           Array<float, 1> &abscissa,
                                           Array<float, 1> &abscissa_p,
                                           std::vector<std::pair<size_t, size_t> > &positions,
                                           double scale
                                           )
        {
          const size_t length = positions.size();
          const size_t length_p = abscissa_p.shape(0);
          for (size_t i = 0; i < length; ++ i) {
            const size_t i0 = positions[i].first;
            const size_t i1 = positions[i].second;
            gassert(i0 < length_p, i0 << " " << length_p);
            gassert(i1 < length_p, i1 << " " << length_p);
            const float t = (abscissa(i) - abscissa_p(i0)) /
              (abscissa_p(i1) - abscissa_p(i0));
            const complex64 offs_px_intp = offset_px[i0] + t * (offset_px[i1] - offset_px[i0]);
            const complex64 gain_px_intp = gain_px[i0] + t * (gain_px[i1] - gain_px[i0]);
            data_px[i] *= scale;
            data_px[i] *= gain_px_intp;
            data_px[i] += offs_px_intp;
          }

        }
#else
#warning "YES SSE!"
      __m128d sse_read_complex(const complex64 *src);
      __m128d sse_complex_mul(__m128d a, __m128d b);

        void complex_calibration_innerloop(complex64 const *const gain_px,
                                           complex64 const *const offset_px,
                                           complex128 *data_px,
                                           Array<float, 1> &abscissa,
                                           Array<float, 1> &abscissa_p,
                                           std::vector<std::pair<size_t, size_t> > &positions,
                                           double scale
                                           )
        {
          const size_t length = positions.size();
          const size_t length_p = abscissa_p.size();
          for (size_t i = 0; i < length; ++ i) {
            const size_t i0 = positions[i].first;
            const size_t i1 = positions[i].second;
            gassert(i0 < length_p, i0 << " " << length_p);
            gassert(i1 < length_p, i1 << " " << length_p);

            // Find interpolation factor
            __m128d y1 = _mm_cvtps_pd(_mm_load_ss(&abscissa(i)));
            __m128d y0 = _mm_cvtps_pd(_mm_load_ss(&abscissa_p(i0)));
            __m128d x1 = _mm_cvtps_pd(_mm_load_ss(&abscissa_p(i1)));
            // __m128d x0 = _mm_load_sd(&abscissa_p(i0));

            y1 = _mm_sub_sd(y1, y0);
            x1 = _mm_sub_sd(x1, y0);
            __m128d t = _mm_div_sd(y1, x1);
            t = _mm_movedup_pd(t);

            // Interpolate gain and offset
            __m128d offs0 = sse_read_complex(&offset_px[i0]);
            __m128d offs1 = sse_read_complex(&offset_px[i1]);
            offs1 = _mm_sub_pd(offs1, offs0);
            offs1 = _mm_mul_pd(offs1, t);
            __m128d offs_px_intp = _mm_add_pd(offs0, offs1);

            __m128d gain0 = sse_read_complex(&gain_px[i0]);
            __m128d gain1 = sse_read_complex(&gain_px[i1]);
            gain1 = _mm_sub_pd(gain1, gain0);
            gain1 = _mm_mul_pd(gain1, t);
            __m128d gain_px_intp = _mm_add_pd(gain0, gain1);

            // Perform calibration
            __m128d s = _mm_load1_pd(&scale);
            s = _mm_movedup_pd(s);
            __m128d data = _mm_loadu_pd(reinterpret_cast<double *>(&data_px[i]));
            data = _mm_mul_pd(data, s);
            data = sse_complex_mul(data, gain_px_intp);
            data = _mm_add_pd(data, offs_px_intp);

            // Write result
            _mm_storeu_pd(reinterpret_cast<double *>(&data_px[i]), data);

          }
        }

        inline __m128d sse_read_complex(const complex64 *src) {
          __m128 a = _mm_load_ss(reinterpret_cast<const float *>(src));
          __m128 b = _mm_load_ss(reinterpret_cast<const float *>(src) + 1);
          __m128 c = _mm_shuffle_ps(a, b, _MM_SHUFFLE(0, 0, 0, 0));
          c = _mm_shuffle_ps(c, c, _MM_SHUFFLE(0, 0, 2, 0));
          return _mm_cvtps_pd(c);
        }

        inline __m128d sse_complex_mul(__m128d a, __m128d b) {
            // A = (a.real, a.real)
          __m128d A = _mm_movedup_pd(a);

          // C = (b.imag, a.imag)
          __m128d C = _mm_unpackhi_pd(b, a);

          // D = (a.imag, b.real)
          __m128d D = _mm_shuffle_pd(a, b, _MM_SHUFFLE2(0, 1));

          __m128d AB = _mm_mul_pd(A, b);
          __m128d CD = _mm_mul_pd(C, D);

          __m128d R = _mm_addsub_pd(AB, CD);
          return R;
        }

        void test_sse_read_complex(const float re, const float im) {
          complex64 z(re, im);
          __m128d m = sse_read_complex(&z);
          std::vector<double> v(2);
          _mm_storeu_pd(&v[0], m);
          std::cout << "read: " << v.at(0) << " " << v.at(1) << std::endl;
        }

#endif
      }

    }
}
