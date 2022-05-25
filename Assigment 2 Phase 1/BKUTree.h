#include <iostream>
#include <cstdlib>
#include <queue>
#include <string>
#include <vector>
#include <stdexcept>
#include <math.h>
using namespace std;

template <class K, class V>
class BKUTree {
public:
    class AVLTree;
    class SplayTree;

    class Entry {
    public:
        K key;
        V value;
        Entry(K key, V value) : key(key), value(value) {}
    };

private:
    AVLTree* avl;
    SplayTree* splay;
    queue<K> keys;
    int maxNumOfKeys;

public:
    BKUTree(int maxNumOfKeys = 5) {
		this->avl= new AVLTree;
		this->splay= new SplayTree;
		this->maxNumOfKeys= maxNumOfKeys;
	}
    ~BKUTree(){
		this->clear(); 
		delete this->avl;
		delete this->splay;
		this->avl=NULL;
		this->splay=NULL; 
	}

    void add(K key, V value);
    void remove(K key);
    V search(K key, vector<K>& traversedList);

    void traverseNLROnAVL(void (*func)(K key, V value)){
    	this->avl->traverseNLR(func);
	};
    void traverseNLROnSplay(void (*func)(K key, V value)){
    	this->splay->traverseNLR(func);
	};

    void clear();

    class SplayTree {
    public:
        class Node {
            public:
            Entry* entry;
            Node* left;
            Node* right;
            Node* parent;
            typename AVLTree::Node* corr;

            Node(Entry* entry = NULL, Node* parent= NULL, Node* left = NULL, Node* right = NULL) {
                this->entry = entry;
                this->parent = parent;
                this->left = left;
                this->right = right;
                this->corr = NULL;
            }
        };
	
    public: //helping functions
    	void split(Node* &x, Node* &s, Node* &t);
    	void deleteNodeHelper(Node* node, K key);
    	Node* join(Node* s, Node* t);
    	Node* maximum(Node* node);
    	void Splay(Node* &node, bool opt= false);
        Node* zig(Node* node);
        Node* zag(Node* node);
        Node* zig_zig(Node* node);
        Node* zag_zag(Node* node);
        Node* zig_zag(Node* node);
        Node* zag_zig(Node* node);
		void add_BKU(K key, V value, Node* &BKU_node);
    	
    public:
        Node* root;

        SplayTree(): root(NULL) {};
        ~SplayTree() { this->clear(); };
        
        void add(K key, V value);
        void add(Entry* entry);
        void remove(K key);
        Node* find_node(K key, vector<K>& traversedList);
        V search(K key);

        void traverseNLR(void (*func)(K key, V value)){
        	printPreorder(root, func);
		}
		
		void printPreorder(Node* node, void(*func)(K key, V value))
		{
			if (node == NULL)
				return;
			func(node->entry->key, node->entry->value);
			printPreorder(node->left,func);
			printPreorder(node->right,func);
		}
		
        void clear();
    };

    class AVLTree {
    public:
        class Node {
        public:
            Entry* entry;
            Node* left;
            Node* right;
            int balance; 
            typename SplayTree::Node* corr;

            Node(Entry* entry = NULL, Node* left = NULL, Node* right = NULL) {
                this->entry = entry;
                this->left = left;
                this->right = right;
                this->balance = 0;
                this->corr = NULL;
            }
        };

	public:
		Node* add_rec(Node* root, K key, V value, Node* &BKU_node);
		
	private:
		int getBalance(Node* root);
		int getHeightRec(Node* root);
		Node* leftRotate(Node* root);
		Node* rightRotate(Node* root);
		Node* deleteNode(Node* root, K key);
		Node* maxValueNode(Node* node);
		
    public:
        Node* root;

        AVLTree(): root(NULL) {};
        ~AVLTree() { this->clear(); };
        
        void add(K key, V value);
        void add(Entry* entry);
        void remove(K key);
        Node* find_node(K key, vector<K>& traversedList, Node* splay_root= NULL, Node* break_node= NULL);
        V search(K key);

		void printPreorder(Node* node, void(*func)(K key, V value))
		{
			if (node == NULL)
				return;
			func(node->entry->key, node->entry->value);
			printPreorder(node->left,func);
			printPreorder(node->right,func);
		}
		
        void traverseNLR(void (*func)(K key, V value)){
        	printPreorder(root, func);
		}

