#include <boost/python.hpp>

char const* greet(){
	return "Hello world";
}

BOOST_PYTHON_MODULE(greet)
{
	using namespace boost::python;
	def("greet", greet);
}
