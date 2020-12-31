#include <iostream>
#include <map>
#include <memory>

typedef std::owner_less<std::weak_ptr<int>> wp_less;
typedef std::map<std::weak_ptr<int>, int, wp_less> wp_map;
bool map_cmp(const wp_map &a, const wp_map &b) {
  return std::lexicographical_compare(
      a.begin(), a.end(), b.begin(), b.end(),
      [](std::pair<std::weak_ptr<int> const, int> const &p,
         std::pair<std::weak_ptr<int> const, int> const &q) -> bool {
        return wp_less()(p.first, q.first);
      });
  //{ return wp_less()(p.first, q.first)
  //    || ( ! (wp_less()(q.first, p.first)) && p.second < q.second); });
}

int main() {
  std::shared_ptr<int> sp_int(std::make_shared<int>(5));
  std::weak_ptr<int> wp_int(sp_int);
  wp_map m1, m2;
  m1[wp_int] = 1;
  m2[wp_int] = 2;
  std::cout << "m1 < m2=" << map_cmp(m1, m2) << "\nm2 < m1=" << map_cmp(m2, m1);
  return 0;
}