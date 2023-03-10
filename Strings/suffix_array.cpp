struct suffix_array {
    std::vector<int> order, suffix_position, lcp;

    template<typename T>
    suffix_array(T str = T()) {
        str.push_back(*std::min_element(str.begin(), str.end()) - 1);
        int n = str.size();

        order.resize(n);
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](const int i, const int j) {
            return str[i] < str[j];
        });

        suffix_position.resize(n);
        for (int i = 0; i < n; i++)
            suffix_position[order[i]] = i == 0 ? 0 : suffix_position[order[i - 1]] + (str[order[i]] != str[order[i - 1]]);

        std::vector<int> ptr(n), new_order(n), new_suffix_positoin(n);
        for (int len = 1; suffix_position[order.back()] != n - 1; len <<= 1) {
            std::fill(ptr.begin(), ptr.begin() + suffix_position[order.back()] + 1, 0);
            for (int i = 0; i < n; i++)
                if (suffix_position[i] + 1 < n)
                    ptr[suffix_position[i] + 1]++;

            for (int i = 1; i <= suffix_position[order.back()]; i++)
                ptr[i] += ptr[i - 1];

            for (const int position : order) {
                int suffix = (position - len + n) % n;
                new_order[ptr[suffix_position[suffix]]++] = suffix;
            }
            std::swap(order, new_order);

            for (int i = 0; i < n; i++)
                new_suffix_positoin[order[i]] = i == 0 ? 0 : new_suffix_positoin[order[i - 1]]
                + (suffix_position[order[i]] != suffix_position[order[i - 1]]
                || suffix_position[(order[i] + len) % n] != suffix_position[(order[i - 1] + len) % n]);

            std::swap(suffix_position, new_suffix_positoin);
        }
        assert(order[0] == n - 1);

        lcp.resize(n);
        int current_lcp = 0;
        for (int suffix = 0; suffix < n - 1; suffix++, current_lcp = current_lcp == 0 ? 0 : current_lcp - 1) {
            int previous_suffix = order[suffix_position[suffix] - 1];
            while (str[suffix + current_lcp] == str[previous_suffix + current_lcp])
                current_lcp++;

            lcp[suffix_position[suffix]] = current_lcp;
        }
    }
};
