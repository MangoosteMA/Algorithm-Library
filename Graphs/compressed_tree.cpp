/*
 * Include lca_tree to use it.
 * Zero based.
 * Type T is the type of the weight of each edge (int by default).
 * The constructor gets lca_tree with .build(root) method runned.
*/

template<typename T = int>
struct compressed_tree {
    lca_tree<T> lca;
    std::vector<std::vector<pair<int, T>>> g;
    std::vector<int> used_vertexes;

    compressed_tree(const lca_tree<T> &lca = lca_tree<T>()) : lca(lca), g(lca.size()) {}

    // Gets vertexes of the compressed tree.
    // Stores all used vertexes in used_vertexes.
    // For every used vertexes stores its adjacent vertexes.
    // Returns the root of the compressed tree (or -1 if it is empty).
    int build(std::vector<int> vertexes) {
        for (auto v : used_vertexes)
            g[v].clear();

        used_vertexes.clear();

        auto compare_tin = [&](int v, int u) {
            return lca.tin[v] < lca.tin[u];
        };
        std::sort(vertexes.begin(), vertexes.end(), compare_tin);

        int n_vertexes = vertexes.size();
        for (int i = 0; i + 1 < n_vertexes; i++)
            vertexes.push_back(lca.lca(vertexes[i], vertexes[i + 1]));

        std::sort(vertexes.begin(), vertexes.end(), compare_tin);
        vertexes.resize(std::unique(vertexes.begin(), vertexes.end()) - vertexes.begin());

        auto add = [&](int v, int u) {
            g[v].emplace_back(u, lca.depth[u] - lca.depth[v]);
            g[u].emplace_back(v, lca.depth[u] - lca.depth[v]);
        };

        std::vector<int> current_path;
        for (auto v : vertexes) {
            while (!current_path.empty() && !lca.is_ancestor(current_path.back(), v))
                current_path.pop_back();
            
            if (!current_path.empty())
                add(current_path.back(), v);
            
            current_path.push_back(v);
        }

        used_vertexes.swap(vertexes);
        return current_path.empty() ? -1 : current_path.front();
    }
};
