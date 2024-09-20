/*
 * Zero based.
 * Don't forget to run .build(root) method before using it.
 */
class tree_bfs_ordering {
private:
    int n, timer;
    std::vector<int> depth_left;

    void dfs(int v) {
        tin[v] = timer++;
        for (auto u : g[v]) {
            if (u == parent[v]) {
                continue;
            }
            parent[u] = v;
            dfs(u);
        }
        tout[v] = timer;
    }

    void bfs(int root) {
        depth.assign(n, -1);
        order = {root};
        order.reserve(n);
        depth[root] = 0;
        for (int ptr = 0; ptr < n; ptr++) {
            int v = order[ptr];
            for (int u : g[v]) {
                if (depth[u] == -1) {
                    depth[u] = depth[v] + 1;
                    order.push_back(u);
                }
            }
        }

        int max_depth = depth[order.back()];
        depth_left.assign(max_depth + 2, 0);
        for (int v = 0; v < n; v++) {
            depth_left[depth[v] + 1]++;
        }
        for (int d = 0; d <= max_depth; d++) {
            depth_left[d + 1] += depth_left[d];
        }
    }

    int search(int range_depth, int t) {
        int left = depth_left[range_depth];
        int right = depth_left[range_depth + 1];
        return std::partition_point(order.begin() + left, order.begin() + right, [&](int v) {
            return tin[v] < t;
        }) - order.begin();
    }

public:
    std::vector<std::vector<int>> g;
    std::vector<int> tin, tout, depth, parent, order;

    tree_bfs_ordering(int n = 0) : n(n), g(n) {}

    void add(int v, int u) {
        g[v].push_back(u);
        g[u].push_back(v);
    }

    int size() const {
        return n;
    }

    void build(int root) {
        timer = 0;
        tin.resize(n);
        tout.resize(n);
        parent.assign(n, -1);
        dfs(root);
        bfs(root);
    }

    // Returns true if v == u or v is ancestor of u.
    bool is_ancestor(int v, int u) const {
        return tin[v] <= tin[u] && tout[u] <= tout[v];
    }

    /*
     * Returns interval [l, r), such that order[l : r] contains verteces
       from the subtree of vertex 'v' at the distance of 'dist' from vertex 'v'.
     * If there's no such verteces, returns [0, 0).
     * Complexity: O(log(n)).
     */
    std::pair<int, int> range(int v, int dist) {
        int range_depth = depth[v] + dist;
        if (range_depth + 1 >= static_cast<int>(depth_left.size())) {
            return {0, 0};
        }
        return {search(range_depth, tin[v]), search(range_depth, tout[v])};
    }

    /*
     * Returns set of at most 2 * dist + 1 disjoint ranges, containing all verteces at the distance of at most 'dist'.
     * Complexity: O(dist * log(n)).
     */
    std::vector<std::pair<int, int>> ranges(int v, int dist) {
        std::vector<std::pair<int, int>> ranges;
        ranges.reserve(2 * dist + 1);
        int p = v;
        for (int d = depth[v] + dist; d >= std::max(0, depth[v] - dist); d--) {
            if (parent[p] != -1 && depth[v] + d - 2 * depth[parent[p]] <= dist) {
                p = parent[p];
            }
            auto [l, r] = range(p, d - depth[p]);
            if (l < r) {
                ranges.emplace_back(l, r);
            }
        }
        return ranges;
    }
};
