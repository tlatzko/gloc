//
// Copyright 2013 by Forschungszentrum Juelich GmbH
//

#include "gloripex.hpp"
#include "calibration.hpp"
#include "spikehandling.hpp"
#include "stats.hpp"
#include "numpy.hpp"
#include "array.hpp"
#include "mathutil.hpp"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#undef NO_IMPORT_ARRAY

namespace gloripex {

    /// Registers the gloripex::Exception with Python, so it can be catched.
    void translateException(const Exception& e) {
        // Use the Python 'C' API to set up an exception object
        PyErr_SetString(PyExc_RuntimeError, e.what());
    }

    /// Simple converter to make Python tuples from std::pair.
    /// Borrowed from http://wiki.python.org/moin/boost.python/HowTo
    /// Needs to be instantiated for each type of pair.
//    template<class T1, class T2>
//    struct PairToTupleConverter {
//        static PyObject* convert(const std::pair<T1, T2>& pair) {
//            return python::incref(python::make_tuple(pair.first, pair.second).ptr());
//        }
//    };
//    python::to_python_converter<std::pair<int, int>,
//                                PairToTupleConverter<int, int> > conv_pair_int;

    /// Exports a std::vector with some more functions than supported by the
    /// indexing suite.
    template <typename T>
    void exportStdVector(const std::string& name) {
        typedef std::vector<T> vector;

        void (vector::*push_back_1)(const T&) = &vector::push_back;
#if ! __CYGWIN__ && defined __GNUC__ && defined __GNUC_MINOR__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 4))
        void (vector::*resize_1)(typename vector::size_type) = &vector::resize;
#endif

        python::class_<vector, boost::shared_ptr<std::vector<T> > >(name.c_str())
            .def(python::vector_indexing_suite<vector>())
            .def(python::init<typename vector::size_type>())
            .def(python::init<typename vector::size_type, T>())
            .def("__str__", &toString<vector>)
            .def("__repr__", &toString<vector>)
#if ! __CYGWIN__ && defined __GNUC__ && defined __GNUC_MINOR__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 4))
            .def("resize", resize_1)
#else
            .def("resize", &vector::resize)
#endif
            .def("push_back", push_back_1);
    }


    /// Tests exception hand
    void test_exception() {
        throw Exception("test");
    }

    /// Demonstration function to show python-callback capabilities
    void test_plot() {
        python::object pylab(python::import("pylab"));
        python::object x = numpy::createFromScratch<double>({100});
        for (int i = 0; i < 100; ++ i) {
            x[i] = i * 2. * 3.14 / 100.;
        }
        pylab.attr("plot")(x, pylab.attr("sin")(x));
        pylab.attr("show")();
    }

    void test_arraypassing2(const Array<double, 3>& array) {
        std::cout << array << "\n";
    }
    void test_arraypassing3(Array<double, 3> array) {
        std::cout << array << "\n";
    }
    void test_arraypassing1(python::object& object) {
        test_arraypassing2(object);
    }

    /// Demonstration function to show python-callback capabilities
    python::object test_arraycreation() {
        Array<int, 2> scratch({5, 5});
        int value = 1;
        for (Array<int, 2>::size_type i = 0; i < scratch.shape(0); ++ i) {
            for (Array<int, 2>::size_type j = 0; j < scratch.shape(1); ++ j) {
                scratch(i, j) = value;
                value *= 2;
            }
        }
        return scratch.object();
    }

    /// Function to discover from python how the library was compiled.
    bool with_openmp() {
#ifdef _OPENMP
        return true;
#else
        return false;
#endif
    }

    BOOST_PYTHON_MODULE(gloripex)
    {
        python::numeric::array::set_module_and_type("numpy", "ndarray");
        import_array();
        python::register_exception_translator<Exception>(&translateException);

        python::class_<spikehandling::Index>("Index")
            .def_readwrite("i", &spikehandling::Index::i, "index in i direction")
            .def_readwrite("j", &spikehandling::Index::j, "index in j direction")
            .def_readwrite("k", &spikehandling::Index::k, "index in k direction");

        exportStdVector<uint32_t>("intvec");
        exportStdVector<spikehandling::Index>("IndexList");

        python::def("with_openmp", with_openmp);
        python::def("_test_exception", test_exception);
        python::def("_test_plot", test_plot);
        python::def("_test_arraycreation", test_arraycreation);
        python::def("_test_arraypassing1", test_arraypassing1);
        python::def("_test_arraypassing2", test_arraypassing2);
        python::def("_test_arraypassing3", test_arraypassing3);
        python::def("get_spiked_pixel", spikehandling::get_spiked_pixel);
        python::def("looking_for_chains", spikehandling::looking_for_chains);
        python::def("repair_spikes_linear", spikehandling::repair_spikes_linear);
        python::def("identify_spikes_std", spikehandling::identify_spikes_std);
        python::def("correct_spikes_std", spikehandling::correct_spikes_std);
        python::def("perform_complex_calibration",
                    calibration::perform_complex_calibration);
        python::def("compute_cub_stats_no_zopd", stats::compute_cub_stats_no_zopd);
        python::def("compute_cub_stats", stats::compute_cub_stats);
        python::def("compute_cub_normalized_mean_ifg", stats::compute_cub_normalized_mean_ifg);
        python::def("goertzel", mathutil::goertzel);
        python::def("detrend_cub",
                    mathutil::detrend_cub<cub_t>);
        python::def("detrend_and_normalize_cub",
                    mathutil::detrend_and_normalize_cub<cub_t>);
        python::def("detrend_cub",
                    mathutil::detrend_cub<double>);
        python::def("detrend_and_normalize_cub",
                    mathutil::detrend_and_normalize_cub<double>);
        python::def("detrend_and_normalize_cub_stats",
                    mathutil::detrend_and_normalize_cub_stats<cub_t>);
        python::def("detrend_and_normalize_cub_stats",
                    mathutil::detrend_and_normalize_cub_stats<double>);
#ifdef WITH_SSE
        python::def("test_sse_read_complex",
                    calibration::detail::test_sse_read_complex);
#endif
    }
}

