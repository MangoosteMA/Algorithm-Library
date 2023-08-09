/*
 * Include static_modular_int to use it.
 * No need to run any init function.
 * If MOD is not prime inv and ifact won't be correct.
*/

namespace combinatorics {
    std::vector<mint> fact_, ifact_, inv_;

    void reserve(int size) {
        fact_.reserve(size + 1);
        ifact_.reserve(size + 1);
        inv_.reserve(size + 1);
    }

    void resize(int size) {
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
            if (pos >= int(data.size()))
                resize(pos);

            return data[pos];
        }
    } fact(fact_), ifact(ifact_), inv(inv_);

    // From n choose k.
    mint choose(int n, int k) {
        if (n < k || k < 0 || n < 0)
            return mint(0);
 
        return fact[n] * ifact[k] * ifact[n - k];
    }

    // From n choose k.
    // O(min(k, n - k)).
    mint choose_slow(int n, int k) {
        if (n < k || k < 0 || n < 0)
            return mint(0);
        
        k = std::min(k, n - k);
        mint result = 1;
        for (int i = k; i >= 1; i--) {
            result *= (n - i + 1);
            result *= inv[i];
        }
        return result;
    }
}

using combinatorics::fact;
using combinatorics::ifact;
using combinatorics::inv;
using combinatorics::choose;
using combinatorics::choose_slow;
