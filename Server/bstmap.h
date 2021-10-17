#ifndef _BST_H_
#define _BST_H_

#include <cassert>
#include <iostream>
#include <string>
#include "bstaux.h"

using namespace std;

/*
  An associative container (map/dict) using a binary search tree.

  Assumes:
    - K is totally ordered and can be compared via <
*/

template <typename K, typename T>
class BSTMap {
public:
	// creates an empty BSTMap with 0 items
	BSTMap();

	// deletes all nodes in the BSTMap
	~BSTMap();

	// add the item with the given key, replacing
	// the old item at that key if the key already exists
	void update(const K& key, const T& item);

	//returns the min score 
	int minScore() const;

	//returns name of min score
	string minName() const;

	// remove the key and its associated item
	void remove(const K& key);

	//return false if tree is skewed
	bool isBalanced() const;

	// returns true iff the key exists
	bool hasKey(const K& key) const;

	// access the item at the given key, allows assignment
	// as an l-value, eg. tree["Zac"] = 20;
	// where tree is an instance of BSTMap<string, int>
	T& operator[](const K& key);

	// does not create the entry if it does not exist
	const T& at(const K& key) const;

	// returns the size of the tree
	unsigned int size() const;

	// returns an iterator to the first item (ordered by key)
	BSTIterator<K,T> begin() const;

	// returns an iterator signalling the end iterator
	BSTIterator<K,T> end() const;

private:
	BSTNode<K,T> *root;
	unsigned int bstSize;

	// returns a pointer to the node containing the key,
	// or to what its parent node would be if the key does not exist,
	// or NULL if the tree is currently empty
	BSTNode<K,T>* findNode(const K& key) const;

	// assumes at least one child of node is NULL, will delete
	// the node and move its only child (if any) to its place
	void pluckNode(BSTNode<K,T>* node);
};

// ***** IMPLEMENTATION STARTS HERE *****

template <typename K, typename T>
BSTMap<K,T>::BSTMap() {
	this->root = NULL;
	this->bstSize = 0;
}

template <typename K, typename T>
BSTMap<K,T>::~BSTMap() {
	if (this->root != NULL) {
		delete this->root;

		// point to NULL
		this->root = NULL;

		this->bstSize = 0;
	}
}

template <typename K, typename T>
void BSTMap<K,T>::update(const K& key, const T& item) {
	BSTNode<K,T>* node = findNode(key);

	// if there was no node in the tree with this key, create one
	if (node == NULL || node->key != key) {
		BSTNode<K,T> *newNode = new BSTNode<K,T>(key, item, NULL, NULL, node);
		assert(newNode != NULL);

		// change the left or right pointer of the parent node
		// whichever is appropriate to preserve the BST property
		if (node == NULL) {
			// the tree was empty, so this is the new root
			root = newNode;
		}
		else {
			// the tree was not empty, so put it as the appropriate child of "node"
			if (key < node->key) {
				node->left = newNode;
			}
			else {
				node->right = newNode;
			}
		}
		++bstSize;
	}
	else {
		// the key existed, so just update the item
		node->item = item;
	}
}

template <typename K, typename T>
void BSTMap<K,T>::remove(const K& key) {
	BSTNode<K,T>* node = findNode(key);

	// make sure the key is in the tree
	// we only assume < is implemented for the key type, not necessarily !=
	assert(node != NULL && !(node->key < key || key < node->key));

	// find the maximum-key node in the left subtree of the node to remove
	BSTNode<K,T> *tmp = node->left, *pluck = node;
	while (tmp != NULL) {
		pluck = tmp;
		tmp = tmp->right;
	}
	// pluck is the one to remove, it might be pluck == node if node has
	// no left child

	// copy contents from node we are deleting
	node->key = pluck->key;
	node->item = pluck->item;

	// this function will delete a node with no right child and
	// restructure the tree
	pluckNode(pluck);
}

template <typename K, typename T>
bool BSTMap<K,T>::hasKey(const K& key) const {

	// "find" the node, and then check it really has the key
	BSTNode<K,T> *node = findNode(key);
	return node != NULL && !(node->key != key);
}

template <typename K, typename T>
T& BSTMap<K,T>::operator[](const K& key) {

	// "find" the node, if not found then create an entry
	// using the default constructor for the item type
	if (!hasKey(key)) {
		update(key, T());
	}

	return findNode(key)->item;
}

template <typename K, typename T>
const T& BSTMap<K,T>::at(const K& key) const {
	const BSTNode<K,T> *node = findNode(key);
	assert(node != NULL && !(node->key != key));

	return node->item;
}

template <typename K, typename T>
unsigned int BSTMap<K,T>::size() const {
	return this->bstSize;
}

/////////ADDED CODE////////
//return minimum score key
template <typename K, typename T>
int BSTMap<K,T>::minScore() const {
	BSTNode<K,T> *node = this->root;
	while(node->left != NULL){
		node = node->left;
	}
	return node->key;
}

/////////ADDED CODE////////
//return name of min score
template <typename K, typename T>
string BSTMap<K,T>::minName() const {
	BSTNode<K,T> *node = this->root;
	while(node->left != NULL){
		node = node->left;
	}
	return node->item;
}

/////////ADDED CODE////////
//if a full (5) bst with no left children -> unbalanced
template <typename K, typename T>
bool BSTMap<K,T>::isBalanced() const {
	BSTNode<K,T> *node = this->root;
	if(!node->left){ //if root has no lc, is skewed
		return false;
	}
	else{
		return true;
	}
}

template <typename K, typename T>
BSTNode<K,T>* BSTMap<K,T>::findNode(const K& key) const {
	BSTNode<K,T> *node = this->root, *parent = NULL;

	// traverse down the tree, going left and right as appropriate,
	// until the key is found or we fall off of the leaf node
	while (node != NULL && node->key != key) {
		parent = node;
		if (key < node->key) {
			node = node->left;
		}
		else {
			node = node->right;
		}
	}

	if (node == NULL) {
		// key not found
		// here, parent == NULL if and only if the tree is empty
		return parent;
	}
	else {
		// key found
		return node;
	}
}

template <typename K, typename T>
void BSTMap<K,T>::pluckNode(BSTNode<K,T>* node) {
	BSTNode<K,T> *child;
	if (node->left) {
		child = node->left;
		// make sure the node does not have two children
		assert(child->right == NULL);
	}
	else {
		// might still be NULL, meaning we are plucking a leaf node
		child = node->right;
	}
	// now child is the only non-null child of "node"
	// unless "node" was a leaf, in which case child is null

	// adjust the appropriate child pointer of the node's parent
	if (node->parent == NULL) {
		// deleting the root node
		this->root = child;
	}
	else if (node->parent->left == node) {
		node->parent->left = child;
	}
	else {
		node->parent->right = child;
	}

	// if we are not deleting a leaf, the child gets a new parent
	if (child) {
		child->parent = node->parent;
	}

	// ensures ~BSTNode() does not recursively delete other parts of the tree
	node->left = node->right = NULL;
	delete node;

	--bstSize;
}

template <typename K, typename T>
BSTIterator<K,T> BSTMap<K,T>::begin() const {
	return BSTIterator<K,T>(this->root);
}

template <typename K, typename T>
BSTIterator<K,T> BSTMap<K,T>::end() const {
	return BSTIterator<K,T>(NULL);
}

#endif
