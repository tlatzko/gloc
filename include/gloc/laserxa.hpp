#ifndef LASERXA_HPP
#define LASERXA_HPP

#include <iostream>
#include <string>
#include <exception>
#include <fstream>

// GLORIA IFM/LMI units
#define TICKS_PER_MS 80000.
#define XTICKS_PER_CM 1e11

namespace gloc
{

//***************** exceptions *************************
  class LxaEndException : public std::exception
  {
    virtual const char*
    what () const throw ()
    {
      return "End of lxa reached";
    }
  };

  enum IfgDirection
  {
    fwd, bck
  };

//***************** File structures *************************
  struct LaserxaInfo
  {
    uint32_t version;
    uint32_t file_type;
  }__attribute__ ((packed));

  struct LaserxaData
  {
    uint32_t frame_number;
    uint32_t timestamp_ms;
    uint32_t timestamp_tick;
    uint32_t event_counter;
  }__attribute__ ((packed));

//*************** Memory structures *************************

  struct laserxa
  {
    uint32_t n_measures;
    uint32_t first_measure;
    uint32_t laser_length;
    uint32_t length_offset;

  };

  class LaserxaFile
  {
  protected:
    // file handling paramert
    std::string filename_;
    std::ifstream file_in_;

    // file specific parameter
    LaserxaInfo info_;
    size_t data_count_;

    /**
     * initialize the laserxa files
     */
    void
    init ()
    {
      // read the info header
      this->close ();
      file_in_.open (filename_, std::ios::in | std::ios::binary);
      file_in_.seekg (0, std::ios::beg);
      file_in_.read ((char*) &info_, sizeof(LaserxaInfo));

      // length of size
      auto current_pos = file_in_.tellg ();
      file_in_.seekg (0, std::ios::end);
      auto end = file_in_.tellg ();

      data_count_ = (static_cast<size_t> (end)
	  - static_cast<size_t> (current_pos)
	      / static_cast<size_t> (sizeof(LaserxaData)));

    }

  public:

    LaserxaFile (const std::string& filename) :
	filename_ (filename)
    {
      init ();
    }

    ~LaserxaFile ()
    {
      this->close ();
    }

    size_t
    size () const
    {
      return data_count_;
    }

    void
    close ()
    {
      if (file_in_.is_open ())
	{
	  file_in_.close ();
	}
    }

    const LaserxaInfo&
    get_info () const
    {
      return info_;
    }

    LaserxaData
    operator[] (size_t i)
    {
      return this->get_data (i);
    }

    /**
     * get time at datapoint i 
     * will return the time in [s]
     * computed for the ith zerocrossing
     */
    double
    get_time_at (size_t i)
    {
      auto data = this->get_data (i);
      double xr = data.timestamp_ms + data.timestamp_tick / TICKS_PER_MS;
      return xr / 1000.;
    }

    /**
     * get the data at the ith position
     * throws out of file exception
     */
    LaserxaData
    get_data (size_t i)
    {
      if (i >= data_count_)
	{
	  throw LxaEndException ();
	}
      const size_t pos = sizeof(LaserxaInfo) + i * sizeof(LaserxaData);
      LaserxaData d;
      file_in_.seekg (pos, std::ios::beg);
      file_in_.read ((char*) &d, sizeof(LaserxaData));

      return d;

    }

  };

//*************** general lxafunctions *************************
  /**
   * the spectrum scan in two directions. It is necessary to identify the direction
   * because the different phase characteristic. We determine the direction
   * using the eventcounter.
   *
   */
  int
  get_direction_from_lxa (LaserxaFile& lxa)
  {
    size_t len2 = lxa.size () / 2;

    auto data1 = lxa.get_data (len2);
    auto data2 = lxa.get_data (len2 + 1);
    uint32_t l1 = data1.event_counter & 0xffffff;
    uint32_t l2 = data2.event_counter & 0xffffff;

    auto direction_id = l2 - l1;

    if (direction_id > 0)
      {
	return 0;
      }
    return 1;

  }

  /**
   * The LMI/IFM has a time delay between the actual measurement and the laser
   * offset. This is may induced from the ROIC. Tom Neubert estimate the delay
   * with an lab experiment.
   * return: the time in seconds
   */
  double
  time_delay_from_tint (double t_int, double t_frame = 159.198, double t_reset =
			    24.3,
			double t_lmi = 4.1)
  {
    double t_delay = t_frame - (.5 * t_int + t_reset) - t_lmi;
    return t_delay * 1e-6;
  }

  /**
   * return the offset for the stored timings in the lmi data vector
   */
  size_t
  laser_offset_from_tint (double t_int, double t_frame = 159.198,
			  double t_reset = 24.3, double t_lmi = 4.1)
  {
    const double t_delay = time_delay_from_tint (t_int, t_frame, t_reset,
						 t_lmi);
    size_t offset = static_cast<size_t> (t_delay * 1e3 * TICKS_PER_MS);
    return offset;
  }

} // namespace gloc

#endif //LASERXA_HPP
