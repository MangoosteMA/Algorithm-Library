template<typename T>
struct Splay {
    struct Node {
        T x, mn;
        int sz, parent, left, right;

        Node(T x) : x(x), mn(x), sz(1), parent(-1), left(-1), right(-1) {}
    };

    vector<Node> tree;
    int root;

    Splay() : root(-1) {}

    int new_node(int x) {
        tree.push_back(Node(x));
        return int(tree.size()) - 1;
    }

    void reserve(int n) {
        tree.reserve(n);
    }

    int size(int x) const {
        return (x == -1 ? 0 : tree[x].sz);
    }

    int size() const {
        return size(root);
    }

    T mn(int x) const {
        return (x == -1 ? numeric_limits<T>::max() : tree[x].mn);
    }

    T mn() const {
        return mn(root);
    }

    void relax(int x) {
        tree[x].sz = 1 + size(tree[x].left) + size(tree[x].right);
        tree[x].mn = min({ tree[x].x, mn(tree[x].left), mn(tree[x].right) });
    }

    void rotate(int x) {
        int y = tree[x].parent, z = tree[y].parent;

        tree[x].parent = z;
        if (z != -1)
            (y == tree[z].left ? tree[z].left : tree[z].right) = x;

        if (tree[y].left == x) {
            tree[y].left = tree[x].right;
            if (tree[y].left != -1)
                tree[tree[y].left].parent = y;

            tree[x].right = y;
        } else {
            tree[y].right = tree[x].left;
            if (tree[y].right != -1)
                tree[tree[y].right].parent = y;

            tree[x].left = y;
        }

        tree[y].parent = x;
        relax(y);
        relax(x);
    }

    void splay(int x) {
        root = x;
        if (x == -1)
            return;

        while (tree[x].parent != -1) {
            int y = tree[x].parent;

            if (tree[y].parent != -1) {
                int z = tree[y].parent;
                rotate(((y == tree[z].left) == (x == tree[y].left)) ? y : x);
            }
            rotate(x);
        }
    }

    bool insert(T x) {
        if (root == -1) {
            root = new_node(x);
            return true;
        }

        while (true) {
            if (tree[root].x == x) {
                splay(root);
                return false;
            }

            if (x < tree[root].x) {
                if (tree[root].left == -1) {
                    tree[root].left = new_node(x);
                    tree[tree[root].left].parent = root;

                    splay(tree[root].left);
                    return true;
                }

                root = tree[root].left;
            } else {
                if (tree[root].right == -1) {
                    tree[root].right = new_node(x);
                    tree[tree[root].right].parent = root;

                    splay(tree[root].right);
                    return true;
                }

                root = tree[root].right;
            }
        }
    }

    int rightest(int x) {
        while (tree[x].right != -1)
            x = tree[x].right;

        splay(x);
        return x;
    }

    int leftest(int x) {
        while (tree[x].left != -1)
            x = tree[x].left;

        splay(x);
        return x;
    }

    int merge(int l, int r) {
        if (l == -1)
            return r;

        if (r == -1)
            return l;

        int x = rightest(l);
        tree[x].right = r;
        tree[r].parent = x;

        relax(x);
        return x;
    }

    pair<int, int> splitX(int v, T x) {                                  // ((-INF, x], (x, INF))
        if (mn(v) > x)
            return {-1, v};

        while (true) {
            if (tree[v].x <= x) {
                if (mn(tree[v].root) > x) {
                    splay(v);
                    break;
                }

                v = tree[v].root;
            } else {
                if (tree[v].left == -1) {
                    splay(v);
                    break;
                }

                v = tree[v].left;
            }
        }

        int u = tree[v].root;
        tree[v].root = -1;
        relax(v);

        if (u != -1)
            tree[u].parent = -1;

        return {v, u};
    }

    int at(int v, int i) {
        while (true) {
            int lsz = size(tree[v].left);
            if (i < lsz) {
                v = tree[v].left;
                continue;
            }

            i -= lsz + 1;
            if (i == -1) {
                splay(v);
                return v;
            }

            v = tree[v].root;
        }
    }

    int at(int i) {
        return tree[at(root, i)].x;
    }

    pair<int, int> splitN(int v, int n) {                                // (first n elements, last N - n elements)
        v = at(v, n - 1);
        int u = tree[v].root;

        tree[v].root = -1;
        relax(v);
        if (u != -1)
            tree[u].parent = -1;

        return {v, u};
    }

    bool erase(T x) {
        if (root == -1)
            return false;

        while (true) {
            if (tree[root].x == x) {
                splay(root);
                if (tree[root].left != -1)
                    tree[tree[root].left].parent = -1;

                if (tree[root].right != -1)
                    tree[tree[root].right].parent = -1;

                root = merge(tree[root].left, tree[root].right);
                return true;
            }

            if (x < tree[root].x) {
                if (tree[root].left == -1) {
                    splay(root);
                    return false;
                }

                root = tree[root].left;
            } else {
                if (tree[root].right == -1) {
                    splay(root);
                    return false;
                }

                root = tree[root].right;
            }
        }
    }

    bool contains(T x) {
        if (root == -1)
            return false;

        while (true) {
            if (tree[root].x == x) {
                splay(root);
                return true;
            }

            if (x < tree[root].x) {
                if (tree[root].left == -1) {
                    splay(root);
                    return false;
                }

                root = tree[root].left;
            } else {
                if (tree[root].right == -1) {
                    splay(root);
                    return false;
                }

                root = tree[root].right;
            }
        }
    }

    int lowerBound(int x) {
        if (root == -1)
            return -1;

        int result = -1;
        while (true) {
            if (tree[root].x >= x) {
                result = root;

                if (tree[root].left == -1) {
                    splay(root);
                    break;
                }

                root = tree[root].left;
            } else {
                if (tree[root].right == -1) {
                    splay(root);
                    break;
                }

                root = tree[root].right;
            }
        }

        return result;
    }

    int underBound(int x) {
        if (root == -1)
            return -1;

        int result = -1;
        while (true) {
            if (tree[root].x <= x) {
                result = root;

                if (tree[root].right == -1) {
                    splay(root);
                    break;
                }

                root = tree[root].right;
            } else {
                if (tree[root].left == -1) {
                    splay(root);
                    break;
                }

                root = tree[root].left;
            }
        }

        return result;
    }
};
