class Object {
public:
  enum TYPE { BLOB, TREE, COMMIT, TAG };
  Object(TYPE t) : type(t) {}
  TYPE type;

private:
};
class Blob : public Object {
public:
  Blob() : Object(BLOB) {}
};
class Tree : public Object {
public:
  Tree() : Object(TREE) {}
};
class Commit : public Object {
public:
  Commit() : Object(COMMIT) {}
};
class Tag : public Object {
  Tag() : Object(TAG) {}
};
