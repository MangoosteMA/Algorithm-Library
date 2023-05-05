template<typename T>
struct directed_minimum_spanning_tree {
    struct edge {
        int from, to;
        T cost;

        bool operator<(const edge &e) const {
            return cost != e.cost ? cost < e.cost : (from != e.from ? from < e.from : to < e.to);
        }
    };

    int n;
    std::vector<edge> edges;

    directed_minimum_spanning_tree(int n = 0) : n(n) {}

    int size() const {
        return n;
    }

    void add(int from, int to, T cost) {
        edges.push_back({from, to, cost});
    }

    template<typename U>
    std::pair<U, std::vector<int>> solve(int root) const {
        std::vector<int> parent(n);
        std::iota(parent.begin(), parent.end(), 0);
        std::vector<std::set<edge>> input_edges(n);
        for (auto &e : edges)
            input_edges[e.to].insert(e);

        std::function<int(int)> get_root = [&](int v) {
            return parent[v] == v ? v : parent[v] = get_root(parent[v]);
        };

        std::vector<T> delta(n);
        int current_layer = 0;
        std::vector<std::vector<std::pair<int, int>>> added_edges(n);

        auto unite = [&](int v, int u) {
            v = get_root(v);
            u = get_root(u);
            assert(v != u);
            if (input_edges[v].size() < input_edges[u].size())
                std::swap(v, u);

            for (auto e : input_edges[u]) {
                e.cost += delta[u] - delta[v];
                input_edges[v].insert(e);
            }
            std::set<edge>().swap(input_edges[u]);
            parent[u] = v;
        };

        std::vector<int> pos_in_stack(n, -1);
        for (int i = 0; i < n; i++) {
            if (get_root(i) == get_root(root))
                continue;

            std::stack<edge> st;
            pos_in_stack[i] = 0;
            while (true) {
                int real_v = st.empty() ? i : st.top().from, v = get_root(real_v);
                while (!input_edges[v].empty() && get_root(input_edges[v].begin()->from) == v)
                    input_edges[v].erase(input_edges[v].begin());

                if (input_edges[v].empty())
                    return {U(), {}}; // the component of vertex v can't be reached from root

                int real_u = input_edges[v].begin()->from, u = get_root(real_u);
                delta[v] = -input_edges[v].begin()->cost;

                if (u == get_root(root)) {
                    st.push(*input_edges[v].begin());
                    int new_layer = current_layer + st.size();
                    current_layer = new_layer;
                    while (!st.empty()) {
                        added_edges[st.top().from].emplace_back(st.top().to, --new_layer);
                        unite(st.top().to, root);
                        st.pop();
                    }
                    break;
                }

                if (pos_in_stack[u] == -1) {
                    st.push(*input_edges[v].begin());
                    pos_in_stack[u] = st.size();
                    continue;
                }

                added_edges[real_u].emplace_back(real_v, current_layer);
                while (int(st.size()) != pos_in_stack[u]) {
                    added_edges[st.top().from].emplace_back(st.top().to, current_layer);
                    unite(st.top().from, u);
                    st.pop();
                }
                pos_in_stack[get_root(u)] = pos_in_stack[u];
                current_layer++;
            }
        }

        std::vector<int> mst_parent(n, -1);
        std::priority_queue<std::tuple<int, int, int>> pq;
        pq.emplace(0, root, root);
        while (!pq.empty()) {
            auto [_, v, p] = pq.top();
            pq.pop();
            if (mst_parent[v] != -1)
                continue;

            mst_parent[v] = p;
            for (const auto &[u, w] : added_edges[v])
                pq.emplace(-w, u, v);
        }
        return {-accumulate(delta.begin(), delta.end(), U(0)), mst_parent};
    }
};
