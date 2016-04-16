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

struct WplRec {
    WplRec * left;   //WplPtr
    int item;        //key?
    int WT;           //Weight associated with node
    WplRec * right;  //WplPtr
};

class WplTree {

  public:
    WplTree();
    void insert(int);
    void deleteNode(int);
    //WplRec * search(int);

    //printing
    void printInorder();
    void printPreorder();

    WplRec * root;

  //private:
    //WplRec * root;


    //Helper Functions
    struct WplRec* newNode(int); //NOT SURE
    bool lessThan(int a, int b);
    WplRec * NewWplPtr(WplRec * l, int i, int WT, WplRec * r);
    int wt (WplRec * t);


    void rightRotate (WplRec * t, WplRec * parent);
    void leftRotate (WplRec * t, WplRec * parent);
    WplRec * insertLookup (WplRec * t, int, WplRec * p);
    //void insertLookup (WplRec * t, int, WplRec * p);
    void checkRightRotation(WplRec * t, WplRec * parent);
    void checkLeftRotation(WplRec * t, WplRec * parent);

    void checkRightRotationTrue(WplRec * t, WplRec * parent);
    void checkLeftRotationTrue(WplRec * t, WplRec * parent);

    WplRec * findMin(WplRec * t);
    WplRec* deleteNode(int x, WplRec* t);


    //printing
    void printInorder(WplRec * t);
    void printPreorder(WplRec * t);

    WplRec* testInsert ( WplRec* node, int key, int weight);
    WplRec * testInsert(int key, int weight);


};

WplTree::WplTree() {
    root = NULL;
}

WplRec* WplTree::testInsert ( WplRec* node, int key, int weight) {
    if(node == NULL) {
        WplRec * newNode = new WplRec;
        newNode->item = key;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->WT = weight;

        return newNode;
    }
    if(key<node->item)
        node->left = testInsert(node->left, key, weight);
    else //key >= node->key
        node->right = testInsert(node->right, key, weight);
    return node;
}

WplRec * WplTree::testInsert(int key, int weight) {
    root = testInsert(root, key, weight);
    return root;
}


bool WplTree::lessThan(int a, int b){
    if (a < b) {
        return true;
    }
    return false;
}

WplRec * WplTree::NewWplPtr(WplRec * l, int i, int WT, WplRec * r) {
    WplRec * newNode = new WplRec;
    newNode->left = l;
    newNode->item = i;
    newNode->WT = WT;
    newNode->right = r;
    return newNode;
}

int WplTree::wt (WplRec * t) {

    int wt = t->WT;
    if (t->left != NULL)
      wt = wt - t->left->WT;
    if (t->right != NULL)
      wt = wt - t->right->WT;
    return wt;
}

void WplTree::rightRotate(WplRec * t, WplRec * parent) {
  if(parent == NULL) {
    //std::cout << "root change" << std::endl;
    root = t->left;
  }

  int totalWT = t->WT;
  WplRec * tmp = t;
  t = t->left;


  if (tmp->left->right == NULL) {tmp->WT = tmp->WT - tmp->left->WT+1;}
  else {tmp->WT = tmp->WT - tmp->left->WT + tmp->left->right->WT;}

  tmp->left = tmp->left->right;

  t->right = tmp;
  t->WT = totalWT;
  if(parent!=NULL && parent->item>t->item) parent->left = t;
  else if(parent!=NULL) parent->right = t;

  //root = t;
  return;
}


void WplTree::leftRotate(WplRec * t, WplRec * parent) {
  //std::cout << "inLeft"<< t->item << std::endl;

  if(parent == NULL) {
    //std::cout << "root change" << std::endl;
    root = t->right;
  }
  int totalWT = t->WT;
  WplRec * tmp = t;
  t = t->right;

  if (tmp->right->left == NULL) {tmp->WT = tmp->WT - tmp->right->WT+1; }
  else {tmp->WT = tmp->WT - tmp->right->WT + tmp->right->left->WT; }

  tmp->right = tmp->right->left;
  t->left = tmp;
  t->WT = totalWT;

  if(parent!=NULL && parent->item>t->item) parent->left = t;
  else if(parent!=NULL) parent->right = t;


/*
  int totalWT = t->WT;
  WplRec * tmp = t;
  t = t->right;
  tmp->WT = tmp->WT - tmp->right->WT + tmp->right->left->WT;
  tmp->left = tmp->right->left;
  t->right = tmp;
  t->WT = totalWT;
  root = t;

  std::cout << "IMPORTANT\n"
            << root->item << " " << root->WT << "\n"<< root->left->item<<" "<<root->left->WT<<"\n"
            << root->right->item<<" "<<root->right->WT<<"\n"<< root->right->left->item
            << " "<<root->right->left->WT<< std::endl;
*/
  return;
}

void WplTree::checkRightRotationTrue(WplRec * t, WplRec * parent) {
  if (t->left != NULL && t->left->right != NULL) {
    //if (2*t->left->WT > t->left->right->WT+t->WT) {
    //if (t->left->right == NULL || 2*t->left->WT > t->left->right->WT+t->WT) {
      //std::cout << "checkRightRotation is TRUE: "<< t->item << std::endl;
      rightRotate(t, parent);
      if (t->right!=NULL) checkLeftRotationTrue (t->right, parent);
    //}
  }
  return;
}

