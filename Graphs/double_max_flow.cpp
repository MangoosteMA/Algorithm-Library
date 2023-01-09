using namespace std;
using ld = long double;

const ld EPS = 1e-7;

struct max_flow {
    struct edge {
        int to, rev;
        ld capasity, flow;
    };

    int n;
    vector<vector<edge>> g;
    vector<int> height;

    max_flow(int n) : n(n), g(n), height(n) {}

    void add(int from, int to, ld capasity) {
        g[from].push_back({to, int(g[to].size()), capasity, 0});
        g[to].push_back({from, int(g[from].size()) -1, 0, 0});
    }

    ld solve(int source, int sink) {
        vector<int> count(n + 1);
        count[0] = n;
        vector<bool> activated(n);
        activated[sink] = true;
        vector<ld> excess(n);
        vector<vector<int>> layers(n);
        int layer = 0;

        auto activate = [&](int v) {
            if (!activated[v] && excess[v] > EPS && height[v] < n) {
                activated[v] = true;
                layer = max(layer, height[v]);
                layers[height[v]].push_back(v);
            }
        };

        auto push = [&](int v) {
            for (auto &e : g[v])
                if (height[e.to] == height[v] - 1) {
                    ld pushed = min(e.capasity - e.flow, excess[v]);
                    if (pushed > EPS) {
                        e.flow += pushed;
                        g[e.to][e.rev].flow -= pushed;
                        excess[v] -= pushed;
                        excess[e.to] += pushed;
                        activate(e.to);
                        if (excess[v] < EPS)
                            break;
                    }
                }
        };

        auto relabel = [&](int v) {
            count[height[v]]--;
            height[v] = n;
            for (auto &e : g[v])
                if (e.flow + EPS < e.capasity)
                    height[v] = min(height[v], height[e.to] + 1);

            activate(v);
            count[height[v]]++;
        };

        for (auto &e : g[source])
            excess[source] += e.capasity;

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
            if (excess[v] > EPS) {
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
};
