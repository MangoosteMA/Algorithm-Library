// TODO: add deteminant and inverse matrix

template<typename T>
struct matrix {
    std::vector<std::vector<T>> mat;
    
    matrix() {}
    matrix(const std::vector<std::vector<T>> &mat) : mat(mat) {}
    matrix(const matrix<T> &m) : mat(m.mat) {}

    matrix(int n) : mat(n, std::vector<T>(n)) {}
    matrix(int n, int m) : mat(n, std::vector<T>(m)) {}

    std::vector<T>& operator[](size_t i) {
        return mat[i];
    }

    const std::vector<T>& operator[](size_t i) const {
        return mat[i];
    }

    const int size() const {
        return mat.size();
    }

    const std::pair<int, int> shape() const {
        return mat.empty() ? std::pair<int, int>{0, 0} : std::pair<int, int>{mat.size(), mat[0].size()};
    }

    matrix<T>& operator*=(const matrix &m) {
        auto [n1, n2] = shape();
        auto [m1, m2] = m.shape();
        assert(n2 == m1);
        std::vector<std::vector<T>> new_mat(n1, std::vector<T>(m2));
        for (int i = 0; i < n1; i++)
            for (int j = 0; j < m2; j++)
                for (int k = 0; k < n2; k++)
                    new_mat[i][j] += mat[i][k] * m.mat[k][j];

        mat = new_mat;
        return *this;
    }

    friend matrix<T> operator*(const matrix<T> &a, const matrix<T> &b) {
        return matrix<T>(a) *= b;
    }

    template<typename U>
    matrix<T> power(U deg) const {
        assert(deg >= 0);
        auto [n, m] = shape();
        assert(n == m);

        auto a = *this;
        matrix<T> prod(n, n);
        for (int i = 0; i < n; i++)
            prod[i][i] = 1;

        for (; deg; deg >>= 1) {
            if (deg & 1)
                prod *= a;

            a *= a;
        }
        return prod;
    }
};
