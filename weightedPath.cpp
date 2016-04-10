#include <iostream>
#include <string>

struct WplRec {
    WplRec * left;   //WplPtr
    int * item;        //key?
    int WT;           //Weight associated with node
    WplRec * right;  //WplPtr
};

class WplTree {

  public:
    WPTtree();


  private:
    WplRec * root;


    //Helper Functions
    struct WplRec* newNode(int); //NOT SURE

    //Defines an ordering of ItemPtrs
    //Returns true if a < b
    bool lessThan(int * a, int * b);


    //TODO:: Not sure if fxn returns void or ptr
    //Constructor NewWplPtr creates a new ptr given struct values
    WplRec * NewWplPtr(WplRec * l, int* i, int WT, WplRec * r);

    //Returns wt for any given WplPtr
    int wt (WplRec * t);


    //Performs a simple right rotation on it's argument
    void rightRotate (WplRec * t);

}

WplTree::WplTree() {
    root = NULL;
}

WplRec * WplTree::NewWplPtr(WplRec * l, int* i, int WT, WplRec * r) {
    WplRec * newNode = new WplRec;
    newNode->left = l;
    newNode->item = i;
    newNode->WT = WT;
    newNode->right = r;
    return newNode;
}

bool WplTree::lessThan(int * a, int * b){
    if (*a)->item < (*b)->item) {
        retun true;
    }
    return false;
}

int WplTree::wt (WplRec * t) {
    int wt = (*t)->WT - (*(*t)->left)->WT) - (*(*t)->right)->WT;
    return wt;
}

void WplTree::rightRotate(WplRec * t) {
    WplTree * right = NewWplPtr( ((*t)->left)->right,
                                 (*t)->item,
                                 wt(t),
                                 (*t)->right );
    t = NewWplPtr(  ((*t)->left)->left,
                    ((*t)->left)->item,
                    wt( (*t)->left ),
                    right );
    return;
}
