namespace sieve {
    std::vector<int> primes, smallest_factor;

    void init(int n) {
        smallest_factor.resize(n + 1);
        std::iota(smallest_factor.begin(), smallest_factor.end(), 0);
        for (int i = 2; i <= n; i++) {
            if (smallest_factor[i] == i)
                primes.push_back(i);

            for (int j = 0; j < int(primes.size()) && primes[j] <= smallest_factor[i] && primes[j] * i <= n; j++)
                smallest_factor[primes[j] * i] = primes[j];
        }
    }

    bool is_prime(int n) {
        if (n <= 1)
            return false;

        return smallest_factor[n] == n;
    }

    std::vector<std::pair<int, int>> get_prime_factors(int n) {
        std::vector<std::pair<int, int>> factors;
        while (n != 1) {
            int prime = smallest_factor[n];
            factors.emplace_back(prime, 0);

            while (smallest_factor[n] == prime) {
                factors.back().second++;
                n /= prime;
            }
        }
        return factors;
    }

    std::vector<int> get_all_factors(int value) {
        std::vector<int> divs{1};
        for (auto [p, d] : get_prime_factors(value)) {
            int prev_size = divs.size();
            for (int i = 0; i < prev_size; i++) {
                int coeff = 1;
                for (int j = 0; j < d; j++) {
                    coeff *= p;
                    divs.push_back(divs[i] * coeff);
                }
            }
        }

        sort(divs.begin(), divs.end());
        return divs;
    }
} // namespace sieve

using sieve::primes;
using sieve::smallest_factor;
using sieve::is_prime;
using sieve::get_prime_factors;
using sieve::get_all_factors;