void WplTree::checkLeftRotationTrue(WplRec * t, WplRec * parent) {
  if (t->right!=NULL && t->right->left != NULL) {

    //if (2*t->right->WT > t->right->left->WT+t->WT) {
    //if (t->right->left == NULL || 2*t->right->WT > t->right->left->WT+t->WT) {
      //std::cout << "checkLeftRotation is TRUE, node: "<< t->item << std::endl;
      leftRotate(t, parent);

      if (t->left!=NULL) checkRightRotationTrue (t->left, parent);
    //}
  }
  return;
}

void WplTree::checkRightRotation (WplRec * t, WplRec * parent) {
  if (t->left != NULL && t->left->right != NULL) {
    if (2*t->left->WT > t->left->right->WT+t->WT) {
    //if (t->left->right == NULL || 2*t->left->WT > t->left->right->WT+t->WT) {
      //std::cout << "checkRightRotation is TRUE: "<< t->item << std::endl;
      rightRotate(t, parent);
      if (t->right!=NULL) checkLeftRotationTrue (t->right, parent);
    }
  }
  return;
}

void WplTree::checkLeftRotation (WplRec * t, WplRec * parent) {
  //std::cout << "t_lRot: " << t->item << std::endl;
  if (t->right!=NULL && t->right->left != NULL) {

    if (2*t->right->WT > t->right->left->WT+t->WT) {
    //if (t->right->left == NULL || 2*t->right->WT > t->right->left->WT+t->WT) {
      //std::cout << "checkLeftRotation is TRUE, node: "<< t->item << std::endl;
      leftRotate(t, parent);

      if (t->left!=NULL) checkRightRotationTrue (t->left, parent);
    }
  }
  return;
}

WplRec * WplTree::insertLookup(WplRec * t, int i, WplRec * parent) {
  //Moved this to top for ease of coding
  if(t==NULL) {
    t = NewWplPtr(NULL, i, 1, NULL);

    //std::cout <<"hi" << t->item << std::endl;
    if (parent == NULL) root = t;
    else if(parent->item < t->item) parent->right = t;
    else parent->left = t;

    return t;
  }

  else if( lessThan(i, t->item) ) {
    insertLookup(t->left, i, t);
    //std::cout << "t: " << t->item << std::endl;
    t->WT = t->WT+1;
    checkRightRotation(t, parent);
    return t;
  }
  else if (lessThan(t->item, i) ) {
    insertLookup(t->right, i, t);
    //std::cout << "hi0" << std::endl;
    t->WT = t->WT+1;
    checkLeftRotation(t, parent);
    return t;
  }
  //Should NOT reach this case unless node is already in here
  else {
    t->WT = t->WT+1;
    return t;
  }

}

void WplTree::insert(int i) {
  //std::cout << "inserting: " << i << std::endl;
  //root = insertLookup(root, i, NULL);
  insertLookup(root, i, NULL);
  return;
}

void WplTree::printInorder(struct WplRec * leaf) {
    if (leaf != NULL) {
        printInorder(leaf->left);
        std::cout << leaf->item <<"\t"<<leaf->WT<<"\t"<< std::endl;
        printInorder(leaf->right);
    }
    return;
}

void WplTree::printInorder() {
  printInorder(root);
  return;
}
// void wplTree::globalInsertLookup(WplRec * t, int i) {
//   //Not sure
//   //i =
//   insertLookup(t, i);
//   return;
// }


void WplTree::printPreorder(struct WplRec * leaf) {
    if (leaf != NULL) {
      std::cout << leaf->item <<"\t"<<leaf->WT<<"\t"<< " ";//std::endl;
        printPreorder(leaf->left);
        printPreorder(leaf->right);
    }
    return;
}

void WplTree::printPreorder() {
  printPreorder(root);
  return;
}

WplRec * WplTree::findMin(WplRec * t) {
  if(t == NULL)
      return NULL;
  else if(t->left == NULL)
      return t;
  else
      return findMin(t->left);
}

WplRec* WplTree::deleteNode(int x, WplRec* t) {
  WplRec* temp;
  if(t == NULL)
      return NULL;
  else if(x < t->item)
      t->left = deleteNode(x, t->left);
  else if(x > t->item)
      t->right = deleteNode(x, t->right);
  else if(t->left && t->right)
  {
      temp = findMin(t->right);
      t->item = temp->item;
      t->right = deleteNode(t->item, t->right);
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

void WplTree::deleteNode(int x) {
    root = deleteNode(x, root);
}

void t_100() {
     WplTree tree;
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
     WplTree tree;
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
    WplTree tree;
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
     WplTree tree;
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
     WplTree tree;
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
    WplTree tree;
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
     WplTree tree;
    int size = 750000;
    // int ins_nums [size];

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
     WplTree tree;
    int size = 1000000;
    // int ins_nums [size];

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
  t_1000();
  t_10000();
  t_100000();
  t_250000();
  t_500000();
  t_750000();
  t_1000000();


  return 0;
}
