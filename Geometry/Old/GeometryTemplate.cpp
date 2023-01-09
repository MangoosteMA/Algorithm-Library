template<typename T>
int sgn(T x) {
    if (x < 0)
        return -1;

    return x > 0;
}

template<typename T = long long>
struct Point {
    T x, y;

    Point() : x(0), y(0) {}
    Point(T x, T y) : x(x), y(y) {}

    Point(const Point<T> &a, const Point<T> &b) : x(b.x - a.x), y(b.y - a.y) {}

    template<typename A>
    Point(const Point<A> &pt) : x(pt.x), y(pt.y) {}

    template<typename A = double>
    Point<A> normilized(A len = 1) const {
        Point<A> result(*this);
        result.x *= len / length<A>();
        result.y *= len / length<A>();
        return result;
    }

    T operator*(const Point &pt) const {
        return x * pt.x + y * pt.y;
    }

    T operator%(const Point &pt) const {
        return x * pt.y - y * pt.x;
    }

    Point<T> operator+(const Point &pt) const {
        return Point<T>(x + pt.x, y + pt.y);
    }

    Point<T> operator-(const Point &pt) const {
        return Point<T>(x - pt.x, y - pt.y);
    }

    bool operator==(const Point &pt) const {
        return x == pt.x && y == pt.y;
    }

    bool operator!=(const Point &pt) const {
        return x != pt.x || y != pt.y;
    }

    bool operator<(Point pt) const {
        return x < pt.x || (x == pt.x && y < pt.y);
    }

    bool operator>(Point pt) const {
        return x > pt.x || (x == pt.x && y > pt.y);
    }

    T length2() const {
        return x * x + y * y;
    }

    template<typename A = double>
    A length() const {
        return sqrt(A(x) * x + A(y) * y);
    }

    friend std::istream& operator>>(std::istream &in, Point &pt) {
        return in >> pt.x >> pt.y;
    }

    friend std::ostream& operator<<(std::ostream &out, const Point &pt) {
        return out << pt.x << ' ' << pt.y;
    }
};

template<typename T>
bool inAngle(const Point<T> &a, const Point<T> &o, const Point<T> &b, const Point<T> &c) {
    assert(Point<T>(o, b) % Point<T>(o, a) != 0);
    return sgn(sgn(Point<T>(o, b) % Point<T>(o, c)) * sgn(Point<T>(o, b) % Point<T>(o, a))) >= 0
           && sgn(sgn(Point<T>(o, a) % Point<T>(o, c)) * sgn(Point<T>(o, a) % Point<T>(o, b))) >= 0;
}

template<typename T = long long>
struct Line {
    T a, b, c;
    Point<T> anyPoint;

    Line() : a(0), b(0), c(0), anyPoint(Point<T>()) {}
    Line(T a, T b, T c) : a(a), b(b), c(c) {}

    Line(const Point<T> &pa, const Point<T> &pb)
        : a(pb.y - pa.y), b(pa.x - pb.x), c(-a * pa.x - b * pa.y), anyPoint(pa)
    {}

    template<typename A>
    Line(const Line<A> &ln) : a(ln.a), b(ln.b), c(ln.c), anyPoint(ln.anyPoint) {}

    void normilize() {
        if (a < 0 || (a == 0 && b < 0))
            a *= -1, b *= -1, c *= -1;

        T g = gcd(abs(a), abs(b));
        a /= g, b /= g, c /= g;
    }

    Line normilized() const {
        Line result = *this;
        if (a < 0 || (a == 0 && b < 0))
            result.a *= -1, result.b *= -1, result.c *= -1;

        T g = gcd(abs(a), abs(b));
        result.a /= g, result.b /= g, result.c /= g;
        return result;
    }

    bool operator==(const Line &ln) const {
        auto current = normilized();
        ln.normilize();
        return current.a == ln.a && current.b == ln.b && current.c == ln.c;
    }

    bool operator<(const Line &ln) const {
        Line tmp = normilized();
        ln.normilize();
        return std::array<T, 3>{tmp.a, tmp.b, tmp.c} < std::array<T, 3>{ln.a, ln.b, ln.c};
    }

    bool operator>(const Line &ln) const {
        return ln < *this;
    }

    bool operator!=(const Line &ln) const {
        return !(*this == ln);
    }

    T getVal(const Point<T> &pt) const {
        return a * pt.x + b * pt.y + c;
    }

    template<typename A = double>
    A distance(const Point<T> &pt) const {
        return abs(A(getVal(pt)) / Point<T>(a, b).template length<A>());
    }

