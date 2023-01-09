// building suffix tree from suffix array
struct suffix_tree {
    struct node {
        struct edge {
            node* to;
            int left, right; // substring [left, right)
        };

        std::vector<edge> childs; // sorted lexicographically
        bool terminal;
        int depth;

        node() : terminal(false), depth(0) {}
    };

    node* root;

    template<typename T>
    suffix_tree(T str = T()) {
        int n = str.size();
        suffix_array sa(str);

        root = new node();
        root->terminal = true;
        std::vector<node*> st;
        st.reserve(n);
        st.push_back(root);

        for (int i = 1; i <= n; i++) {
            while (st.size() > 1 && st.end()[-2]->depth >= sa.lcp[i])
                st.pop_back();

            node* v = new node(), *u = st.back();
            v->terminal = true;
            v->depth = n - sa.order[i];

            if (u->depth == sa.lcp[i]) {
                u->childs.push_back({v, sa.order[i] + sa.lcp[i], n});
            } else {
                assert(st.size() > 1);
                node* w = st.end()[-2], *aux = new node();
                assert(w->childs.back().to == u);
                int delta = sa.lcp[i] - w->depth;
                aux->childs.push_back(w->childs.back());
                aux->childs.back().left += delta;
                aux->depth = w->depth + delta;
                w->childs.back().to = aux;
                w->childs.back().right = aux->childs.back().left;
                aux->childs.push_back({v, sa.order[i] + sa.lcp[i], n});

                st.pop_back();
                st.push_back(aux);
            }
            st.push_back(v);
        }
    }

    std::vector<node*> get_vertices() const {
        std::vector<node*> vertices;
        vertices.push_back(root);
        for (int i = 0; i < int(vertices.size()); i++)
            for (auto &edge : vertices[i]->childs)
                vertices.push_back(edge.to);

        return vertices;
    }

    ~suffix_tree() {
        for (node* vertex : get_vertices())
            delete vertex;
    }
};
