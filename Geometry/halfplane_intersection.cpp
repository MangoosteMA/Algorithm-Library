// only finite case

template<typename T>
struct point {
    T x, y;

    T operator%(const point<T> &p) const {
        return x * p.y - y * p.x;
    }
};

struct plane {
    ll a, b, c;

    point<ld> intersection(const plane &p) const {
        ld denom = ld(a * p.b) - ld(b * p.a);
        assert(abs(denom) > 1e-5);
        return {(b * p.c - c * p.b) / denom, (c * p.a - a * p.c) / denom};
    }

    bool contains(const point<ld> &p) const {
        return a * p.x + b * p.y + c > -1e-8;
    }

    point<ll> get_vector() const {
        return point<ll>{a, b};
    }

    point<ld> any_point() const {
        if (b != 0)
            return point<ld>{1, ld(-c - a) / b};

        assert(a != 0);
        return point<ld>{ld(-c - b) / a, 1};
    }
};

vector<point<ld>> halfplane_intersection(vector<plane> planes) {
    int n = len(planes);

    auto half = [&](const plane &p) {
        return pair{p.a, p.b} < pair{0ll, 0ll};
    };

    sort(all(planes), [&](const plane &a, const plane &b) {
        if (half(a) != half(b))
            return half(a) < half(b);

        auto prod = a.get_vector() % b.get_vector();
        if (prod != 0)
            return prod > 0;

        return !b.contains(a.any_point());
    });

    // erase useless planes
    vector<plane> new_planes;
    for (int i = 0; i < n; i++)
        if (i == n - 1 || half(planes[i]) != half(planes[i + 1])
        || planes[i].get_vector() % planes[i + 1].get_vector() != 0)
            new_planes.push_back(planes[i]);

    planes = new_planes;
    n = len(planes);

    deque<point<ld>> points;
    deque<plane> hull;

    for (auto &p : planes) {
        while (!points.empty() && !p.contains(points.back())) {
            points.pop_back();
            hull.pop_back();
        }

        while (!points.empty() && !p.contains(points.front())) {
            points.pop_front();
            hull.pop_front();
        }

        if (!hull.empty()) {
            auto pt = p.intersection(hull.back());
            if (!hull[0].contains(pt))
                continue;

            points.push_back(pt);
        }
        hull.push_back(p);
    }

    if (points.empty())
        return {};

    // be careful with infinity intersection here
    points.push_back(hull.back().intersection(hull.front()));
    return vector(all(points));
}
