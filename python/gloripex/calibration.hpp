#ifndef GLORIPEX_CALIBRATION_HPP
#define GLORIPEX_CALIBRATION_HPP

#include "gloripex.hpp"
#include "numpy.hpp"
#include "array.hpp"


#include <utility>

namespace gloripex {
  namespace calibration {
    void perform_complex_calibration(numpy::ndarray &data,
                                     const numpy::ndarray &gain,
                                     const numpy::ndarray &offset,
                                     const numpy::ndarray &abscissa,
                                     const numpy::ndarray &abscissa_p,
                                     float scale,
                                     int num_threads);

    namespace detail {
      void complex_calibration_innerloop(complex64 const *const gain_px,
                                         complex64 const *const offset_px,
                                         complex128 *data_px,
                                         Array<float, 1> &abscissa,
                                         Array<float, 1> &abscissa_p,
                                         std::vector<std::pair<size_t, size_t> > &positions,
                                         double scale
                                         );
      void test_sse_read_complex(float re, float im);

    }
  }
}

#endif

