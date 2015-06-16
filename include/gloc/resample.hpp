#ifndef RESAMPLE_HPP_
#define RESAMPLE_HPP_

namespace gloc
{



class BufferedResampler {
 protected:
  unsigned int kernel_len_;

 public:
  BufferedResampler();
};
} // gloc

#endif // RESAMPLE_HPP_
