#include <iostream>
#include <chrono>
#include <thread>
#include <Timer.hpp>

int main()
{
  Timer t;
  std::cout << "Hello waiter" << std::endl;
  t.tic();
  auto start = std::chrono::high_resolution_clock::now();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  t.ptoc();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end-start;
  std::cout << "Waited " << elapsed.count() << " ms\n";
}
