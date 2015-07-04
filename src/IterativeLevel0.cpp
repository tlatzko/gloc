/*
 * perftester.cc
 *
 *  Created on: Aug 29, 2014
 *      Author: Thomas Latzko
 */
#include <assert.h>
#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <memory>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "gloc/cuboid.hpp"
#include "gloc/channel.hpp"
#include "gloc/parallel.hpp"
#include "IterativeLevel0.hpp"

namespace gloc {

frame_ptr ConvertArrayToMat::operator()(const CuboidFrame &frame){
  uint16_t* values = frame.pixel_values;
  auto width = frame.width_;
  auto height = frame.height_;
  frame_ptr res(new frame_t(width, height));
  for(size_t j = 0; j < width; ++j){
    for(size_t i=0; i < height; ++i){
      const size_t pos = j * height + i;
      (*res)(i, j) = static_cast<double>(values[pos]);
    }
  }
  return res;
}




StreamReader::StreamReader(const std::string& fname) :
    frame_channel_(), header_channel_(), fname_(fname),
    reader_(fname, frame_channel_, header_channel_)
{

  reader_.start();
  
}

StreamReader::StreamReader(const StreamReader& other):
    frame_channel_(), header_channel_(), fname_(other.fname_),
    reader_(other.fname_, frame_channel_, header_channel_)
{

  reader_.start();
  
}

StreamReader::~StreamReader(){
  reader_.stop();
}

frame_ptr StreamReader::get_next(){
  std::cout << "get frame" << std::endl;
  auto recv = frame_channel_.recv();
  return recv;
}


IteratorLevel0::IteratorLevel0(const std::string& fname){
  
}


}

