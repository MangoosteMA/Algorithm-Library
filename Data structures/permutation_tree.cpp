class permutation_tree {
private:
    struct segtree_node {
        std::pair<int, int> min = {0, 1};
        int mod = 0;

        void apply(int delta, int, int) {
            min.first += delta;
            mod += delta;
        }

        void push(segtree_node &child, int, int, int cl, int cr) {
            child.apply(mod, cl, cr);
        }

        void clear_after_push() {
            mod = 0;
        }

        static segtree_node merge(const segtree_node &left, const segtree_node &right) {
            segtree_node result;
            result.min.first = std::min(left.min.first, right.min.first);
            result.min.second = (left.min.first == result.min.first ? left.min.second : 0)
                              + (right.min.first == result.min.first ? right.min.second : 0);
            return result;
        }
    };

public:
    enum class type_t {
        CUT_NODE,
        INCREASING_JOIN_NODE,
        DECREASING_JOIN_NODE
    };

    int root;
    std::vector<std::vector<int>> g;
    std::vector<int> from, to;
    std::vector<type_t> type;

    permutation_tree(const std::vector<int> &perm) {
        int n = perm.size();
        segtree<segtree_node> tree(n);
        std::vector<int> stack_min{-1}, stack_max{-1};

        std::stack<int> vers;
        std::vector<int> leftmost(n);
        std::iota(leftmost.begin(), leftmost.end(), 0);
        from = to = perm;
        g.resize(n);
        type.resize(n, type_t::INCREASING_JOIN_NODE);
        type.reserve(2 * n);
        g.reserve(2 * n);
        from.reserve(2 * n);
        to.reserve(2 * n);
        leftmost.reserve(2 * n);

        for (int i = 0; i < n; i++) {
            tree.update(0, i, -1);
            while (stack_min.size() > 1 && perm[stack_min.back()] > perm[i]) {
                tree.update(stack_min.end()[-2] + 1, stack_min.back() + 1, perm[stack_min.back()] - perm[i]);
                stack_min.pop_back();
            }
            while (stack_max.size() > 1 && perm[stack_max.back()] < perm[i]) {
                tree.update(stack_max.end()[-2] + 1, stack_max.back() + 1, perm[i] - perm[stack_max.back()]);
                stack_max.pop_back();
            }
            stack_min.push_back(i);
            stack_max.push_back(i);

            int cut_index = tree.find_first(0, [&](const segtree_node &nd, int, int) {
                return nd.min.first == 0;
            });
            int v = i;
            while (!vers.empty() && leftmost[vers.top()] >= cut_index) {
                if (i - leftmost[vers.top()] == std::max(to[vers.top()], to[v]) - std::min(from[vers.top()], from[v])) {
                    bool is_child = false;
                    is_child |= type[vers.top()] == type_t::INCREASING_JOIN_NODE && to[vers.top()] < from[v];
                    is_child |= type[vers.top()] == type_t::DECREASING_JOIN_NODE && from[vers.top()] > to[v];
                    if (vers.top() >= n && is_child && type[vers.top()] != type_t::CUT_NODE) {
                        g[vers.top()].push_back(v);
                        from[vers.top()] = std::min(from[vers.top()], from[v]);
                        to[vers.top()] = std::max(to[vers.top()], to[v]);
                        v = vers.top();
                    } else {
                        int u = g.size();
                        g.push_back({vers.top(), v});
                        from.push_back(std::min(from[vers.top()], from[v]));
                        to.push_back(std::max(to[vers.top()], to[v]));
                        type.push_back(to[vers.top()] < from[v] ? type_t::INCREASING_JOIN_NODE
                                                                : type_t::DECREASING_JOIN_NODE);
                        leftmost.push_back(leftmost[vers.top()]);
                        v = u;
                    }
                    vers.pop();
                } else {
                    int u = g.size();
                    g.push_back({v});
                    from.push_back(from[v]);
                    to.push_back(to[v]);
                    type.push_back(type_t::CUT_NODE);
                    leftmost.push_back(leftmost[v]);
                    while (!vers.empty()) {
                        from[u] = std::min(from[u], from[vers.top()]);
                        to[u] = std::max(to[u], to[vers.top()]);
                        leftmost[u] = leftmost[vers.top()];
                        g[u].push_back(vers.top());
                        vers.pop();
                        if (tree.query(leftmost[u], leftmost[u] + 1).min.first == 0) {
                            break;
                        }
                    }
                    std::reverse(g[u].begin(), g[u].end());
                    v = u;
                }
            }
            vers.push(v);
        }
        assert(vers.size() == 1);
        root = vers.top();
    }

    int size() const {
        return g.size();
    }

    template<typename T>
    T get_good_segs() const {
        T total = size();
        for (int i = 0; i < size(); i++) {
            if (type[i] != type_t::CUT_NODE && !g[i].empty()) {
                total += T(g[i].size() + 1) * T(g[i].size()) / 2 - T(g[i].size()) - 1;
            }
        }
        return total;
    }
};