    bool contain(const Point<T> &pt) const {
        return getVal(pt) == 0;
    }

    int differentSide(const Point<T> &pa, const Point<T> &pb) const {
        return sgn(sgn(getVal(pa)) * sgn(getVal(pb)));
    }

    bool isParallel(const Line &ln) const {
        return sgn(Point<T>(a, b) % Point<T>(ln.a, ln.b)) == 0;
    }

    template<typename A = double>
    Point<A> intersect(const Line &ln) const {
        A denominator = Point<A>(a, ln.a) % Point<A>(b, ln.b);
        return Point<A>((Point<A>(b, ln.b) % Point<A>(c, ln.c)) / denominator,
                        -(Point<A>(a, ln.a) % Point<A>(c, ln.c)) / denominator);
    }

    template<typename A = double>
    A distance(const Line &ln) const {
        if (isParallel(ln))
            return ln.distance<A>(anyPoint);

        return 0;
    }

    friend std::istream& operator>>(std::istream &in, Line &ln) {
        return in >> ln.a >> ln.b >> ln.c;
    }

    friend std::ostream& operator<<(std::ostream &out, const Line &ln) {
        return out << '{' << ln.a << ", " << ln.b << ", " << ln.c << '}';
    }
};

template<typename T = long long>
struct Segment {
    Point<T> a, b;

    Segment() : a(Point<T>()), b(Point<T>()) {}
    Segment(const Point<T> &a, const Point<T> &b) : a(a), b(b) {}

    template<typename A>
    Segment(const Segment<A> &seg) : a(seg.a), b(seg.b) {}

    bool operator==(const Segment &seg) const {
        return (a == seg.a && b == seg.b) || (a == seg.b && b == seg.a);
    }

    bool operator!=(const Segment &seg) const {
        return !(*this == seg);
    }

    bool operator<(const Segment &seg) const {
        if ((a < b) ^ (seg.a < seg.b))
            swap(seg.a, seg.b);

        return a < seg.a || (a == seg.a && b < seg.b);
    }

    bool operator>(const Segment &seg) const {
        return seg < *this;
    }

    bool contains(const Point<T> &pt) const {
        return Line<T>(a, b).contain(pt) && sgn(Point<T>(pt, a) * Point<T>(pt, b)) <= 0;
    }

    template<typename A = double>
    A distance(const Point<T> &pt) const {
        if (contains(pt))
            return 0;

        if (sgn(Point<T>(a, b) * Point<T>(b, pt)) >= 0)
            return Point<T>(b, pt).template length<A>();

        if (sgn(Point<T>(b, a) * Point<T>(a, pt)) >= 0)
            return Point<T>(a, pt).template length<A>();

        return Line<T>(a, b).template distance<A>(pt);
    }

    bool intersect(const Line<T> &ln) const {
        return ln.differentSide(a, b) <= 0;
    }

    template<typename A = double>
    A distance(const Line<T> &ln) const {
        if (intersect(ln))
            return 0;

        return min(ln.template distance<A>(a), ln.template distance<A>(b));
    }

    bool intersect(const Segment &seg) const {
        if (contains(seg.a) || contains(seg.b) || seg.contains(a) || seg.contains(b))
            return true;

        return Line<T>(a, b).differentSide(seg.a, seg.b) < 0
               && Line<T>(seg.a, seg.b).differentSide(a, b) < 0;
    }

    template<typename A = double>
    A distance(const Segment &seg) const {
        if (intersect(seg))
            return 0;

        return min({distance<A>(seg.a), distance<A>(seg.b), seg.distance<A>(a), seg.distance<A>(b)});
    }

    friend std::istream& operator>>(std::istream &in, Segment &seg) {
        return in >> seg.a >> seg.b;
    }

    friend std::ostream& operator<<(std::ostream &out, const Segment &seg) {
        return out << '(' << seg.a << ", " << seg.b << ')';
    }
};

template<typename T = long long>
struct Ray {
    Point<T> o, vec;

    Ray() : o(Point<T>()), vec(Point<T>()) {}
    Ray(Point<T> o, Point<T> vec) : o(o), vec(vec) {}
    template<typename A>
    Ray(Ray<A> ray) : o(ray.o), vec(ray.vec) {}

    bool contains(const Point<T> &pt) const {
        return Line<T>(o, o + vec).contain(pt) && sgn(Point<T>(o, pt) * vec) >= 0;
    }

