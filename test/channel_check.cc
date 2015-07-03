#define BOOST_TEST_MODULE ChannelCheck
#include <boost/test/unit_test.hpp>
#include <thread>
#include <memory>

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
        BOOST_ASSERT(i * 0.5 == arr[i]);
    }
}

void thread_a(gloc::channel<int> c){
    int i = 5;
    c.send(i);
    int j = c.recv();
    BOOST_ASSERT(j == 6);
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

BOOST_AUTO_TEST_CASE(Work) {
    gloc::channel<int> c;
    std::thread a(thread_a, c);
    std::thread b(thread_b, c);
    a.join();
    b.join();
}

BOOST_AUTO_TEST_CASE(Unique){
    gloc::channel<darr> c;
    std::thread ta(generator, c);
    std::thread tb(sink, c);
    ta.join();
    tb.join();
}

BOOST_AUTO_TEST_CASE(Limit){
    gloc::channel<int, 2> c1;
    std::thread t1(test_send10_ints<decltype(c1)>, c1);
    int j;
    try{
    for(int i = 0; i <10;++i){
        j = c1.recv();
        BOOST_ASSERT(j == i);

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


BOOST_AUTO_TEST_CASE(pointer){
    using up = std::unique_ptr<test>;
    gloc::channel<up> c;
    std::thread t(txsend, c);
    auto r = c.recv();
//    const test f = *r.get();
    t.join();
    std::cout<< r->t[0];
}