        void clear();
    };
};

// ==================================================== BKU TREE FUNCTIONS ==================================================== // 

template<class K, class V>
void BKUTree<K, V>::add(K key, V value){
	typename SplayTree::Node* add_spl_node= new typename BKUTree<K, V>::SplayTree::Node(new Entry(key, value));
    typename AVLTree::Node* add_avl_node= new typename BKUTree<K, V>::AVLTree::Node(new Entry(key, value));
    try{
    	this->avl->root=this->avl->add_rec(this->avl->root, key,value,add_avl_node);	
		  this->splay->add_BKU(key,value,add_spl_node);
	}
	catch(const char* ex){
		string error= ex;
		if( error == "Duplicate key"){
			delete	add_avl_node;
			delete	add_spl_node;
			throw	"Duplicate key";
		}
	}
	add_avl_node->corr=add_spl_node;
    add_spl_node->corr=add_avl_node;
    if((int)this->keys.size()== this->maxNumOfKeys)	this->keys.pop();
	this->keys.push(key);
}

template<class K, class V>
void BKUTree<K, V>::remove(K key){
	this->avl->remove(key);
	this->splay->remove(key);
	if(this->splay->root != NULL){
		for(unsigned int i=0; i<this->keys.size(); i++){
			if(this->keys.front() == key){
				this->keys.pop();
				i--;
			}	
			else{
				this->keys.push(this->keys.front());
				this->keys.pop();
			}
		}
		if((int)this->keys.size() == this->maxNumOfKeys)	this->keys.pop();
		this->keys.push(this->splay->root->entry->key);
	}
	else{
		while((int)this->keys.size() != 0)	this->keys.pop();
	}
}

template<class K, class V>
V BKUTree<K, V>::search(K key, vector<K>& traversedList){
	if(key == this->splay->root->entry->key){
		if((int)this->keys.size() == this->maxNumOfKeys)	this->keys.pop();
		this->keys.push(this->splay->root->entry->key);	
		return this->splay->root->entry->value;
	}	
	else{
		bool is_in=0;
		for(unsigned int i=0; i< this->keys.size(); i++){
			if(this->keys.front() == key) is_in=1;
				this->keys.push(this->keys.front());
				this->keys.pop();
		}
		typename BKUTree<K, V>::AVLTree::Node* avl_f_node= NULL;
		typename BKUTree<K, V>::SplayTree::Node* splay_f_node= NULL;
		if(!is_in){
			try{
				avl_f_node= this->avl->find_node(key, traversedList, this->splay->root->corr, NULL);	
			}
			catch(const char* ex){
				string error =ex;
				if(error == "Not found"){
					avl_f_node= this->splay->root->corr;
					avl_f_node= this->avl->find_node(key, traversedList, NULL, avl_f_node);
				}
			}
			// one time splay
			this->splay->Splay(avl_f_node->corr, 1);
			// push queue
			if((int) this->keys.size() == this->maxNumOfKeys)	this->keys.pop();
			this->keys.push(avl_f_node->entry->key);
			return avl_f_node->entry->value;
		}
		else{
			splay_f_node= this->splay->find_node(key, traversedList);
			this->splay->Splay(splay_f_node, 1);
			if((int) this->keys.size() == this->maxNumOfKeys)	this->keys.pop();
			this->keys.push(key);
			return splay_f_node->entry->value;
		}
	}
}

template<class K, class V>
void BKUTree<K, V>::clear(){
	while(this->avl->root!= NULL && this->splay->root!=NULL){
		this->remove(this->avl->root->entry->key);
	}
	while(!this->keys.size())	this->keys.pop();
}

// ==================================================== SPLAY TREE FUNCTIONS =============================================== //

// =========== ZIG - ZAG functions =========== //

template<class K, class V>
void BKUTree<K,V>::SplayTree::Splay(Node* &node, bool opt){
	while (node->parent != NULL)
    {
        Node *parent = node->parent;
        Node *grand_parent = parent->parent;
        if (grand_parent == NULL){
        	if(parent->left == node)	zig(node);
			else	zag(node);
		}
        else{
        	if (grand_parent->left == parent && parent->left == node) zig_zig(node);
			else{
				if (grand_parent->right == parent && parent->right == node) zag_zag(node);
        		else{
        			if(grand_parent->left == parent && parent->right == node)	zig_zag(node);
        			else	zag_zig(node);
				}
			} 		
		}
		if(opt)	break;
    }
    if(node->parent == NULL || opt == 0)	this->root = node;
}