    template<typename A = double>
    A distance(Point<T> pt) const {
        if (sgn(vec * Point<T>(o, pt)) >= 0)
            return Line<T>(o, o + vec).template distance<A>(pt);

        return Point<T>(o, pt).template length<A>();
    }

    bool intersect(const Segment<T> &seg) const {
        if (seg.contains(o))
            return true;

        if (Line<T>(seg.a, seg.b).contain(o))
            return contains(seg.a);

        return inAngle(seg.a, o, seg.b, o + vec);
    }

    template<typename A = double>
    A distance(const Segment<T> &seg) const {
        if (intersect(seg))
            return 0;

        return min({seg.template distance<A>(o), distance<A>(seg.a), distance<A>(seg.b)});
    }

    bool intersect(const Line<T> &ln) const {
        if (ln.differentSide(o, o + vec) <= 0)
            return true;

        if (abs(ln.getVal(o)) > abs(ln.getVal(o + vec)))
            return true;

        return false;
    }

    template<typename A = double>
    A distance(const Line<T> &ln) const {
        if (intersect(ln))
            return 0;

        return ln.template distance<A>(o);
    }

    bool intersect(const Ray<T> &ray) const {
        if (sgn(vec % ray.vec) == 0)
            return contains(ray.o) || ray.contains(o);

        return ray.intersect(Line<T>(o, o + vec)) && intersect(Line<T>(ray.o, ray.o + ray.vec));
    }

    template<typename A = double>
    A distance(const Ray<T> &ray) const {
        if (intersect(ray))
            return 0;

        return min(distance<A>(ray.o), ray.distance<A>(o));
    }

    friend std::istream& operator>>(std::istream &in, Ray &ray) {
        return in >> ray.o >> ray.vec;
    }

    friend std::ostream& operator<<(std::ostream &out, Ray ray) {
        return out << '{' << ray.o << ", " << ray.vec << '}';
    }
};

template<typename T = long long>
struct ConvexPolygon {
    std::vector<Point<T>> pol;

    ConvexPolygon() {}
    ConvexPolygon(int n) : pol(n) {}
    ConvexPolygon(const std::vector<Point<T>> &points) : pol(points) {
        normilize();
    }

    void rotate() {
        std::rotate(pol.begin(), min_element(pol.begin(), pol.end()), pol.end());
    }

    void normilize() {
        std::sort(pol.begin(), pol.end());
        pol.resize(unique(pol.begin(), pol.end()) - pol.begin());
        if (empty())
            return;

        std::sort(pol.begin() + 1, pol.end(), [&](const Point<T> &a, const Point<T> &b) {
            auto vecA = a - pol[0];
            auto vecB = b - pol[0];
            auto p = sgn(vecA % vecB);
            if (p != 0)
                return p == 1;

            return vecA.length2() < vecB.length2();
        });

        std::vector<Point<T>> result;
        result.push_back(pol[0]);
        for (int i = 1; i < size(); i++) {
            while (result.size() > 1 && sgn((result.back() - result.end()[-2]) % (pol[i] - result.back())) <= 0)
                result.pop_back();

            result.push_back(pol[i]);
        }

        pol = result;
    }

    bool empty() const {
        return pol.empty();
    }

    int size() const {
        return pol.size();
    }

    void push_back(Point<T> pt) {
        pol.push_back(pt);
    }

    void pop_back() {
        pol.pop_back();
    }

    void resize(int n) {
        pol.resize(n);
    }

    void shrink_to_fit() {
        pol.shrink_to_fit();
    }

    Point<T>& back() {
        return pol.back();
    }

    const Point<T>& back() const {
        return pol.back();
    }

    Point<T>& operator[](size_t pos) {
        return pol[pos];
    }

    ConvexPolygon operator+(const ConvexPolygon &poly) const {
        ConvexPolygon result;
        result.push_back(pol[0] + poly.pol[0]);

        auto add = [&](Point<T> vec) {
            if (result.size() == 1
                || !(sgn((result.back() - result[result.size() - 2]) % vec) == 0
                     && sgn((result.back() - result[result.size() - 2]) * vec) >= 0))
                result.push_back(result.back() + vec);
            else
                result.back() = result.back() + vec;
        };

        int j = 0;
        for (int i = 0; i < size(); i++) {
            while (j < poly.size() && sgn((poly[(j + 1) % poly.size()] - poly[j]) % (pol[(i + 1) % size()] - pol[i])) >= 0) {
                add(poly[(j + 1) % poly.size()] - poly[j]);
                j++;
            }
            add(pol[(i + 1) % size()] - pol[i]);
        }

        while (j < poly.size()) {
            add(poly[(j + 1) % poly.size()] - poly[j]);
            j++;
        }

        result.pop_back();
        result.shrink_to_fit();
        return result;
    }

