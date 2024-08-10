/*
Type T require to have:
    - Default constroctor
    - operator+(T, T)
    - operator-(T, T)
*/

template<typename T>
class prefix_sums {
private:
    int n;
    std::vector<T> pref;

public:
    prefix_sums() : n(0), pref(0) {}

    template<typename V>
    prefix_sums(const V &container) : n(container.size()), pref(n + 1) {
        for (int i = 0; i < n; i++) {
            pref[i + 1] = pref[i] + static_cast<T>(container[i]);
        }
    }

    // Returns the sum of the elements on the interval [l, r)
    T query(int l, int r) const {
        assert(l <= r);
        return pref[r] - pref[l];
    }

    int size() const {
        return n;
    }
};
