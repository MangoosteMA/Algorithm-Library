template<typename T, typename A, typename B>
std::vector<T> matroid_intersection(const std::vector<T> &ground_set, const A &matroid1, const B &matroid2) {
    int n = ground_set.size();
    std::vector<char> in_set(n), in_matroid1(n), in_matroid2(n);
    std::vector<bool> used(n);
    std::vector<int> par(n), left, right;
    left.reserve(n);
    right.reserve(n);

    while (true) {
        A m1 = matroid1;
        B m2 = matroid2;
        left.clear();
        right.clear();
        for (int i = 0; i < n; i++)
            if (in_set[i]) {
                m1.add(ground_set[i]);
                m2.add(ground_set[i]);
                left.push_back(i);
            } else {
                right.push_back(i);
            }

        std::fill(in_matroid1.begin(), in_matroid1.end(), 0);
        std::fill(in_matroid2.begin(), in_matroid2.end(), 0);
        bool found = false;
        for (int i : right) {
            in_matroid1[i] = m1.independed_with(ground_set[i]);
            in_matroid2[i] = m2.independed_with(ground_set[i]);
            if (in_matroid1[i] && in_matroid2[i]) {
                in_set[i] = 1;
                found = true;
                break;
            }
        }

        if (found)
            continue;

        std::fill(used.begin(), used.end(), false);
        std::fill(par.begin(), par.end(), -1);
        std::queue<int> que;
        for (int i : right)
            if (in_matroid1[i]) {
                used[i] = true;
                que.push(i);
            }

        while (!que.empty() && !found) {
            int v = que.front();
            que.pop();

            if (in_set[v]) {
                A m = matroid1;
                for (int i : left)
                    if (i != v)
                        m.add(ground_set[i]);

                for (int u : right)
                    if (!used[u] && m.independed_with(ground_set[u])) {
                        par[u] = v;
                        used[u] = true;
                        que.push(u);

                        if (in_matroid2[u]) {
                            found = true;
                            for (; u != -1; u = par[u])
                                in_set[u] ^= 1;

                            break;
                        }
                    }
            } else {
                /*
                  * This part can be optimized by not creating matroid each time.
                  * You can create a matroid B and add to it all elemets from left
                    and element ground[v].
                  * Then you only need to check for all u in left if this matroid
                    without element ground[u] is good.
                  * Possible implementation of this optimization written below.
                    For it you need to implement two extra functions: add_extra
                    and independed_without
                */

                // slow version:
                for (auto u : left) {
                    if (used[u])
                        continue;

                    B m = matroid2;
                    for (int i : left)
                        if (i != u)
                            m.add(ground_set[i]);

                    if (m.independed_with(ground_set[v])) {
                        par[u] = v;
                        used[u] = true;
                        que.push(u);
                    }
                }

                /*
                // fast version:
                B m = m2;
                m.add_extra(ground_set[v]);
                for (auto u : left)
                    if (!used[u] && m.independed_without(ground_set[u])) {
                        par[u] = v;
                        used[u] = true;
                        que.push(u);
                    }
                */
            }
        }
        if (!found)
            break;
    }

    std::vector<T> res;
    for (int i = 0; i < n; i++)
        if (in_set[i])
            res.push_back(ground_set[i]);

    return res;
}

/*
In matroid structure you need to implement two functions:
1. add:
  In this function you need to process adding an element to the current set
  It is guaranteed that after adding this element to the matroid it still will
  be independed
2. independed_with
  In this function you need to query if the set would be independed after inserting
  this element to the current set (but don't need to insert it)

If you want to use an optimizations written above in the matroid intersections
implementation you need to implement two extra functions:
1. add_extra
  In this function you need to process adding one and only one extra element in the current set.
  It is not guaranteed that after this the set would be independed
2. independed_without
  In this funciton you need to say if the set would be independed adter removing this element
  from it. It guaranteed that this function would be called only after add_extra



examples:

struct colorful_matroid {
    vector<int> cnt;  // or map<int, int>
    int cnt_bad;

    colorful_matroid(int max_color) : cnt(max_color), cnt_bad(0) {}

    void add(const item &item) {
        auto x = item.color;
        assert(cnt[x] == 0);
        cnt[x]++;
    }

    bool independed_with(const item &item) const {
        auto x = item.color;
        return cnt[x] == 0;
    }

    void add_extra(const item &item) {
        auto x = item.color;
        cnt_bad += cnt[x] == 1;
        cnt[x]++;
    }

    bool independed_without(const item &item) const {
        auto x = item.color;
        return cnt_bad == 0 || cnt[x] == 2;
    }
};

struct linear_matroid {
    vector<ll> gauss = vector<ll>(64, -1);

    void add(const item &item) {
        auto x = item.value;
        while (x != 0) {
            int b = __lg(x);
            if (gauss[b] == -1) {
                gauss[b] = x;
                return;
            }
            x ^= gauss[b];
        }
        assert(false);
    }

    bool independed_with(const item &item) const {
        auto x = item.value;
        for (int i = 63; i >= 0; i--)
            if (gauss[i] != -1)
                x = min(x, x ^ gauss[i]);

        return x > 0;
    }
};

struct graph_matroid {
    vector<int> par;

    graph_matroid(int n) : par(n) {
        iota(all(par), 0);
    }

    int root(int v) {
        return par[v] == v ? v : par[v] = root(par[v]);
    }

    bool independed_with(const item &item) {
        int v = item.v, u = item.u;
        return root(v) != root(u);
    }

    void add(const item &item) {
        int v = item.v, u = item.u;
        assert(root(v) != root(u));
        par[root(v)] = root(u);
    }
};
*/
