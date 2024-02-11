struct suffix_array {
    std::vector<int> order, suffix_position, lcp;

    template<typename T>
    suffix_array(T str = T()) {
        str.push_back(*std::min_element(str.begin(), str.end()) - 1);
        int n = str.size();

        order.resize(n);
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](const int i, const int j) {
            return str[i] < str[j];
        });

        suffix_position.resize(n);
        for (int i = 0; i < n; i++)
            suffix_position[order[i]] = i == 0 ? 0 : suffix_position[order[i - 1]] + (str[order[i]] != str[order[i - 1]]);

        std::vector<int> ptr(n), new_order(n), new_suffix_position(n);
        for (int len = 1; suffix_position[order.back()] != n - 1; len <<= 1) {
            std::fill(ptr.begin(), ptr.begin() + suffix_position[order.back()] + 1, 0);
            for (int i = 0; i < n; i++)
                if (suffix_position[i] + 1 < n)
                    ptr[suffix_position[i] + 1]++;

            for (int i = 1; i <= suffix_position[order.back()]; i++)
                ptr[i] += ptr[i - 1];

            for (const int position : order) {
                int suffix = (position - len + n) % n;
                new_order[ptr[suffix_position[suffix]]++] = suffix;
            }
            std::swap(order, new_order);

            for (int i = 0; i < n; i++)
                new_suffix_position[order[i]] = i == 0 ? 0 : new_suffix_position[order[i - 1]]
                + (suffix_position[order[i]] != suffix_position[order[i - 1]]
                || suffix_position[(order[i] + len) % n] != suffix_position[(order[i - 1] + len) % n]);

            std::swap(suffix_position, new_suffix_position);
        }
        assert(order[0] == n - 1);

        lcp.resize(n);
        int current_lcp = 0;
        for (int suffix = 0; suffix < n - 1; suffix++, current_lcp = current_lcp == 0 ? 0 : current_lcp - 1) {
            int previous_suffix = order[suffix_position[suffix] - 1];
            while (str[suffix + current_lcp] == str[previous_suffix + current_lcp])
                current_lcp++;

            lcp[suffix_position[suffix]] = current_lcp;
        }
    }
};

// faster implementation
// source: https://judge.yosupo.jp/submission/42086
#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <vector>

