#include<iostream>
using namespace std;

//Can improve delete operation
//Just got something working

struct BSTnode
{
    int data;
    BSTnode* left;
    BSTnode* right;
};

class BSTtree
{
  public:
    BSTtree();
    ~BSTtree();
    void insert(int);
    void remove(int);
    bool search(int);
    void printInorder();

  private:
    BSTnode* root;

    BSTnode * makeEmpty(BSTnode *);
    BSTnode * insert(int, BSTnode *);
    BSTnode * findMin(BSTnode *);
    BSTnode * findMax(BSTnode *);
    BSTnode * remove(int, BSTnode *);
    bool search(int, BSTnode *);

    //print
    void printInorder(BSTnode * t);

};

BSTtree::BSTtree() {
    root = NULL;
}

BSTtree::~BSTtree() {
  root = makeEmpty(root);
}

void BSTtree::insert(int x) {
  root = insert(x, root);
}

void BSTtree::remove(int x) {
    root = remove(x, root);
}

bool BSTtree::search(int x, BSTnode* t) {
  if(t == NULL)
      return false;
  else if(x < t->data)
      return search(x, t->left);
  else if(x > t->data)
      return search(x, t->right);
  else
      return true;
}

bool BSTtree::search(int x) {
  return search(x, root);
}

BSTnode* BSTtree::makeEmpty(BSTnode* t) {
  if(t == NULL)
      return NULL;
  {
      makeEmpty(t->left);
      makeEmpty(t->right);
      delete t;
  }
  return NULL;
}

BSTnode* BSTtree::insert(int x, BSTnode* t) {
  if(t == NULL)
  {
      t = new BSTnode;
      t->data = x;
      t->left = t->right = NULL;
  }
  else if(x < t->data)
      t->left = insert(x, t->left);
  else if(x > t->data)
      t->right = insert(x, t->right);
  return t;
}

BSTnode* BSTtree::remove(int x, BSTnode* t) {
  BSTnode* temp;
  if(t == NULL)
      return NULL;
  else if(x < t->data)
      t->left = remove(x, t->left);
  else if(x > t->data)
      t->right = remove(x, t->right);
  else if(t->left && t->right)
  {
      temp = findMin(t->right);
      t->data = temp->data;
      t->right = remove(t->data, t->right);
  }
  else
  {
      temp = t;
      if(t->left == NULL)
          t = t->right;
      else if(t->right == NULL)
          t = t->left;
      delete temp;
  }

  return t;
}

BSTnode * BSTtree::findMin(BSTnode * t) {
  if(t == NULL)
      return NULL;
  else if(t->left == NULL)
      return t;
  else
      return findMin(t->left);
}

BSTnode * BSTtree::findMax(BSTnode * t) {
  if(t == NULL)
      return NULL;
  else if(t->right == NULL)
      return t;
  else
      return findMax(t->right);
}

void BSTtree::printInorder(BSTnode* t) {
  if(t == NULL)
      return;
  printInorder(t->left);
  std::cout << t->data << std::endl;
  printInorder(t->right);
}

void BSTtree::printInorder() {
  return printInorder(root);
}

int main()
{
    BSTtree t;
    t.insert(20);
    t.insert(25);
    t.insert(15);
    t.insert(10);
    t.insert(30);
    t.printInorder();
    t.remove(20);
    t.printInorder();
    t.remove(25);
    t.printInorder();
    t.remove(30);
    t.printInorder();
}
