/*
 * T is a capacity integral type.
 * An implementation of the Dinic algorithm in O(V^2E).
 */
template<typename T>
class max_flow {
public:
    struct edge {
        int to, rev;
        T capacity, flow;
    };

    struct path {
        T flow;
        std::vector<int> path, edges;
    };

private:
    int n;
    std::vector<std::vector<edge>> g;
    std::vector<int> dist, head;
    int source, sink;

    bool bfs() {
        dist.assign(n, n);
        dist[source] = 0;
        std::vector<int> que{source};
        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (const auto &[u, rev, capacity, flow] : g[v]) {
                if (dist[u] > dist[v] + 1 && flow < capacity) {
                    dist[u] = dist[v] + 1;
                    que.push_back(u);
                }
            }
        }
        return dist[sink] != n;
    }

    T dfs(int v, T f) {
        if (v == sink) {
            return f;
        }
        for (; head[v] < int(g[v].size()); head[v]++) {
            auto &[u, rev, capacity, flow] = g[v][head[v]];
            if (dist[u] == dist[v] + 1 && flow < capacity) {
                if (T delta = dfs(u, std::min(f, capacity - flow))) {
                    flow += delta;
                    g[u][rev].flow -= delta;
                    return delta;
                }
            }
        }
        return 0;
    }

    T decompose_dfs(int v, T f, std::vector<int> &path, std::vector<int> &edges) {
        if (v == sink) {
            path.push_back(sink);
            return f;
        }
        for (; head[v] < int(g[v].size()); head[v]++) {
            auto &[u, rev, capacity, flow] = g[v][head[v]];
            if (flow > 0) {
                if (T delta = decompose_dfs(u, std::min(f, flow), path, edges)) {
                    flow -= delta;
                    g[u][rev].flow += delta;
                    path.push_back(v);
                    edges.push_back(head[v]);
                    return delta;
                }
            }
        }
        return 0;
    }

public:
    max_flow(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    /*
     * Returns an index of the added edge.
     * To get information about the edge use get_edge(from, index).
     */
    int add(int from, int to, T forward_capacity, T backward_capacity = 0) {
        int id = g[from].size();
        g[from].push_back({to, int(g[to].size()) + (from == to), forward_capacity, 0});
        g[to].push_back({from, id, backward_capacity, 0});
        return id;
    }

    // Returns i-th edge of vertex v.
    edge get_edge(int v, int id) const {
        return g[v][id];
    }

    void clear_flow() {
        for (int v = 0; v < n; v++) {
            for (auto &e : g[v]) {
                e.flow = 0;
            }
        }
    }

    /*
     * U is a max flow integral type.
     ! If source and sink are different from previous call of solve, then call clear_flow() before.
     */
    template<typename U>
    U solve(int source_, int sink_) {
        source = source_, sink = sink_;
        U flow = 0;
        while (bfs()) {
            head.assign(n, 0);
            while (T delta = dfs(source, std::numeric_limits<T>::max())) {
                flow += delta;
            }
        }
        return flow;
    }

    /*
     * Returns true if v is on the left side of the mincut.
     ! Require: solve must be called before using it.
     */
    bool left_of_mincut(int v) const {
        return dist[v] != n;
    }

    /*
     * Returns list of (flow, path, edges) where:
        - 'flow' is the flow value of the path.
        - 'path' is the list of vertices (path[0] == source && path.back() == sink).
        - 'edges' is the list of edge indices of the path returned by add method.
     ! Require: solve must be called before using it.
     ! WARNING: invalidates the flow values.
     */
    std::vector<path> decompose() {
        std::vector<path> paths;
        std::vector<int> path, edges;
        head.assign(n, 0);
        while (T flow = decompose_dfs(source, std::numeric_limits<T>::max(), path, edges)) {
            std::reverse(path.begin(), path.end());
            std::reverse(edges.begin(), edges.end());
            paths.push_back({flow, path, edges});
            path.clear();
            edges.clear();
        }
        return paths;
    }
};

/*
 * T is a capacity integral type.
 * An implementation of the Dinic algorithm with flow scaling in O(VE log(C)).
 */
template<typename T>
class max_flow {
public:
    struct edge {
        int to, rev;
        T capacity, flow;
    };

private:
    int n;
    std::vector<std::vector<edge>> g;

    std::vector<int> dist, head;
    int source, sink;
    T smallest;

    bool bfs() {
        dist.assign(n, n);
        dist[source] = 0;
        std::vector<int> que{source};
        for (int ptr = 0; ptr < int(que.size()); ptr++) {
            int v = que[ptr];
            for (const auto &[u, rev, capacity, flow] : g[v]) {
                if (dist[u] > dist[v] + 1 && capacity - flow >= smallest) {
                    dist[u] = dist[v] + 1;
                    que.push_back(u);
                }
            }
        }
        return dist[sink] != n;
    }

    T dfs(int v, T min_delta) {
        if (v == sink) {
            return min_delta;
        }
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

public:
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

    // U is a max flow integral type.
    template<typename U>
    U solve(int source_, int sink_) {
        source = source_, sink = sink_;
        U flow = 0;
        for (smallest = T(1) << std::__lg(std::numeric_limits<T>::max()); smallest >= 1; smallest >>= 1) {
            while (bfs()) {
                head.assign(n, 0);
                while (T delta = dfs(source, std::numeric_limits<T>::max())) {
                    flow += delta;
                }
            }
        }
        return flow;
    }
};
