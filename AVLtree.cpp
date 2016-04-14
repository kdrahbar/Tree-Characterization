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


struct AVLnode
{
  int key;
  struct AVLnode *left;
  struct AVLnode *right;
  int height;
};

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

class AVLtree
{
  public:
   AVLtree();
    //~BSTtree();
    struct AVLnode * insert(int);
    struct AVLnode * deleteNode(int);
    struct AVLnode * search(int);

    void printPreorder();
    struct AVLnode * root;

  private:
    //struct AVLnode *root;
    struct AVLnode * insert(struct AVLnode * , int );
    struct AVLnode * deleteNode(struct AVLnode *, int);
    struct AVLnode * search(struct AVLnode *, int);
    //void insert(struct AVLnode * , int );


    // printing
    void printPreorder(struct AVLnode *leaf);
    void printPostorder(struct AVLnode *leaf);

    //helper functions
    int height(struct AVLnode *); //get height of tree
    struct AVLnode* newNode(int );
    struct AVLnode *rightRotate(struct AVLnode *);
    struct AVLnode *leftRotate(struct AVLnode *);
    int getBalance(struct AVLnode * N);
    struct AVLnode * minValueNode(struct AVLnode* node);


};

AVLtree::AVLtree()
{
  root=NULL;
  return;
}

int AVLtree::height(struct AVLnode *node)
{
    if (node==NULL)
        return 0;
    return node->height;
}

struct AVLnode * AVLtree::newNode(int key)
{
    struct AVLnode* node = new AVLnode;
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    //std::cout << node->key << std::endl;
    return (node);
}

struct AVLnode * AVLtree::rightRotate(struct AVLnode * y)
{
    struct AVLnode *x = y->left;
    struct AVLnode *T2 = x->right;

    // perform rotation
    x->right = y;
    y->left = T2;

    // update heights
    y->height = std::max(height(y->left), height(y->right))+1;
    x->height = std::max(height(x->left), height(x->right))+1;

    //return new root
    return x;
}

struct AVLnode * AVLtree::leftRotate(struct AVLnode * x)
{
    struct AVLnode *y = x->right;
    struct AVLnode *T2 = y->left;

    // perform rotation
    y->left = x;
    x->right = T2;

    //return new root
    x->height = std::max(height(x->left), height(x->right))+1;
    y->height = std::max(height(y->left), height(y->right))+1;

    return y;
}

int AVLtree::getBalance(struct AVLnode *N)
{
    if (N==NULL)
        return 0;
    return height(N->left)-height(N->right);
}

struct AVLnode * AVLtree::insert(struct AVLnode* node, int key)
//void AVLtree::insert(struct AVLnode* node, int key)
{
    //BST insert
    if (node==NULL) {
        return newNode(key);
        //struct AVLnode * a = newNode(key);
        //return a;
    }
    if (key < node->key)
        node->left = insert(node->left, key);
    else
        node->right = insert(node->right, key);

    //Update height of ancestor node
    node->height = std::max(height(node->left), height(node->right))+1;

    //Get balance factor
    int balance = getBalance(node);

    //Left Left Case
    if (balance>1 && key<node->left->key)
        return rightRotate(node);

    //Right Right Case
    else if (balance<-1 && key>node->right->key)
        return leftRotate(node);

    //Left Right Case
    else if (balance>1 && key>node->left->key)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    //Right Left Case
    else if (balance<-1 && key<node->right->key)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    // SHOULD NEVER GET HERE

    return node;
}

struct AVLnode * AVLtree::minValueNode(struct AVLnode* node) {
    struct AVLnode * cur = node;
    while (cur->left != NULL) {
        cur = cur->left;
    }
    return cur;
}

void AVLtree::printPreorder(struct AVLnode* leaf)
{
    //std::cout << "0" << std::endl;
    //std::cout << leaf->key << std::endl;
    if(leaf!=NULL)
    {
        std::cout << leaf->key << std::endl;
        printPreorder(leaf->left);
        printPreorder(leaf->right);
    }
    return;
}

struct AVLnode * AVLtree::deleteNode(struct AVLnode* node, int key)
{
    // STEP 1: PERFORM STANDARD BST DELETE

    if (node == NULL)
        return node;

    // If the key to be deleted is smaller than the node's key,
    // then it lies in left subtree
    if ( key < node->key )
        node->left = deleteNode(node->left, key);

    // If the key to be deleted is greater than the node's key,
    // then it lies in right subtree
    else if( key > node->key )
        node->right = deleteNode(node->right, key);

    // if key is same as node's key, then This is the node
    // to be deleted
    else
    {
        // node with only one child or no child
        if( (node->left == NULL) || (node->right == NULL) )
        {
            struct AVLnode *temp = node->left ? node->left : node->right;

            // No child case
            if(temp == NULL)
            {
                temp = node;
                node = NULL;
            }
            else // One child case
             *node = *temp; // Copy the contents of the non-empty child

            free(temp);
        }
        else
        {
            // node with two children: Get the inorder successor (smallest
            // in the right subtree)
            struct AVLnode* temp = minValueNode(node->right);

            // Copy the inorder successor's data to this node
            node->key = temp->key;

            // Delete the inorder successor
            node->right = deleteNode(node->right, temp->key);
        }
    }

    // If the tree had only one node then return
    if (node == NULL)
      return node;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    node->height = std::max(height(node->left), height(node->right)) + 1;

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether
    //  this node became unbalanced)
    int balance = getBalance(node);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance(node->left) >= 0)
        return rightRotate(node);

    // Left Right Case
    if (balance > 1 && getBalance(node->left) < 0)
    {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && getBalance(node->right) <= 0)
        return leftRotate(node);

    // Right Left Case
    if (balance < -1 && getBalance(node->right) > 0)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

AVLnode * AVLtree::search(struct AVLnode * node, int key) {
    if (root==NULL || root->key==key)
        return root;

    if (root->key < key)
        return search(root->right, key);

    return search(root->left, key);
}

AVLnode * AVLtree::insert(int key)
//void AVLtree::insert(int key)
{
    root = insert(root, key);
    return root;
}

AVLnode * AVLtree::deleteNode(int key)
{
    root = deleteNode(root, key);
    return root;
}

AVLnode * AVLtree::search(int key)
{
    return search(root, key);
}

void AVLtree::printPreorder() {
    return printPreorder(root);
}

void t_100() {
     AVLtree tree;
    int size = 100;
    int ins_nums [size];

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
     AVLtree tree;
    int size = 1000;
    int ins_nums [size];

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
     AVLtree tree;
    int size = 10000;
    int ins_nums [size];

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
     AVLtree tree;
    int size = 100000;
    int ins_nums [size];

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
     AVLtree tree;
    int size = 250000;
    int ins_nums [size];

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
    AVLtree tree;
    int size = 500000;
    int ins_nums [size];

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
     AVLtree tree;
    int size = 750000;
    int ins_nums [size];

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
     AVLtree tree;
    int size = 1000000;
    int ins_nums [size];

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
    AVLtree tree;
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
