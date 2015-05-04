#include <thread>
#include <memory>
#include <boost/optional.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gloc/channel.hpp"

typedef std::unique_ptr<double[]> darr;

void generator(gloc::ochannel<darr> c){

    darr arr(new double[5]);
    for(int i=0; i<5;++i){
        arr[i] = i * 0.5;
    }
    c.send(std::move(arr));
}

void sink(gloc::ichannel<darr> c){
    auto arr = c.recv();
    for(int i=0; i< 5;++i){
        ASSERT_TRUE(i * 0.5 == arr[i]);
    }
}

void thread_a(gloc::channel<int> c){
    int i = 5;
    c.send(i);
    int j = c.recv();
    ASSERT_TRUE(j == 6);
}

void thread_b(gloc::channel<int> c){
    int i = c.recv();
    int j = 6;
    c.send(j);
    assert(i == 5);
}

template<class channel>
void test_send10_ints(channel c){
    for(int i = 0; i < 10; ++i){
        c.send(i);
    }
}

void test_eos(gloc::channel<int> c){
    for(int i = 0; i < 10; ++i){
        c.send(i);
    }
}

void test_eos_rec(gloc::channel<int> c){
        int i;
        while(true){
            i = c.recv();
            std::cout<< i<< std::endl;
        }
}

TEST(TestChannel, Work) {
    gloc::channel<int> c;
    std::thread a(thread_a, c);
    std::thread b(thread_b, c);
    a.join();
    b.join();
}

TEST(TestChannel, Unique){
    gloc::channel<darr> c;
    std::thread ta(generator, c);
    std::thread tb(sink, c);
    ta.join();
    tb.join();
}

TEST(TestChannel, Limit){
    gloc::channel<int, 2> c1;
    std::thread t1(test_send10_ints<decltype(c1)>, c1);
    int j;
    try{
    for(int i = 0; i <10;++i){
        j = c1.recv();
        ASSERT_TRUE(j == i);

    }}
    catch(std::exception& e){
        std::cout<< e.what()<<std::endl;
    }
    catch(...){
        std::cout<< "exception";
    }
    t1.join();
}

class test{
    public:
        double* t;
    public:
        test(){
            t = new double[5];
            t[0] = 10.;
        }
        ~test(){
            delete t;
        }
};



void txsend(gloc::channel<std::unique_ptr<test> > c){
    std::unique_ptr<test> p(new test);
    c.send(std::move(p));
}


TEST(TestChannel, pointer){
    using up = std::unique_ptr<test>;
    gloc::channel<up> c;
    std::thread t(txsend, c);
    auto r = c.recv();
//    const test f = *r.get();
    t.join();
    std::cout<< r->t[0];
}


void opsend( gloc::channel<boost::optional<int>> channel){
 boost::optional<int> i1(4);
    boost::optional<int> i2;
    std::cout<< "values \n";
    channel.send(std::move(10));
      std::cout<< "sended";   
    channel.send(std::move(boost::none));
    std::cout<< "sended";
}

TEST(TestChannel, option){
    gloc::channel<boost::optional<int>> channel;
    std::thread t(opsend, channel);
    std::cout<<"init\n";
       auto a = channel.recv();
    if(a){
        std::cout<< *a;
    }
    auto b = channel.recv();
    if(!b){
        std::cout<<" none is recv\n";
    }
    t.join();
}
