struct bipartite_disjoin_set_union {
    std::vector<int> par, rank, value;
    bool bipartite = true;

    struct update {
        int v, u;
        bool flipped_u, old_bipartite;
    };
    std::vector<update> updates;

    bipartite_disjoin_set_union(int n = 0) : par(n), rank(n, 1), value(n) {
        std::iota(par.begin(), par.end(), 0);
    }

    int version() const {
        return updates.size();
    }

    void reset() {
        std::iota(par.begin(), par.end(), 0);
        std::fill(rank.begin(), rank.end(), 1);
        std::fill(value.begin(), value.end(), 0);
        bipartite = true;
        updates.clear();
    }

    int color(int v) {
        return par[v] == v ? value[v] : (value[v] ^ color(par[v]));
    }

    int root(int v) {
        return par[v] == v ? v : root(par[v]);
    }

    void flip_component(int v) {
        value[root(v)] ^= 1;
    }

    void unite(int v, int u) {
        if (!bipartite)
            return;

        int v_root = root(v), u_root = root(u);
        if (v_root == u_root) {
            updates.push_back({-1, -1, false, true});
            bipartite &= color(v) != color(u);
            return;
        }

        if (rank[v_root] < rank[u_root]) {
            std::swap(v, u);
            std::swap(v_root, u_root);
        }

        bool flipped_u = color(v) == color(u);
        if (flipped_u)
            value[u_root] ^= 1;

        rank[v_root] += rank[u_root];
        par[u_root] = v_root;
        updates.push_back({v_root, u_root, flipped_u, true});
    }

    void roll_back() {
        auto [v, u, flipped_u, old_bipartite] = updates.back();
        updates.pop_back();

        bipartite = old_bipartite;
        if (v != -1) {
            rank[v] -= rank[u];
            par[u] = u;
            if (flipped_u)
                value[u] ^= 1;
        }
    }

    void roll_back(int old_version) {
        assert(old_version <= version());
        while (version() != old_version)
            roll_back();
    }
};
