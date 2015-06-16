/*
 * cuboid.hpp
 *
 *  Created on: Oct 22, 2014
 *      Author: latzko
 */

#ifndef CUBOID_H_
#define CUBOID_H_
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <exception>
#include <sstream>

namespace gloc
{

#define CUB4_START_FRAME 512

/// exceptions
  class CubEndException : public std::exception
  {
    virtual const char*
    what () const throw ()
    {
      return "End of cub reached";
    }
  };

  class FrameToShortException : public std::exception
  {
    virtual const char*
    what () const throw ()
    {
      return "The frame data is too short even for the header";
    }
  };

/// Common info struct to discern file types.
  struct CuboidInfo
  {
    uint32_t version;
    uint32_t file_type;
  };

/// The Cuboid header contains relevant information for the used data
  struct CuboidHeader
  {
    uint16_t v_width;
    uint16_t h_width;
    uint16_t v_offset;
    uint16_t h_offset;
    uint32_t length;
  }__attribute__((packed));

/// contains the frame information
  struct CuboidFrameHeader
  {
    uint32_t frame_number;
    uint32_t timestamp_ms;
    uint32_t timestamp_tick;
    uint32_t status;
  }__attribute__((packed));

/// is fixed for unsigned int values direct
/// taken from the a/d converter
  class CuboidFrame
  {

  public:
    /*
     * Parameters
     */
    CuboidFrameHeader header;

    uint16_t *pixel_values;

    size_t width_;
    size_t height_;
    size_t frame_offset_;

    CuboidFrame () = delete;

    CuboidFrame (size_t width, size_t height) :
	width_ (width), height_ (height)
    {
      frame_offset_ = 2 * width;
      pixel_values = new uint16_t[(width + 1) * height];
    }

    ~CuboidFrame ()
    {
      delete pixel_values;
    }

    /*
     * method parts
     */
    void
    setFrameHeader ()
    {
      if (height_ * width_ < sizeof(CuboidFrameHeader))
	{
	  throw FrameToShortException ();
	}
      //            std::cout << "sizeof"<< sizeof(CuboidFrameHeader) << ":: ";
      //    for(int i; i<sizeof(CuboidFrameHeader); ++i){
      //      std::cout<< pixel_values[i]<< " ";
      //            }
      std::memcpy ((char*) &header, (char*) &pixel_values[0],
		   sizeof(CuboidFrameHeader));
    }

    CuboidFrameHeader
    getFrameHeader ()
    {
      return this->header;
    }

    std::string
    to_string ()
    {
      std::stringstream ss;
      ss << "fnumber: " << header.frame_number << "\nstatus: " << header.status
	  << "\ntick: " << header.timestamp_tick << "\ntimestamp: "
	  << header.timestamp_ms << "ms\n";
      return ss.str ();
    }

    /**
     * return the pointer plus the frame offset such that
     * the values is red after the header
     */
    uint16_t*
    getFrameValues ()
    {
      return this->pixel_values + this->frame_offset_;
    }

  };

  /**
   * is a viewer class for the cubfile
   */
  class CuboidFile
  {
  protected:

    CuboidInfo info_;
    CuboidHeader header_;
    std::string filename_;
    std::ifstream file_in_;
    int first_frame_;
    int current_frame_position_;
    size_t frame_count_;

    /**
     * init the cubfile
     */
    void
    init ()
    {
      file_in_.open (filename_, std::ios::in | std::ios::binary);
      readHeader ();
    }

    /**
     * read the header of the initialized file.
     */
    void
    readHeader ()
    {
      file_in_.seekg (0, std::ios::beg);

      file_in_.read ((char*) &info_, sizeof(CuboidInfo));
      file_in_.read ((char*) &header_, sizeof(CuboidHeader));

      file_in_.seekg (0, std::ios::end);
      std::streampos end = file_in_.tellg ();

      file_in_.seekg (getCubOffset (), std::ios::beg);
      first_frame_ = file_in_.tellg ();

      current_frame_position_ = -1;
      frame_count_ =
	  (static_cast<long> (end) - static_cast<long> (first_frame_))
	      / (2 * static_cast<long> (header_.length));

    }
  public:
    /**
     * direct intance without open a file
     */
    CuboidFile ()
    {
    }

    /**
     * direct file open and read header
     * @param filename
     */
    CuboidFile (std::string filename)
    {
      filename_ = filename;
      if (file_in_.is_open ())
	{
	  file_in_.close ();
	}
      init ();
    }

    /**
     * standard
     */
    ~CuboidFile ()
    {
      if (file_in_.is_open ())
	{
	  file_in_.close ();
	}
    }

    /**
     * close an existing file and open a new one based on the filename.
     * @param filename
     */
    void
    loadCub (std::string filename)
    {
      filename_ = filename;
      if (file_in_.is_open ())
	{
	  file_in_.close ();
	}
      init ();
    }

    /**
     * will print the current header / more for debugging issues
     */
    void
    printHeader ()
    {
      std::cout << this->to_string ();
    }

    std::string
    to_string ()
    {
      std::stringstream ss;
      ss << "name: " << filename_ << "\nv_width: " << header_.v_width
	  << "\nh_width: " << header_.h_width << "\nv_offset: "
	  << header_.v_offset << "\nh_offset: " << header_.h_offset
	  << "\nlength: " << header_.length << "\nframecount: " << frame_count_
	  << std::endl;
      return ss.str ();
    }

    /// getter/setter region
    CuboidHeader
    getHeader ()
    {
      return header_;
    }

    int
    getCubOffset ()
    {
      return 512;
    }

    size_t
    getFrameCount ()
    {
      /**
       * return the number of frames
       **/
      return frame_count_;
    }

    CuboidFrame
    getNextFrame ()
    {
      current_frame_position_++;
      return getFrame (current_frame_position_);

    }

    CuboidFrame
    getFrame (size_t i)
    {
      if (i >= frame_count_)
	{
	  throw CubEndException ();
	}
      CuboidFrame frame (header_.v_width, header_.h_width);

      const size_t pos_header = first_frame_
	  + static_cast<size_t> (2 * header_.length) * i;
      const size_t pos_pixel = pos_header + frame.frame_offset_;

      file_in_.seekg (pos_header, std::ios::beg);
      file_in_.read ((char*) &frame.header, sizeof(frame.header));

      file_in_.seekg (pos_pixel, std::ios::beg);
      file_in_.read ((char*) frame.pixel_values,
		     sizeof(uint16_t) * frame.height_ * frame.width_);
      frame.setFrameHeader ();
      return frame;
    }

  };

  class FrameReader
  {
  public:
    FrameReader (std::string filename)
    {

    }
  };

}  // namespace gloc
#endif /* CUBOID_H_ */

