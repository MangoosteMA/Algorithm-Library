using int128 = __int128;

istream& operator>>(istream &in, int128 &x) {
    string y; in >> y;
    x = 0;
    bool neg = false;
    if (y[0] == '-') neg = true, y.erase(y.begin());
    for (auto c : y) x = ((x + (x << 2)) << 1) + c - '0';
    if (neg) x = -x;
    return in;
}

ostream& operator<<(ostream &out, int128 x) {
    if (x == 0) return out << "0";
    if (x < 0) x = -x, out << '-';
    string s;
    for (; x; x /= 10) s.push_back(x % 10 + '0');
    reverse(all(s));
    return out << s;
}

inline int128 abs(int128 x) { return x < 0 ? -x : x; }
inline void swap(int128 &a, int128 &b) { a ^= b, b ^= a, a ^= b; }
inline int128 gcd(int128 a, int128 b) { a = abs(a); b = abs(b); for (; b; swap(a, b)) a %= b; return a; }
inline int128 lcm(int128 a, int128 b) { return a * b / gcd(a, b); }
