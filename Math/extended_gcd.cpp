// Returns {gcd, x, y} such that x * a + y * b = gcd.
template<typename T>
std::array<T, 3> extgcd(T a, T b) {
    if (b == 0)
        return {a, 1, 0};

    auto [g, x, y] = extgcd(b, a % b);
    return {g, y, x - y * (a / b)};
}
