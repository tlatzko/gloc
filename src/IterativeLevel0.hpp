#ifndef ITERATIVE_LEVEL0_HPP
#define ITERATIVE_LEVEL0_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <exception>
#include "gloc/parallel.hpp"
#include "gloc/matrix.hpp"
#include "gloc/channel.hpp"
#include "gloc/cuboid.hpp"



namespace gloc {


typedef Mat<double> frame_t;
typedef std::shared_ptr<frame_t> frame_ptr;


/**
 * a function wich transmute a CuboidFrame
 * to a double frame
 */
struct ConvertArrayToMat{
  frame_ptr operator()(const CuboidFrame &frame);

};


/**
 * The ReadNode should read frames transform into
 * a an image usually an opencv image and send it
 * with a channel
 **/

template<class frametype, class frame_to_frametype, int buffer_size=5>
class ReadNode : public Runnable{
 private:
  std::string file_name_;
  gloc::ochannel<frametype, buffer_size> frame_out_;
  gloc::ochannel<gloc::CuboidFrameHeader> frame_header_out_;

 public:
  
  ReadNode(std::string file_name, gloc::ochannel<frametype, buffer_size> out,
           ochannel<CuboidFrameHeader> header_out):
      file_name_(file_name), frame_out_(out), frame_header_out_(header_out){
  }
  ~ReadNode(){}
  
  void run() {
    
    CuboidFile cub(file_name_);
            
    frame_to_frametype F;
    const size_t frame_count = cub.getFrameCount();
    for(size_t i =0; i < frame_count; ++i){

      auto frame = cub.getFrame(i);
      std::cout << "loop " << i << std::endl;            
      auto header = frame.getFrameHeader();
      auto image = F(frame);
      
      frame_out_.send(std::move(image));
      
      //frame_header_out_.send(header);
      std::cout << "send" << std::endl;
    }
    std::cout<<"finish read node\n";
    frame_out_.send(nullptr);
    //CuboidFrameHeader he;
    //frame_header_out_.send(he);
  }
};


class StreamReader{
 protected:
  
  channel<frame_ptr, 5> frame_channel_;
  channel<CuboidFrameHeader> header_channel_;
  std::string fname_;
  ReadNode<frame_ptr, ConvertArrayToMat> reader_;
  
 public:
  StreamReader(const std::string& fname);
  StreamReader(const StreamReader& other);
  ~StreamReader();
  frame_ptr
  get_next();
};

/** @brief
 * The IteratorLevel0 class is used as a basic level 0 processor
 * with NLC and Resampling.
 */
class IteratorLevel0
{
protected:
  channel<frame_ptr, 2> out_channel;

  // init all the channels
  void
  init ();

public:

  IteratorLevel0 (const std::string& file_name);

  ~IteratorLevel0 ();

  /**
   * All channels run inside this class and are connected
   * each other. If the get next method is called
   * the end of the level 0 chain is called.
   */
  frame_t
  get_next ();

};

}
#endif // ITERATIVE_LEVEL0_HPP
