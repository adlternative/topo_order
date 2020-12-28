#include <bits/stdc++.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

long long string_to_bit(const string &str) // transfer hex-string to bit
{
  long long result;

  result = strtoll(str.c_str(), NULL, 16);
  return result;
}
struct Oid {
  Oid(std::string hs) : hash(hs) {}
  std::string hash;
  friend bool operator==(const Oid &lfs, const Oid &rfs);
};
bool operator==(const Oid &lfs, const Oid &rfs) { return lfs.hash == rfs.hash; }

template <> struct std::hash<Oid> {
  size_t operator()(const Oid &__val) const noexcept {
    // return string_to_bit(__val.hash);
    return hash<string>()(__val.hash);
  }
};

struct CommitNode {
  CommitNode(Oid oid) : _hash(oid), inDegree(0), outDegree(0), parentCnt(0) {}
  int inDegree;
  int outDegree;
  Oid _hash;
  int parentCnt;
  shared_ptr<CommitNode> parentNode[2];
  void nodePrintCool();
  friend bool operator==(const CommitNode &lfs, const CommitNode &rfs);
  friend ostream &operator<<(ostream &os, CommitNode n);
};
ostream &operator<<(ostream &os, CommitNode n) {
  // os << n.inDegree << "\t" << n.outDegree << "\t" << n._hash.hash << "\t"
  //  << n.parentCnt << endl;
  os << n._hash.hash << std::endl;
  return os;
}

bool operator==(const CommitNode &lfs, const CommitNode &rfs) {
  return lfs._hash == rfs._hash;
}

template <> struct std::hash<CommitNode> {
  size_t operator()(const CommitNode &__val) const noexcept {
    return hash<Oid>()(__val._hash);
  }
};

void CommitNode::nodePrintCool() {
  ;
  ;
}
vector<string> getAllHashCodeOfCommits() {
  vector<string> v;
  FILE *fp = popen("git log --oneline --pretty=format:'%H'", "r");
  char *line = NULL;
  size_t len = 0;
  int retLen = -1;
  while ((retLen = getline(&line, &len, fp)) != -1) {
    if (line[retLen - 1] == '\n')
      line[retLen - 1] = '\0';
    v.emplace_back(line);
  }
  free(line);
  return v;
}
vector<string> getParentsHashFromSubHash(string subHash) {
  vector<string> v;
  char *line = NULL;
  size_t len = 0;
  int retLen = -1;
  int maxParents = 2;
  string s = "git cat-file -p ";
  s += subHash;
  FILE *fp = popen(s.c_str(), "r");
  retLen = getline(&line, &len, fp);
  assert(retLen != -1 && !strncmp(line, "tree ", 5));
  while (maxParents--) {
    retLen = getline(&line, &len, fp);
    assert(retLen != -1);
    if (!strncmp(line, "parent ", 7)) {
      if (line[retLen - 1] == '\n')
        line[retLen - 1] = '\0';
      v.emplace_back(line + 7);
    }
  }
  free(line);
  return v;
}

void topoOrder(
    unordered_map<Oid, shared_ptr<CommitNode>, hash<Oid>> &commit_map) {
  queue<Oid> q;
  for (auto &&pai : commit_map) {
    if (pai.second->inDegree == 0) {
      q.push(pai.first);
    }
  }
  while (!q.empty()) {
    auto oid = q.front();
    auto Node = commit_map[oid];
    std::cout << *Node << std::endl;

    q.pop();
    commit_map.erase(oid);
    for (int i = 0; i < Node->parentCnt; i++) {
      /* 父节点减少入度 */
      /* 如果入度为0再入队 */
      if (!--Node->parentNode[i]->inDegree) {
        q.push(Node->parentNode[i]->_hash);
      }
    }
  }
}

int main(int argc, char const *argv[]) {

  unordered_map<Oid, shared_ptr<CommitNode>, hash<Oid>> commit_map;
  auto commits = getAllHashCodeOfCommits(); /* 获取所有git提交的hash值 */
  for (auto &&oid : commits) {
    commit_map[oid] =
        make_shared<CommitNode>(oid); /* 将hash<--->节点建立映射关系 */
  }

  for (auto &&subHash : commits) { /* 对于每次的子提交, */
    auto parents =
        getParentsHashFromSubHash(subHash); /* 获取所有git提交的父亲hash值 */
    // std::cout << subHash << std::endl;
    auto sNode = commit_map[subHash]; /* 找到map中的子节点 */
    for (auto &&par : parents) {
      // std::cout << "[parentHash]:" << par << std::endl;
      auto pNode = commit_map[par];
      sNode->outDegree++;
      sNode->parentNode[sNode->parentCnt++] = pNode;
      pNode->inDegree++;
    }
  }
  topoOrder(commit_map);

  return 0;
}
