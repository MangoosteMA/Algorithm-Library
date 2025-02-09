/*
 * Iteratively builds two trees for even and odd palindromes.
 * Each node corresponds to a unique palindrome.
 * The parent of each palindrome P is P[1:-1].
 */
template<int ALPHABET = 26, int OFFSET = 'a'>
class palindromic_tree {
private:
    int new_node() {
        tree.push_back(node());
        return static_cast<int>(tree.size()) - 1;
    }

    int find_suffix(int v) {
        int n = str.size();
        while (tree[v].length == n - 1 || str.back() != str[n - 2 - tree[v].length]) {
            v = tree[v].suflink;
        }
        return v;
    }

public:
    struct node {
        int length = 0;
        int suflink = -1;  // the longest palindrome-suffix node
        int to[ALPHABET];  // for each symbol 'c' stores the node of the palindrome "c(this)c"

        node() {
            memset(to, -1, sizeof(to));
        }
    };

    int even;   // even palindromes tree root (corresponds to an empty palindrome)
    int odd;    // odd palindromes tree root (corresponds to the palindrome of length -1)
    int last;   // longest suffix palindrome
    std::vector<node> tree;
    std::vector<int> str;

    palindromic_tree() {
        odd = new_node();
        even = new_node();
        tree[even].suflink = tree[odd].suflink = odd;
        tree[odd].length = -1;
        last = even;
    }

    void add(int symbol) {
        symbol -= OFFSET;
        str.push_back(symbol);
        last = find_suffix(last);
        if (tree[last].to[symbol] == -1) {
            int v = new_node();
            tree[v].length = tree[last].length + 2;
            int u = find_suffix(tree[last].suflink);
            if (tree[u].to[symbol] != -1) {
                tree[v].suflink = tree[u].to[symbol];
            } else {
                tree[v].suflink = even;
            }
            tree[last].to[symbol] = v;
        }
        last = tree[last].to[symbol];
    }

    // Returns the number of unique non-empty palindromes.
    int palindromes() const {
        return static_cast<int>(tree.size()) - 2;
    }
};
