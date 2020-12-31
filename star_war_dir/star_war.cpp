#include "sha256.h"
#include <bits/stdc++.h>
#include <float.h>
#include <openssl/sha.h>
using namespace std;
class Star;
class Station;

class Star {
  friend class Station;
  friend double distances(const shared_ptr<Star> &lhs,
                          const shared_ptr<Star> &rhs);

public:
  Star(string s, double x, double y, double z) : x_(x), y_(y), z_(z), name_(s) {
    Sha256General g;
    g.init();
    auto size = s.size();
    g.set(std::move(s), size);
    /* 根据星球的编号生成星球独一无二的sha256 */
    hash_ = std::move(g.getString());
  }
  string getMinCostToAnotherStar(shared_ptr<Star> dest) { /* 目标dest */
    set<string> adjStarSet; /* 一个存已经找到最小距离的星球集合 */
    double minCost = DBL_MAX;
    string minCostStarHash = "NULL";
    for (auto &&item : costMap) { /* 遍历所有可达星球 */
      if (adjStarSet.find(item.first) == adjStarSet.end()) { /* 不在集合内 */
        if (item.second < minCost) {
          /* 如果找到有比当前最小的值还要小的星球，更新min */
          minCost = item.second;
          minCostStarHash = item.first;
        }
      }
    }
    return minCostStarHash;
  }

  string getMinAdjStarHash(set<string> s, double &minValue) {
    double minCost = DBL_MAX;
    string minCostHash = "NULL";
    for (auto &&i : costMap) {
      if (s.find(i.first) == s.end()) {
        if (minCost > i.second) {
          minCost = i.second;
          minCostHash = i.first;
        }
      }
    }
    minValue = minCost;
    return minCostHash;
  }
  void debug() const {
    std::cout << "[costMap]" << std::endl;
    for (auto &&i : costMap) {
      std::cout << "adj->" << i.first;
      std::cout << " cost:" << i.second << std::endl;
    }
  }
  void print() const {
    printf("[%s]", name_.c_str());
    /* 打印坐标 */
    printf("(%.2f,%.2f,%.2f)", x_, y_, z_);
  }
  void setCostToAnotherStar(string s, double m) { costMap[s] = m; }
  void updateCostToAnotherStar(string s, double m) {
    auto iter = costMap.find(s);
    if (iter == costMap.end()) {
      costMap[s] = m;
    } else {
      // std::cout << "[debug]" << std::endl;
      // std::cout << s << std::endl;
      // std::cout << m << std::endl;
      // std::cout << iter->second << std::endl;
      iter->second = min(iter->second, m);
    }
  }
  double getCostToAnotherStar(string s) {
    auto it = costMap.find(s);
    // std::cout << "[DEBUG]" << std::endl;
    // std::cout << it->second << std::endl;
    // std::cout << "[DEBUG]" << s << std::endl;
    if (it != costMap.end()) {
      // std::cout << "[DEBUG]" << std::endl;
      // std::cout << it->second << std::endl;
      return it->second;
    } else {
      return DBL_MAX;
    }
  }
  map<string, double> &getCostToAllAdjStar() { return costMap; }
  std::string getHash() { return hash_; }

private:
  /* ,key是hash,value是钱数 */
  map<string, double> costMap;
  // priority_queue<string, vector<string>,bool(*)(string l, string r)> que;
  /* 星球的坐标 */
  double x_;
  double y_;
  double z_;
  /* 星球的名称  */
  std::string name_;
  std::string hash_;
  shared_ptr<Station> stat_;
};

double distances(const shared_ptr<Star> &lhs, const shared_ptr<Star> &rhs) {
  return pow(pow(lhs->x_ - rhs->x_, 3) + pow(lhs->y_ - rhs->y_, 3) +
                 pow(lhs->z_ - rhs->z_, 3),
             1.0 / 3);
}
/*
3
a
1 2 3
b
4 5 6
c
7 8 9
a b 3
y
a c 10
y
b c 1
n
 */
class Station {
public:
  // double distances(const string &h1, const string &h2) {

