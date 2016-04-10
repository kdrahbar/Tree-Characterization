// inserting into a vector
#include <iostream>
#include <vector>
#include <string>
#include <string>
#include <utility>
#include <vector>
#include <map>

using namespace std;

struct node
{
       int orig_val;
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
      void insertfix(node *);
      void leftrotate(node *);
      void rightrotate(node *);
      void del(int);
      node* successor(node *);
      void delfix(node *);
      void disp();
      void display( node *);
      void search(int);
};
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

                node *y = z->parent->parent->left; // left instead of right
                if (y != NULL && y->color == 'r') // is red?
                {
                    z->parent->color = 'b'; // color = black
                    y->color = 'b'; // color = black
                    z->parent->parent->color = 'r'; // color = red
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->left) // left instead of right
                    {
                        z = z->parent;
                        rightrotate(z);
                    }
                    z->parent->color = 'b'; // color is black
                    z->parent->parent->color = 'r'; // color is red
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
           cout<<"\nEmpty Tree." ;
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
            cout<<"\nElement Not Found.";
            return ;
     }
     else
     {

         if(p->parent!=NULL)
               cout<<"\nParent: "<<p->parent->val;
         else
               cout<<"\nThere is no parent of the node.  ";
         if(p->right!=NULL)
               cout<<"\nRight Child: "<<p->right->val;
         else
               cout<<"\nThere is no right child of the node.  ";
         if(p->left!=NULL)
               cout<<"\nLeft Child: "<<p->left->val;
         else
               cout<<"\nThere is no left child of the node.  ";
         cout<<"\nNode Deleted.";
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

void RBtree::disp()
{
     display(root);
}
void RBtree::display(node *p)
{
     if(root==NULL)
     {
          cout<<"\nEmpty Tree.";
          return ;
     }
     if(p!=NULL)
     {
                cout<<"\n\t NODE: ";
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
    if(p->left)
    {
                 cout<<"\n\nLeft:\n";
     display(p->left);
    }
    /*else
     cout<<"\nNo Left Child.\n";*/
    if(p->right)
    {
     cout<<"\n\nRight:\n";
                 display(p->right);
    }
    /*else
     cout<<"\nNo Right Child.\n"*/
     }
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

struct ET_Node {
    int val;
    // ET_Node *left_child;
    // ET_Node *right_child;
    ET_Node *children;
    ET_Node *sibling;
    node *first_visit;
    node *last_visit;
};

struct ET_forestNode {
    std::vector<node*> euler_path;
    RBtree* rb_tree;
    ET_Node *orig_root;
};

class EulerTourTree {
    map <string, ET_forestNode*> forest;
    node *find_root(ET_Node*);
    void euler_tour(ET_Node*, std::vector<node*>&);
    ET_Node* find_node(int, string);

  public:
    void add_tree (string id, ET_Node*);
    void link(int, int, string, string);
    // void cut(int, int, string, string);
    void cut(ET_Node*, ET_Node*, string, string);
    bool connected(ET_Node*, ET_Node*);
};

ET_Node* EulerTourTree::find_node(int u, string id) {
    ET_Node *root = forest[id]->orig_root;
    vector<ET_Node*> stack;

    stack.push_back(root);
    ET_Node *tmp;
    while(stack.size() > 0) {
        tmp = stack[stack.size()-1];
        stack.pop_back();

        if (tmp->val == u) {
            break;
        }
        tmp = tmp->children;
        
        while(tmp != NULL) { 
            stack.push_back(tmp);
            tmp = tmp->sibling;
        }
    }

    return tmp;
}

// Finds the root of the original tree which should
// always be the left most node of the bbst
// always O(lg(n))
node* EulerTourTree::find_root(ET_Node* n) {
    node *tmp = n->first_visit;
    while(tmp->parent != NULL) {
         tmp = tmp->parent;
    }
    while(tmp->left != NULL) {
        tmp = tmp->left;
    }
    return tmp;
}

bool EulerTourTree::connected(ET_Node* u, ET_Node* v) {
    node* u_root = find_root(u);
    node* v_root = find_root(v);

    if (u_root->orig_val == v_root->orig_val) {
        return true;
    }
    return false;
}

// Split E int E1, V, E2
// V is span of first occ of v to last occ of v
// Make new Tree containing V
// void EulerTourTree::cut(int u, int v, string id, string new_id) {
void EulerTourTree::cut(ET_Node* u_orig, ET_Node* v_orig, string id, string new_id) {
    // ET_Node *v_orig = find_node(v, id);
    // ET_Node *u_orig = find_node(u, id);

    int first_occ = v_orig->first_visit->val;

    int last_occ = v_orig->last_visit->val;
    int extra_u;

    // v is the only child of u so u is now a leaf
    if (u_orig->first_visit->val == forest[id]->euler_path[first_occ-1]->val &&
        u_orig->last_visit->val == forest[id]->euler_path[last_occ+1]->val) {
        u_orig->last_visit = NULL;
        extra_u = first_occ;
    }
    // one of the two visits of u is the first visit
    else if (u_orig->first_visit->val == forest[id]->euler_path[first_occ-1]->val) {
     extra_u = first_occ;   
    }
    // one of the visits is the last visit of u
    else if (u_orig->last_visit->val == forest[id]->euler_path[last_occ+1]->val) {
        extra_u = first_occ-1;
    }
    else
        extra_u = first_occ;


    vector<node*> V;
    V.insert(V.end(), forest[id]->euler_path.begin()+first_occ,  forest[id]->euler_path.begin()+last_occ);
    forest[id]->euler_path.erase(forest[id]->euler_path.begin()+first_occ,  forest[id]->euler_path.begin()+last_occ);

    // Delete the extra u
    forest[id]->euler_path.erase(forest[id]->euler_path.begin()+extra_u);

    for(auto & del_node : V) {
        forest[id]->rb_tree->del(del_node->val);
    }

    // Create new rb tree
    RBtree *rb_tree = new RBtree;
    for (int i =0; i < V.size(); i++) {
        V[i]->val = i;
        rb_tree->insert(V[i]);
    }
    
    // Create new tree with V
    ET_forestNode *fn = new ET_forestNode;
    fn->euler_path = V;
    fn->rb_tree = rb_tree;
    fn->orig_root = v_orig;

    forest[new_id] = fn;

}



void EulerTourTree::link(int u, int v, string u_id, string v_id) {
    int rotate = 0;
    // int rotate = find_node(u, u_id)->first_visit->val;
    vector<node*> rotate_nodes;

    // Find the first occurence of u in the euler path
    for (int i = 0; i < forest[u_id]->euler_path.size(); i++) {
        if (forest[u_id]->euler_path[i]->orig_val == u) {
            rotate = i;
            break;
        }
        else {
            rotate_nodes.push_back(forest[u_id]->euler_path[i]);
        }
    }

    // rotate around the first occurence of u in the euler path
    if (rotate != 0) {
        forest[u_id]->euler_path.erase(forest[u_id]->euler_path.begin(), forest[u_id]->euler_path.begin()+rotate-1);
        forest[u_id]->euler_path.insert(forest[u_id]->euler_path.end(), rotate_nodes.begin(), rotate_nodes.end());
    }
    rotate_nodes.clear();

    // rotate = find_node(v, v_id)->first_visit->val;
    for (int i = 0; i < forest[v_id]->euler_path.size(); i++) {
        if (forest[v_id]->euler_path[i]->orig_val == v) {
            rotate = i;
            break;
        }
        else {
            rotate_nodes.push_back(forest[v_id]->euler_path[i]);
        }
    }

    if (rotate != 0) {
        forest[v_id]->euler_path.erase(forest[v_id]->euler_path.begin(), forest[v_id]->euler_path.begin()+rotate-1);
        forest[v_id]->euler_path.insert(forest[v_id]->euler_path.end(), rotate_nodes.begin(), rotate_nodes.end());
    }

    // Concatenate E1, E2 and u
    forest[u_id]->euler_path.insert(forest[u_id]->euler_path.end(), forest[v_id]->euler_path.begin(), forest[v_id]->euler_path.end());
    ET_Node *new_root = find_node(u, u_id);
    ET_Node *v_orig = find_node(v, v_id);
    

    node* last_visit = new node;
    new_root->last_visit = last_visit;
    last_visit->orig_val = u;
    forest[u_id]->euler_path.push_back(last_visit);
    
    // Reform balanced tree
    RBtree* rb_tree = new RBtree;
    for (int i = 0; i < forest[u_id]->euler_path.size(); i++) {
        forest[u_id]->euler_path[i]->val = i;
        rb_tree->insert(forest[u_id]->euler_path[i]);
    }

    forest[u_id]->rb_tree = rb_tree;
    forest.erase(v_id);

}

void EulerTourTree::euler_tour(ET_Node *current, std::vector<node*>& result) {
    if(current == NULL) {
        return;
    }

    node* et_first_visit = new node;
    et_first_visit->orig_val = current->val;
    current->first_visit = et_first_visit;

    ET_Node *tmp = current->children;
    result.push_back(et_first_visit);
    while(tmp) {
        node* curr_visit = new node;
        curr_visit->orig_val = current->val;

        euler_tour(tmp, result);
        result.push_back(curr_visit);
        current->last_visit = curr_visit;

        tmp = tmp->sibling;
    }
}

void EulerTourTree::add_tree(string id, ET_Node *root) {
    if (root == NULL)
        return;

    std::vector<node*> et;
    RBtree* rb_tree = new RBtree;

    euler_tour(root, et);
    for (int i = 0; i < et.size(); i++) {
        et[i]->val = i;
        rb_tree->insert(et[i]);
    }

    ET_forestNode *fn = new ET_forestNode;
    fn->euler_path = et;
    fn->rb_tree = rb_tree;
    fn->orig_root = root;

    forest[id] = fn;
}

int main ()
{
    EulerTourTree *et = new EulerTourTree;
    ET_Node* n = new ET_Node;
    n->val = 1;
    ET_Node* c1 = new ET_Node;
    c1->val = 3;
    n->children = c1;
    n->sibling = NULL;
    c1->children = NULL;
    c1->sibling = NULL;

    ET_Node* c2 = new ET_Node;
    c2->val = 4;
    c2->children = NULL;


    ET_Node* c3 = new ET_Node;
    c3->val = 8;
    c3->children = NULL;

    c1->sibling = c2;
    c2->sibling = c3;
    c3->sibling = NULL;

    ET_Node* b = new ET_Node;
    b->sibling = NULL;
    b->val = 99;
    ET_Node* d1 = new ET_Node;
    d1->children = NULL;
    d1->val = 21;
    b->children = d1;


    et->add_tree("a",n);
    et->add_tree("b", b);
    bool con = et->connected(n,b);

    et->link(1, 99, "a", "b");
    con = et->connected(n,b);
    if (con)
        cout << "yeah" << endl;
    et->cut(n, b, "a", "b");
    con = et->connected(n,b);
    if (con)
        cout << "yeah" << endl;
    return 0;
}
