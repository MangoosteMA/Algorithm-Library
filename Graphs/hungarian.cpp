template<typename T>
struct hungarian {
    int n;
    std::vector<std::vector<T>> mat;
    std::vector<int> matched_with_row, matched_with_col;

    hungarian(int n = 0, T value = std::numeric_limits<T>::max()) :
        n(n),
        mat(n, std::vector<T>(n, value)),
        matched_with_row(n, -1),
        matched_with_col(n, -1)
    {}

    hungarian(const std::vector<std::vector<T>> &a) :
        n(a.size()),
        mat(a),
        matched_with_row(n, -1),
        matched_with_col(n, -1)
    {}

    std::vector<T>& operator[](size_t i) {
        return mat[i];
    }

    const std::vector<T>& operator[](size_t i) const {
        return mat[i];
    }

    int size() const {
        return n;
    }

    template<typename result_t>
    result_t min_cost() {
        std::fill(matched_with_row.begin(), matched_with_row.end(), -1);
        std::fill(matched_with_col.begin(), matched_with_col.end(), -1);

        std::vector<T> add_to_row(n), add_to_col(n);
        std::vector<bool> used_col(n);
        std::vector<int> parent_row(n);
        std::vector<std::pair<T, int>> minimum_in_col(n);

        for (int row = 0; row < n; row++) {
            std::fill(used_col.begin(), used_col.end(), false);
            std::fill(minimum_in_col.begin(), minimum_in_col.end(), std::pair{std::numeric_limits<T>::max(), -1});
            std::fill(parent_row.begin(), parent_row.end(), -1);
            std::vector<int> rows{row};

            while (true) {
                int row = rows.back(), col = -1;
                for (int i = 0; i < n; i++)
                    if (!used_col[i]) {
                        minimum_in_col[i] = std::min(minimum_in_col[i],
                            std::pair{mat[row][i] + add_to_row[row] + add_to_col[i], row});

                        if (col == -1 || minimum_in_col[i] < minimum_in_col[col])
                            col = i;
                    }

                assert(col != -1);
                T minimum_value;
                std::tie(minimum_value, row) = minimum_in_col[col];

                for (int i = 0; i < n; i++)
                    if (used_col[i])
                        add_to_col[i] += minimum_value;
                    else
                        minimum_in_col[i].first -= minimum_value;

                for (auto row : rows)
                    add_to_row[row] -= minimum_value;

                if (matched_with_col[col] == -1) {
                    for (; row != -1; row = parent_row[row]) {
                        matched_with_col[col] = row;
                        std::swap(col, matched_with_row[row]);
                    }
                    break;
                }

                rows.push_back(matched_with_col[col]);
                parent_row[matched_with_col[col]] = row;
                used_col[col] = true;
            }
        }

        if constexpr (std::is_same<result_t, void>::value)
            return void();
        else
            return -std::accumulate(add_to_row.begin(), add_to_row.end(), result_t(0))
                  - std::accumulate(add_to_col.begin(), add_to_col.end(), result_t(0));
    }
};
