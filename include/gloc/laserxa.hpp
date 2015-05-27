#ifndef LASERXA_HPP
#define LASERXA_HPP

#include <iostream>

namespace gloc{

        
//***************** File structures *************************
struct laserxa_info{
  uint32_t version;
  uint32_t file_type;
}__attribute__ ((packed));

struct laserxa_data{
  uint32_t frame_number;
  uint32_t timestamp_ms;
  uint32_t timestamp_tick;
  uint32_t event_counter;
}__attribute__ ((packed));

struct laserxa_file{
  struct laserxa_info info;
  struct laserxa_data *data;
}__attribute__ ((packed));

//*************** Memory structures *************************

struct laserxa {
  uint32_t n_measures;
  uint32_t first_measure;
  laser_length_t laser_length;
  long_laser_length_t length_offset;
  struct timing timing_offset;
  struct timing *timing;
};



} // namespace gloc

#endif //LASERXA_HPP
