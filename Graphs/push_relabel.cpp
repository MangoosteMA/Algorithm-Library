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
        std::vector<int> count(n + 1);
        count[0] = n;
        std::vector<U> excess(n);
        std::vector<bool> activated(n);
        activated[sink] = true;
        std::vector<std::vector<int>> layers(n);
        int layer = 0;

        auto activate = [&](int v) {
            if (!activated[v] && excess[v] > 0 && height[v] < n) {
                layer = std::max(layer, height[v]);
                layers[height[v]].push_back(v);
                activated[v] = true;
            }
        };

        auto push = [&](int v) {
            for (auto &e : g[v])
                if (height[e.to] == height[v] - 1) {
                    T pushed = std::min<U>(e.capacity - e.flow, excess[v]);
                    if (pushed > 0) {
                        e.flow += pushed;
                        g[e.to][e.rev].flow -= pushed;
                        excess[v] -= pushed;
                        excess[e.to] += pushed;
                        activate(e.to);
                        if (excess[v] == 0)
                            break;
                    }
                }
        };

        auto relabel = [&](int v) {
            count[height[v]]--;
            height[v] = n;
            for (auto &e : g[v])
                if (e.flow < e.capacity)
                    height[v] = std::min(height[v], height[e.to] + 1);

            activate(v);
            count[height[v]]++;
        };

        for (auto &e : g[source])
            excess[source] += e.capacity;

        activate(source);
        while (layer >= 0) {
            if (layers[layer].empty()) {
                layer--;
                continue;
            }

            int v = layers[layer].back();
            layers[layer].pop_back();
            activated[v] = false;

            push(v);
            if (excess[v] > 0) {
                if (count[height[v]] == 1) {
                    int cur = height[v];
                    for (int u = 0; u < n; u++)
                        if (height[u] >= cur) {
                            count[height[u]]--;
                            height[u] = n;
                            count[height[u]]++;
                            activate(u);
                        }
                } else {
                    relabel(v);
                }
            }
        }

        height[source] = n;
        return excess[sink];
    }

    bool left_of_mincut(int v) const {
        return height[v] == n;
    }
};
