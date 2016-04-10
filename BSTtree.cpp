#include <iostream>
#include <string>

struct BSTnode{
    int key;
    struct BSTnode * left;
    struct BSTnode * right;
};

class BSTtree{
  public:
    BSTtree();
    struct BSTnode * insert(int );
    void printInorder();

  private:
    BSTnode * root;

    struct BSTnode * newNode(int);
    void printInorder(struct BSTnode *);

    struct BSTnode * insert(struct BSTnode*, int );

};

BSTtree::BSTtree() {
    root = NULL;
    return;
}

struct BSTnode * BSTtree::newNode(int key) {
    struct BSTnode * temp = new BSTnode;
    temp->key = key;
    temp->left = temp->right = NULL;
    return temp;
}

void BSTtree::printInorder(struct BSTnode * leaf) {
    if (leaf != NULL) {
        printInorder(leaf->left);
        std::cout << leaf->key << std::endl;
        printInorder(leaf->right);
    }
    return;
}

struct BSTnode* BSTtree::insert (struct BSTnode* node, int key) {
    if(node == NULL)
        return newNode(key);

    if(key<node->key)
        node->left = insert(node->left, key);
    else //key >= node->key
        node->right = insert(node->right, key);
    return node;
}

struct BSTnode * BSTtree::insert(int key) {
    root = insert(root, key);
    return root;
}

void BSTtree::printInorder() {
    printInorder(root);
    return;
}

int main() {

    BSTtree tree;
    tree.insert(10);
    tree.insert(40);
    tree.insert(20);
    tree.insert(30);
    tree.insert(50);
    tree.insert(10);
   
    tree.printInorder(); 



    return 0;
}







