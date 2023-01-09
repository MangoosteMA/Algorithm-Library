template<typename T>
struct graph {
    struct edge {
        int from, to;
        T cost;
    };

    int n;
    std::vector<edge> edges;
    std::vector<std::vector<int>> g;

    graph(int n = 0) : n(n), g(n) {}

    int size() const {
        return n;
    }

    void resize(int new_n) {
        n = new_n;
        g.resize(new_n);
    }

    void reserve(int mem) {
        edges.reserve(mem);
    }

    int another(int id, int v) const {
        return v ^ edges[id].from ^ edges[id].to;
    }

    virtual int add(int from, int to, T cost) = 0;
};
