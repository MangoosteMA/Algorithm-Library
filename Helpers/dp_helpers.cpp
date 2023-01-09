template<typename A, typename B>
bool setmin(A &a, const B &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}

template<typename A, typename B>
bool setmax(A &a, const B &b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
