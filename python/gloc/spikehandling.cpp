#define NO_IMPORT_ARRAY
 

#include "gloripex.hpp"
#include "spikehandling.hpp"
#include "array.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <boost/preprocessor/seq/for_each.hpp>
#include <xmmintrin.h>

namespace gloripex {
    namespace spikehandling {
        /**
         * check for spike which affects only a few pixel. The method use more collumns to detect
         * @param probspike
         * @param idxlist
         * @param frame
         * @param min_len
         *
         */
        void handle_propable_spike(IndexList &probspike, IndexList &idxlist,
                                   uint32_t frame, uint32_t min_len) {
            size_t i, j;
            auto len = probspike.size();
            if (len > min_len) {
                for (i = 0; i < len - min_len; ++i) {
                    Index& idx = probspike[i];
                    uint32_t val = idx.k;
                    Index sidx(idx);
                    sidx.k = frame;
                    std::stack<Index> stack;
                    stack.push(sidx);
                    if (val != 0) {
                        for (j = i + 1; j < len; ++j) {
                            Index &idx2 = probspike[j];
                            if (idx2.k == val) {
                                idx2.k = 0;
                                Index sidx(idx2);
                                sidx.k = frame;
                                stack.push(sidx);
                            }
                        }
                    }

                    if (stack.size() > min_len) {
                        while (!stack.empty()) {
                            Index &sidx = stack.top();
                            idxlist.push_back(sidx);
                            stack.pop();
                        }
                    }
                }
            }
        }


        /**
         * The measured data contains spikes. Thus have a specific pattern that results in chains of the same value. This method locate chains to detect spikes
         * @param cub_data
         * @return a list of spikes
         */
        IndexList looking_for_chains(const Array<cub_t, 3>& cub_data) {
            const uint32_t n1 = static_cast<uint32_t>(cub_data.shape(0));
            const uint32_t n2 = static_cast<uint32_t>(cub_data.shape(1));
            const uint32_t n3 = static_cast<uint32_t>(cub_data.shape(2));

            IndexList idxlist;

            uint32_t min_len = 5;
            uint32_t len = 0;

            for (uint32_t k = 0; k < n3; ++k) {
                IndexList propableSpike;
                for (uint32_t j = 0; j < n2; ++j) {
                    uint32_t last_val = cub_data(0, j, k);
                    len = 1;
                    for (uint32_t i = 1; i < n1; ++i) {
                        const uint32_t var = cub_data(i, j, k);
                        if (var != last_val || i == (n1 - 1)) {

                            if (var == last_val) {
                                ++ len;
                                ++ i;
                            }
                            if (len > min_len) {
                                for (uint32_t l = 1; l <= len; ++l) {

                                    idxlist.push_back(Index(i - l, j, k));
                                }
                                //std::cout << i << ", " << j << ", " << k << " ," << len    << " \n";
                            } else if (len > 1) {
                                for (uint32_t l = 1; l <= len; ++l) {
                                    propableSpike.push_back(Index(i - l, j, last_val));
                                }
                            }
                            last_val = var;
                            len = 1;
                        } else {
                            len++;
                        }

                    }
                }
                handle_propable_spike(propableSpike, idxlist, k, 20);
            }
            return idxlist;
        }


        /**
         * A linear interpolation between the left and right position around the spike
         * @param cub_data
         * @param idxlist
         */
        void repair_spikes_linear(
                Array<cub_t, 3> cub_data, const IndexList& idxlist) {
            const auto n3 = cub_data.shape(2);

            auto len = idxlist.size();
            for (size_t i = 0; i < len; ++i) {
                auto& idx = idxlist[i];
                auto k = idx.k;
                auto kl = k > 0 ? k - 1 : k + 1;
                auto kr = k < n3 - 1 ? k + 1 : k - 1;

                const uint32_t val_left = static_cast<uint32_t>(cub_data(idx.i, idx.j, kl));
                const uint32_t val_right = static_cast<uint32_t>(cub_data(idx.i, idx.j, kr));
                cub_data(idx.i, idx.j, k) = static_cast<cub_t>((val_left + val_right) / 2);
            }
        }


        /**
         * convert the spike list to a python list
         * @param idxlist
         * @return
         */
        python::list get_spiked_pixel(const IndexList& idxlist) {
            python::list pixel;
            uint32_t id = 0;
            auto len = idxlist.size();
            for (size_t i = 0; i < len; ++i) {
                auto& idx = idxlist[i];
                if (id != idx.k) {
                    id = idx.k;
                    pixel.append(id);
                }
            }
            return pixel;
        }


        python::list identify_and_correct_spikes_std(Array<cub_t, 3> cub_data, bool correct_spikes) {
            double stddev_threshold = -1;
            const uint32_t mid_exclusion = 300;

            python::list result;

            const auto n1 = cub_data.shape(0), n2 = cub_data.shape(1), n3 = cub_data.shape(2);
            const auto mid = n3 / 2;

            // determine means of interferograms for normalisation
            std::vector<double> means_inv(n1 * n2, 0.);
            for (uint32_t i = 0; i < n1; ++ i) {
                for (uint32_t j = 0; j < n2; ++ j) {
                    double mean = 0;
                    for (uint32_t k = 0; k < n3; ++ k) {
                        mean += static_cast<double>(cub_data(i, j, k));
                    }
                    means_inv[i * n2 + j] = static_cast<double>(n3) / mean;
                }
            }

            IndexList idxlist;
            for (uint32_t k = 0; k < n3; ++ k) {
                if (k < mid - mid_exclusion || k > mid + mid_exclusion) {
                    // determine standard deviation of normalised frame
                    double mean = 0;
                    double stddev = 0;
                    int n = 0;
                    for (uint32_t i = 0; i < n1; ++ i) {
                        for (uint32_t j = 0; j < n2; ++ j) {
                            double temp = (cub_data(i, j, k) * means_inv[n]) - mean;
                            n += 1;
                            double n_inv = 1. / n;
                            mean += temp * n_inv;
                            stddev += temp * temp * (n - 1) * n_inv;
                        }
                    }
                    // exclude frames surpassing threshold outside of ifg-center
                    stddev = sqrt(stddev / n);
                    if (stddev_threshold < 0) {
                        stddev_threshold = 3 * stddev;
                    }
                    if (stddev > stddev_threshold) {
                        result.append(k);
                        if (correct_spikes) {
                            for (uint32_t i = 0; i < n1; ++ i) {
                                for (uint32_t j = 0; j < n2; ++ j) {
                                    double temp = cub_data(i, j, k) * means_inv[i * n2 + j];
                                    if (std::abs(temp - mean) > 3 * stddev_threshold) {
                                        idxlist.push_back(Index(i, j, k));
                                    }
                                }
                            }
                        }
                    } else {
                        stddev_threshold = (3 * stddev_threshold + 3 * stddev) / 4.;
                    }
                } else {
                    stddev_threshold = -1;
                }
            }
            if (correct_spikes) {
                repair_spikes_linear(cub_data, idxlist);
            }
            return result;
        }
    } //namespace spikehandling
}//namespace gloripex
