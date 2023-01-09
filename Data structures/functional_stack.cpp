template<typename T, typename merge_t>
struct functional_stack {
    std::vector<std::pair<T, T>> st;
    const merge_t merge;

    functional_stack(const merge_t &merge) : merge(merge) {}

    void reserve(size_t n) {
        st.reserve(n);
    }

    int size() const {
        return st.size();
    }

    bool empty() const {
        return st.empty();
    }

    void push(T x) {
        st.emplace_back(x, st.empty() ? x : merge(st.back().second, x));
    }

    void pop() {
        assert(!st.empty());
        st.pop_back();
    }

    T query() const {
        return st.back().second;
    }

    void reverse() {
        std::reverse(st.begin(), st.end());
        for (int i = 0; i < int(st.size()); i++)
            st[i].second = i == 0 ? st[i].first : merge(st[i - 1].second, st[i].first);
    }
};
