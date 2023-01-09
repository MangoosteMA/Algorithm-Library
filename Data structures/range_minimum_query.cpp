template<typename T, typename cmp_t>
struct range_minimum_query {
    static constexpr int BLOCK = 64, BLOCK_LOG = std::__lg(BLOCK);

    cmp_t cmp;
    std::vector<std::vector<T>> sparse;
    std::vector<T> values;
    std::vector<uint64_t> states;

    range_minimum_query(const cmp_t &cmp) : cmp(cmp) {}

    range_minimum_query(const std::vector<T> &a, const cmp_t &cmp) : cmp(cmp), values(a), states(a.size()) {
        const int n = int(a.size()), sparse_size = (n + BLOCK - 1) >> BLOCK_LOG, lg = std::__lg(sparse_size);
        sparse.reserve(lg + 1);
        sparse.push_back(std::vector<T>(sparse_size));

        for (int i = 0; i < n; i += BLOCK) {
            T cur_min = a[i];
            for (int j = i + 1; j < i + BLOCK && j < n; j++)
                if (cmp(a[j], cur_min))
                    cur_min = a[j];

            sparse[0][i >> BLOCK_LOG] = cur_min;
        }

        for (int level = 1; level <= lg; level++) {
            sparse.push_back(std::vector<T>(sparse_size - (1 << level) + 1));
            for (int i = 0; i < int(sparse[level].size()); i++)
                sparse[level][i] = cmp(sparse[level - 1][i], sparse[level - 1][i + (1 << (level - 1))])
                    ? sparse[level - 1][i] : sparse[level - 1][i + (1 << (level - 1))];
        }

        std::vector<int> st;
        st.reserve(BLOCK);
        uint64_t cur_mask;
        for (int i = 0, block_start; i < n; i++) {
            if (!(i & (BLOCK - 1))) {
                st.clear();
                cur_mask = 0;
                block_start = i;
            }

            while (!st.empty() && !cmp(a[st.back()], a[i])) {
                cur_mask ^= uint64_t(1) << (st.back() - block_start);
                st.pop_back();
            }

            st.push_back(i);
            cur_mask ^= uint64_t(1) << (i - block_start);
            states[i] = cur_mask;
        }
    }

    T query(int l, int r) const {
        assert(l < r);
        r--;

        static int block_start, id;
        static T value;

        if ((l >> BLOCK_LOG) == (r >> BLOCK_LOG)) {
            block_start = l >> BLOCK_LOG << BLOCK_LOG;
            id = l - block_start;
            return values[block_start + __builtin_ctzll(states[r] >> id << id)];
        }

        T res;
        bool relaxed = false;

        if (l & (BLOCK - 1)) {
            block_start = l >> BLOCK_LOG << BLOCK_LOG;
            id = l - block_start;
            value = values[block_start + __builtin_ctzll(states[block_start | (BLOCK - 1)] >> id << id)];
            if (!relaxed || cmp(value, res))
                res = value, relaxed = true;

            l += BLOCK;
        }

        if ((r + 1) & (BLOCK - 1)) {
            value = values[(r >> BLOCK_LOG << BLOCK_LOG) + __builtin_ctzll(states[r])];
            if (!relaxed || cmp(value, res))
                res = value, relaxed = true;

            r -= BLOCK;
        }

        l >>= BLOCK_LOG;
        r >>= BLOCK_LOG;
        r++;

        if (l < r) {
            const int level = std::__lg(r - l);
            value = cmp(sparse[level][l], sparse[level][r - (1 << level)])
                ? sparse[level][l] : sparse[level][r - (1 << level)];

            if (!relaxed || cmp(value, res))
                res = value, relaxed = true;
        }

        return res;
    }
};
