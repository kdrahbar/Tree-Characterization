#include <iostream>
#include <string>

//http://www.csee.umbc.edu/courses/undergraduate/341/fall98/frey/ClassNotes/Class17/splay.html
//http://www.cs.cornell.edu/courses/cs3110/2011sp/recitations/rec25-splay/splay.htm
// http://software.ucv.ro/~mburicea/lab7ASD.pdf

struct splayNode{
    int key;
    splayNode * left;
    splayNode * right;
    splayNode * parent;
};

class splayTree {
  public:
    splayTree();
    ~splayTree();

    bool search(int key); //returns true if in tree
    void insert(int key); //inserts key into tree

    //printing
    void printPostorder();

    splayNode * root;
  private:
    //splayNode * root;
    splayNode * newNode(int key);

    void single_rotate(splayNode * x);            //zig left || zig right
    void double_rotate(splayNode * x); //either zig zig || zig zag
    // Zig case
    void zig_left(splayNode * x); //x is left child
    void zig_right(splayNode * x); //x is right child
    // ZigZig case
    void zig_zig_left(splayNode * x);  //x is left left
    void zig_zig_right(splayNode * x); //x is right right
    // ZigZag
    void zig_zag_left(splayNode * x); //x is left right
    void zig_zag_right(splayNode * x); //x is right left

    splayNode * splay(splayNode * node, splayNode * treeRoot); //Splay node in tree treeRoot

    bool search(int key, splayNode * treeRoot);
    splayNode * insert(int key, splayNode * treeRoot, splayNode * p_node);

    splayNode * makeEmpty(splayNode* t);

    //printing
    void printInorder(splayNode * node);
    void printPostorder(splayNode * node);

};

splayTree::splayTree() {
  this->root = new splayNode;
  this->root = NULL;
  return;
}

splayTree::~splayTree() {
  root = makeEmpty(root);
}
splayNode* splayTree::makeEmpty(splayNode* t)
{
    if(t == NULL)
        return NULL;
    {
        makeEmpty(t->left);
        makeEmpty(t->right);
        delete t;
    }
    return NULL;
}

splayNode * splayTree::newNode(int key) {
  splayNode * x = new splayNode;
  x->key = key;
  x->left = NULL;
  x->right = NULL;
  x->parent = NULL;
  return x;
}

void splayTree::single_rotate(splayNode * x) {
  if (x->parent->left == x)
    zig_left(x);
  else
    zig_right(x);
  return;
}

void splayTree::double_rotate(splayNode * x) {
  if (x->parent->left == x && x->parent->parent->left == x->parent)
    zig_zig_left(x);
  else if (x->parent->left==x && x->parent->parent->right==x->parent)
    zig_zag_left(x);
  else if (x->parent->right==x && x->parent->parent->right==x->parent)
    zig_zig_right(x);
  else // (x->parent->right==x && x->parent->parent->left==x->parent)
    zig_zag_right(x);
  return;
}

void splayTree::zig_left(splayNode * x) {
  splayNode * parent = x->parent;
  splayNode * b = x->right;

  x->right = parent;
  x->parent = NULL;
  if (b != NULL)
    b->parent = parent;
  parent->left = b;
  parent->parent = x;

  return;
}

void splayTree::zig_right(splayNode * x) {
  splayNode * parent = x->parent;
  splayNode * b = x->left;

  x->left = parent;
  x->parent = NULL;
  if (b != NULL)
    b->parent = parent;
  parent->right = b;
  parent->parent = x;

  return;
}

void splayTree::zig_zig_left(splayNode * x) {
  splayNode * parent = x->parent;
  splayNode * grandParent = parent->parent;
  splayNode * greatGrandParent = grandParent->parent;
  splayNode * b = x->right;
  splayNode * c = parent->right;

  x->parent = greatGrandParent;
  if (greatGrandParent!=NULL && greatGrandParent->left == grandParent)
    greatGrandParent->left = x;
  else if (greatGrandParent!=NULL && greatGrandParent->right == grandParent)
    greatGrandParent->right = x;

  x->right = parent;
  parent->parent = x;
  parent->right = grandParent;
  grandParent->parent = parent;

  if (b != NULL)
    b->parent = parent;
  parent->left = b;

  if (c != NULL)
    c->parent = grandParent;
  grandParent->left = c;

  return;
}

void splayTree::zig_zig_right(splayNode * x) {
  splayNode * parent = x->parent;
  splayNode * grandParent = parent->parent;
  splayNode * greatGrandParent = grandParent->parent;
  splayNode * b = x->left;
  splayNode * c = parent->left;

  x->parent = greatGrandParent;
  if (greatGrandParent!=NULL && greatGrandParent->left == grandParent)
    greatGrandParent->left = x;
  else if (greatGrandParent!=NULL && greatGrandParent->right == grandParent)
    greatGrandParent->right = x;

  x->left = parent;
  parent->parent = x;
  parent->left = grandParent;
  grandParent->parent = parent;

  if (b != NULL)
    b->parent = parent;
  parent->right = b;
  if (c != NULL)
    c->parent = grandParent;
  grandParent->right = c;

  return;
}

