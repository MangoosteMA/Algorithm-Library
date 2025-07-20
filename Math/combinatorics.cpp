/*
 ! WARNING: MOD must be prime.
 * Define modular int class above it.
 * No need to run any init function, it dynamically resizes the data.
 */
namespace combinatorics {
    std::vector<mint> fact_, ifact_, inv_;

    void resize_data(int size) {
        if (fact_.empty()) {
            fact_ = {mint(1), mint(1)};
            ifact_ = {mint(1), mint(1)};
            inv_ = {mint(0), mint(1)};
        }
        for (int pos = fact_.size(); pos <= size; pos++) {
            fact_.push_back(fact_.back() * mint(pos));
            inv_.push_back(-inv_[MOD % pos] * mint(MOD / pos));
            ifact_.push_back(ifact_.back() * inv_[pos]);
        }
    }

    struct combinatorics_info {
        std::vector<mint> &data;

        combinatorics_info(std::vector<mint> &data) : data(data) {}

        mint operator[](int pos) {
            if (pos >= static_cast<int>(data.size())) {
                resize_data(pos);
            }
            return data[pos];
        }
    } fact(fact_), ifact(ifact_), inv(inv_);

    // From n choose k.
    // O(max(n)) in total.
    mint choose(int n, int k) {
        if (n < k || k < 0 || n < 0) {
            return mint(0);
        }
        return fact[n] * ifact[k] * ifact[n - k];
    }

    // From n choose k.
    // O(min(k, n - k)).
    mint choose_slow(int64_t n, int64_t k) {
        if (n < k || k < 0 || n < 0) {
            return mint(0);
        }
        k = std::min(k, n - k);
        mint result = 1;
        for (int i = k; i >= 1; i--) {
            result *= (n - i + 1);
            result *= inv[i];
        }
        return result;
    }

    // Number of balanced bracket sequences with n open and m closing brackets.
    mint catalan(int n, int m) {
        if (m > n || m < 0) {
            return mint(0);
        }
        return choose(n + m, m) - choose(n + m, m - 1);
    }

    // Number of balanced bracket sequences with n open and closing brackets.
    mint catalan(int n) {
        return catalan(n, n);
    }

    // Number of balanced bracket sequences with n open and m closing brackets
    // with initial_balance equak to bal
    mint catalan_init_bal(int n, int m, int bal) {
        return choose(n + m, n) - choose(n + m, n + bal + 1);
    }

    // Number of possible not balanced bracket sequences with n open and m closing brackets
    // with balance <= max_bal
    mint catalan_max_bal(int n, int m, int max_bal) {
        return catalan_init_bal(m, n, max_bal);
    }
} // namespace combinatorics

using namespace combinatorics;