template<class K, class V>
typename BKUTree<K,V>::SplayTree::Node* BKUTree<K,V>::SplayTree::zig(Node* node){
    Node* parent = node->parent;    // parent of node

    Node * r_sub_node = node->right;// right sub tree(node) of node
    node->parent = NULL;
    node->right = parent;
        
    parent->parent = node;
    parent->left = r_sub_node;
        
    if (r_sub_node != NULL) r_sub_node->parent = parent;
    return node;
}

template<class K, class V>
typename BKUTree<K,V>::SplayTree::Node* BKUTree<K,V>::SplayTree::zag(Node* node){
	
	Node* parent = node->parent;    // parent of node

    Node* l_sub_node = node->left; // left sub tree(node) of node
    node->parent = NULL;
    node->left = parent;
        
    parent->parent = node;
    parent->right = l_sub_node;
        
    if (l_sub_node != NULL) l_sub_node->parent = parent;
    return node;
}

template<class K, class V>
typename BKUTree<K,V>::SplayTree::Node* BKUTree<K,V>::SplayTree::zig_zig(Node* node){
    Node* parent = node->parent;
    Node* grand_parent = parent->parent;

    Node* r_sub_node = node->right;
    Node* p_r_sub_node = parent->right;
        
    node->parent = grand_parent->parent;
    node->right = parent;
        
    parent->parent = node;
    parent->left = r_sub_node;
    parent->right = grand_parent;
        
    grand_parent->parent = parent;
    grand_parent->left = p_r_sub_node;

    if ( node->parent != NULL)
        {
            if (node->parent->left == grand_parent) node->parent->left = node;
            else node->parent->right = node;
        }
        
    if (r_sub_node != NULL) r_sub_node->parent = parent;
        
    if (p_r_sub_node != NULL) p_r_sub_node->parent = grand_parent;

    return node;
}

template<class K, class V>
typename BKUTree<K,V>::SplayTree::Node* BKUTree<K,V>::SplayTree::zag_zag(Node* node){
    Node* parent = node->parent;
    Node* grand_parent = parent->parent;

    Node* l_sub_node = node->left;
    Node* p_l_sub_node = parent->left;
        
    node->parent = grand_parent->parent;
    node->left = parent;
        
    parent->parent = node;
    parent->right = l_sub_node;
    parent->left = grand_parent;
        
    grand_parent->parent = parent;
    grand_parent->right = p_l_sub_node;

    if ( node->parent != NULL)
        {
            if (node->parent->left == grand_parent) node->parent->left = node;
            else node->parent->right = node;
        }
        
    if (l_sub_node != NULL) l_sub_node->parent = parent;
        
    if (p_l_sub_node != NULL) p_l_sub_node->parent = grand_parent;

    return node;
}

template<class K, class V>
typename BKUTree<K,V>::SplayTree::Node* BKUTree<K,V>::SplayTree::zig_zag(Node* node){
    Node *parent = node->parent;
    Node *grand_parent = parent->parent;

    Node *l_sub_node = node->left;
    Node *r_sub_node = node->right;
        
    node->parent = grand_parent->parent;
    node->left = parent;
    node->right = grand_parent;
        
    parent->parent = node;
    parent->right = l_sub_node;
        
    grand_parent->parent = node;
    grand_parent->left = r_sub_node;
        
    if (node->parent != NULL)
    {
        if (node->parent->left == grand_parent) node->parent->left = node;
        else node->parent->right = node;
    }
        
    if (l_sub_node != NULL) l_sub_node->parent = parent;
        
    if (r_sub_node != NULL) r_sub_node->parent = grand_parent;

    return node;
}

template<class K, class V>
typename BKUTree<K,V>::SplayTree::Node* BKUTree<K,V>::SplayTree::zag_zig(Node* node){
    Node *parent = node->parent;
    Node *grand_parent = parent->parent;

    Node *l_sub_node = node->left;
    Node *r_sub_node = node->right;
        
    node->parent = grand_parent->parent;
    node->left = grand_parent;
    node->right = parent;
        
    parent->parent = node;
    parent->left = r_sub_node;
        
    grand_parent->parent = node;
    grand_parent->right = l_sub_node;
        
    if (node->parent != NULL)
    {
        if (node->parent->left == grand_parent) node->parent->left = node;
        else node->parent->right = node;
    }
        
    if (l_sub_node != NULL) l_sub_node->parent = grand_parent;
        
    if (r_sub_node != NULL) r_sub_node->parent = parent;
    
    return node;
}

