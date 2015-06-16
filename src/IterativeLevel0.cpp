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

#include "IterativeLevel0.hpp"

using namespace gloc;

