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

    std::vector<std::pair<int, int>> get_factors(int n) {
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
} // namespace sieve

using sieve::primes;
using sieve::smallest_factor;
using sieve::is_prime;
using sieve::get_factors;