void splayTree::zig_zag_left(splayNode * x) {
  splayNode * parent = x->parent;
  splayNode * grandParent = parent->parent;
  splayNode * greatGrandParent = grandParent->parent;
  splayNode * b = x->left;
  splayNode * c = x->right;

  x->parent = greatGrandParent;
  if (greatGrandParent!=NULL && greatGrandParent->left == grandParent)
    greatGrandParent->left = x;
  else if (greatGrandParent!=NULL && greatGrandParent->right == grandParent)
    greatGrandParent->right = x;

  x->left = grandParent;
  grandParent->parent = x;
  x->right = parent;
  parent->parent = x;
  if (b != NULL)
    b->parent = grandParent;
  grandParent->right = b;

  if (c != NULL)
    c->parent = parent;
  parent->left = c;



  return;
}

void splayTree::zig_zag_right(splayNode * x) {
  splayNode * parent = x->parent;
  splayNode * grandParent = parent->parent;
  splayNode * greatGrandParent = grandParent->parent;
  splayNode * b = x->left;
  splayNode * c = x->right;

  x->parent = greatGrandParent;
  if (greatGrandParent!=NULL && greatGrandParent->left == grandParent)
    greatGrandParent->left = x;
  else if (greatGrandParent!=NULL && greatGrandParent->right == grandParent)
    greatGrandParent->right = x;

  x->right = grandParent;
  grandParent->parent = x;
  x->left = parent;
  parent->parent = x;

  if (b != NULL)
    b->parent = parent;
  parent->right = b;

  if (c != NULL)
    c->parent = grandParent;
  grandParent->left = c;

  return;
}

splayNode * splayTree::splay(splayNode * node, splayNode * treeRoot) { //Splay node in tree treeRoot

  splayNode * father = node->parent;
  while (father != NULL) {
    if(father->parent == NULL)
      single_rotate(node);
    else
      double_rotate(node);
    father = node->parent;
  }
  //std::cout << node->key << std::endl;
  treeRoot = node;
  //root = treeRoot;
    std::cout << "splayReturn: " << treeRoot->key << std::endl;
  return treeRoot;
}

// TODO - CHECK SEARCH operation
bool splayTree::search(int key, splayNode * treeRoot) {
  if (treeRoot==NULL) {
    return false;
    // return NULL;
  }

  splayNode * cur = treeRoot; //NODE TO Splay
  splayNode * pred = NULL;
  while (cur != NULL) {
    if(key == cur->key) {
      root = splay(cur, treeRoot);
      return true;
    }

    pred = cur;
    if (key < cur->key) {
      cur = cur->left;
    }
    else {
      cur = cur->right;
    }
  }

  root = splay(pred, treeRoot);
  return false;
}

bool splayTree::search(int key) {
  return search(key, root);
}

splayNode * splayTree::insert(int key, splayNode * treeRoot, splayNode * p_node) {

  if(treeRoot == NULL) {
  //    splayNode * x = newNode(key);
      treeRoot = newNode(key);
      treeRoot->parent = p_node;

      if (p_node!=NULL) {
        if (treeRoot->key < p_node->key && p_node!=NULL) {
          p_node->left = treeRoot;
        }
        else {  //treeRoot->val > p_node->val
          p_node->right = treeRoot;
        }
      }
      std::cout << "insertHelper_treeRoot: " << treeRoot->key << std::endl;
      return treeRoot;
  }
  if(key == treeRoot->key) {
    std::cout << "already in" << std::endl;
    return NULL;
  }
  if(key<treeRoot->key)
      return insert(key, treeRoot->left, treeRoot);
  else //key >= treeRoot->key
      return insert(key, treeRoot->right, treeRoot);
}

void splayTree::insert(int key) {
  splayNode * x = insert(key, root, NULL);
  std::cout << "insert_x: " << x->key << std::endl;
  root = splay(x, root);
  std::cout << "insert_root: " << x->key << std::endl;
  //std::cout << "insert(key, root) = " << (insert(key, root))->key << std::endl;
  return;
}

void splayTree::printInorder(splayNode * node) {
  if(node!=NULL) {
    printInorder(node->left);
    printInorder(node->right);
    std::cout << node->key << std::endl;
  }
  return;
}
void splayTree::printInorder() {
  printInorder(root);
  return;
}


void splayTree::printPostorder(splayNode * node) {
  if(node!=NULL) {
    std::cout << node->key << std::endl;
    printPostorder(node->left);
    printPostorder(node->right);
  }
  return;
}
void splayTree::printPostorder() {
  printPostorder(root);
  return;
}


int main() {
  splayTree t;
  t.insert(3);
  //t.printInorder();

  t.insert(2);
  //t.printInorder();

  std::cout << "root: " << t.root->key << std::endl;
  t.insert(6);
  //t.insert(3);

  std::cout << "root: " << t.root->key << std::endl;
  t.printInorder(); std::cout << std::endl;
  t.printPostorder(); std::cout << std::endl;
  //std::cout << "done" << std::endl;


  return 0;
}
