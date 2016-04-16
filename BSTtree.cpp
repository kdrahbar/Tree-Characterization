#include <unistd.h>
#include <ios>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <chrono>

//Maybe can improve delete operation
//Just got something working


using namespace std;
using  ns = chrono::nanoseconds;
using get_time = chrono::steady_clock ;


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
    void deleteNode(int);
    bool search(int);
    void printInorder();

  private:
    BSTnode* root;

    BSTnode * makeEmpty(BSTnode *);
    BSTnode * insert(int, BSTnode *);
    BSTnode * findMin(BSTnode *);
    BSTnode * findMax(BSTnode *);
    BSTnode * deleteNode(int, BSTnode *);
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

void BSTtree::deleteNode(int x) {
    root = deleteNode(x, root);
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

BSTnode* BSTtree::deleteNode(int x, BSTnode* t) {
  BSTnode* temp;
  if(t == NULL)
      return NULL;
  else if(x < t->data)
      t->left = deleteNode(x, t->left);
  else if(x > t->data)
      t->right = deleteNode(x, t->right);
  else if(t->left && t->right)
  {
      temp = findMin(t->right);
      t->data = temp->data;
      t->right = deleteNode(t->data, t->right);
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

void t_100() {
     BSTtree tree;
    int size = 100;
    ////int ins_nums [size];

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
     BSTtree tree;
    int size = 1000;
    ////int ins_nums [size];

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
     BSTtree tree;
    int size = 10000;
    ////int ins_nums [size];

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
     BSTtree tree;
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
     BSTtree tree;
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
    BSTtree tree;
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
     BSTtree tree;
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
     BSTtree tree;
    int size = 1000000;
    //int ins_nums [size];

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

int main()
{
  t_100();
  t_1000();
  t_10000();
  t_100000();
  t_250000();
  t_500000();
  t_750000();
  t_1000000();

}
//int ins_nums
