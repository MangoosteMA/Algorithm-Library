// #include <ext/pb_ds/assoc_container.hpp>
// #include <ext/pb_ds/tree_policy.hpp>
#include <bits/extc++.h>

struct hash_for_hash_map {
    size_t operator()(size_t x) const {
        static const size_t D = std::chrono::steady_clock::now().time_since_epoch().count();
        return x * x + D;
    }
};

template<typename key_t, typename value_t, typename hash_t = hash_for_hash_map>
using hash_map = __gnu_pbds::gp_hash_table<key_t, value_t, hash_t>;

template<typename value_t, typename hash_t = hash_for_hash_map>
using hash_set = __gnu_pbds::gp_hash_table<value_t, __gnu_pbds::null_type, hash_t>;

template<typename key_t, typename value_t, typename comp = std::less<key_t>>
using ordered_map = __gnu_pbds::tree<key_t, value_t,
                    comp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

template<typename value_t, typename comp = std::less<value_t>>
using ordered_set = ordered_map<value_t, __gnu_pbds::null_type, comp>;