// =========== ADD functions =========== //

template<class K, class V>
void BKUTree<K, V>::SplayTree::add_BKU(K key, V value, Node* &BKU_node){
	Node* t_root= this->root;
    Node* parent= NULL;
    while(t_root != NULL){
        parent= t_root;
        if(key == t_root->entry->key)    throw "Duplicate key";
        else{
            if(key > t_root->entry->key)    t_root = t_root->right;
            else t_root = t_root->left;
        }
    }
    if(parent != NULL){
        BKU_node->parent= parent;
        if(parent->entry->key < key) parent->right= BKU_node;
        else parent->left= BKU_node;
		Splay(BKU_node);
    }
    else this->root= BKU_node;
}

template <class K, class V>
void BKUTree<K, V>::SplayTree::add(K key, V value){
    
    // ADD entry to Splay tree
    Node* t_root= this->root;
    Node* parent= NULL;
    while(t_root != NULL){
        parent= t_root;
        if(key == t_root->entry->key)    throw "Duplicate key";
        else{
            if(key > t_root->entry->key)    t_root = t_root->right;
            else t_root = t_root->left;
        }
    }
    if(parent != NULL){
        Node* add_node= new Node(new Entry(key, value), parent);
        if(parent->entry->key < key) parent->right= add_node;
        else parent->left= add_node;
		Splay(add_node);
    }
    else this->root= new Node(new Entry(key, value));
}

template <class K, class V>
void BKUTree<K, V>::SplayTree::add(Entry* entry){
	this->add(entry->key, entry->value);
}

// =========== DELETE functions =========== //

template<class K, class V>
void BKUTree<K, V>::SplayTree::split(Node* &x, Node* &s, Node* &t){
    Splay(x, 0);
	if (x->right) {
		t = x->right;
		t->parent = NULL;
	} 
    else {
		t = NULL;
	}
	s = x;
	s->right = NULL;
	x = NULL;
}

template<class K, class V>
void BKUTree<K, V>::SplayTree::deleteNodeHelper(Node* node, K key){
    Node* x = NULL;
	Node* t, *s;
	while(node != NULL){
		if(node->entry->key == key){
			x = node;
		}
		if(node->entry->key < key){
			node = node->right;
		} 
		else{
			node = node->left;
		}
	}
	if(x == NULL){
		throw "Not found" ;
	}
	split(x, s, t); // split the tree
	if (s->left){ // remove x
		s->left->parent = NULL;
	}
	root = join(s->left, t);
	delete(s);
	s = NULL;
}


template<class K, class V>
typename BKUTree<K, V>::SplayTree::Node* BKUTree<K, V>::SplayTree::join(Node* s, Node* t){
    if (!s) {
		return t;
	}
	if (!t) {
		return s;
	}
	Node* x = maximum(s);
	Splay(x);
	x->right = t;
	t->parent = x;
	return x;	
}

template<class K, class V>
typename BKUTree<K, V>::SplayTree::Node* BKUTree<K, V>::SplayTree::maximum(Node* node){
    while (node->right != NULL) {
		node = node->right;
	}
	return node;
}

template <class K, class V>
void BKUTree<K, V>::SplayTree::remove(K key){
	Node* node= this->root;
	while(node!=NULL){
		if(node->entry->key == key)	break;
		else{
			if(node->entry->key > key)	node= node->left;
			else	node=node->right;
		}
	}
	if(!node)	throw "Not found";
	else this->deleteNodeHelper(node, key);
}

// =========== SEARCH functions =========== //

template<class K, class V>
typename BKUTree<K, V>::SplayTree::Node* BKUTree<K, V>::SplayTree::find_node(K key, vector<K>& traversedList){
	Node* find_node= this->root;
	while(find_node!= NULL){
		if(find_node->entry->key == key)	return find_node;
		else{
			traversedList.push_back(find_node->entry->key);
			if(find_node->entry->key < key)	find_node= find_node->right;
			else	find_node= find_node->left;
		}
	}
	throw "Not found";
}

