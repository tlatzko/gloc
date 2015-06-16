#ifndef ITERATIVE_LEVEL0_HPP
#define ITERATIVE_LEVEL0_HPP

#include <string>
#include <vector>
#include <memory>
#include "gloc/channel.hpp"

using frame_t = std::vector<double>;
using frame_ptr = std::shared_ptr<frame_t>;

/** @brief
 * The IteratorLevel0 class is used as a basic level 0 processor
 * with NLC and Resampling.
 */
class IteratorLevel0
{
protected:
  gloc::ochannel<frame_ptr> out_channel;

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

#endif // ITERATIVE_LEVEL0_HPP
