template<int SZ, typename T>
struct suffix_automaton {
    struct node {
        node* nxt[SZ], *parent, *link;
        int parent_index, min_length, max_length, end_position;

        node() : parent(nullptr), link(nullptr), parent_index(-1), min_length(0), max_length(0), end_position(-1) {
            std::fill(std::begin(nxt), std::end(nxt), nullptr);
        }
    };

    T index;
    node* root;

    suffix_automaton(T index) : index(index), root(nullptr) {}

    template<typename U>
    node* init(const U &str) {
        if (root)
            _delete();

        root = new node();
        auto v = root;
        for (int i = 0; i < int(str.size()); i++) {
            int current_index = index(str[i]);

            auto u = new node();
            u->link = root;
            u->parent = v;
            u->parent_index = current_index;
            u->min_length = u->max_length = i + 1;
            u->end_position = i;

            for (auto w = v; w; w = w->link) {
                if (!w->nxt[current_index]) {
                    w->nxt[current_index] = u;
                    u->min_length = w->min_length + 1;
                    continue;
                }

                auto c = w->nxt[current_index];
                if (c->parent == w) {
                    u->link = c;
                    break;
                }

                auto clone = new node();
                std::copy(std::begin(c->nxt), std::end(c->nxt), std::begin(clone->nxt));
                clone->link = c->link;
                clone->parent = w;
                clone->parent_index = current_index;
                clone->min_length = c->min_length;
                clone->max_length = w->max_length + 1;
                clone->end_position = c->end_position;

                u->link = c->link = clone;
                c->min_length = clone->max_length + 1;

                for (; w && w->nxt[current_index] == c; w = w->link)
                    w->nxt[current_index] = clone;

                break;
            }
            v = u;
        }
        return v;
    }

    std::vector<node*> get_vertices() const {
        std::vector<node*> vertices;
        vertices.push_back(root);
        for (int i = 0; i < int(vertices.size()); i++)
            for (int j = 0; j < SZ; j++)
                if (vertices[i]->nxt[j] && vertices[i]->nxt[j]->parent == vertices[i])
                    vertices.push_back(vertices[i]->nxt[j]);

        return vertices;
    }

    void _delete() {
        for (node* vertex : get_vertices())
            delete vertex;
    }

    ~suffix_automaton() {
        _delete();
    }
};
