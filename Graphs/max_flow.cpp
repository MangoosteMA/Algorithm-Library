// O(n**2 * m)
template<typename T>
struct max_flow {
    struct edge {
        int to, rev;
        T capacity, flow;
    };

    int n;
    std::vector<std::vector<edge>> g;

    max_flow(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    int add(int from, int to, T forward_capacity, T backward_capacity = 0) {
        int id = g[from].size();
        g[from].push_back({to, int(g[to].size()) + (from == to), forward_capacity, 0});
        g[to].push_back({from, id, backward_capacity, 0});
        return id;
    }

    std::vector<int> dist, head;
    int source, sink;

    bool bfs() {
        std::fill(dist.begin(), dist.end(), n);
        dist[source] = 0;
        std::vector<int> que{source};
        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (const auto &[u, rev, capacity, flow] : g[v])
                if (dist[u] > dist[v] + 1 && flow < capacity) {
                    dist[u] = dist[v] + 1;
                    que.push_back(u);
                }
        }
        return dist[sink] != n;
    }

    T dfs(int v, T min_delta) {
        if (v == sink)
            return min_delta;

        for (; head[v] < int(g[v].size()); head[v]++) {
            auto &[u, rev, capacity, flow] = g[v][head[v]];
            if (dist[u] == dist[v] + 1 && flow < capacity) {
                if (T delta = dfs(u, std::min(min_delta, capacity - flow))) {
                    flow += delta;
                    g[u][rev].flow -= delta;
                    return delta;
                }
            }
        }
        return 0;
    }

    template<typename U>
    U solve(int source_, int sink_) {
        source = source_, sink = sink_;
        for (int v = 0; v < n; v++)
            for (edge &e : g[v])
                e.flow = 0;

        dist.resize(n);
        head.resize(n);
        U flow = 0;
        while (bfs()) {
            std::fill(head.begin(), head.end(), 0);
            while (T delta = dfs(source, std::numeric_limits<T>::max()))
                flow += delta;
        }
        return flow;
    }
};

// flow scaling
template<typename T>
struct max_flow {
    struct edge {
        int to, rev;
        T capacity, flow;
    };

    int n;
    std::vector<std::vector<edge>> g;

    max_flow(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    int add(int from, int to, T forward_capacity, T backward_capacity = 0) {
        int id = g[from].size();
        g[from].push_back({to, int(g[to].size()) + (from == to), forward_capacity, 0});
        g[to].push_back({from, id, backward_capacity, 0});
        return id;
    }

    std::vector<int> dist, head;
    int source, sink;
    T smallest;

    bool bfs() {
        std::fill(dist.begin(), dist.end(), n);
        dist[source] = 0;
        std::vector<int> que{source};
        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (const auto &[u, rev, capacity, flow] : g[v])
                if (dist[u] > dist[v] + 1 && capacity - flow >= smallest) {
                    dist[u] = dist[v] + 1;
                    que.push_back(u);
                }
        }
        return dist[sink] != n;
    }

    T dfs(int v, T min_delta) {
        if (v == sink)
            return min_delta;

        for (; head[v] < int(g[v].size()); head[v]++) {
            auto &[u, rev, capacity, flow] = g[v][head[v]];
            if (dist[u] == dist[v] + 1 && capacity - flow >= smallest) {
                if (T delta = dfs(u, std::min(min_delta, capacity - flow))) {
                    flow += delta;
                    g[u][rev].flow -= delta;
                    return delta;
                }
            }
        }
        return 0;
    }

    template<typename U>
    U solve(int source_, int sink_) {
        source = source_, sink = sink_;
        for (int v = 0; v < n; v++)
            for (edge &e : g[v])
                e.flow = 0;

        dist.resize(n);
        head.resize(n);

        U flow = 0;
        for (smallest = T(1) << std::__lg(std::numeric_limits<T>::max()); smallest >= 1; smallest >>= 1) {
            while (bfs()) {
                std::fill(head.begin(), head.end(), 0);
                while (T delta = dfs(source, std::numeric_limits<T>::max()))
                    flow += delta;
            }
        }
        return flow;
    }
};

// O(n**3)
// but actually it works slower than algorithm above
template<typename T, typename U>
struct max_flow {
    struct edge {
        int to;
        T capacity, flow;
    };

    int n;
    std::vector<edge> edges;
    std::vector<std::vector<int>> g;

    max_flow(int n = 0) : n(n), g(n) {}

    int add(int from, int to, T forward_capacity, T backward_capacity = 0) {
        int id = edges.size();
        g[from].push_back(edges.size());
        edges.push_back({to, forward_capacity, 0});
        g[to].push_back(edges.size());
        edges.push_back({from, backward_capacity, 0});
        return id;
    }

    int source, sink;
    std::vector<int> dist, que;

