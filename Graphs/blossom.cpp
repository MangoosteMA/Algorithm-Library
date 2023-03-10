struct blossom {
    int n;
    std::vector<std::vector<int>> g;
    std::vector<int> matching;

    blossom(int n = 0) : n(n), g(n), matching(n, -1) {}

    int size() const {
        return n;
    }

    void add(int v, int u) {
        assert(0 <= v && v < n);
        assert(0 <= u && u < n);
        g[v].push_back(u);
        g[u].push_back(v);
    }

    int solve(bool need_to_shuffle = false) {
        auto try_to_expand = [&]() {
            std::vector<int> dsu_par(n), dsu_rank(n, 1);
            std::iota(dsu_par.begin(), dsu_par.end(), 0);

            struct edge {
                int initial_v, initial_u, v, u;
            };
            std::vector<std::vector<edge>> blossoms(n);
            std::vector<std::vector<int>> log(n);

            auto root = [&](int v) {
                if (v == -1)
                    return -1;

                while (dsu_par[v] != v)
                    v = dsu_par[v];

                return v;
            };

            std::queue<int> que;
            std::vector<int> type(n);
            std::vector<std::pair<int, int>> bfs_par(n, {-1, -1});
            for (int i = 0; i < n; i++)
                if (matching[i] == -1) {
                    type[i] = 1;
                    que.push(i);
                }

            std::vector<int> lca_used(n);
            int lca_used_timer = 0;

            auto lca = [&](int v, int u) {
                assert(dsu_par[v] == v);
                assert(dsu_par[u] == u);

                lca_used_timer++;
                assert(v != u);
                lca_used[v] = lca_used[u] = lca_used_timer;

                while (true) {
                    int v_root = root(bfs_par[v].first);
                    int u_root = root(bfs_par[u].first);
                    if (v_root == -1 && u_root == -1)
                        return -1;

                    for (int rot = 0; rot < 2; rot++, std::swap(v, u), std::swap(v_root, u_root))
                        if (v_root != -1) {
                            if (lca_used[v_root] == lca_used_timer)
                                return v_root;

                            v = v_root;
                            lca_used[v] = lca_used_timer;
                        }
                }
                assert(false);
            };

            auto get_path = [&](int from, int to) {
                assert(dsu_par[from] == from);
                assert(to == -1 || dsu_par[to] == to);
                std::vector<edge> edges;
                while (from != to) {
                    edges.push_back({bfs_par[from].second, bfs_par[from].first, from, root(bfs_par[from].first)});
                    from = edges.back().u;
                }
                return edges;
            };

            while (!que.empty()) {
                int v = que.front();
                que.pop();

                for (int u : g[v]) {
                    int v_root = root(v), u_root = root(u);
                    if (u_root == v_root)
                        continue;

                    if (type[u] == 0) {
                        assert(matching[u] != -1);
                        int w = matching[u];
                        if (type[w] == 1)
                            continue;

                        assert(dsu_par[w] == w);
                        assert(bfs_par[w].first == -1 && bfs_par[u_root].first == -1);

                        bfs_par[w] = {u, w};
                        bfs_par[u_root] = {v, u};
                        type[w] = 1;
                        que.push(w);
                    } else {
                        int w = lca(v_root, u_root);
                        std::vector<edge> path_v = get_path(v_root, w);
                        std::vector<edge> path_u = get_path(u_root, w);
                        std::reverse(path_v.begin(), path_v.end());
                        for (edge &e : path_v) {
                            std::swap(e.initial_v, e.initial_u);
                            std::swap(e.v, e.u);
                        }

                        auto add_path = [&](std::vector<edge> &path) {
                            for (edge &e : path_v)
                                path.push_back(e);

                            path.push_back({v, u, v_root, u_root});
                            for (edge &e : path_u)
                                path.push_back(e);
                        };

                        if (w == -1) {
                            std::vector<edge> path;
                            add_path(path);

                            std::vector<int> last_changed(n, -1);
                            std::queue<int> to_unfold;

                            auto process_edge = [&](const edge &e) {
                                to_unfold.push(e.v);
                                to_unfold.push(e.u);
                                matching[e.initial_v] = e.initial_u;
                                matching[e.initial_u] = e.initial_v;
                                assert(last_changed[e.v] == -1);
                                assert(last_changed[e.u] == -1);
                                last_changed[e.v] = e.initial_v;
                                last_changed[e.u] = e.initial_u;
                            };

                            for (int i = 1; i < int(path.size()) - 1; i += 2)
                                process_edge(path[i]);

                            while (!to_unfold.empty()) {
                                int v = to_unfold.front();
                                to_unfold.pop();
                                if (log[v].empty())
                                    continue;

                                assert(last_changed[v] != -1);
                                int u = last_changed[v];
                                last_changed[v] = -1;
                                int pos = log[v].back();
                                log[v].pop_back();

                                for (int i = pos; i < int(blossoms[v].size()); i++)
                                    for (int x : {blossoms[v][i].v, blossoms[v][i].u})
                                        dsu_par[x] = x;
                                
                                int u_root = root(u);
                                bool found = false;
                                for (int i = pos + 1; i < int(blossoms[v].size()); i += 2) {
                                    if (blossoms[v][i].v == u_root) {
                                        assert(!found);
                                        found = true;
                                        assert((int(blossoms[v].size()) - i) % 2 == 0);

                                        last_changed[blossoms[v][i].v] = u;
                                        to_unfold.push(blossoms[v][i].v);
                                        for (int j = i + 1; j < int(blossoms[v].size()); j += 2)
                                            process_edge(blossoms[v][j]);
                                    }
                                    
                                    if (blossoms[v][i].u == u_root) {
                                        assert(!found);
                                        found = true;
                                        assert((i - pos) % 2 == 1);

                                        last_changed[blossoms[v][i].u] = u;
                                        to_unfold.push(blossoms[v][i].u);
                                        for (int j = i - 1; j >= pos; j -= 2)
                                            process_edge(blossoms[v][j]);
                                    }
                                }

                                if (!found) {
                                    to_unfold.push(blossoms[v][pos].v);
                                    last_changed[blossoms[v][pos].v] = u;
                                }
                                blossoms[v].resize(pos);
                            }
                            return true;
                        }

                        assert(path_v.size() % 2 == 0);
                        assert(path_u.size() % 2 == 0);

                        auto process_vertex = [&](int v) {
                            assert(type[v] == 0);
                            assert(dsu_par[v] == v);
                            type[v] = 1;
                            que.push(v);
                        };

                        for (int i = 0; i < int(path_v.size()); i += 2) {
                            assert(path_v[i].initial_u == path_v[i].u);
                            process_vertex(path_v[i].u);
                        }

                        for (int i = 0; i < int(path_u.size()); i += 2) {
                            assert(path_u[i].initial_u == path_u[i].u);
                            process_vertex(path_u[i].u);
                        }

                        std::vector<int> vertices;
                        vertices.reserve(path_v.size() + path_u.size() + 1);
                        for (edge &e : path_v)
                            vertices.push_back(e.v);

                        vertices.push_back(v_root);
                        for (edge &e : path_u)
                            vertices.push_back(e.v);

                        int new_root = vertices[0];
                        for (int i = 1; i < int(vertices.size()); i++)
                            if (dsu_rank[new_root] < dsu_rank[vertices[i]])
                                new_root = vertices[i];

                        bfs_par[new_root] = bfs_par[w];
                        for (int v : vertices) {
                            if (v == new_root)
                                continue;

                            assert(type[v] == 1);
                            dsu_par[v] = new_root;
                            dsu_rank[new_root] += dsu_rank[v];
                        }

                        log[new_root].push_back(blossoms[new_root].size());
                        add_path(blossoms[new_root]);
                    }
                }
            }
            return false;
        };

        int pairs = 0;
        for (int v = 0; v < n; v++)
            pairs += matching[v] > v;

        std::vector<int> order(n);
        std::iota(order.begin(), order.end(), 0);
        if (need_to_shuffle)
            std::shuffle(order.begin(), order.end(), std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count()));

        for (int v : order)
            for (int u : g[v])
                if (matching[v] == -1 && matching[u] == -1) {
                    matching[v] = u;
                    matching[u] = v;
                    pairs++;
                }

        while (try_to_expand())
            pairs++;

        return pairs;
    }
};
