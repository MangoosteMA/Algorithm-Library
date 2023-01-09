template<typename T>
struct directed_graph : public graph<T> {
    using graph<T>::n;
    using graph<T>::edges;
    using graph<T>::g;

    directed_graph(int n = 0) : graph<T>(n) {}

    int add(int from, int to, T cost = T(1)) {
        assert(0 <= from && from < n && 0 <= to && to < n);
        int id = int(edges.size());
        g[from].push_back(id);
        edges.push_back({from, to, cost});
        return id;
    }

    void pop_back() {
        assert(!edges.empty() && !g[edges.back().from].empty() && g[edges.back().from].back() == int(edges.size()) - 1);
        g[edges.back().from].pop_back();
        edges.pop_back();
    }

    directed_graph<T> reversed() const {
        directed_graph<T> rev(n);
        for (const auto &[from, to, cost] : edges)
            rev.add(to, from, cost);

        return rev;
    }

    void reverse() {
        *this = reversed();
    }

    template<typename U>
    std::vector<U> dijkstra(const std::vector<int> &initial) const {
        std::vector<U> dist(n, std::numeric_limits<U>::max());
        std::priority_queue<std::pair<U, int>, std::vector<std::pair<U, int>>, std::greater<>> pq;
        for (const int v : initial) {
            dist[v] = 0;
            pq.push({0, v});
        }

        while (!pq.empty()) {
            const auto [cur_dist, v] = pq.top();
            pq.pop();
            if (cur_dist != dist[v])
                continue;

            for (const int id : g[v]) {
                int u = edges[id].to;
                T cost = edges[id].cost;
                if (dist[u] > dist[v] + cost) {
                    dist[u] = dist[v] + cost;
                    pq.push({dist[u], u});
                }
            }
        }
        return dist;
    }

    template<typename U>
    std::vector<U> bfs(const std::vector<int> &initial) const {
        std::vector<U> dist(n, std::numeric_limits<U>::max());
        std::vector<int> que;
        for (const int v : initial) {
            dist[v] = 0;
            que.push_back(v);
        }

        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (const int id : g[v]) {
                int u = edges[id].to;
                if (dist[u] > dist[v] + 1) {
                    dist[u] = dist[v] + 1;
                    que.push_back(u);
                }
            }
        }
        return dist;
    }

    std::vector<int> topsort() const {
        std::vector<int> degree(n);
        for (const auto &[from, to, cost] : edges)
            degree[to]++;

        std::vector<int> que;
        for (int v = 0; v < n; v++)
            if (degree[v] == 0)
                que.push_back(v);

        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (const int id : g[v]) {
                int u = edges[id].to;
                if (--degree[u] == 0)
                    que.push_back(u);
            }
        }

        if (int(que.size()) != n)
            return {};

        return que;
    }

    std::vector<int> order() const {
        std::vector<int> order;
        order.reserve(n);
        std::vector<bool> used(n);

        std::function<void(int v)> dfs = [&](int v) {
            used[v] = true;
            for (const int id : g[v])
                if (!used[edges[id].to])
                    dfs(edges[id].to);

            order.push_back(v);
        };

        for (int v = 0; v < n; v++)
            if (!used[v])
                dfs(v);

        std::reverse(order.begin(), order.end());
        return order;
    }

    std::vector<int> scc() const {
        std::vector<int> where(n, n);
        int current_id = 0;
        directed_graph<T> rev = reversed();

        std::function<void(int v)> dfs = [&](int v) {
            where[v] = current_id;
            for (const int id : rev.g[v])
                if (where[rev.edges[id].to] == n)
                    dfs(rev.edges[id].to);
        };

        for (const int v : order())
            if (where[v] == n) {
                dfs(v);
                current_id++;
            }

        return where;
    }

    directed_graph<T> build_dag(const std::vector<int> &where, bool multipy_edges) const {
        if (n == 0)
            return directed_graph<T>();

        directed_graph<T> dag(*std::max_element(where.begin(), where.end()) + 1);
        if (multipy_edges) {
            for (const auto &[from, to, cost] : edges)
                if (where[from] != where[to])
                    dag.add(where[from], where[to], cost);
        } else {
            std::vector<typename graph<T>::edge> nedges;
            for (const auto &[from, to, cost] : edges)
                if (where[from] != where[to])
                    nedges.push_back({where[from], where[to], cost});

            std::sort(nedges.begin(), nedges.end(), [](const typename graph<T>::edge &a, const typename graph<T>::edge &b) {
                if (a.from != b.from)
                    return a.from < b.from;

                return a.to < b.to;
            });

            for (int i = 0; i < int(nedges.size());) {
                int j = i + 1;
                T sum = nedges[i].cost;
                while (j < int(nedges.size()) && nedges[j].from == nedges[i].from && nedges[j].to == nedges[i].to) {
                    sum += nedges[j].cost;
                    j++;
                }
                dag.add(nedges[i].from, nedges[i].to, sum);
                i = j;
            }
        }
        return dag;
    }

    std::vector<int> eulerian_path() const {
        std::vector<bool> any(n);
        for (const auto &[from, to, cost] : edges)
            any[from] = any[to] = true;

        int first = 0;
        while (first < n && !any[first])
            first++;

        if (first == n)
            return std::vector<int>();

        std::vector<int> balance(n);
        for (const auto &[from, to, cost] : edges) {
            balance[from]++;
            balance[to]--;
        }

        int start = first;
        for (int v = first + 1; v < n; v++) {
            if (!any[v])
                continue;

            if (balance[v] > 1 || (balance[v] == 1 && balance[start] == 1))
                return std::vector<int>();

            if (balance[v] > balance[start])
                start = v;
        }

        std::vector<int> ptr(n);
        std::vector<int> path;

        std::function<void(int, int)> dfs = [&](int v, int prev) {
            while (ptr[v] < int(g[v].size())) {
                int id = g[v][ptr[v]++];
                dfs(edges[id].to, id);
            }
            if (prev != -1)
                path.push_back(prev);
        };

        dfs(start, -1);
        if (path.size() != edges.size())
            return std::vector<int>();

        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<int> build_path(const std::vector<int> &edge_indices) const {
        if (edge_indices.empty())
            return std::vector<int>();

        std::vector<int> path;
        path.reserve(edge_indices.size() + 1);
        for (const int id : edge_indices)
            path.push_back(edges[id].from);

        path.push_back(edges[edge_indices.back()].to);
        return path;
    }
};
