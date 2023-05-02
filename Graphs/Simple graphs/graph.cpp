template<bool directed, typename w_t = int>
struct graph {
    struct edge {
        int from, to;
        w_t w;
        int id;
    };

    int n;
    std::vector<edge> edges;
    std::vector<std::vector<edge>> g;

    graph(int n = 0) : n(n), edges(), g(n) {}

    void add(int v, int u, w_t w = 1) {
        int cur_id = edges.size();
        edges.push_back({v, u, w, cur_id});
        g[v].push_back({v, u, w, cur_id});
        if (v != u && !directed)
            g[u].push_back({u, v, w, cur_id});
    }

    std::vector<edge>& operator[](size_t i) {
        return g[i];
    }

    const std::vector<edge>& operator[](size_t i) const {
        return g[i];
    }

    int size() const {
        return n;
    }
};
