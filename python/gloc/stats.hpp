#ifndef GLORIPEX_STATS_HPP
#define GLORIPEX_STATS_HPP

#include "gloripex.hpp"
#include "numpy.hpp"
#include "array.hpp"

namespace gloripex {
    namespace stats{
        bool compute_cub_stats_no_zopd(Array<cub_t, 3> cub_data,
                                       Array<double, 2> means,
                                       Array<double, 2> vars,
                                       int zopd,
                                       int r);
        bool compute_cub_stats(Array<cub_t, 3> cub_data, 
                               Array<double, 2> means, Array<double, 2> vars);
        bool compute_cub_normalized_mean_ifg(
                const Array<cub_t, 3> cub_data, Array<double, 1> ifg);
    }
}

#endif //STATS_HPP
