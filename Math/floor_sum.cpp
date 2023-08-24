using ll = long long;

ll sumsq(ll n) {
    return n / 2 * ((n - 1) | 1);
}

// \sum_{i = 0}^{n - 1}{(a + d * i) / m}
// O(log m)
ll floor_sum(ll a, ll d, ll m, ll n) {
    ll res = d / m * sumsq(n) + a / m * n;
    d %= m;
    a %= m;
    if (!d)
        return res;

    ll to = (n * d + a) / m;
    return res + (n - 1) * to - floor_sum(m - 1 - a, m, d, to);
}

// \sum_{i = 0}^{n - 1}{(a + d * i) % m}
// O(log m)
ll mod_sum(ll a, ll d, ll m, ll n) {
    a = ((a % m) + m) % m;
    d = ((d % m) + m) % m;
    return n * a + d * sumsq(n) - m * floor_sum(a, d, m, n);
}


// source: https://asfjwd.github.io/2020-04-24-floor-sum-ap/

struct dat {
  long long f, g, h;
  dat(long long f = 0, long long g = 0, long long h = 0) : f(f), g(g), h(h) {};
};

long long mul(long long a, long long b){
  return (a * b) % MOD;
}

dat query(long long a, long long b, long long c, long long n){
  if(!a) return {mul(n + 1, b / c), mul(mul(mul(b / c, n), n + 1), inv2), mul(mul(n + 1, b / c), b /c)};
  long long f, g, h; 
  dat nxt;
  if(a >= c or b >= c){
    nxt = query(a % c, b % c, c, n);
    f = (nxt.f + mul(mul(mul(n, n + 1), inv2), a / c) + mul(n + 1, b / c)) % MOD;
    g = (nxt.g + mul(a / c, mul(mul(n, n + 1), mul(2 * n + 1, inv6))) + mul(mul(b / c, mul(n, n + 1)), inv2)) % MOD;
    h = (nxt.h + 2 * mul(b / c, nxt.f) + 2 * mul(a / c, nxt.g) + mul(mul(a / c, a / c), mul(mul(n, n + 1), mul(2 * n + 1, inv6))) + mul(mul(b / c, b / c), n + 1) + mul(mul(a / c, b / c), mul(n, n + 1)) ) % MOD;   
    return {f, g, h};
  }
  long long m = (a * n + b ) / c;
  nxt = query(c, c - b - 1, a, m - 1);
  f = (mul(m, n) - nxt.f) % MOD;
  g = mul( mul(m, mul(n, n + 1)) - nxt.h - nxt.f, inv2);
  h = (mul(n, mul(m, m + 1)) - 2 * nxt.g - 2 * nxt.f - f) % MOD;
  return {f, g, h};
}

// f = \sum_{x = 0}^{n}{floor((ax + b) / c)}
// g = \sum_{x = 0}^{n}{x * floor((ax + b) / c)}
// h = \sum_{x = 0}^{n}{floor((ax + b) / c)^2}

struct dat{
  ll F[11][11];
};

dat F(long long k1, long long k2, long long a, long long b, long long c, long long n){
  if(!a){
    dat ret;
    for(int _k1 = 0; _k1 <= k1 + k2; ++_k1){
      ret.F[_k1][0] = powerSum(n, _k1);
      for(int _k2 = 1; _k1 + _k2 <= k1 + k2; ++_k2){
        ret.F[_k1][_k2] = (ret.F[_k1][0] * bigMod(b / c, _k2)) % MOD;
      }
    }
    return ret;
  }

  dat ret;

  if(a >= c){
    dat nxt = F(k1, k2, a % c, b, c, n);
    for(int _k1 = 0; _k1 <= k1 + k2; ++_k1){
      ret.F[_k1][0] = powerSum(n, _k1);
      for(int _k2 = 1; _k1 + _k2 <= k1 + k2; ++_k2){
        ret.F[_k1][_k2] = 0;
        for(int y = 0; y <= _k2; ++y){
          ret.F[_k1][_k2] += ((ncr[_k2][y] * bigMod(a / c, y) ) % MOD) * nxt.F[_k1 + y][_k2 - y];
          ret.F[_k1][_k2] %= MOD;
        }
      }
    }
  }

  else if(b >= c){
    dat nxt = F(k1, k2, a, b % c, c, n);
    for(int _k1 = 0; _k1 <= k1 + k2; ++_k1){
      ret.F[_k1][0] = powerSum(n, _k1);
      for(int _k2 = 1; _k1 + _k2 <= k1 + k2; ++_k2){
        ret.F[_k1][_k2] = 0;
        for(int y = 0; y <= _k2; ++y){
          ret.F[_k1][_k2] += ((ncr[_k2][y] * bigMod(b / c, y) ) % MOD) * nxt.F[_k1][_k2 - y];
          ret.F[_k1][_k2] %= MOD;
        }
      }
    }
  }

  else {
    long long m = (a * n + b) / c;
    dat nxt = F(k1, k2, c, c - b - 1, a, m - 1);
    std::vector<long long> psumM(k1 + k2 + 1), pre(k1 + k2 + 1);
    for(int i = 0; i <= k1 + k2; ++i)psumM[i] = powerSum(m - 1, i);
    for(int _k1 = 0; _k1 <= k1 + k2; ++_k1){
      for(int i = 0; i <= k1 + k2; ++i){
        pre[i] = 0;
        for(int j = 0; j <= _k1 + 1; ++j){
          pre[i] += (d[_k1][j] * nxt.F[i][j]) % MOD;
          pre[i] %= MOD;
        }
      }
      ret.F[_k1][0] = powerSum(n, _k1);
      for(int _k2 = 1; _k1 + _k2 <= k1 + k2; ++_k2){
        ret.F[_k1][_k2] = 0;
        for(int i = 0; i < _k2; ++i){
          ret.F[_k1][_k2] += ( (ncr[_k2][i] * psumM[i]) % MOD) * ret.F[_k1][0];
          ret.F[_k1][_k2] -= ncr[_k2][i] * pre[i];
          ret.F[_k1][_k2] %= MOD;
        }
      }
    }
  }
  return ret;
}

// ret[k1][k2] = \sum_{x = 0}^{n}{x^{k1} * floor((ax + b) / c) ^ k2}
