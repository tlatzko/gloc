#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include <thread>
#include <atomic>

namespace gloc
{
// runnable is a wrapper around threads to have an easy access
// to parallel computing
  class Runnable
  {
  public:
    Runnable () :
	stop_ (), thread_ ()
    {
    }
    virtual
    ~Runnable ()
    {
      try
	{
	  stop ();
	}
      catch (...)
	{ /*??*/
	}
    }

    Runnable (Runnable const&) = delete;

    Runnable&
    operator = (Runnable const&) = delete;

    void
    stop ()
    {

      stop_ = true;
      thread_.join ();
    }

    void
    start ()
    {
      thread_ = std::thread (&Runnable::run, this);
    }
    void
    join ()
    {
      thread_.join ();
    }

  protected:
    virtual void
    run () = 0;
    std::atomic<bool> stop_;

  private:
    std::thread thread_;
  };
}
#endif // PARALLEL_HPP
