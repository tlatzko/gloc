
/*
 * perftester.cc
 *
 *  Created on: Aug 29, 2014
 *      Author: Thomas Latzko
 */

#include <vector>
#include <iostream>
#include <fstream>

#include "gloc/cuboid.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace gloc;
string localData = "/home/latzko/work/experimental/data/20120828_162649.cub";

TEST(CuboidTest, readFrame){
	streampos begin, end;

	CuboidFile cub(localData);
        cub.printHeader();
        auto frame = cub.getFrame(1000);
        frame.setFrameHeader();
        cout << frame.to_string();
        cout<<endl;
        auto frame2 = cub.getFrame(1001);
        frame2.setFrameHeader();
        cout << frame2.to_string();


        cout << "size of frame header in byte: " << sizeof(gloc::CuboidFrameHeader)<< endl;
}


TEST(CuboidTest, readCub){
	streampos begin, end;

	CuboidFile cub(localData);
	ifstream file(localData, ios::in | ios::binary);
    	cub.printHeader();
        for(size_t i=0; i<cub.getFrameCount();++i){
            auto frame = cub.getFrame(i);

            auto j = frame.pixel_values[10];
            if(j < 0){
                cout<< "wrong";
            }
        }
	begin = file.tellg();
	file.seekg(0, ios::end);
	end = file.tellg();
	file.close();
        //	cout << "size is : "<< (end - begin)<< " bytes "<< sizeof(CuboidHeader)<< "header" <<"\n";
}


TEST(CuboidTest, readAll){
	streampos begin, end;

	ifstream file(localData,
                ios::in | ios::binary);

	begin = file.tellg();
	file.seekg(0, ios::end);
	end = file.tellg();
        size_t l = (end - begin) / 2;
        uint16_t* pixels = new uint16_t[l];
        file.seekg(0, ios::beg);
        file.read((char*) pixels, (end - begin));
	file.close();
        cout<< pixels[13000]<< endl;
        delete pixels;
	cout << "size is : "<< (end - begin)<< " bytes "<< "\n";
}

