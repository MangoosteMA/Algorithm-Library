struct Hash2D {
    int mod, p, q;
    vector<vector<int>> pref;
    vector<int> power_p, power_q;

    Hash2D() {}
    template<typename T>
    Hash2D(vector<T> s, int mod, int p, int q) : mod(mod), p(p), q(q) {
        int n = int(s.size());
        int m = int(s[0].size());
        power_p.resize(n + 1, 1);
        for (int i = 1; i <= n; i++)
            power_p[i] = mult(power_p[i - 1], p);
        power_q.resize(m + 1, 1);
        for (int i = 1; i <= m; i++)
            power_q[i] = mult(power_q[i - 1], q);
        pref.resize(n + 1, vector<int>(m + 1));
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= m; j++)
                pref[i][j] = add(s[i - 1][j - 1], sub(add(mult(pref[i - 1][j], p), mult(pref[i][j - 1], q)), mult(pref[i - 1][j - 1], mult(p, q))));
    }

    int add(int a, int b) { a += b; return a - mod * (a >= mod); }
    int sub(int a, int b) { a -= b; return a + mod * (a < 0); }
    int mult(int a, int b) { return 1ll * a * b % mod; }

    int get_hash(int x1, int y1, int x2, int y2) {
        int res = pref[x2][y2];
        res = sub(res, mult(pref[x1][y2], power_p[x2 - x1]));
        res = sub(res, mult(pref[x2][y1], power_q[y2 - y1]));
        res = add(res, mult(pref[x1][y1], mult(power_p[x2 - x1], power_q[y2 - y1])));
        return res;
    }
};
