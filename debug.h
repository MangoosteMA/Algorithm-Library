namespace helpers {
    template<typename T, typename = void>
    struct is_printable : std::false_type {};

    template<typename T>
    struct is_printable<T, typename std::enable_if_t<std::is_same_v<decltype(std::cout << std::declval<T>()), std::ostream&>>> : std::true_type {};

    template<typename T, typename = void>
    struct is_iterable : std::false_type {};

    template<typename T>
    struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())), decltype(std::end(std::declval<T>()))>> : std::true_type {};
} // namespace helpers

namespace __to_string {
    std::string to_string(const bool &b) {
        return b ? "true" : "false";
    }

    std::string to_string(const std::string &str) {
        return "\"" + str + "\"";
    }

    template<size_t SZ>
    std::string to_string(const std::bitset<SZ> &bs) {
        std::string s;
        for (size_t i = 0; i < SZ; i++)
            s += bs[i] ? "1" : "0";

        return s;
    }

    std::string to_string(const std::vector<bool> &v) {
        std::string s;
        for (size_t i = 0; i < v.size(); i++)
            s += v[i] ? "1" : "0";

        return s;
    }

    template<typename T>
    typename std::enable_if_t<helpers::is_printable<T>::value, std::string> to_string(const T &t);

    template<typename T>
    typename std::enable_if_t<helpers::is_iterable<T>::value && !helpers::is_printable<T>::value, std::string> to_string(const T &v);

    template<typename A, typename B>
    std::string to_string(const std::pair<A, B> &p);

    template<typename T, typename _Container, typename _Compare>
    std::string to_string(std::priority_queue<T, _Container, _Compare> pq);

    template<typename T>
    typename std::enable_if_t<helpers::is_printable<T>::value, std::string> to_string(const T &t) {
        std::stringstream ss;
        if constexpr (helpers::is_iterable<T>::value)
            ss << '{';

        ss << std::fixed << std::setprecision(10) << t;
        if constexpr (helpers::is_iterable<T>::value)
            ss << '}';

        return ss.str();
    }

    template<typename T>
    typename std::enable_if_t<helpers::is_iterable<T>::value && !helpers::is_printable<T>::value, std::string> to_string(const T &v) {
        std::string s = "{", sep;
        for (const auto &x : v) {
            s += sep + to_string(x);
            sep = ", ";
        }
        return s + "}";
    }

    template<typename A, typename B>
    std::string to_string(const std::pair<A, B> &p) {
        return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
    }

    template<typename T, typename _Container, typename _Compare>
    std::string to_string(std::priority_queue<T, _Container, _Compare> pq) {
        std::string res = "{", sep;
        while (!pq.empty()) {
            res += sep + to_string(pq.top());
            sep = ", ";
            pq.pop();
        }
        res += "}";
        return res;
    }

    template<typename T>
    std::string to_string(std::stack<T> st) {
        std::vector<T> values;
        for (; !st.empty(); st.pop())
            values.push_back(st.top());
        
        std::reverse(values.begin(), values.end());
        return to_string(values);
    }
} // namespace __to_string

namespace tree_drawer {
    constexpr int HEIGHT = 3;
    constexpr int SEPARATOR = 3;
    constexpr char SYMBOL = '_';

    struct tree_grid {
        std::vector<std::vector<std::string>> grid;
        int root_position = 0;

        tree_grid() {}
        tree_grid(int index) : grid({{std::to_string(index)}}), root_position(0) {}

        friend std::ostream& operator<<(std::ostream& out, const tree_grid &grid) {
            if (grid.grid.empty())
                return out << '\n';

            std::vector<int> max_len(grid.grid[0].size());
            for (const auto &v : grid.grid)
                for (int i = 0; i < int(v.size()); i++)
                    max_len[i] = std::max<int>(max_len[i], v[i].size());

            std::vector<std::string> output;
            for (const auto &v : grid.grid) {
                output.push_back("");
                for (int i = 0; i < int(v.size()); i++) {
                    int extra = max_len[i] - v[i].size();
                    if (v[i] == "-")
                        output.back() += v[i] + std::string(extra, '-');
                    else
                        output.back() += std::string(extra / 2, ' ') + v[i] + std::string((extra + 1) / 2, ' ');
                }
            }

            bool first = true;
            for (auto &s : output) {
                if (!first)
                    out << '\n';
                else
                    first = false;

                for (int i = 0; i < int(s.size()); i++) {
                    if (s[i] == ' ') {
                        int j = i;
                        while (j < int(s.size()) && s[j] == ' ')
                            j++;

                        auto check = [](char c) {
                            return isdigit(c) || c == SYMBOL;
                        };
                        if (i != 0 && j + 1 < int(s.size()) && check(s[i - 1]) && check(s[i + 1]))
                            std::fill(s.begin() + i, s.begin() + j, SYMBOL);

                        i = j - 1;
                    }
                }

                out << s;
            }
            return out;
        }
    };

