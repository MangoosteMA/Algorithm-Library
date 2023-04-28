template<typename T>
struct push_relabel {
    struct edge {
        int to, rev;
        T capacity, flow;
    };

    int n;
    std::vector<std::vector<edge>> g;
    std::vector<int> height;

    push_relabel(int n = 0) : n(n), g(n), height(n) {}

    int size() const {
        return n;
    }

    int add(int from, int to, T forward_capacity, T backward_capacity = 0) {
        if (from == to)
            return -1;

        int id = g[from].size();
        g[from].push_back({to, int(g[to].size()) + (from == to), forward_capacity, 0});
        g[to].push_back({from, id, backward_capacity, 0});
        return id;
    }

    template<typename U>
    U solve(int source, int sink) {
        for (int v = 0; v < n; v++)
            for (auto &e : g[v])
                e.flow = 0;

        std::fill(height.begin(), height.end(), 0);
        height[source] = n;
        std::vector<int> count(n + 1);
        count[0] = n - 1;

        std::vector<U> excess(n);
        std::vector<bool> activated(n);
        activated[sink] = true;
        std::vector<std::vector<int>> layers(n + 1);
        std::vector<int> edge_ptr(n);

        auto add_flow = [&](edge &e, const T &pushed) {
            if (pushed == 0)
                return;

            excess[e.to] += pushed;
            e.flow += pushed;
            auto &back = g[e.to][e.rev];
            back.flow -= pushed;
            excess[back.to] -= pushed;
            if (!activated[e.to]) {
                activated[e.to] = true;
                layers[height[e.to]].push_back(e.to);
            }
        };

        for (auto &e : g[source])
            add_flow(e, e.capacity);

        for (int layer = 0; layer >= 0;) {
            if (layers[layer].empty()) {
                layer--;
                continue;
            }

            int v = layers[layer].back();
            layers[layer].pop_back();
            activated[v] = false;

            while (excess[v] > 0 && height[v] < n) {
                if (edge_ptr[v] == int(g[v].size())) {
                    edge_ptr[v] = 0;
                    height[v] = n;
                    for (auto &e : g[v])
                        if (e.flow < e.capacity)
                            height[v] = std::min(height[v], height[e.to] + 1);

                    count[layer]--;
                    count[height[v]]++;
                    if (count[layer] == 0 && layer < n) {
                        for (int u = 0; u < n; u++) {
                            if (layer < height[u] && height[u] < n) {
                                count[height[u]]--;
                                height[u] = n;
                            }
                        }
                    }
                    layer = height[v] == n ? layer : height[v];
                    continue;
                }

                auto &e = g[v][edge_ptr[v]];
                if (height[e.to] == height[v] - 1 && e.flow < e.capacity) {
                    add_flow(e, std::min<U>(excess[v], e.capacity - e.flow));
                } else {
                    edge_ptr[v]++;
                }
            }
        }
        return excess[sink];
    }

    bool left_of_mincut(int v) const {
        return height[v] == n;
    }
};