template <class K, class V>
V BKUTree<K, V>::SplayTree::search(K key){
	Node* find_node= this->root;
	while(find_node!=NULL){
		if(find_node->entry->key == key){
			Splay(find_node, 0);
			return find_node->entry->value;
		}	
		else{
			if(find_node->entry->key > key)	find_node= find_node->left;
			else find_node= find_node->right;
		}
	}
	throw "Not found";
}

// ============ CLEAR FUNCTION =========== //

template <class K, class V>
void BKUTree<K, V>::SplayTree::clear(){
	while(this->root != NULL)	this->remove(this->root->entry->key);
}

// ==================================================== SPLAY TREE END ====================================================  //


// ==================================================== AVL TREE FUNCTIONS =============================================== //

// ================ ROTATION OF AVL TREE ================ //
template <class K, class V>
typename BKUTree<K,V>::AVLTree::Node* BKUTree<K,V>::AVLTree::leftRotate(Node* root){
	Node* r_root = root->right;
    root->right = r_root->left;
    r_root->left = root;
    return r_root;
}

template <class K, class V>
typename BKUTree<K,V>::AVLTree::Node* BKUTree<K,V>::AVLTree::rightRotate(Node* root){
    Node* l_root = root->left;
    root->left = l_root->right;
    l_root->right = root;
    return l_root;
}

// ================ HEIGHT OF AVL TREE + BALANCE OF AVL TREE ================ //

template<class K, class V>
int BKUTree<K,V>::AVLTree::getHeightRec(Node* root){
	if (root == NULL)
        return 0;
    int left_h = this->getHeightRec(root->left);
    int right_h = this->getHeightRec(root->right);
    return (left_h > right_h ? left_h : right_h) + 1;
}

template<class K, class V>
int BKUTree<K,V>::AVLTree::getBalance(Node* root){
	if(root == NULL)	return 0;
	return  this->getHeightRec(root->right) - this->getHeightRec(root->left);
}

// ================ AVL TREE ADD FUNCTIONS ================ //

template <class K, class V>
typename BKUTree<K,V>::AVLTree::Node* BKUTree<K,V>::AVLTree::add_rec(Node* root, K key, V value, Node* &node){
	if(root == NULL){
		return node;
	}	
	if(key < root->entry->key)	root->left= this->add_rec(root->left, key, value, node);
	else{
		if(key == root->entry->key)	throw "Duplicate key";
		else	root->right= this->add_rec(root->right, key, value, node);
	}

	int balance= getBalance(root);
	
	if(balance==-1){
		root->balance=-1;
		return root;
	}	
	if(balance==1){
		root->balance=1;
		return root;
	}	
	
	// Left Left Case  
    if (balance < -1 && key < root->left->entry->key){
    	root->balance=0;
    	root->left->balance=0;
		return rightRotate(root);
	}
    
    // Right Right Case  
    if (balance > 1 && key > root->right->entry->key){
		root->balance=0;
    	root->right->balance=0;
		return leftRotate(root);
	}
		
    // Left Right Case  
    if (balance < -1 && key > root->left->entry->key){
        root->left->balance=0;
		root->left = leftRotate(root->left);
		root->balance=0;
    	root->left->balance=0;
        return rightRotate(root);
    }
    
    // Right Left Case  
    if (balance > 1 && key < root->right->entry->key)
    {
    	root->right->balance=0;
    	root->right = rightRotate(root->right);
    	root->balance=0;
    	root->right->balance=0;
    	return leftRotate(root);
    }
    return root;
}

template <class K, class V>
void BKUTree<K,V>::AVLTree::add(K key, V value){
	BKUTree<K, V>::AVLTree::Node* add_node= new BKUTree<K, V>::AVLTree::Node(new Entry(key, value));
	try{
		this->root=this->add_rec(this->root, key, value, add_node);
	}
	catch(const char* ex){
	    string error =ex;
		if( error == "Duplicate key"){
			delete	add_node;
			throw	"Duplicate key";
		}
	}
}

template <class K, class V>
void BKUTree<K,V>::AVLTree::add(Entry* entry){
    this->add(entry->key, entry->value);
}

// ================ AVL TREE DELETE FUNCTIONS ================ //

template <class K, class V>
typename BKUTree<K,V>::AVLTree::Node* BKUTree<K,V>::AVLTree::maxValueNode(Node* root){
    Node* curr = root;
    while (curr->right != NULL){
    	if(curr->right->right==NULL){
    		if(curr->left!=NULL)	curr->balance=-1;
    		else	curr->balance=0;
		}
		curr = curr->right;
	}
    return curr;
}

