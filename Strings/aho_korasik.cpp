template<int SZ, typename T>
struct aho_korasik {
    struct node {
        node* nxt[SZ], *parent;
        std::vector<int> ends;
        int parent_index, depth;
        node* suffix_link, *supper_suffix_link;

        node() : parent(nullptr), parent_index(-1), depth(0), suffix_link(nullptr), supper_suffix_link(nullptr) {
            std::fill(std::begin(nxt), std::end(nxt), nullptr);
        }

        bool is_terminal() const {
            return !ends.empty();
        }

        node* go_ignoring_extra_edges(int index) const {
            node* v = nxt[index];
            return v && v->depth == depth + 1 ? v : nullptr;
        }
    };

    T index;
    node* root;

    aho_korasik(T index) : index(index), root(new node()) {}

    template<typename U>
    node* add(const U &str, int id = -1) {
        node* v = root;
        for (int i = 0; i < int(str.size()); i++) {
            int cur_index = index(str[i]);
            if (!v->nxt[cur_index]) {
                v->nxt[cur_index] = new node();
                v->nxt[cur_index]->parent = v;
                v->nxt[cur_index]->parent_index = cur_index;
                v->nxt[cur_index]->depth = v->depth + 1;
            }
            v = v->nxt[cur_index];
        }
        if (id != -1)
            v->ends.push_back(id);

        return v;
    }

    void init() {
        std::queue<node*> que;
        que.push(root);

        while (!que.empty()) {
            node* v = que.front();
            que.pop();

            if (v != root) {
                if (v->parent == root)
                    v->suffix_link = root;
                else
                    v->suffix_link = v->parent->suffix_link->nxt[v->parent_index];

                if (v->suffix_link->is_terminal())
                    v->supper_suffix_link = v->suffix_link;
                else
                    v->supper_suffix_link = v->suffix_link->supper_suffix_link;
            }

            for (int i = 0; i < SZ; i++)
                if (v->nxt[i])
                    que.push(v->nxt[i]);
                else if (v == root)
                    v->nxt[i] = root;
                else
                    v->nxt[i] = v->suffix_link->nxt[i];
        }
    }

    std::vector<node*> get_vertices() {
        std::vector<node*> vertices{root};
        for (int i = 0; i < int(vertices.size()); i++)
            for (int j = 0; j < SZ; j++) {
                auto vertex = vertices[i]->go_ignoring_extra_edges(j);
                if (vertex)
                    vertices.push_back(vertex);
            }

        return vertices;
    }

    ~aho_korasik() {
        for (auto vertex : get_vertices())
            delete vertex;
    }
};