  //   return ::distances(getStarFromHash(h1), getStarFromHash(h2));
  // }
  void attach(string name, string _hash, const shared_ptr<Star> &s) {
    hashToStarMap[_hash] = weak_ptr<Star>(s);
    auto ss = hashToStarMap[_hash].lock();
    // if (!ss) {
    //   std::cout << "!" << std::endl;
    // }
    nameToHashMap[name] = _hash;
  }
  string getHashFromName(string name) {
    auto iter = nameToHashMap.find(name);
    if (iter != nameToHashMap.end()) {
      return iter->second;
    }
    return "NULL";
  }
  shared_ptr<Star> getStarFromHash(string hash_) {
    auto iter = hashToStarMap.find(hash_);
    if (iter != hashToStarMap.end()) {
      auto star = iter->second.lock();
      if (star) {
        return star;
      }
    }
    return shared_ptr<Star>();
  }
  shared_ptr<Star> getStarFromName(string name) {
    string hash_ = getHashFromName(name);
    if (hash_ == "NULL")
      return shared_ptr<Star>();
    return (getStarFromHash(hash_));
  }
  void debug() {
    std::cout << "------debug hashToStarMap-----" << std::endl;
    for (auto &&i : hashToStarMap) {
      cout << "debug的行星的hash" << i.first << endl; /* hash */
      auto s = i.second.lock();                       /* weak->shared star */
      cout << "debug的行星的其余信息" << i.first << endl; /* hash */
      if (s) {
        s->print();
        s->debug();
        std::cout << std::endl;
      } else {
        printf("是空的\n");
      }
    }
    std::cout << "-----------" << std::endl;
    for (auto &&i : nameToHashMap) {
      cout << i.first << endl;
      cout << i.second << endl;
    }
    std::cout << "-----------" << std::endl;
  }
  bool Dijkstra(string name1, string name2, double &minMoney) {
    shared_ptr<Star> star1 = getStarFromName(name1);
    shared_ptr<Star> star2 = getStarFromName(name2);

    if (star1 && star2) {
      set<std::string> adjStarSet;
      adjStarSet.insert(star1->getHash());
      while (1) {
        double minValue;
        string minHash = star1->getMinAdjStarHash(adjStarSet, minValue);
        adjStarSet.insert(minHash);
        if (minHash != "NULL") {
          shared_ptr<Star> adj = getStarFromHash(minHash);
          if (adj) {
            auto &adjCostMap =
                adj->getCostToAllAdjStar(); /* 找到adj去其他adj的所有adj的开销
                                             */
            for (auto &&costitem : adjCostMap) {
              if (adjStarSet.find(costitem.first) == adjStarSet.end()) {
                // std::cout << "[DBG]" << minValue << " " << costitem.second
                // << std::endl;
                star1->updateCostToAnotherStar(
                    costitem.first,
                    /* distances(star1->getHash(), costitem.first) **/ (
                        minValue + costitem.second)); /* 更新star1 */
              }
            }
          }

        } else {
          break;
        }
      }
      // debug();
      // star1->print();
      // std::cout << "[D]" << star1->getHash() << std::endl;
      // star2->print();
      // std::cout << "[D]" << star2->getHash() << std::endl;
      minMoney = star1->getCostToAnotherStar(star2->getHash());
      if (minMoney == DBL_MAX)
        return false;
      // auto mincostHash = star1->getMinCostToAnotherStar(star2);
      // minStar.
      /* 找到star1相邻的最小星星A，
             把A和相邻的星星的距离和当前每个星星距离作比较，
             并将A */
      // minMoney = star1->getMinCostStar();
      return true;
    }
    return false;
  }

  bool setConnection(string n1, string n2, double money) {
    auto h1 = getHashFromName(n1);
    auto h2 = getHashFromName(n2);
    auto s1 = getStarFromHash(h1);
    auto s2 = getStarFromHash(h2);
    if (s1 && s2) {
      // std::cout << "[DEBUG]" << std::endl;
      // std::cout << distances(h1, h2) << std::endl;
      s1->setCostToAnotherStar(h2, /* distances(h1, h2)* */ money);
      s2->setCostToAnotherStar(h1, /* distances(h1, h2)* */ money);
      return true;
    }
    return false;
  }

private:
  map<string, weak_ptr<Star>> hashToStarMap; /* hash到Star的map */
  map<string, string> nameToHashMap;         /* 姓名到HASH供用户查找*/
};

int main(int argc, char const *argv[]) {
  double x, y, z;
  int n;
  shared_ptr<Station> stat = make_shared<Station>();
  vector<shared_ptr<Star>> v;
  cout << "输入行星数量n:";
  cin >> n;
  for (int i = 0; i < n; i++) {
    cout << "输入行星名字:";
    string name;
    cin >> name;
    cout << "输入行星坐标(x,y,z):";
    cin >> x >> y >> z;
    auto star = make_shared<Star>(name, x, y, z);
    v.push_back(star);
    string hashOfStar = star->getHash();
    cout << "这颗星宿的hash是" << hashOfStar << endl;
    stat->attach(name, hashOfStar, star);
  }
  // stat->debug();
  string n1, n2;
  int c;

  while (1) {
    cout << "输入行星间越迁每光年所需要的油费(整数)！" << endl;
    cout << "[name1] [name2]";
    cout << " [cost]";
    cin >> n1 >> n2 >> c;
    stat->setConnection(n1, n2, c);
    cout << "继续？(y)" << endl;
    char op;
    cin >> op;
    if (op != 'y') {
      break;
    }
  }

  cout << "你在哪个行星" << endl;
  cout << "[name1]:";
  cin >> n1;
  cout << "你想去哪个行星" << endl;
  cout << "[name2]:";
  cin >> n2;
  double minMoney = DBL_MAX;
  if (stat->Dijkstra(n1, n2, minMoney)) {
    std::cout << "最小的钱是" << minMoney << std::endl;
  } else {
    std::cout << "暂时你去不了这里" << std::endl;
  }

  return 0;
}
