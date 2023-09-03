/*
 * T is a type of lines parameters.
 * Gets lines in the k-increasing order.
*/

template<typename T>
struct convex_hull_trick_max {
    struct line {
        T k, b;

        T intersect(const line &another) {
            if (k == another.k)
                return b >= another.b ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min();

            auto floor_div = [](T a, T b) {
                if (a >= 0)
                    return a / b;

                return -((-a + b - 1) / b);
            };

            assert(k < another.k);
            return floor_div(b - another.b, another.k - k);
        }
    };

    std::vector<line> hull;
    std::vector<T> coords;

    void add(T k, T b) {
        line new_line{k, b};
        while (!coords.empty() && coords.back() >= hull.back().intersect(new_line)) {
            coords.pop_back();
            hull.pop_back();
        }

        if (hull.size() == 1 && hull.back().k == k && hull.back().b <= b)
            hull.pop_back();

        if (hull.empty() || hull.back().k != k) {
            if (!hull.empty())
                coords.push_back(hull.back().intersect(new_line));

            hull.push_back({k, b});
        }
    }

    template<typename result_t>
    result_t query(T x) {
        int best_line = lower_bound(coords.begin(), coords.end(), x) - coords.begin();
        return result_t(hull[best_line].k) * x + hull[best_line].b;
    }
};

/*
 * T is a type of lines parameters.
 * Gets lines in the k-decreasing order.
*/

template<typename T>
struct convex_hull_trick_min {
    convex_hull_trick_max<T> hull;

    void add(T k, T b) {
        hull.add(-k, -b);
    }

    template<typename result_t>
    result_t query(T x) {
        return -(hull.template query<result_t>(x));
    }
};
