#include <unistd.h>
#include <ios>
#include <cstring>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cmath>

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

class GoatNode
{
    public:
        GoatNode *right, *left, *parent;
        int value;
        GoatNode()
        {
            value = 0;
            right = NULL;
            left = NULL;
            parent = NULL;
        }
        GoatNode(int val)
        {
            value = val;
            right = NULL;
            left = NULL;
            parent = NULL;
        }
};
 
class ScapeGoatTree
{
    private:
        GoatNode *root;
        int n, q;
    public:
        ScapeGoatTree()
        {
            root = NULL;
            n = 0;
        }
 
        // Calc size
        int size(GoatNode *r)
        {
            if (r == NULL)
                return 0;
            else
            {
                int l = 1;
                l += size(r->left);
                l += size(r->right);
                return l;
            }
        }
 
        GoatNode* search(int val)
        {
            return search(root, val);
        }
 

        GoatNode* search(GoatNode *r, int val)
        {
            bool found = false;
            while ((r != NULL) && !found)
            {
                int rval = r->value;
                if (val < rval)
                    r = r->left;
                else if (val > rval)
                    r = r->right;
                else
                {
                    found = true;
                    break;
                }
                r = search(r, val);
            }
            return r;
        }
 
        int size()
        {
            return n;
        }
 
        static int const log32(int q)
        {
            double const log23 = 2.4663034623764317;
            return (int)ceil(log23 * log(q));
        }
 
        bool insert(int x)
        {
            GoatNode *u = new GoatNode(x);
            int d = addWithDepth(u);
            if (d > log32(q))
            {
                GoatNode *w = u->parent;
                while (3 * size(w) <= 2 * size(w->parent))
                    w = w->parent;
                rebuild(w->parent);
            }
            return d >= 0;
        }
    
        void rebuild(GoatNode *u)
        {
            int ns = size(u);
            GoatNode *p = u->parent;
            GoatNode **a = new GoatNode* [ns];
            condense(u, a, 0);
            if (p == NULL)
            {
                root = buildBalanced(a, 0, ns);
                root->parent = NULL;
            }
            else if (p->right == u)
            {
                p->right = buildBalanced(a, 0, ns);
                p->right->parent = p;
            }
            else
            {
                p->left = buildBalanced(a, 0, ns);
                p->left->parent = p;
            }
        }
 
        int condense(GoatNode *u, GoatNode *a[], int i)
        {
            if (u == NULL)
            {
                return i;
            }
            i = condense(u->left, a, i);
            a[i++] = u;
            return condense(u->right, a, i);
        }
 
        GoatNode *buildBalanced(GoatNode **a, int i, int ns)
        {
            if (ns == 0)
                return NULL;
            int m = ns / 2;
            a[i + m]->left = buildBalanced(a, i, m);
            if (a[i + m]->left != NULL)
                a[i + m]->left->parent = a[i + m];
            a[i + m]->right = buildBalanced(a, i + m + 1, ns - m - 1);\
            if (a[i + m]->right != NULL)
                a[i + m]->right->parent = a[i + m];
            return a[i + m];
        }
 
        int addWithDepth(GoatNode *u)
        {
            GoatNode *w = root;
            if (w == NULL)
            {
                root = u;
                n++;
                q++;
                return 0;
            }
            bool done = false;
            int d = 0;
            do
            {
                if (u->value < w->value)
                {
                    if (w->left == NULL)
                    {
                        w->left = u;
                        u->parent = w;
                        done = true;
                    }
                    else
                    {
                        w = w->left;
                    }
                }
                else if (u->value > w->value)
                {
                    if (w->right == NULL)
                    {
                        w->right = u;
                        u->parent = w;
                        done = true;
                    }
                    else
                    {
                        w = w->right;
                    }
                }
                else
                    return -1;
                d++;
            }
            while (!done);
            n++;
            q++;
            return d;
        }

      void remove(int x) {
        GoatNode* remove_node = search(x);
        if (remove_node) {
            if (2*n < q) {
                rebuild(root);
                q = n;
            }
        }
    }
};
 

void t_100() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 100:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_1000() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 1000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_10000() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 10000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_100000() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 100000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_250000() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 250,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_500000() {
    ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }

    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 500000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_750000() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 750,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_1000000() {
     ScapeGoatTree tree;
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
        tree.remove(test[i]);
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
    

  return 0;
}