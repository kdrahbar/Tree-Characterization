// inserting into a vector
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ios>
#include <cstring>
#include <unistd.h>
#include <utility>
#include <map>
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

struct node
{
       int val;
       node *parent;
       char color;
       node *left;
       node *right;
};

class RBtree
{
      node *root;
      node *q;
   public :
      RBtree()
      {
              q=NULL;
              root=NULL;
      }
      void insert(node *);
      void insert(int);
      void insertfix(node *);
      void leftrotate(node *);
      void rightrotate(node *);
      void del(int);
      node* successor(node *);
      void delfix(node *);
      void search(int);
};

void RBtree::insert(int x) {
  node *n = new node;
  n->val = x;
  insert(n);
}

void RBtree::insert(node *t)
{
     node *p,*q;
     t->left=NULL;
     t->right=NULL;
     t->color='r';
     p=root;
     q=NULL;
     if(root==NULL)
     {
           root=t;
           t->parent=NULL;
     }
     else
     {
         while(p!=NULL)
         {
              q=p;
              if(p->val<t->val)
                  p=p->right;
              else
                  p=p->left;
         }
         t->parent=q;
         if(q->val<t->val)
              q->right=t;
         else
              q->left=t;
     }
     insertfix(t);
}
void RBtree::insertfix(node *z)
{
    if (z->parent != NULL && z->parent->parent != NULL)
    {
        while (z != NULL && z->parent != NULL &&
        z->parent->parent != NULL && !z->parent->color == 'b')
        // ass long as color is not black, thus red
        {
            if (z->parent == z->parent->parent->left)
            {
                node *y = z->parent->parent->right;
                if (y != NULL && y->color == 'r')
                {
                    z->parent->color = 'b';
                    y->color = 'b';
                    z->parent->parent->color = 'r';
                    z = z->parent->parent;
                }
                else if (z == z->parent->right)
                {
                    z = z->parent;
                    leftrotate(z);
                }
                z->parent->color = 'b';
                z->parent->parent->color = 'r';
                rightrotate(z->parent->parent);

            }
            else
            {

                node *y = z->parent->parent->left; 
                if (y != NULL && y->color == 'r') 
                {
                    z->parent->color = 'b'; 
                    y->color = 'b'; 
                    z->parent->parent->color = 'r'; 
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->left) 
                    {
                        z = z->parent;
                        rightrotate(z);
                    }
                    z->parent->color = 'b'; 
                    z->parent->parent->color = 'r'; 
                    leftrotate(z->parent->parent);
                }
            }
        }
    }
}
void RBtree::del(int x)
{
     if(root==NULL)
     {
           return ;
     }
    
     node *p;
     p=root;
     node *y=NULL;
     node *q=NULL;
     int found=0;
     while(p!=NULL&&found==0)
     {
           if(p->val==x)
               found=1;
           if(found==0)
           {
                 if(p->val<x)
                    p=p->right;
                 else
                    p=p->left;
           }
     }
     if(found==0)
     {
            return ;
     }
     else
     {
         if(p->left==NULL||p->right==NULL)
              y=p;
         else
              y=successor(p);
         if(y->left!=NULL)
              q=y->left;
         else
         {
              if(y->right!=NULL)
                   q=y->right;
              else
                   q=NULL;
         }
         if(q!=NULL)
              q->parent=y->parent;
         if(y->parent==NULL)
              root=q;
         else
         {
             if(y==y->parent->left)
                y->parent->left=q;
             else
                y->parent->right=q;
         }
         if(y!=p)
         {
             p->color=y->color;
             p->val=y->val;
         }
         if(y->color=='b')
             delfix(q);
     }
}