    bool contains(const Point<T> &pt) const {
        if (size() == 0)
            return false;

        if (size() == 1)
            return pt == pol[0];

        if (size() == 2)
            return Segment<T>(pol[0], pol[1]).contains(pt);

        if (pt == pol[0])
            return true;

        if (sgn((pol[1] - pol[0]) % (pt - pol[0])) == -1)
            return false;

        int l = 1, r = size() - 1, mid;
        while (r - l > 1) {
            mid = (l + r) >> 1;
            if (sgn((pol[mid] - pol[0]) % (pt - pol[mid])) >= 0)
                l = mid;
            else
                r = mid;
        }

        return abs((pol[l] - pol[0]) % (pol[l + 1] - pol[0])) == abs((pol[0] - pt) % (pol[l] - pt))
               + abs((pol[l] - pt) % (pol[l + 1] - pt)) + abs((pol[l + 1] - pt) % (pol[0] - pt));
    }

    std::pair<int, int> tangents(const Point<T> &pt) const {
        if (size() == 0)
            return {-1, -1};

        if (size() == 1)
            return {0, 0};

        if (size() == 2)
            return {0, 1};

        auto compare = [&](int i, int j) {
            return sgn((pol[i] - pt) % (pol[j] - pt));
        };

        int who = compare(0, 1);
        if (who == 0)
            who = (pol[0] - pt).length2() < (pol[1] - pt).length2() ? -1 : 1;

        int first, second;
        {
            int l = 0, r = size() + 1, mid, real;
            while (r - l > 1) {
                mid = (l + r) >> 1;
                real = (mid == size() ? 0 : mid);
                if (real != 0 && compare(0, real) <= 0)
                    (who == -1 ? l : r) = mid;
                else
                    (compare(mid - 1, real) > 0 ? l : r) = mid;
            }
            first = l % size();
        }
        {
            int l = 0, r = size() + 1, mid, real;
            while (r - l > 1) {
                mid = (l + r) >> 1;
                real = (mid == size() ? 0 : mid);
                if (real != 0 && compare(0, real) >= 0)
                    (who == -1 ? r : l) = mid;
                else
                    (compare(real, mid - 1) > 0 ? l : r) = mid;
            }
            second = l % size();
        }
        return {first, second};
    }

    T area2() const {
        T sum = 0;
        for (int i = 0; i < size(); i++)
            sum += pol[i] % pol[(i + 1) % size()];

        return abs(sum);
    }

    template<typename A = double>
    A perimeter() const {
        A sum = 0;
        for (int i = 0; i < size(); i++)
            sum += (pol[i] - pol[(i + 1) % size()]).template length<A>();

        return sum;
    }

    template<typename A = double>
    A diameter() const {
        int l = min_element(all(pol)) - pol.begin();
        int r = max_element(all(pol)) - pol.begin();
        A answer;
        for (int i = 0; i < 2 * size(); i++) {
            A current = (pol[r] - pol[l]).template length<A>();
            if (i == 0 || answer < current)
                answer = current;

            if (sgn((pol[(l + 1) % size()] - pol[l]) % (pol[r] - pol[(r + 1) % size()])) >= 0) {
                l++;
                if (l == size())
                    l = 0;
            } else {
                r++;
                if (r == size())
                    r = 0;
            }
        }
        return answer;
    }

    template<typename A = double>
    A distance(const Point<T> &pt) const {
        A answer;
        for (int i = 0; i < size(); i++) {
            auto current = Segment<T>(pol[i], pol[(i + 1) % size()]).template distance<A>(pt);
            if (i == 0 || answer > current)
                answer = current;
        }
        return answer;
    }

    template<typename A = double>
    A distance(ConvexPolygon<T> poly) const {
        auto current = *this;
        current.rotate();
        for (auto &pt : poly.pol)
            pt.x *= -1, pt.y *= -1;

        poly.rotate();
        return (current + poly).template distance<A>(Point<T>(0, 0));
    }

    friend std::istream& operator>>(std::istream &in, ConvexPolygon &pol) {
        for (auto &pt : pol.pol)
            in >> pt;

        pol.normilize();
        return in;
    }

    friend std::ostream& operator<<(std::ostream &out, ConvexPolygon pol) {
        out << '{';
        std::string sep;
        for (auto p : pol.pol)
            out << sep << p, sep = ", ";

        return out << '}';
    }
};
