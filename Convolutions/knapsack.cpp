/*
 * For each 0 <= w <= max_weight returns number of ways to represent w as follows:
       w = x_1 + x_2 + \ldots + x_k, x_i \in a
 * Two ways are considered different if they have different multiset of {x_i}.
 */
polynom knapsack(std::vector<int> weights, int max_weight) {
    std::sort(weights.begin(), weights.end());
    weights.resize(unique(weights.begin(), weights.end()) - weights.begin());
    polynom p(max_weight + 1);
    for (auto w : weights) {
        for (int k = w; k <= max_weight; k += w) {
            p[k] += inv[k / w];
        }
    }
    return p.exp(max_weight + 1);
}

/*
 * Returns list of weights summing up to 'weight'.
 * If it's impossible to find such list, returns an empty vector.
 */
std::vector<int> restore_weights(std::vector<int> weights, int weight) {
    auto ways = knapsack(weights, weight);
    if (ways[weight] == 0) {
        return {};
    }

    std::vector<int> result;
    for (auto w : weights) {
        while (w <= weight && ways[weight - w] != 0) {
            result.push_back(w);
            weight -= w;
        }
    }
    assert(weight == 0);
    return result;
}
