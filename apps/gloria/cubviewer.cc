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


using namespace gloc;



//std::string localData = "/media/latzko/store/20120828_152843.cub";
//
std::string localData = "../data/20120913_135249.cub";
//typedef std::shared_ptr<CuboidFrameHeader>;
typedef Eigen::ArrayXXd frame_t;
typedef std::shared_ptr<frame_t> frame_ptr;

class Runnable
{
    public:
        Runnable() : stop_(), thread_() { }
        virtual ~Runnable() { try { stop(); } catch(...) { /*??*/ } }

        Runnable(Runnable const&) = delete;

    Runnable& operator =(Runnable const&) = delete;

        void stop() {

            stop_ = true; thread_.join();
        }

    void start() { thread_ = std::thread(&Runnable::run, this); }
    void join() { thread_.join(); }

protected:
        virtual void run() = 0;
        std::atomic<bool> stop_;

    private:
        std::thread thread_;
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
        ReadNode() = delete;
        ReadNode(std::string file_name, gloc::ochannel<frametype, buffer_size> out,
                ochannel<CuboidFrameHeader> header_out):
            file_name_(file_name), frame_out_(out), frame_header_out_(header_out){
            }
        ~ReadNode(){}

    void run() {
                
            CuboidFile cub(localData);
            
            frame_to_frametype F;
            const size_t frame_count = cub.getFrameCount();
            for(size_t i =0; i < frame_count; ++i){
                auto frame = cub.getFrame(i);
                auto header = frame.getFrameHeader();
                auto image = F(frame);
                std::cout<< "push frame " <<i<< std::endl;
                frame_out_.send(std::move(image));
                frame_header_out_.send(header);
            }
            std::cout<<"finish read node\n";
            frame_out_.send(nullptr);
            CuboidFrameHeader he;
            frame_header_out_.send(he);
        }
};



template<class frametype>
class FrameRecv : public Runnable{
    protected:
        channel<frametype, 5> frames_;

    public:
        FrameRecv(channel<frametype, 5> chan): frames_(chan){
        }

        void finalize(){
            std::cout<< "final steps\n";
        }


        void run(){
            int i = 0;
            while(!stop_){
                auto frame = frames_.recv();
                if(frame == nullptr){
                    break;
                }

                ++i;
            }
            std::cout<< "frame recv end: "<< i<< "\n ";

            this->finalize();
        }

};

template<class frametype>
class FrameMean : public Runnable{
    protected:
        ichannel<frametype, 5> frames_;
        ochannel<frametype, 5> frames_out_;

    public:
        FrameMean(ichannel<frametype, 5>chan, ochannel<frametype, 5> chanout):
            frames_(chan), frames_out_(chanout){}
        void run(){

            auto frame = frames_.recv();
            assert(frame != nullptr);
                
            size_t i = 1;
            frame_t M_(*frame);
            while(true){
                auto frame = frames_.recv();
                if(frame == nullptr){
                    frames_out_.send(std::move(frame));
                    break;
                }

                auto M = *frame;
                M_ = M_ + M;
                frames_out_.send(std::move(frame));
                ++i;
                std::cout<< "frame recv " << i << std::endl;
            }
            std::cout<< "mean end\n";
            //std::cout<< M_ / i<< std::endl;
        }
};


template<class frametype, size_t buffer_size>
class BufferedMean : public Runnable{
    protected:
        ichannel<frametype, 5> frames_;
        ochannel<frametype, 5> frames_out_;
        std::deque<frametype> buffer_;

public:
        BufferedMean(ichannel<frametype, 5> chan,
                ochannel<frametype, 5> chan_out):
            frames_(chan),
            frames_out_(chan_out){
            }

        frame_ptr compute_the_mean(){
            frame_ptr m0(nullptr);

            for(size_t i=0; i < buffer_size; ++i){
                if(i == 0){
                    m0.reset(new frame_t((*buffer_[i])));
                }

                else{
                    if(!buffer_[i]){
                        std::cout<< "chrash";
                        break;
                    }

                    (*m0) += (*buffer_[i]);

                }

            }
            return m0;
        }

        bool fill_the_buffer(){
            for(size_t i =0; i < buffer_size; ++i){
                std::cout << "recv frame\n";
                auto frame = frames_.recv();
                if(frame == nullptr){
                    frames_out_.send(nullptr);
                    return false;
                }
                buffer_.push_back(std::move(frame));

            }
            buffer_.shrink_to_fit();
            return true;
        }

        void run(){
           bool filled = fill_the_buffer();

           if(filled){
               while(true){
                   auto frame = frames_.recv();
                   if(frame == nullptr){
                       frames_out_.send(std::move(frame));
                       break;
                    }
                   auto fout = compute_the_mean();
                   this->buffer_.pop_front();
                   this->buffer_.push_back(std::move(frame));

                   frames_out_.send(std::move(fout));
               }
           }
           std::cout<< "end buffer\n";
        }

};

class HeaderRecv : public Runnable{
    private:
        ichannel<CuboidFrameHeader>  c_;

    public:
        HeaderRecv(ichannel<CuboidFrameHeader> chan): c_(chan){
        }
        void run(){
            int i = 0;
            while(true){
                auto header = c_.recv();
                ++i;
                if(header.status == 0){
                    break;
                }

            }
            std::cout<< "reach end header\n" << std::endl;
        }

};
struct convertArrayToMat{
    frame_ptr operator()(const CuboidFrame &frame){
        uint16_t* values = frame.pixel_values;
        auto width = frame.width_;
        auto height = frame.height_;
        frame_ptr res(new frame_t(width, height));
        for(size_t j = 0; j < width; ++j){
                for(size_t i=0; i < height; ++i){
                const size_t pos = j * height + i;
                (*res)(j, i) = static_cast<double>(values[pos]);
            }
        }
        return res;
    }
};



int main(){
    Eigen::initParallel();
    CuboidFile cub(localData);
    cub.printHeader();

    // required channels
    channel<frame_ptr, 5> frame_chan;
    channel<frame_ptr, 5> buffer_chan;
    channel<frame_ptr, 5> mean_chan;
    channel<CuboidFrameHeader> header_chan;
    // nodes
    ReadNode<frame_ptr, convertArrayToMat, 5> r(localData, frame_chan, header_chan);
    BufferedMean<frame_ptr, 40> bf(frame_chan, buffer_chan);
    FrameRecv<frame_ptr> fr(mean_chan);
    HeaderRecv hr(header_chan);
    FrameMean<frame_ptr> mf(buffer_chan, mean_chan);
    fr.start();
    bf.start();
    mf.start();
    hr.start();
    r.start();
    r.join();
    std::cout<< "end reading stop all other " << std::endl;
    hr.join();
    mf.join();
    bf.join();
    fr.join();
    std::cout<< "measure time";

    return 0;
}
