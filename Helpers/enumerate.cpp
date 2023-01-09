template<typename iterator>
struct enumerated {
    struct enumerated_iterator {
        iterator it;
        int id;

        enumerated_iterator(iterator it, int id) : it(it), id(id) {}

        enumerated_iterator& operator++() {
            it++, id++;
            return *this;
        }

        auto operator*() {
            return std::make_pair(*it, id);
        }

        bool operator!=(const enumerated_iterator &another) const {
            return id != another.id;
        }
    };

    iterator t_begin, t_end;
    int size;

    enumerated(iterator t_begin, iterator t_end) : t_begin(t_begin), t_end(t_end), size(std::distance(t_begin, t_end)) {}

    enumerated_iterator begin() const {
        return enumerated_iterator(t_begin, 0);
    }

    enumerated_iterator end() const {
        return enumerated_iterator(t_end, size);
    }
};

template<typename T>
auto enumerate(T& container) {
    return enumerated(std::begin(container), std::end(container));
}
