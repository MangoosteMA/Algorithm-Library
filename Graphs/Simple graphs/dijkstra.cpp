template<typename T, bool directed, typename w_t>
std::pair<std::vector<T>, std::vector<int>> dijkstra(const graph<directed, w_t> &g, int start) {
    const int n = g.size();
    std::vector<bool> used(n);
    std::vector<T> distance(n);
    std::vector<int> parent(n, -1);
    std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>, std::greater<>> pq;
    pq.emplace(0, start);
    used[start] = true;

    while (!pq.empty()) {
        auto [current_distance, v] = pq.top();
        pq.pop();
        if (current_distance != distance[v])
            continue;

        for (auto &[_, u, w, id] : g[v]) {
            if (!used[u] || distance[u] > current_distance + w) {
                used[u] = true;
                distance[u] = current_distance + w;
                parent[u] = v;
                pq.emplace(distance[u], u);
            }
        }
    }
    return {distance, parent};
}
