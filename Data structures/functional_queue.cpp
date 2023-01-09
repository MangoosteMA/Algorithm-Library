template<typename T, typename merge_t>
struct functional_queue {
    functional_stack<T, merge_t> front_stack, back_stack;
    const merge_t merge;

    functional_queue(const merge_t &merge) : front_stack(merge), back_stack(merge), merge(merge) {}

    int size() const {
        return front_stack.size() + back_stack.size();
    }

    bool empty() const {
        return front_stack.empty() && back_stack.empty();
    }

    void push(T x) {
        back_stack.push(x);
    }

    void pop() {
        assert(!empty());
        if (front_stack.empty()) {
            std::swap(front_stack.st, back_stack.st);
            front_stack.reverse();
        }
        assert(!front_stack.empty());
        front_stack.pop();
    }

    T query() const {
        assert(!empty());
        if (back_stack.empty())
            return front_stack.query();

        if (front_stack.empty())
            return back_stack.query();

        return merge(front_stack.query(), back_stack.query());
    }
};
