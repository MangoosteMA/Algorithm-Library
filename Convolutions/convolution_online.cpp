/*
 * T is a polynom-like class.
 * Initially f = 0 and g = 0.
 * Calling .push_back(a, b) makes:
       f := f + a * x^{degree(f) + 1}
       g := g + b * x^{degree(g) + 1}
 * .query(i) return [x^i](f * g). Require: i <= degree(f).
 */
template<typename T>
class convolution_online {
private:
    int degree = 0;
    T prod, f, g, aux_f, aux_g;
    std::vector<T> fft_f, fft_g;

    void expand(int degree) {
        if (static_cast<int>(prod.size()) <= degree) {
            prod.resize(degree + 1);
        }
    }

public:
    void push_back(typename T::value_type a, typename T::value_type b) {
        f.push_back(a);
        g.push_back(b);
        expand(degree);
        prod[degree] += f[0] * g[degree] + (degree == 0 ? 0 : f[degree] * g[0]);
        degree++;

        for (int size = 1, l = 0; size <= degree; size <<= 1, l++) {
            if ((degree & ((size << 1) - 1)) != size) {
                continue;
            }
            if (size == degree) {
                if (size == 1) {
                    continue;
                }
                T cur = T(f.begin(), f.end()) * T(g.begin(), g.end());
                expand(degree + size - 1);
                for (int i = 0; i < size - 1; i++) {
                    prod[degree + i] += cur[degree + i];
                }
            } else {
                if (degree == 3 * size) {
                    fft_f.emplace_back(f.begin(), f.begin() + (size << 1));
                    fft_g.emplace_back(g.begin(), g.begin() + (size << 1));
                    T::fft(fft_f.back());
                    T::fft(fft_g.back());
                }

                aux_f.assign(size << 1, 0);
                aux_g.assign(size << 1, 0);
                std::copy(f.end() - size, f.end(), aux_f.begin());
                std::copy(g.end() - size, g.end(), aux_g.begin());
                T::fft(aux_f);
                T::fft(aux_g);
                for (int i = 0; i < (size << 1); i++) {
                    aux_f[i] = fft_g[l][i] * aux_f[i] + fft_f[l][i] * aux_g[i];
                }
                T::inv_fft(aux_f);

                expand(degree + size - 1);
                for (int i = 0; i < size; i++) {
                    prod[degree + i] += aux_f[size + i];
                }
            }
        }
    }

    typename T::value_type query(int i) const {
        assert(i < degree);
        return prod[i];
    }
};
