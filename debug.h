namespace helpers {
    template<typename T, typename = void>
    struct is_printable : std::false_type {};

    template<typename T>
    struct is_printable<T, typename std::enable_if_t<std::is_same_v<decltype(std::cout << std::declval<T>()), std::ostream&>>> : std::true_type {};

    template<typename T, typename = void>
    struct is_iterable : std::false_type {};

    template<typename T>
    struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())), decltype(std::end(std::declval<T>()))>> : std::true_type {};
}

namespace __to_string {
    std::string to_string(const bool &b) {
        return b ? "true" : "false";
    }

    template<size_t SZ>
    std::string to_string(const std::bitset<SZ> &bs) {
        std::string s;
        for (size_t i = 0; i < SZ; i++)
            s += bs[i] ? "1" : "0";

        return s;
    }

    std::string to_string(const std::vector<bool> &v) {
        std::string s;
        for (size_t i = 0; i < v.size(); i++)
            s += v[i] ? "1" : "0";

        return s;
    }

    template<typename T>
    typename std::enable_if_t<helpers::is_printable<T>::value, std::string> to_string(const T &t);

    template<typename T>
    typename std::enable_if_t<helpers::is_iterable<T>::value && !helpers::is_printable<T>::value, std::string> to_string(const T &v);

    template<typename A, typename B>
    std::string to_string(const std::pair<A, B> &p);

    template<typename T, typename _Container, typename _Compare>
    std::string to_string(std::priority_queue<T, _Container, _Compare> pq);

    template<typename T>
    typename std::enable_if_t<helpers::is_printable<T>::value, std::string> to_string(const T &t) {
        std::stringstream ss;
        if constexpr (helpers::is_iterable<T>::value)
            ss << '{';

        ss << std::fixed << std::setprecision(10) << t;
        if constexpr (helpers::is_iterable<T>::value)
            ss << '}';

        return ss.str();
    }

    template<typename T>
    typename std::enable_if_t<helpers::is_iterable<T>::value && !helpers::is_printable<T>::value, std::string> to_string(const T &v) {
        std::string s = "{", sep;
        for (const auto &x : v) {
            s += sep + to_string(x);
            sep = ", ";
        }
        return s + "}";
    }

    template<typename A, typename B>
    std::string to_string(const std::pair<A, B> &p) {
        return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
    }

    template<typename T, typename _Container, typename _Compare>
    std::string to_string(std::priority_queue<T, _Container, _Compare> pq) {
        std::string res = "{", sep;
        while (!pq.empty()) {
            res += sep + to_string(pq.top());
            sep = ", ";
            pq.pop();
        }
        res += "}";
        return res;
    }
}

void __dbg_out() {
    std::cerr << std::endl;
}

template<typename Head, typename... Tail>
void __dbg_out(Head head, Tail... tail) {
    std::cerr << __to_string::to_string(head);
    if (sizeof...(tail))
        std::cerr << ", ";

    __dbg_out(tail...);
}

#define dprint(...) __dbg_out(__VA_ARGS__)
#define dbg(...) std::cerr << "#" << __LINE__ << " [" << #__VA_ARGS__ << "]: ", __dbg_out(__VA_ARGS__)
