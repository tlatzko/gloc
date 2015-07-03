#ifndef RESAMPLER_H
#define RESAMPLER_H

#include <vector>
#include <string>
#include <iostream>
#include "gloc/math.hpp"
#include "gloc/channel.hpp"


/**
 * Non linearity correction with the parameter set for 
 * detector 2b. The detector is used in the TACTS campaign.
 */
template<class array>
void nlc_det2b(gloc::ichannel<array> in_chan, gloc::ochannel<array> out_chan){
}



/**
 * The resample control class is a basic class to resample
 * a basic measurement.
 
*/
class ResampleControl{
 public:
    ResampleControl(const std::string& file_name);

 protected:
    void init();

    std::string file_name_;
};


class Hello {
public:
  Hello();

  char const* say_hello();
  
};

#endif // RESAMPLER_H