template <class K, class V>
typename BKUTree<K,V>::AVLTree::Node* BKUTree<K,V>::AVLTree::deleteNode(Node* root, K key){
	if (root == NULL)
    	return root;  
    if (key < root->entry->key)
    	root->left = deleteNode(root->left, key);  
    else{
    	if (key > root->entry->key)	root->right = deleteNode(root->right, key);	
		else // tim thay key
        {
            // Node co 1 la hoac ko co la nao 
            if ((root->left == NULL) || (root->right == NULL))
            {
				      Node* temp= root->left ?root->left :root->right;
                // TH1: ko co la nao
                if (temp == NULL){
                    temp = root;
                    root = NULL;
                }
                else{	// TH2: co 1 la ( trai hoac phai)
                    *root = *temp;
					if(root->corr!=NULL)	root->corr->corr= root;
				} // sao chep noi dung cua la 
            }
            else // Node la 1 cay
            {
            	Node* temp = maxValueNode(root->left);
				root->entry = temp->entry;
				root->corr = temp->corr;
				if(root->corr !=NULL)	root->corr->corr = root;
                root->left = deleteNode(root->left,temp->entry->key);
            }
        }
	}
        
    if (root == NULL)
		return root;
        
        
	// kiem tra balance
    int balance = getBalance(root);
	
	if(balance==-1){
		root->balance=-1;
		return root;
	}	
	if(balance==1){
		root->balance=1;
		return root;
	}	
	
	// Left Left Case  
    if (balance < -1 && getBalance(root->left)<=0){
    	root->balance=0;
    	root->left->balance=0;
		return rightRotate(root);
	}
    
    // Right Right Case  
    if (balance > 1 && getBalance(root->right)>=0){
		root->balance=0;
    	root->right->balance=0;
		return leftRotate(root);
	}
		
    // Left Right Case  
    if (balance < -1 && getBalance(root->left)>0){
        root->left->balance=-1;
		root->left = leftRotate(root->left);
		root->balance=0;
    	root->left->balance=0;
        return rightRotate(root);
    }
    
    // Right Left Case  
    if (balance > 1 && key < getBalance(root->right)<0)
    {
    	root->right->balance=1;
    	root->right = rightRotate(root->right);
    	root->balance=0;
    	root->right->balance=0;
    	return leftRotate(root);
    }
    return root;
}

template <class K, class V>
void BKUTree<K,V>::AVLTree::remove(K key){
	Node* t_root= this->root;
	while(t_root!=NULL){
		if(t_root->entry->key > key)	t_root=t_root->left;
		else{
			if(t_root->entry->key == key)	break;
			else	t_root= t_root->right;
		}
	}
	if(t_root == NULL)	throw	"Not found";
	else	this->root= deleteNode(root, key);
}

// ================ AVL SEARCH FUNCTIONS ================ //

template<class K, class V>
typename BKUTree<K, V>::AVLTree::Node* BKUTree<K, V>::AVLTree::find_node(K key, vector<K>& traversedList, Node* splay_root, Node* break_node){
	Node* curr= NULL;
	if(splay_root != NULL)	curr= splay_root;
	else	curr= this->root;
	while(curr!=NULL){
		if(break_node !=NULL && curr->entry->key == break_node->entry->key)	throw "Not found";
		if(key == curr->entry->key)	return curr;
		else{
			traversedList.push_back(curr->entry->key);
			if(key > curr->entry->key)	curr=curr->right;
    		else	curr=curr->left;
		}
	}
	throw "Not found";
}

template <class K, class V>
V BKUTree<K,V>::AVLTree::search(K key){
	if (this->root == NULL) throw "Not found";
	else {
		BKUTree<K,V>::AVLTree::Node* temp = this->root;
		while (temp!= NULL) {
			if (temp->entry->key == key) return temp->entry->value;
			else {
				if (temp->entry->key < key) {
					temp = temp->right;
				}
				else temp = temp->left;
			}
		}

		throw "Not found";
	}
}

template <class K, class V>
void BKUTree<K,V>::AVLTree::clear(){
    while(this->root!=NULL)
    	this->remove(this->root->entry->key);
}

// ================================================== END AVL TREE ============================================== //