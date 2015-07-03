#include <boost/python.hpp>
#include <string>
#include "Resampler.h"

char const* greet(){
	return "Hello world";
}


std::string invite(){
  return "test";
}


BOOST_PYTHON_MODULE(greet)
{
	using namespace boost::python;
	def("greet", greet);
        class_<Hello>("Hello", init<>())
            .def("say_hello", &Hello::say_hello)
            ;

        def("invite", invite);
}