    tree_grid dfs(const std::vector<std::vector<int>> &g, int v, int p = -1) {
        std::vector<tree_grid> childs;
        for (auto u : g[v])
            if (u != p)
                childs.push_back(dfs(g, u, v));

        if (childs.empty())
            return tree_grid(v);

        int max_height = 0;
        for (const auto &grid : childs)
            max_height = std::max<int>(max_height, grid.grid.size());

        for (auto &grid : childs)
            grid.grid.resize(max_height, std::vector<std::string>(grid.grid[0].size()));

        int width = (childs.size() - 1) * SEPARATOR;
        for (const auto &grid : childs)
            width += grid.grid[0].size();

        tree_grid merged;
        merged.grid.resize(HEIGHT + max_height);
        for (int i = 0; i < HEIGHT; i++)
            merged.grid[i].resize(width);

        for (int i = 0; i < max_height; i++) {
            bool first = true;
            for (const auto &grid : childs) {
                if (!first) {
                    for (int j = 0; j < SEPARATOR; j++)
                        merged.grid[i + HEIGHT].push_back("");
                } else {
                    first = false;
                }
                merged.grid[HEIGHT + i].insert(merged.grid[HEIGHT + i].end(), grid.grid[i].begin(), grid.grid[i].end());
            }
        }
        
        if (childs.size() % 2 == 1) {
            for (int i = 0; i < int(childs.size() / 2); i++)
                merged.root_position += childs[i].grid[0].size() + SEPARATOR;

            merged.root_position += childs[childs.size() / 2].root_position;
        } else {
            for (int i = 0; i < int(childs.size() / 2); i++)
                merged.root_position += childs[i].grid[0].size() + SEPARATOR;

            merged.root_position -= SEPARATOR - SEPARATOR / 2;
        }
        merged.grid[0][merged.root_position] = std::to_string(v);

        int start_pos = 0;
        for (int i = 0; i < int(childs.size()); i++) {
            auto &grid = childs[i];
            std::pair<int, int> cur_pos{0, merged.root_position};
            std::pair<int, int> dest_pos{HEIGHT, start_pos + grid.root_position};

            std::vector<std::pair<int, int>> path;
            while (cur_pos != dest_pos) {
                path.push_back(cur_pos);
                if (cur_pos.second == dest_pos.second) {
                    cur_pos.first++;
                } else if (cur_pos.second < dest_pos.second) {
                    cur_pos.first += abs(dest_pos.second - cur_pos.second) <= HEIGHT;
                    cur_pos.second = std::min(dest_pos.second, cur_pos.second + 1);
                } else {
                    cur_pos.first += abs(dest_pos.second - cur_pos.second) <= HEIGHT;
                    cur_pos.second = std::max(dest_pos.second, cur_pos.second - 1);
                }
            }
            path.push_back(dest_pos);

            for (int i = 1; i + 1 < int(path.size()); i++) {
                const auto &now = path[i];
                const auto &prev = path[i - 1];
                if (now.first == prev.first)
                    merged.grid[now.first][now.second] = SYMBOL;
                else if (now.second == prev.second)
                    merged.grid[now.first][now.second] = "|";
                else if (now.second < prev.second)
                    merged.grid[now.first][now.second] = "/";
                else
                    merged.grid[now.first][now.second] = "\\";
            }

            start_pos += grid.grid[0].size() + SEPARATOR;
        }
        return merged;
    }

    template<typename T, typename _ = typename std::enable_if_t<std::is_integral_v<T>, T>>
    void draw_tree(const std::vector<std::vector<T>> &g, int root) {
        std::cerr << dfs(g, root) << '\n' << std::flush;
    }

    template<typename U,
             typename V,
             typename _ = typename std::enable_if_t<std::is_integral_v<U> && std::is_integral_v<V>, V>>
    void draw_tree(const std::vector<std::vector<std::pair<U, V>>> &g, int root) {
        std::vector<std::vector<U>> g_copy(g.size());
        for (int i = 0; i < int(g.size()); i++)
            for (const auto &[a, b] : g[i])
                g_copy[i].push_back(a);

        draw_tree(g_copy, root);
    }

    template<typename T, int array_size>
    void draw_tree(const std::vector<std::array<T, array_size>> &g, int root) {
        std::vector<std::vector<T>> g_copy(g.size());
        for (int i = 0; i < int(g.size()); i++)
            for (const auto &arr : g[i])
                g_copy[i].push_back(arr[0]);

        draw_tree(g_copy, root);
    }

    template<typename edge_t,
             typename _ = typename std::enable_if_t<!std::is_integral_v<edge_t>, edge_t>,
             typename __ = int>
    void draw_tree(const std::vector<std::vector<edge_t>> &g, int root) {
        std::vector<std::vector<int>> g_copy(g.size());
        for (int i = 0; i < int(g.size()); i++)
            for (const auto &edge : g[i])
                g_copy[i].push_back(edge.to); // TODO: change this line in case edge_t has another dest field

        draw_tree(g_copy, root);
    }
} // namespace tree_drawer

namespace array_drawer {
    template<typename array_t>
    void draw_array(const array_t &arr) {
        if (arr.empty()) {
            std::cerr << '\n' << std::flush;
            return;
        }

        auto max_element = *std::max_element(arr.begin(), arr.end());
        max_element = std::max(max_element, 0);

        std::vector<std::string> a(max_element, std::string(arr.size(), '.'));
        for (int i = 0; i < int(arr.size()); i++)
            for (int j = 0; j < arr[i]; j++)
                a.rbegin()[j][i] = '#';

        for (const auto &s : a)
            std::cerr << s << '\n';

        std::cerr << '\n' << std::flush;
    }
} // namespace array_drawer

void __dbg_out() {
    std::cerr << std::endl;
}

template<typename Head, typename... Tail>
void __dbg_out(Head head, Tail... tail) {
    std::cerr << __to_string::to_string(head);
    if (sizeof...(tail))
        std::cerr << ", ";

    __dbg_out(tail...);
}

#define dprint(...) __dbg_out(__VA_ARGS__)
#define dbg(...) std::cerr << "#" << __LINE__ << " [" << #__VA_ARGS__ << "]: ", __dbg_out(__VA_ARGS__)
#define debug if constexpr (true)
#define draw_tree(graph, root) tree_drawer::draw_tree(graph, root)
#define draw_array(array) array_drawer::draw_array(array)
