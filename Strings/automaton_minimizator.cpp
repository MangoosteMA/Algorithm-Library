template<int SZ>
struct automaton_minimizator {
    struct node {
        std::vector<int> input[SZ];
        bool terminal = false;
        int nxt[SZ];

        node() {
            std::fill(nxt, nxt + SZ, -1);
        }
    };

    int n;
    std::vector<node> nodes;

    automaton_minimizator(int n = 0) : n(n), nodes(n) {}

    void add(int from, int to, int index) {
        assert(0 <= index && index < SZ);
        assert(0 <= from && from < n);
        assert(0 <= to && to < n);
        assert(nodes[from].nxt[index] == -1);
        nodes[to].input[index].push_back(from);
        nodes[from].nxt[index] = to;
    }

    void set_terminal(int v, bool terminal = true) {
        nodes[v].terminal = terminal;
    }

    std::vector<int> solve(int root) {
        auto bfs = [&]() {
            std::vector<bool> used(n);
            std::queue<int> que;
            que.push(root);
            used[root] = true;
            while (!que.empty()) {
                int v = que.front();
                que.pop();
                for (int c = 0; c < SZ; c++) {
                    if (nodes[v].nxt[c] != -1 && !used[nodes[v].nxt[c]]) {
                        used[nodes[v].nxt[c]] = true;
                        que.push(nodes[v].nxt[c]);
                    }
                }
            }
            return used;
        };
        auto used_from_root = bfs();

        std::vector<int> current_class(n, -1);
        for (int i = 0; i < n; i++)
            if (used_from_root[i])
                current_class[i] = nodes[i].terminal;

        if (!std::count(current_class.begin(), current_class.end(), 0) ||
            !std::count(current_class.begin(), current_class.end(), 1))
            for (int i = 0; i < n; i++)
                if (current_class[i] != -1)
                    current_class[i] = 0;

        int class_id = *max_element(current_class.begin(), current_class.end()) + 1;
        std::vector<std::unordered_set<int>> sets(class_id);
        std::queue<int> que;
        for (int i = 0; i < class_id; i++) {
            sets[i].rehash(2 * n);
            que.push(i);
        }

        for (int i = 0; i < n; i++)
            if (current_class[i] != -1)
                sets[current_class[i]].insert(i);

        std::unordered_map<int, std::vector<int>> vertexes;
        vertexes.rehash(2 * n);
        while (!que.empty()) {
            int num = que.front();
            que.pop();
            for (int c = 0; c < SZ; c++) {
                vertexes.clear();
                for (auto v : sets[num])
                    for (auto u : nodes[v].input[c])
                        if (current_class[u] != -1)
                            vertexes[current_class[u]].push_back(u);

                for (const auto &[to_split, adj] : vertexes) {
                    if (sets[to_split].size() == adj.size())
                        continue;

                    sets.push_back({});
                    sets.back().rehash(2 * adj.size());
                    for (auto v : adj) {
                        sets.back().insert(v);
                        sets[to_split].erase(v);
                        current_class[v] = class_id;
                    }
                    que.push(sets[to_split].size() < sets.back().size() ? to_split : class_id);
                    class_id++;
                }
            }
        }
        return current_class;
    }
};
