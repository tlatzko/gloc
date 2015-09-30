#ifndef WAVELET_HPP_
#define WAVELET_HPP_

#include <array>

template<class real>
class Wavelet{
public:
  Wavelet();
  Wavelet(std::array<real> dec_lo, std::array<real> dec_hi, std::array<real> rec_lo, std::array<real> rec_hi);

  const std::array<real>& dec_lo() const{
    return dec_lo_;
  }
protected:
  std::array<real> dec_lo_;
};

#endif // WAVELET_HPP_
