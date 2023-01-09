inline int max(int a, int b) { return a > b ? a : b; }
inline long long max(long long a, long long b) { return a > b ? a : b; }

inline int min(int a, int b) { return a < b ? a : b; }
inline long long min(long long a, long long b) { return a < b ? a : b; }

inline void swap(int &a, int &b) { a ^= b; b ^= a; a ^= b; }
inline void swap(long long &a, long long &b) { a ^= b; b ^= a; a ^= b; }

inline int gcd(int a, int b) { a = abs(a); b = abs(b); while (b) { a %= b; swap(a, b); } return a; }
inline long long gcd(long long a, long long b) { a = abs(a); b = abs(b); while (b) { a %= b; swap(a, b); } return a; }

inline long long lcm(int a, int b) { return 1ll * a * b / gcd(a, b); }
inline long long lcm(long long a, long long b) { return a * b / gcd(a, b); }