void RBtree::delfix(node *p)
{
    node *s;
    while(p!=root&&p->color=='b')
    {
          if(p->parent->left==p)
          {
                  s=p->parent->right;
                  if(s->color=='r')
                  {
                         s->color='b';
                         p->parent->color='r';
                         leftrotate(p->parent);
                         s=p->parent->right;
                  }
                  if(s->right->color=='b'&&s->left->color=='b')
                  {
                         s->color='r';
                         p=p->parent;
                  }
                  else
                  {
                      if(s->right->color=='b')
                      {
                             s->left->color=='b';
                             s->color='r';
                             rightrotate(s);
                             s=p->parent->right;
                      }
                      s->color=p->parent->color;
                      p->parent->color='b';
                      s->right->color='b';
                      leftrotate(p->parent);
                      p=root;
                  }
          }
          else
          {
                  s=p->parent->left;
                  if(s->color=='r')
                  {
                        s->color='b';
                        p->parent->color='r';
                        rightrotate(p->parent);
                        s=p->parent->left;
                  }
                  if(s->left->color=='b'&&s->right->color=='b')
                  {
                        s->color='r';
                        p=p->parent;
                  }
                  else
                  {
                        if(s->left->color=='b')
                        {
                              s->right->color='b';
                              s->color='r';
                              leftrotate(s);
                              s=p->parent->left;
                        }
                        s->color=p->parent->color;
                        p->parent->color='b';
                        s->left->color='b';
                        rightrotate(p->parent);
                        p=root;
                  }
          }
       p->color='b';
       root->color='b';
    }
}

void RBtree::leftrotate(node *p)
{
     if(p->right==NULL)
           return ;
     else
     {
           node *y=p->right;
           if(y->left!=NULL)
           {
                  p->right=y->left;
                  y->left->parent=p;
           }
           else
                  p->right=NULL;
           if(p->parent!=NULL)
                y->parent=p->parent;
           if(p->parent==NULL)
                root=y;
           else
           {
               if(p==p->parent->left)
                       p->parent->left=y;
               else
                       p->parent->right=y;
           }
           y->left=p;
           p->parent=y;
     }
}
void RBtree::rightrotate(node *p)
{
     if(p->left==NULL)
          return ;
     else
     {
         node *y=p->left;
         if(y->right!=NULL)
         {
                  p->left=y->right;
                  y->right->parent=p;
         }
         else
                 p->left=NULL;
         if(p->parent!=NULL)
                 y->parent=p->parent;
         if(p->parent==NULL)
               root=y;
         else
         {
             if(p==p->parent->left)
                   p->parent->left=y;
             else
                   p->parent->right=y;
         }
         y->right=p;
         p->parent=y;
     }
}

node* RBtree::successor(node *p)
{
      node *y=NULL;
     if(p->left!=NULL)
     {
         y=p->left;
         while(y->right!=NULL)
              y=y->right;
     }
     else
     {
         y=p->right;
         while(y->left!=NULL)
              y=y->left;
     }
     return y;
}


void RBtree::search(int x)
{
     if(root==NULL)
     {
           cout<<"\nEmpty Tree\n" ;
           return  ;
     }

     node *p=root;
     int found=0;
     while(p!=NULL&& found==0)
     {
            if(p->val==x)
                found=1;
            if(found==0)
            {
                 if(p->val<x)
                      p=p->right;
                 else
                      p=p->left;
            }
     }
     if(found==0)
          cout<<"\nElement Not Found.";
     else
     {
                cout<<"\n\t FOUND NODE: ";
                cout<<"\n val: "<<p->val;
                cout<<"\n Colour: ";
    if(p->color=='b')
     cout<<"Black";
    else
     cout<<"Red";
                if(p->parent!=NULL)
                       cout<<"\n Parent: "<<p->parent->val;
                else
                       cout<<"\n There is no parent of the node.  ";
                if(p->right!=NULL)
                       cout<<"\n Right Child: "<<p->right->val;
                else
                       cout<<"\n There is no right child of the node.  ";
                if(p->left!=NULL)
                       cout<<"\n Left Child: "<<p->left->val;
                else
                       cout<<"\n There is no left child of the node.  ";
                cout<<endl;

     }
}


void t_100() {
     RBtree tree;
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
        tree.del(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 100:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_1000() {
     RBtree tree;
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
        tree.del(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 1000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_10000() {
     RBtree tree;
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
        tree.del(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 10000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_100000() {
     RBtree tree;
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
        tree.del(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 100000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_250000() {
     RBtree tree;
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
        tree.del(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 250,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}

void t_500000() {
    RBtree tree;
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
        tree.del(test[i]);
    }

    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 500000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_750000() {
     RBtree tree;
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
        tree.del(test[i]);
    }
    end = get_time::now();
    diff = end - start;
    cout<<"Avg delete time for 750,000:  "<< chrono::duration_cast<ns>(diff).count()/size<<" ns "<<endl;
    cout << endl;
}


void t_1000000() {
    RBtree tree;
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
        tree.del(test[i]);
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
