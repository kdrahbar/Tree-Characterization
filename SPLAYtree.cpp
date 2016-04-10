#include <iostream>

struct SPLAYnode
{
    int key;
    SPLAYnode *left;
    SPLAYnode * right;
};

class SPLAYtree {
  public:
  	SPLAYtree();

	SPLAYnode * insert(int);
	SPLAYnode * search(int);

	void printPreorder();

  private:
  	SPLAYnode * root;

  	SPLAYnode * insert(SPLAYnode *, int);
	SPLAYnode * search(SPLAYnode *, int);

	//printing
	void printPreorder(SPLAYnode *);


  	SPLAYnode * newNode(int);
  	SPLAYnode * rightRotate(SPLAYnode *);
	SPLAYnode * leftRotate(SPLAYnode *);
	SPLAYnode * splay(SPLAYnode *, int);
};

SPLAYtree::SPLAYtree() {
	root = NULL;
	return;
}

SPLAYnode * SPLAYtree::newNode(int key) {
	SPLAYnode* node = new SPLAYnode;
    node->key   = key;
    node->left  = NULL;
    node->right = NULL;
    return (node);
}

SPLAYnode * SPLAYtree::rightRotate(SPLAYnode * x) {
	SPLAYnode * y = x->left;
	x->left = y->right;
	y->right = x;
	return y;
}

SPLAYnode * SPLAYtree::leftRotate(SPLAYnode * x)
{
    SPLAYnode *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

SPLAYnode * SPLAYtree::splay(SPLAYnode *node, int key)
{
    // Base cases: node is NULL or key is present at node
    if (node == NULL || node->key == key)
        return node;
 
    // Key lies in left subtree
    if (node->key > key)
    {
        // Key is not in tree, we are done
        if (node->left == NULL) return node;
 
        // Zig-Zig (Left Left)
        if (node->left->key > key)
        {
            // First recursively bring the key as node of left-left
            node->left->left = splay(node->left->left, key);
 
            // Do first rotation for node, second rotation is done after else
            node = rightRotate(node);
        }
        else if (node->left->key < key) // Zig-Zag (Left Right)
        {
            // First recursively bring the key as node of left-right
            node->left->right = splay(node->left->right, key);
 
            // Do first rotation for node->left
            if (node->left->right != NULL)
                node->left = leftRotate(node->left);
        }
 
        // Do second rotation for node
        return (node->left == NULL)? node: rightRotate(node);
    }
    else // Key lies in right subtree
    {
        // Key is not in tree, we are done
        if (node->right == NULL) return node;
 
        // Zag-Zig (Right Left)
        if (node->right->key > key)
        {
            // Bring the key as node of right-left
            node->right->left = splay(node->right->left, key);
 
            // Do first rotation for node->right
            if (node->right->left != NULL)
                node->right = rightRotate(node->right);
        }
        else if (node->right->key < key)// Zag-Zag (Right Right)
        {
            // Bring the key as node of right-right and do first rotation
            node->right->right = splay(node->right->right, key);
            node = leftRotate(node);
        }
 
        // Do second rotation for node
        return (node->right == NULL) ? node: leftRotate(node);
    }
}

SPLAYnode * SPLAYtree::search(SPLAYnode * node, int key) {
	return splay(node, key);
}

SPLAYnode * SPLAYtree::insert(SPLAYnode * node, int key)
{
    // Simple Case: If tree is empty
    if (node == NULL) return newNode(key);
 
    // Bring the closest leaf node to node
    node = splay(node, key);
 
    // If keyey is already present, then return
    if (node->key == key) return node;
 
    // Otherwise allocate memory for new node
    	SPLAYnode *newnode  = newNode(key);
 
    // If node's keyey is greater, makeye node as right child
    // of newnode and copy the left child of node to newnode
    if (node->key > key)
    {
        newnode->right = node;
        newnode->left = node->left;
        node->left = NULL;
    }
 
    // If node's keyey is smaller, makeye node as left child
    // of newnode and copy the right child of node to newnode
    else
    {
        newnode->left = node;
        newnode->right = node->right;
        node->right = NULL;
    }
 
    return newnode; // newnode becomes new node
}

SPLAYnode * SPLAYtree::insert(int key){
	root = insert(root, key);
	return root;
}

void SPLAYtree::printPreorder(SPLAYnode * node)
{
    if (node != NULL)
    {
        std::cout << node->key << std::endl;
        printPreorder(node->left);
        printPreorder(node->right);
    }
}

void SPLAYtree::printPreorder() {
	printPreorder(root);
}


int main() {

	SPLAYtree tree;
	tree.insert(10);
	tree.insert(20);
		
	tree.insert(30);
	tree.insert(40);
	tree.printPreorder();
	return 0;
}