    bool bfs() {
        dist.assign(n, n);
        dist[source] = 0;
        que.resize(1, source);

        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (auto i : g[v])
                if (edges[i].flow < edges[i].capacity && dist[edges[i].to] == n) {
                    dist[edges[i].to] = dist[v] + 1;
                    que.push_back(edges[i].to);
                }
        }
        return dist[sink] != n;
    }

    std::vector<U> tot_out, tot_in;
    std::vector<bool> removed;
    std::vector<std::vector<int>> new_g;
    std::vector<int> forward_head, backward_head, changes;

    void remove(int v) {
        removed[v] = true;
        tot_in[v] = tot_out[v] = 0;
        for (auto i : new_g[v]) {
            int u = edges[i].to;
            if (dist[u] == dist[v] - 1 && !removed[u]) {
                tot_out[u] -= edges[i ^ 1].capacity - edges[i ^ 1].flow;
                if (tot_out[u] == 0)
                    remove(u);
            } else if (dist[u] == dist[v] + 1 && !removed[u]) {
                tot_in[u] -= edges[i].capacity - edges[i].flow;
                if (tot_in[u] == 0)
                    remove(u);
            }
        }
    }

    U push_forward(int v, U to_push) {
        changes.push_back(v);
        if (v == sink)
            return to_push;

        U tot_pushed = 0;
        for (; forward_head[v] < int(new_g[v].size()); forward_head[v]++) {
            int i = new_g[v][forward_head[v]], u = edges[i].to;
            if (dist[u] == dist[v] + 1 && edges[i].flow < edges[i].capacity && !removed[u]) {
                T pushed = push_forward(u, std::min<U>(to_push - tot_pushed, edges[i].capacity - edges[i].flow));
                tot_pushed += pushed;

                edges[i].flow += pushed;
                edges[i ^ 1].flow -= pushed;
                tot_out[v] -= pushed;
                tot_in[u] -= pushed;

                if (tot_pushed == to_push)
                    break;
            }
        }
        return tot_pushed;
    }

    U push_backward(int v, U to_push) {
        changes.push_back(v);
        if (v == source)
            return to_push;

        U tot_pushed = 0;
        for (; backward_head[v] < int(new_g[v].size()); backward_head[v]++) {
            int i = new_g[v][backward_head[v]], u = edges[i].to;
            if (dist[u] == dist[v] - 1 && edges[i ^ 1].flow < edges[i ^ 1].capacity && !removed[u]) {
                T pushed = push_backward(u, std::min<U>(to_push - tot_pushed, edges[i ^ 1].capacity - edges[i ^ 1].flow));
                tot_pushed += pushed;

                edges[i ^ 1].flow += pushed;
                edges[i].flow -= pushed;
                tot_out[u] -= pushed;
                tot_in[v] -= pushed;

                if (tot_pushed == to_push)
                    break;
            }
        }
        return tot_pushed;
    }

    U solve(int source_, int sink_) {
        source = source_, sink = sink_;
        for (auto &e : edges)
            e.flow = 0;

        que.reserve(n);
        changes.reserve(n);
        new_g.resize(n);
        for (int v = 0; v < n; v++)
            new_g[v].reserve(g[v].size());

        U flow = 0;
        while (bfs()) {
            for (auto v : que)
                new_g[v].clear();

            tot_out.assign(n, 0);
            tot_in.assign(n, 0);
            removed.assign(n, true);
            removed[sink] = false;

            for (int i = int(que.size()) - 1; i >= 0; i--) {
                int v = que[i];
                if (removed[v])
                    continue;

                for (auto i : g[v])
                    if (dist[edges[i].to] == dist[v] - 1 && edges[i ^ 1].flow < edges[i ^ 1].capacity) {
                        removed[edges[i].to] = false;
                        tot_out[edges[i].to] += edges[i ^ 1].capacity - edges[i ^ 1].flow;
                        tot_in[v] += edges[i ^ 1].capacity - edges[i ^ 1].flow;
                        new_g[v].push_back(i);
                        new_g[edges[i].to].push_back(i ^ 1);
                    }
            }
            tot_in[source] = tot_out[sink] = std::numeric_limits<U>::max();

            forward_head.assign(n, 0);
            backward_head.assign(n, 0);

            while (!removed[source] && !removed[sink]) {
                U min_p;
                int start = -1;
                for (auto v : que)
                    if (!removed[v]) {
                        U cur_p = std::min(tot_out[v], tot_in[v]);
                        if (start == -1 || cur_p < min_p) {
                            start = v;
                            min_p = cur_p;
                        }
                    }

                changes.clear();
                push_forward(start, min_p);
                push_backward(start, min_p);
                flow += min_p;

                for (auto v : changes)
                    if (!removed[v] && std::min(tot_out[v], tot_in[v]) == 0)
                        remove(v);
            }
        }
        return flow;
    }
};
