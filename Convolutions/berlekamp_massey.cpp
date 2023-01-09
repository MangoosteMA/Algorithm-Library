namespace berlekamp_massey {
    template<typename T>
    std::vector<T> find_recurrence(const std::vector<T> &values) {
        if (values.empty())
            return {};

        std::vector<T> add_option{}, recurrence{};
        int add_position = -1;
        T add_value = values[1];
        for (int i = 0; i < int(values.size()); i++) {
            T value = 0;
            for (int j = 0; j < int(recurrence.size()); j++)
                value += values[i - j - 1] * recurrence[j];

            T delta = values[i] - value;
            if (delta == 0)
                continue;

            std::vector<T> new_recurrence;
            if (add_position == -1) {
                new_recurrence.assign(i + 1, 0);
            } else {
                new_recurrence = add_option;
                std::reverse(new_recurrence.begin(), new_recurrence.end());
                new_recurrence.push_back(-1);
                for (int it = 0; it < i - add_position - 1; it++)
                    new_recurrence.push_back(0);
                
                std::reverse(new_recurrence.begin(), new_recurrence.end());
                T coeff = -delta / add_value;
                for (auto &x : new_recurrence)
                    x *= coeff;

                if (new_recurrence.size() < recurrence.size())
                    new_recurrence.resize(recurrence.size());

                for (int i = 0; i < int(recurrence.size()); i++)
                    new_recurrence[i] += recurrence[i];
            }

            if (i - int(recurrence.size()) >= add_position - int(add_option.size()) || add_position == -1) {
                add_option = recurrence;
                add_position = i;
                add_value = delta;
            }
            recurrence = new_recurrence;
        }
        return recurrence;
    }

    template<typename T>
    std::vector<T> multiply(const std::vector<T> &first, const std::vector<T> &second) {
        // can be replaced with fft multiplication
        std::vector<T> prod(max(0, int(first.size() + second.size()) - 1));
        for (int i = 0; i < int(first.size()); i++)
            for (int j = 0; j < int(second.size()); j++)
                prod[i + j] += first[i] * second[j];

        return prod;
    }

    template<typename T>
    std::vector<T> find_remainder(std::vector<T> first, const std::vector<T> &second) {
        // can be replaced with fft division
        if (second.empty())
            return {};

        assert(second.back() != 0);
        while (!first.empty() && first.size() >= second.size()) {
            if (first.back() == 0) {
                first.pop_back();
                continue;
            }
            T coeff = first.back() / second.back();
            for (int i = 0; i < int(second.size()); i++)
                first[int(first.size()) - int(second.size()) + i] -= coeff * second[i];
        }
        return first;
    }

    template<typename T>
    std::vector<T> find_remainder(std::vector<T> recurrence, int64_t k) {
        while (!recurrence.empty() && recurrence.back() == 0)
            recurrence.pop_back();

        std::vector<T> result{1}, value{0, 1};
        for (; k; k >>= 1) {
            if (k & 1)
                result = find_remainder(multiply(result, value), recurrence);

            value = find_remainder(multiply(value, value), recurrence);
        }
        return result;
    }

    template<typename T>
    T find_kth(const std::vector<T> &values, std::vector<T> recurrence, int64_t k) {
        std::reverse(recurrence.begin(), recurrence.end());
        recurrence.push_back(-1);
        std::vector<T> poly = find_remainder(recurrence, k);
        T result = 0;
        for (int i = 0; i < int(poly.size()); i++)
            result += poly[i] * values[i];

        return result;
    }
} // berlekamp_massey
