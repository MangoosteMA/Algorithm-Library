struct link_cut {
    struct node {
        int parent;
        std::array<int, 2> sons;
        bool inv;
        int size;

        node() : parent(-1), sons({-1, -1}), inv(false), size(1) {}
    };

    std::vector<node> tree;

    int size() const {
        return tree.size();
    }

    void push(int v) {
        if (tree[v].inv) {
            tree[v].inv = false;
            std::swap(tree[v].sons[0], tree[v].sons[1]);
            for (const auto u : tree[v].sons)
                if (u != -1)
                    tree[u].inv ^= 1;
        }
    }

    void relax(int v) {
        push(v);
        tree[v].size = 1;
        for (const auto x : tree[v].sons)
            if (x != -1)
                tree[v].size += tree[x].size;
    }

    void rotate(int v) {
        int u = tree[v].parent, w = tree[u].parent;
        push(u), push(v);

        tree[v].parent = w;
        if (w != -1)
            for (auto &x : tree[w].sons)
                if (x == u)
                    x = v;

        int i = tree[u].sons[1] == v;
        tree[u].sons[i] = tree[v].sons[i ^ 1];
        if (tree[v].sons[i ^ 1] != -1)
            tree[tree[v].sons[i ^ 1]].parent = u;

        tree[v].sons[i ^ 1] = u;
        tree[u].parent = v;
        relax(u), relax(v);
    }

    bool is_root(int v) const {
        return tree[v].parent == -1 || (tree[tree[v].parent].sons[0] != v && tree[tree[v].parent].sons[1] != v);
    }

    void splay(int v) {
        while (!is_root(v)) {
            int u = tree[v].parent;
            if (!is_root(u))
                rotate((tree[tree[u].parent].sons[0] == u) == (tree[u].sons[0] == v) ? u : v);

            rotate(v);
        }
        push(v);
    }

    int expose(int v) {
        int prev = -1;
        for (int u = v; u != -1; prev = u, u = tree[u].parent) {
            splay(u);
            tree[u].sons[1] = prev;
            relax(u);
        }
        splay(v);
        assert(tree[v].sons[1] == -1 && tree[v].parent == -1);
        return prev;
    }

    link_cut(int n = 0) : tree(n) {}

    int add() {
        tree.push_back(node());
        return int(tree.size()) - 1;
    }

    void set_root(int root) {
        expose(root);
        tree[root].inv ^= 1;
        push(root);
    }

    bool connected(int v, int u) {
        if (v == u)
            return true;

        expose(v), expose(u);
        return tree[v].parent != -1;
    }

    bool link(int v, int u) {
        if (connected(v, u))
            return false;

        tree[u].inv ^= 1;
        tree[u].parent = v;
        expose(u);
        return true;
    }

    bool cut(int v, int u) {
        if (v == u)
            return false;

        set_root(v), expose(u);
        if (tree[u].sons[0] != v)
            return false;

        tree[u].sons[0] = -1;
        relax(u);
        tree[v].parent = -1;
        return true;
    }

    int parent(int v, int root) {
        if (!connected(v, root))
            return -1;

        set_root(root), expose(v);
        if (tree[v].sons[0] == -1)
            return -1;

        v = tree[v].sons[0];
        while (push(v), tree[v].sons[1] != -1)
            v = tree[v].sons[1];

        splay(v);
        return v;
    }

    int distance(int v, int u) {
        if (!connected(v, u))
            return -1;

        set_root(v), expose(u);
        return tree[u].sons[0] == -1 ? 0 : tree[tree[u].sons[0]].size;
    }

    int lca(int v, int u, int root) {
        set_root(root), expose(v);
        return expose(u);
    }
};
