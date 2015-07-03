#ifndef GLORIPEX_DESPIKE_HPP
#define GLORIPEX_DESPIKE_HPP

#include "gloripex.hpp"
#include "numpy.hpp"
#include "array.hpp"

namespace gloripex {
    namespace spikehandling {
        struct Index {
            uint32_t i;
            uint32_t j;
            uint32_t k;
            Index() : i(0), j(0), k(0){}
            Index(uint32_t ii, uint32_t ij, uint32_t ik) :
                i(ii), j(ij), k(ik) {}
            Index(const Index& cpy) : i(cpy.i), j(cpy.j), k(cpy.k) {}
            bool operator==(const Index& other) {
                return i == other.i && j == other.j && k == other.k;
            }
        };

        inline std::ostream& operator<<(std::ostream& os, const Index& idx) {
            os << "[" << idx.i << ", " << idx.j << ", " << idx.k << "]";
            return os;
        }

        typedef std::vector<Index> IndexList;

        IndexList looking_for_chains(const Array<cub_t, 3>& cub_data);

        void repair_spikes_linear(Array<cub_t, 3> cub_data, const IndexList& idxlist);

        void handle_propable_spike(
                IndexList &probspike, IndexList &idxlist, uint32_t frame, uint32_t min_len);

        python::list get_spiked_pixel(const IndexList& idxlist);

        /// Function takes a cub and tries to identified spiked frames by
        /// analysing the frames variance.
        ///
        /// Takes an ndarray of cubdata as input. Returns a list of frame
        /// numbers.
        ///
        /// The basic idea is to first normalise the cub by deriving the mean of
        /// each frame. This shall cancel the different gain of the individual
        /// pixels. Then all values of a frame should be very similar (at least
        /// for blackbody measurements). Thus, frames with a high variance are
        /// likely affected by a spike.
        python::list identify_and_correct_spikes_std(
                Array<cub_t, 3> cub_data, bool correct_spikes);

        /// Frontend for identify_and_correct_spikes_std
        inline python::list identify_spikes_std(Array<cub_t, 3> cub_data) {
            return identify_and_correct_spikes_std(cub_data, false);
        }
        /// Frontend for identify_and_correct_spikes_std
        inline python::list correct_spikes_std(Array<cub_t, 3> cub_data) {
            return identify_and_correct_spikes_std(cub_data, true);
        }
    }
}
#endif //DESPIKE_HPP
