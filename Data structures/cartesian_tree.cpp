std::mt19937 __rng__(std::chrono::steady_clock::now().time_since_epoch().count());

template<typename T>
struct cartesian_tree {
    struct node {
        T x;
        int left, right, sz, y;

        node(T x) : x(x), left(-1), right(-1), sz(1), y(__rng__()) {}
    };

    int root;
    std::vector<node> tree;

    cartesian_tree() : root(-1) {}

    void relax(int v) {
        tree[v].sz = 1 + size(tree[v].left) + size(tree[v].right);
    }

    int size(int v) const {
        return (v == -1 ? 0 : tree[v].sz);
    }

    int new_node(T x) {
        int id = int(tree.size());
        tree.push_back(node(x));
        return id;
    }

    std::pair<int, int> split_by_key(int v, T x) {
        if (v == -1)
            return {v, v};

        if (tree[v].x <= x) {
            auto cur = split_by_key(tree[v].right, x);
            tree[v].right = cur.first;
            relax(v);
            return {v, cur.second};
        } else {
            auto cur = split_by_key(tree[v].left, x);
            tree[v].left = cur.second;
            relax(v);
            return {cur.first, v};
        }
    }

    std::pair<int, int> split_by_size(int v, int n) {
        if (v == -1)
            return {v, v};

        if (size(tree[v].left) + 1 <= n) {
            auto cur = split_by_size(tree[v].right, n - size(tree[v].left) - 1);
            tree[v].right = cur.first;
            relax(v);
            return {v, cur.second};
        } else {
            auto cur = split_by_size(tree[v].left, n);
            tree[v].left = cur.second;
            relax(v);
            return {cur.first, v};
        }
    }

    int merge(int left, int right) {
        if (left == -1)
            return right;

        if (right == -1)
            return left;

        if (tree[left].y > tree[right].y) {
            tree[left].right = merge(tree[left].right, right);
            relax(left);
            return left;
        } else {
            tree[right].left = merge(left, tree[right].left);
            relax(right);
            return right;
        }
    }

    int insert(int v, T x, int nv) {
        if (v == -1)
            return nv;

        if (tree[nv].y < tree[v].y) {
            if (x < tree[v].x)
                tree[v].left = insert(tree[v].left, x, nv);
            else
                tree[v].right = insert(tree[v].right, x, nv);

            relax(v);
            return v;
        } else {
            auto [left, right] = split_by_key(v, x);
            tree[nv].left = left;
            tree[nv].right = right;
            relax(nv);
            return nv;
        }
    }

    void insert(T x) {
        int v = new_node(x);
        root = insert(root, x, v);
    }

    int erase(int v, T x) {
        if (v == -1)
            return -1;

        if (tree[v].x == x)
            return merge(tree[v].left, tree[v].right);

        if (x < tree[v].x)
            tree[v].left = erase(tree[v].left, x);
        else
            tree[v].right = erase(tree[v].right, x);

        relax(v);
        return v;
    }

    void erase(T x) {
        root = erase(root, x);
    }

    int size() const {
        return size(root);
    }

    T at(int v, int pos) const {
        if (size(tree[v].left) > pos)
            return at(tree[v].left, pos);

        pos -= size(tree[v].left);
        if (!pos)
            return tree[v].x;

        return at(tree[v].right, pos - 1);
    }

    T at(int pos) const {
        return at(root, pos);
    }

    bool contains(int v, T x) const {
        if (v == -1)
            return false;

        if (tree[v].x == x)
            return true;

        if (tree[v].x < x)
            return contains(tree[v].right, x);

        return contains(tree[v].left, x);
    }

    bool contains(T x) const {
        return contains(root, x);
    }
};
