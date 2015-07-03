#define NO_IMPORT_ARRAY

#include "gloripex.hpp"
#include "stats.hpp"
#include "array.hpp"

namespace gloripex { namespace stats {

    bool compute_cub_stats_no_zopd(Array<cub_t, 3> cub_data,
                                   Array<double, 2> means,
                                   Array<double, 2> vars,
                                   int zopd,
                                   int r) {

        auto n = cub_data.shape(0);
        auto m = cub_data.shape(1);
        auto l = cub_data.shape(2);

        if (n != means.shape(0) || m != means.shape(1)) {
            return false;
        }

        double sum;
        double sumSqr;

        for (size_t i = 0; i < n; ++ i) {
            for (size_t j = 0; j < m; ++ j) {
                sum = 0;
                sumSqr = 0;
                for (size_t k = 0; k < l; ++ k) {
                    if((k < zopd - r) || (k > zopd + r)){
                            const double value = static_cast<double>(cub_data(i, j, k));
                            sum += value;
                            sumSqr += (value * value);
                        }
                }
                const double mean = sum / static_cast<double>(l);
                const double var = (sumSqr - (sum * mean)) / static_cast<double>(l - 1);
                means(i, j) = mean;
                vars(i, j) = var;

            }
        }
        return true;
    }

    bool compute_cub_stats(
        Array<cub_t, 3> cub_data, Array<double, 2> means, Array<double, 2> vars) {

        auto n = cub_data.shape(0);
        auto m = cub_data.shape(1);
        auto l = cub_data.shape(2);

        if (n != means.shape(0) || m != means.shape(1)) {
            return false;
        }

        double sum;
        double sumSqr;

        for (size_t i = 0; i < n; ++ i) {
            for (size_t j = 0; j < m; ++ j) {
                sum = 0;
                sumSqr = 0;
                for (size_t k = 0; k < l; ++ k) {
                    const double value = static_cast<double>(cub_data(i, j, k));
                    sum += value;
                    sumSqr += (value * value);
                }
                const double mean = sum / static_cast<double>(l);
                const double var = (sumSqr - (sum * mean)) / static_cast<double>(l - 1);
                means(i, j) = mean;
                vars(i, j) = var;

            }
        }
        return true;
    }


    bool compute_cub_normalized_mean_ifg(
        const Array<cub_t, 3> cub_data, Array<double, 1> ifg) {
        auto n = cub_data.shape(0);
        auto m = cub_data.shape(1);
        auto l = cub_data.shape(2);

        if (l != ifg.shape(0)) {
            return false;
        }
        double cnt = 0;
        for (size_t i = 0; i < n; ++ i) {
            for (size_t j = 0; j < m; ++ j) {
                double norm = 0;
                for (size_t k = 0; k < l; ++ k) {
                    double temp = cub_data(i, j, k);
                    norm += temp * temp;
                }
                norm = sqrt(norm);
                if (norm > 0) { 
                    cnt += 1;
                    for (size_t k = 0; k < l; ++ k) {    
                        ifg(k) += cub_data(i, j, k) / norm;
                    }
                }
            }
        }
        for (size_t k = 0; k < l; ++ k) {
            ifg(k) /= cnt;
        }
        return true;
    }


}}
