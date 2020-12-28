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
  Oid(std::string hs = string()) : hash(hs) {}
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
  Oid _tree;
  string _author;
  string _email;
  string _time;
  string _commiter;
  string _message;
  int parentCnt;
  shared_ptr<CommitNode> parentNode[2];
  void PrintAll();
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

void CommitNode::PrintAll() {
  std::cout << "-----------" << std::endl;
  std::cout << "Author:" << _author << _email << std::endl;
  std::cout << "Date:" << _time << std::endl;
  std::cout << std::endl;
  std::cout << _message << std::endl;
  std::cout << "-----------" << std::endl;
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
void catCommitFile(
    string subHash,
    unordered_map<Oid, shared_ptr<CommitNode>, hash<Oid>> &commit_map) {
  char *line = NULL;
  size_t len = 0;
  int retLen = -1;
  int maxParents = 2;
  string s = "git cat-file -p ";
  s += subHash;
  FILE *fp = popen(s.c_str(), "r");
  auto subNode = commit_map[subHash];
  // /* 获得tree */
  // retLen = getline(&line, &len, fp);
  // assert(retLen != -1 && !strncmp(line, "tree ", 5));
  // auto subNode = commit_map[subHash];
  // subNode->_tree.hash = line[6];
  // if (line[retLen - 1] == '\n')
  //   line[retLen - 1] = '\0';

  // subNode->
  while (1) {
    retLen = getline(&line, &len, fp);
    assert(retLen != -1);
    if (line[retLen - 1] == '\n')
      line[retLen - 1] = '\0';
    if (!strncmp(line, "tree ", 5)) {
      subNode->_tree.hash = line + 5;
    } else if (!strncmp(line, "parent ", 7)) {
      auto pNode = commit_map[string(line + 7)];
      subNode->outDegree++;
      subNode->parentNode[subNode->parentCnt++] = pNode;
      pNode->inDegree++;
    } else if (!strncmp(line, "author ", 7)) {
      char *emailBeg = strchr(line, '<');
      char *emailEnd = strchr(line, '>');
      emailBeg[-1] = '\0';
      emailEnd[1] = '\0';
      subNode->_author = line + 7;
      subNode->_email = emailBeg;
      subNode->_time = emailEnd + 2;
    } else if (!strncmp(line, "commiter ", 9)) {
      subNode->_commiter = line + 9;
    } else if (line[0] == '\0') {
      while ((retLen = getline(&line, &len, fp)) != -1) {
        subNode->_message += line;
      }
      break;
    }
  }
  free(line);
  fclose(fp);
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
    Node->PrintAll();

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
  for (auto &&oidHash : commits) {
    commit_map[oidHash] =
        make_shared<CommitNode>(oidHash); /* 将hash<--->节点建立映射关系 */
  }

  // for (auto &&subHash : commits) { /* 对于每次的子提交, */
  //   auto parents =
  //       getParentsHashFromSubHash(subHash); /* 获取所有git提交的父亲hash值 */
  //   // std::cout << subHash << std::endl;
  //   auto sNode = commit_map[subHash]; /* 找到map中的子节点 */
  //   for (auto &&par : parents) {
  //     // std::cout << "[parentHash]:" << par << std::endl;
  //     auto pNode = commit_map[par];
  //     sNode->outDegree++;
  //     sNode->parentNode[sNode->parentCnt++] = pNode;
  //     pNode->inDegree++;
  //   }
  // }
  for (auto &&oidHash : commits) {
    catCommitFile(oidHash, commit_map);
  }

  topoOrder(commit_map);

  return 0;
}
