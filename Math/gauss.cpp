template<typename T, typename is_zero_t>
struct gauss {
    std::vector<std::vector<T>> mat;
    is_zero_t is_zero;

    gauss(is_zero_t is_zero) : is_zero(is_zero) {}
    gauss(int n, is_zero_t is_zero) : mat(n, std::vector<T>(n + 1)), is_zero(is_zero) {}

    std::vector<T>& operator[](size_t i) {
        return mat[i];
    }

    const std::vector<T>& operator[](size_t i) const {
        return mat[i];
    }

    int size() const {
        return mat.size();
    }

    void transform() {
        std::vector<bool> used(int(mat.size()));
        for (int col = 0; col < int(mat.size()); col++) {
            int row = 0;
            while (row < int(mat.size()) && (used[row] || is_zero(mat[row][col])))
                row++;

            if (row == int(mat.size()))
                continue;

            std::swap(row, col);
            used[col] = true;
            for (int i = 0; i < int(mat.size()); i++)
                if (i != col) {
                    T coeff = mat[i][col] / mat[col][col];
                    if (is_zero(coeff))
                        continue;

                    for (int j = col; j <= int(mat.size()); j++)
                        mat[i][j] -= mat[col][j] * coeff;
                }
        }
    }

    std::vector<T> solutions() {
        transform();
        std::vector<T> res(mat.size());
        for (int i = 0; i < int(mat.size()); i++)
            if (!is_zero(mat[i][i]))
                res[i] = -mat[i].back() / mat[i][i];

        return res;
    }
};
