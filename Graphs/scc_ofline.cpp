/*
Type T is disjoint_set_union like class with:
    - constructor(vertices)
    - unite(v, u) (must return true if v and u were united)
    - root(v) (root vertex of component of vertex v)
*/
template<typename T>
class scc_ofline {
private:
    struct edge {
        int from, to;
    };

    int n;
    std::vector<edge> edges;

    std::vector<std::vector<int>> g;
    int v_num, scc_num;
    std::vector<int> num, low, scc, st;

    void tarjan(int v) {
        st.push_back(v);
        num[v] = low[v] = v_num++;
        for (auto u : g[v]) {
            if (num[u] == -1) {
                tarjan(u);
                low[v] = std::min(low[v], low[u]);
            } else if (scc[u] == -1) {
                low[v] = std::min(low[v], num[u]);
            }
        }

        if (num[v] == low[v]) {
            while (true) {
                int u = st.back();
                st.pop_back();
                scc[u] = scc_num;
                if (u == v) {
                    break;
                }
            }
            scc_num++;
        }
    }

    void solve(int l, int r, const std::vector<int> &eset, T &dsu,
               std::vector<std::vector<std::pair<int, int>>> &unions) {
        v_num = scc_num = 0;
        for (auto i : eset) {
            if (i >= r) {
                continue;
            }
            for (auto v : {dsu.root(edges[i].from), dsu.root(edges[i].to)}) {
                g[v].clear();
                num[v] = low[v] = scc[v] = -1;
            }
        }

        for (auto i : eset) {
            if (i < r) {
                int v = dsu.root(edges[i].from);
                int u = dsu.root(edges[i].to);
                if (v != u) {
                    g[v].push_back(u);
                }
            }
        }

        for (auto i : eset) {
            if (i <= r) {
                int v = dsu.root(edges[i].from);
                if (num[v] == -1) {
                    tarjan(v);
                }
            }
        }

        std::vector<int> neset;
        neset.reserve(eset.size());

        for (auto i : eset) {
            if (i < r) {
                int v = dsu.root(edges[i].from);
                int u = dsu.root(edges[i].to);
                if (v != u && scc[v] == scc[u]) {
                    neset.push_back(i);
                }
            }
        }

        if (r - l > 1) {
            int m = (l + r) / 2;
            solve(l, m, neset, dsu, unions);
            solve(m, r, neset, dsu, unions);
            return;
        }

        for (auto i : neset) {
            if (dsu.unite(edges[i].from, edges[i].to)) {
                unions[l].emplace_back(edges[i].from, edges[i].to);
            }
        }
    }

public:
    scc_ofline(int n = 0) : n(n), g(n), num(n), low(n), scc(n) {}

    // Adds directed edge (from -> to)
    void add(int from, int to) {
        edges.push_back({from, to});
    }

    // For each edge returns list of unions of scc caused by it.
    std::vector<std::vector<std::pair<int, int>>> solve() {
        T dsu(n);
        std::vector<std::vector<std::pair<int, int>>> unions(edges.size());

        std::vector<int> eset(edges.size());
        std::iota(eset.begin(), eset.end(), 0);

        solve(0, edges.size(), eset, dsu, unions);
        return unions;
    }
};
