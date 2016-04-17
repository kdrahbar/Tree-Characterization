#include <unistd.h>
#include <ios>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <chrono>

using namespace std;
using  ns = chrono::nanoseconds;
using get_time = chrono::steady_clock ;

//Straightforward topdown splayTree
//Operations like standard BST, but splay after operation

//http://www.csee.umbc.edu/courses/undergraduate/341/fall98/frey/ClassNotes/Class17/splay.html
//http://www.cs.cornell.edu/courses/cs3110/2011sp/recitations/rec25-splay/splay.htm
// http://software.ucv.ro/~mburicea/lab7ASD.pdf

// http://lcm.csa.iisc.ernet.in/dsa/node94.html

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

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
    bool deleteNode(int key); //deletes key from tree

    //printing
    void printInorder();
    void printPreorder();

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
    //splayNode * deleteNode(int key, splayNode * root);

    splayNode * splayMax(splayNode * treeRoot);
    splayNode * join(splayNode * SL, splayNode * SR);
    //void split(splayNode * S, splayNode ** SL, splayNode ** SR, int i);



    splayNode * findMin(splayNode *);
    splayNode * findMax(splayNode *);

    splayNode * makeEmpty(splayNode* t);

    //printing
    void printInorder(splayNode * node);
    void printPreorder(splayNode * node);

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
  treeRoot = node;

  //std::cout << "splayReturn: " << treeRoot->key << std::endl;
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
      //std::cout << "insertHelper_treeRoot: " << treeRoot->key << std::endl;
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
  root = splay(x, root);
  //std::cout << "insert_x: " << x->key << std::endl;
  //std::cout << "insert_root: " << x->key << std::endl;
  return;
}


// NEED TO DOUBLE CHECK
// parent assignement
bool splayTree::deleteNode(int key) {
  if ( search(key) ) {
    splayNode * SL = root->left;
    if(SL != NULL) SL->parent = NULL;
    splayNode * SR = root->right;
    if(SR != NULL) SR->parent = NULL;

    root = join(SL, SR);
    return true;

  }
  return false;
}

splayNode * splayTree::splayMax(splayNode * treeRoot) {
  //Don't need to check if treeRoot = NULL;
  if(treeRoot == NULL) return NULL;

  splayNode * cur = treeRoot;
  splayNode * pred = NULL;
  while (cur != NULL) {
    pred = cur;
    cur = cur->right;
  }
  return splay(pred, treeRoot);
}

splayNode * splayTree::join(splayNode * SL, splayNode * SR) {
  if(SL != NULL) {
    //NOT EFFICIENT BUT WORKS
    splayNode * leftMax = findMax(SL);
    search(leftMax->key, SL);
    SL->parent = NULL;
    SL->right = SR;
    return SL;
    //set
  }
  else if (SR != NULL) {
    return SR;
  }
  else {
    return NULL;
  }
}


/*void splayTree::split(splayNode * S, splayNode ** SL, splayNode ** SR, int i) {


  return ;
}
*/


splayNode * splayTree::findMin(splayNode * t) {
  if(t == NULL)
      return NULL;
  else if(t->left == NULL)
      return t;
  else
      return findMin(t->left);
}

splayNode * splayTree::findMax(splayNode * t) {
  if(t == NULL)
      return NULL;
  else if(t->right == NULL)
      return t;
  else
      return findMax(t->right);
}



void splayTree::printInorder(splayNode * node) {
  if(node!=NULL) {
    printInorder(node->left);
    printInorder(node->right);
    std::cout << node->key << " ";//std::endl;
  }
  return;
}
void splayTree::printInorder() {
  printInorder(root);
  return;
}


void splayTree::printPreorder(splayNode * node) {
  if(node!=NULL) {
    std::cout << node->key << " ";// std::endl;
    printPreorder(node->left);
    printPreorder(node->right);
  }
  return;
}
void splayTree::printPreorder() {
  printPreorder(root);
  return;
}

void t_100() {
     splayTree tree;
    int size = 100;
    //int ins_nums [size];

    int test [100];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 100:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 100:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_1000() {
     splayTree tree;
    int size = 1000;
    //int ins_nums [size];

    int test [1000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 1000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 1000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_10000() {
     splayTree tree;
    int size = 10000;
    //int ins_nums [size];

    int test [10000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 10000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 10000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_100000() {
     splayTree tree;
    int size = 100000;
    //int ins_nums [size];

    int test [100000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 100000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 100000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_250000() {
     splayTree tree;
    int size = 250000;
    //int ins_nums [size];

    int test [250000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    // int x = getmem();
    // cout << x << endl;
    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 250,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 250,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_500000() {
    splayTree tree;
    int size = 500000;
    //int ins_nums [size];

    int test [500000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 500000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }

    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 500000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_750000() {
     splayTree tree;
    int size = 750000;
    //int ins_nums [size];

    int test [750000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 750,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 750,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_1000000() {
    splayTree tree;
    int size = 1000000;
    ////int ins_nums [size];

    int test [1000000];
    int pool [1000000];

    for (int i = 0; i < 1000000; i++) {
        pool[i] = i;
    }
    random_shuffle(std::begin(pool), std::end(pool));

    for (int i = 0; i < size; i++) {
        test[i] = pool[i];
    }

    random_shuffle(std::begin(test), std::end(test));
    auto start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.insert(test[i]);
    }

    double vm, rss;
    process_mem_usage(vm, rss);
    cout << "VM: " << vm << "; RSS: " << rss << endl;

    auto end = get_time::now();
    auto diff = end - start;
    cout<<"Avg insert time for 1,000,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;

    random_shuffle(std::begin(test), std::end(test));
    start = get_time::now();
    for (int i = 0; i < size; i++) {
        tree.deleteNode(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 1,000,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

int main() {
  t_100();
  t_100();
  t_1000();
  t_10000();
  t_100000();
  t_250000();
  t_500000();
  t_750000();
  t_1000000();

  return 0;
}