void SA_IS(const int *s, int *SA, int n, int K) {
  // s 为字符串数组[0..n-1] 必须保证 s[n-1]=0 且为最小值
  // SA 为存储后缀数组[0..n-1]
  // n 为字符串长度
  // K 为值域

  bool *t = new bool[n]; // 类型数组
  int *bkt = new int[K]; // 桶
  int *bkt_l = new int[K];
  int *bkt_r = new int[K];
  int n1 = 0; // LMS-后缀的数量
  int *p1;    //按出现顺序存储所有 LMS-后缀的索引

#define is_S_type(x) (t[x])
#define is_L_type(x) (!t[x])
#define is_LMS_type(x) (is_S_type(x) && x != 0 && is_L_type(x - 1))

  // 预处理每一个后缀的类型
  t[n - 1] = true; // 0 为 S-型后缀且为 LMS-型后缀
  for (int i = n - 2; i >= 0; --i) {
    t[i] = (s[i] < s[i + 1] || (is_S_type(i + 1) && s[i] == s[i + 1]));
    n1 += is_LMS_type(i + 1); // s[0] 必然不是 LMS-型后缀，不会影响
  }

  // 预处理桶的边界
  for (int i = 0; i != K; ++i) bkt[i] = 0;
  for (int i = 0; i != n; ++i) ++bkt[s[i]];
  for (int i = 0, sum = 0; i != K; ++i) sum += bkt[i], bkt_r[i] = sum, bkt_l[i] = sum - bkt[i];

#define indecud_sort()                                                                             \
  do {                                                                                             \
    for (int i = 0; i != K; ++i) bkt[i] = bkt_l[i];                                                \
    for (int i = 0, j; i != n; ++i)                                                                \
      if ((j = SA[i] - 1) >= 0 && is_L_type(j)) SA[bkt[s[j]]++] = j;                               \
    for (int i = 0; i != K; ++i) bkt[i] = bkt_r[i];                                                \
    for (int i = n - 1, j; i >= 0; --i)                                                            \
      if ((j = SA[i] - 1) >= 0 && is_S_type(j)) SA[--bkt[s[j]]] = j;                               \
  } while (0)

  // 将所有 LMS-后缀放入 SA 对应桶的末尾并诱导排序
  p1 = new int[n1];
  for (int i = 0, j = 0; i != n; ++i) {
    SA[i] = -1;
    if (is_LMS_type(i)) p1[j++] = i;
  }
  for (int i = 0; i != K; ++i) bkt[i] = bkt_r[i];
  for (int i = n1 - 1; i >= 0; --i) SA[--bkt[s[p1[i]]]] = p1[i];
  indecud_sort();

  int *s1 = new int[n1];  // 新的字符串
  int *SA1 = new int[n1]; // 存储新的字符串排的后缀数组
  for (int i = 0, j = 0; i != n; ++i)
    if (is_LMS_type(SA[i])) SA1[j++] = SA[i]; // 存储 LMS-子串的相对顺序
  int name = 0;
  // 对所有 LMS-子串命名
  for (int i = 0, prev = -1; i != n1; ++i) {
    int pos = SA1[i];
    for (int j = 0;; ++j) // 无需设置范围，因为 s[n-1]=0 为最小值且不会出现在其余位置
      if (prev == -1 || s[pos + j] != s[prev + j] || is_S_type(pos + j) != is_S_type(prev + j)) {
        prev = pos, ++name;
        break;
      } else if (j != 0 && (is_LMS_type(pos + j) || is_LMS_type(prev + j))) // 到末尾了停止比较
        break;
    SA[pos] = name - 1; // 利用 SA 暂时存储新字符串的 name
  }
  for (int i = 0; i != n1; ++i) s1[i] = SA[p1[i]];

  if (name != n1)
    SA_IS(s1, SA1, n1, name);
  else
    for (int i = 0; i != n1; ++i) SA1[s1[i]] = i;

  for (int i = 0; i != K; ++i) bkt[i] = bkt_r[i];
  for (int i = 0; i != n; ++i) SA[i] = -1;
  for (int i = n1 - 1; i >= 0; --i) SA[--bkt[s[p1[SA1[i]]]]] = p1[SA1[i]];
  indecud_sort();

  delete[] SA1;
  delete[] s1;
  delete[] p1;
  delete[] bkt_r;
  delete[] bkt_l;
  delete[] bkt;
  delete[] t;

#undef is_S_type
#undef is_L_type
#undef is_LMS_type
#undef indecud_sort
}

std::vector<int> get_sa(const std::string &s) {
  int len = s.size();
  std::vector<int> SA(len + 1), s_cpy(len + 1);
  for (int i = 0; i != len; ++i) s_cpy[i] = s[i];
  s_cpy.back() = 0;
  SA_IS(s_cpy.data(), SA.data(), len + 1, 128);
  return std::vector<int>(SA.begin() + 1, SA.end());
}

struct suffix_array {
    std::vector<int> order, suffix_position, lcp;

    template<typename T>
    suffix_array(T str = T()) {
        int n = str.size() + 1;
        order = get_sa(str);
        order.insert(order.begin(), n - 1);
        
        suffix_position.resize(n);
        for (int i = 0; i < n; i++) {
            suffix_position[order[i]] = i;
        }

        lcp.resize(n);
        int current_lcp = 0;
        for (int suffix = 0; suffix < n - 1; suffix++, current_lcp = current_lcp == 0 ? 0 : current_lcp - 1) {
            int previous_suffix = order[suffix_position[suffix] - 1];
            while (str[suffix + current_lcp] == str[previous_suffix + current_lcp])
                current_lcp++;

            lcp[suffix_position[suffix]] = current_lcp;
        }
    }
};
