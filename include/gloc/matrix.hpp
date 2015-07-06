#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <string>
#include <exception>
#include <memory>
#include <vector>

namespace gloc {
/**
 * @brief The Mat class can forget the pointer
 * such that the data can be reused withoud
 * the Mat reference. To prevent that this will
 * be done via accident the Mat class have to forget.
 *
 * If not an exception will be thrown
 */
class PointerBoundException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "The pointer is bound to class";
  }
} PointerIsBound;

/**
 * @brief The standard 2D Matrix format. Ideal format
 * for Frames
 */
template<class real>
class Mat{
 protected:

  size_t height_;
  size_t width_;
  
  
  std::vector<real> data_;

 public:
  Mat(){}
  
  Mat(const size_t& height, const size_t& width):
      height_(height), width_(width), data_(height * width){
    
  }


  /* FIXIT now there should be no copies
     Mat(const Mat& other) : height_(other.height), width_(other.width), forget_(false){
     data_ = new real[height_ * width_];
    
     }
  */


  /**********************  access functions ** */ 
  real &operator[](const size_t& pos){
    return data_[pos];
  }
  
  real &operator()(const size_t& i, const size_t& j){
    const size_t  pos = i + j * width_;
    
    return data_[pos];
  }

  size_t height(){
    return this->height_;
  }
  size_t width(){
    return width_;
  }

  size_t get_size(){
    return data_.size();
  }


  /**
   * The get_data function will allow
   * to hand over the data.
   * It can now be deleted without delete also
   * the data pointer.
   */
  std::vector<real>& get_data() {

    return data_;
  }

  real* get_raw_data(){
    return data_.data();
  }
      
};



}// gloc
#endif // MATRIX_HPP
