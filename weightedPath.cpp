#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

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
    void remove(int);
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

    WplRec * findMin(WplRec * t);
    WplRec* remove(int x, WplRec* t);


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
    std::cout << "root change" << std::endl;
    root = t->left;
  }

  int totalWT = t->WT;
  WplRec * tmp = t;
  t = t->left;


  if (tmp->left->right == NULL) {tmp->WT = tmp->WT - tmp->left->WT;}
  else {tmp->WT = tmp->WT - tmp->left->WT + tmp->left->right->WT;}

  tmp->left = tmp->left->right;

  t->right = tmp;
  t->WT = totalWT;
  if(parent!=NULL && parent->item>t->item) parent->left = t;
  else if(parent!=NULL) parent->right = t;

  //root = t;
  return;
}

/*
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
*/



void WplTree::leftRotate(WplRec * t, WplRec * parent) {
  //std::cout << "inLeft"<< t->item << std::endl;

  if(parent == NULL) {
    std::cout << "root change" << std::endl;
    root = t->right;
  }
  int totalWT = t->WT;
  WplRec * tmp = t;
  t = t->right;

  if (tmp->right->left == NULL) {tmp->WT = tmp->WT - tmp->right->WT; }
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

void WplTree::checkRightRotation (WplRec * t, WplRec * parent) {
  if (t->left != NULL && t->left->right != NULL) {
    if (2*t->left->WT > t->left->right->WT+t->WT) {
    //if (t->left->right == NULL || 2*t->left->WT > t->left->right->WT+t->WT) {
      //fstd::cout << "checkRightRotation is TRUE: "<< t->item << std::endl;
      rightRotate(t, parent);
      if (t->right!=NULL) checkLeftRotation (t->right, parent);
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

      if (t->left!=NULL) checkRightRotation (t->left, parent);
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

WplRec* WplTree::remove(int x, WplRec* t) {
  WplRec* temp;
  if(t == NULL)
      return NULL;
  else if(x < t->item)
      t->left = remove(x, t->left);
  else if(x > t->item)
      t->right = remove(x, t->right);
  else if(t->left && t->right)
  {
      temp = findMin(t->right);
      t->item = temp->item;
      t->right = remove(t->item, t->right);
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

void WplTree::remove(int x) {
    root = remove(x, root);
}



void test50() {
  std::srand(unsigned(std::time(0)));
  WplTree t;

  //int test [50];
  int pool [1000];
  for (int i = 0; i < 1000; i++) {
      pool[i] = i;
  }
  std::random_shuffle(std::begin(pool), std::end(pool));
  for (int i=0; i<1000; ++i ) {

    std::cout << pool[i] << " ";
    //std::cout <<t.root<<std::endl;
  }
  std::cout << std::endl;

  for (int i=0; i<100; i++ ) {
    //std::cout << i+1 << std::endl;
    t.insert(pool[i]);
    //std::cout <<"root: " <<t.root->item<<std::endl;
    //t.printPreorder(); std::cout << std::endl;
    std::cout <<t.root->item<<std::endl;
  }
  //t.printInorder();
}

int main() {

  WplTree t;

  //test50();

/*
  t.testInsert(4, 1);
  t.testInsert(2, 1);
  t.testInsert(3, 1);
  t.printPreorder();
  std::cout << "root: "<<t.root->item << std::endl;
  std::cout << std::endl;


  t.leftRotate(t.root->left, t.root);
  std::cout << "root: "<<t.root->item << std::endl;

  t.printPreorder();
  std::cout << std::endl;
  */
  //test50();

  t.insert(28);
  t.printPreorder(); std::cout << std::endl;

  t.insert(47);
  t.printPreorder(); std::cout << std::endl;

  t.insert(46);
  t.printPreorder(); std::cout << std::endl;
  t.insert(46);
  t.printPreorder(); std::cout << std::endl;
  t.remove(46);
  t.printPreorder(); std::cout << std::endl;
  return 0;
}
