template<typename T>
std::pair<std::vector<int>, std::vector<int>> manacher(const T &str) {
    int n = str.size();
    std::vector<int> odd(n), even(n);
    for (int rot : {0, 1}) {
        auto &current = rot == 0 ? odd : even;
        for (int i = 0, l = 0, r = 0; i < n; i++) {
            if (i < r - rot)
                current[i] = std::min(r - i - rot, current[l + (r - i - 1) - rot]);

            while (i - current[i] >= 0 && i + current[i] + rot < n && str[i - current[i]] == str[i + current[i] + rot])
                current[i]++;

            if (i + current[i] + rot > r) {
                l = i - current[i] + 1;
                r = i + current[i] + rot;
            }
        }
    }
    return {odd, even};
}
