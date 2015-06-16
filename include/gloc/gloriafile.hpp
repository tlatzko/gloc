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
   * the base class of
   */
  class GloriaFile
  {
  protected:

    std::string filename_;
    std::ifstream file_in_;

    /**
     * init the cubfile
     */
    void
    init ()
    {
      file_in_.open (filename_, std::ios::in | std::ios::binary);
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
    GloriaFile ()
    {
    }

    /**
     * direct file open and read header
     * @param filename
     */
    GloriaFile (const std::string& filename)
    {
      this->load_file (filename);
    }

    /**
     * standard
     */
    ~GloriaFile ()
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
    load_file (std::string filename)
    {
      filename_ = filename;
      if (file_in_.is_open ())
	{
	  file_in_.close ();
	}
      init ();
    }

    std::string
    to_string ()
    {
      std::stringstream ss;
      ss << "to string from baseclass for file: " << filename_;

      return ss.str ();
    }

  };

}  // namespace gloc
#endif /* CUBOID_H_ */

