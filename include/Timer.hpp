#ifndef TIMER_HPP__
#define TIMER_HPP__

#include <chrono>
#include <iostream>

class Timer
{
  std::chrono::time_point<std::chrono::system_clock> start;
public:
  Timer ()
  {
    tic ();
  }

  /**
   * starts the timer
   */
  void
  tic ()
  {
    start = std::chrono::high_resolution_clock::now ();
  }

  /**
   * return time in [ms] counted until tic()
   */
  double
  toc ()
  {
    std::chrono::duration<double, std::milli> time_duration =
	std::chrono::high_resolution_clock::now () - start;
    return time_duration.count ();
  }

  void
  ptoc ()
  {
    std::cout << toc () << "ms" << std::endl;
  }

};

#endif // TIMER_HPP__
