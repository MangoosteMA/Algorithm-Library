template<typename W_t, typename T, typename A, typename B>
std::vector<std::vector<T>> weighted_matroid_intersection(const std::vector<T> &ground_set, const A &matroid1, const B &matroid2) {
    int n = ground_set.size();
    std::vector<std::vector<T>> res;
    std::vector<char> in_set(n), in_matroid1(n), in_matroid2(n);
    std::vector<std::pair<W_t, int>> dist(n);
    std::vector<std::pair<int, int>> edges;
    std::vector<int> par(n), left, right;
    left.reserve(n);
    right.reserve(n);

    while (true) {
        res.push_back({});
        for (int i = 0; i < n; i++)
            if (in_set[i])
                res.back().push_back(ground_set[i]);

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
        for (int i : right) {
            in_matroid1[i] = m1.independed_with(ground_set[i]);
            in_matroid2[i] = m2.independed_with(ground_set[i]);
        }

        edges.clear();
        for (int i : left) {
            {
                A m = matroid1;
                for (int j : left)
                    if (i != j)
                        m.add(ground_set[j]);

                for (int j : right)
                    if (m.independed_with(ground_set[j]))
                        edges.emplace_back(i, j);
            }
            {
                B m = matroid2;
                for (int j : left)
                    if (i != j)
                        m.add(ground_set[j]);

                for (int j : right)
                    if (m.independed_with(ground_set[j]))
                        edges.emplace_back(j, i);
            }
        }

        static constexpr W_t INF = std::numeric_limits<W_t>::max();
        std::fill(dist.begin(), dist.end(), std::pair{INF, -1});
        std::fill(par.begin(), par.end(), -1);
        for (int i : right)
            if (in_matroid1[i])
                dist[i] = {-ground_set[i].weight, 0};

        while (true) {
            bool any = false;
            for (auto &[v, u] : edges) {
                if (dist[v].first == INF)
                    continue;

                int coeff = in_set[v] ? -1 : 1;
                if (dist[u] > std::pair{dist[v].first + coeff * ground_set[u].weight, dist[v].second + 1}) {
                    par[u] = v;
                    dist[u] = {dist[v].first + coeff * ground_set[u].weight, dist[v].second + 1};
                    any = true;
                }
            }
            if (!any)
                break;
        }

        int finish = -1;
        for (int v : right)
            if (in_matroid2[v] && dist[v].first != INF && (finish == -1 || dist[finish] > dist[v]))
                finish = v;

        if (finish == -1)
            break;

        for (; finish != -1; finish = par[finish])
            in_set[finish] ^= 1;
    }
    return res;
}
