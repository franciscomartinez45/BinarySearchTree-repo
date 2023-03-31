#pragma once
#include "Map.h"
#include<iostream>
#include <cassert>

Node::Node(KeyType name, ValueType number) : key(name), value(number), left(nullptr), right(nullptr) {};

int treeSize(Node* ptr) { //recursively returns tree size
	if (ptr != nullptr) {
		return 1 + treeSize(ptr->left) + treeSize(ptr->right);
	}
	return 0;
}
bool change(Node*& ptr, const KeyType& key, const ValueType& value) { //locates node to change value
	if (key < ptr->key) {
		change(ptr->left, key, value);
	}
	else if (key > ptr->key) {
		change(ptr->right, key, value);
	}
	else if (key == ptr->key) {
		ptr->value = value;
		return true;
	}

}


Node* locateNode(Node*& ptr, const KeyType& key, ValueType& value) { //returns a pointer to node with a specific key
	if (ptr != nullptr) {
		if (key < ptr->key) {
			return locateNode(ptr->left, key, value);
		}
		else if (key > ptr->key) {
			return locateNode(ptr->right, key,value);
		}
		else if (key == ptr->key) {
			value = ptr->value;
			return ptr;
		}
		

	}
	return ptr;
}
bool iterate(Node* ptr, int i, KeyType& key, ValueType& value, int& iterator,const int size) { //iterates through map using a counter and the size of the map 
	
	if (ptr != nullptr && key.empty()) { //key.empty() avoids setting key and value after the recursive calls unstack
			if (iterator == i) {
				key = ptr->key;
				value = ptr->value;
				return true;
			}
			else if(iterator<i){
					iterator++;
					iterate(ptr->left, i, key, value, iterator,size);
					iterate(ptr->right, i, key, value, iterator, size);
				}
			
			}

		
	
	
}
void recurInsert(Node*& ptr, const KeyType& key, const ValueType& value) { //finds the next null pointer to dynamically allocate memory 
	if (ptr != nullptr) {
		if (key<ptr->key) {
			recurInsert(ptr->left, key, value);
		}
		else if (key>ptr->key) {
			recurInsert(ptr->right, key, value);
		}
	}
	if (ptr == nullptr) {
		ptr = new Node(key, value);
	}
}

Map::Map() : root(nullptr) {} 
// Create an empty map (i.e., one with no key/value pairs)
Node* copy_tree(Node* p) { //copy a tree recursively 
	if (p != nullptr) {
		Node* ptr = new Node(p->key, p->value);
		ptr->left = copy_tree(p->left);
		ptr->right = copy_tree(p->right);
		return ptr;
	}
	return nullptr;
}
Map::Map(const Map& b) { //copy constructor
	if (this->root != nullptr) { //make sure tree is empty
		this->~Map(); //else delete this tree
	}
	Node* ptr = b.root;
	root = copy_tree(ptr);

}
Map& Map::operator=(const Map& b) { //assigment operator
	if (&root != &b.root) {

		if (this->root != nullptr) {
			//make sure tree is empty
			this->~Map();//else delete this tree
		}
		Node* ptr = b.root;
		root = copy_tree(ptr);
	}
	return *this;
}
bool Map::empty() const { //returns true if root is null
	if (root == nullptr) {
		return true;
	}
	else {
		return false;
	}
} // Return true if the map is empty, otherwise false.

int Map::size() const { //calls recursize function treeSize that takes a pointer
	Node* ptr = root;
	int size = treeSize(ptr);
	return size;
}// Return the number of key/value pairs in the map.

bool Map::insert(const KeyType& key, const ValueType& value) {
	if (root == nullptr) { //insert first item at the root
		root = new Node(key, value);
	}
	else {
		Node* ptr = root;
		recurInsert(root, key, value); //recursively find next nullptr
		return true;
	}
}
// If key is not equal to any key currently in the map, and if the
// key/value pair can be added to the map, then do so and return true.
// Otherwise, make no change to the map and return false (indicating
// that either the key is already in the map).

bool Map::update(const KeyType& key, const ValueType& value) {
	Node* ptr = root;
	ValueType temp{};
	Node* found = locateNode(ptr, key, temp); // locates the node with key
	if (found) { //if a node is located
		change(ptr, key, value); //change the contents
		return true;
	}
	else if (!found) {//return false if a node with the key was not found
		return false;
	}
}
// If key is equal to a key currently in the map, then make that key no
// longer map to the value it currently maps to, but instead map to
// the value of the second parameter; return true in this case.
// Otherwise, make no change to the map and return false.
bool Map::insertOrUpdate(const KeyType& key, const ValueType& value) {
	Node* ptr = root;
	if (this->contains(key)) { //check if this tree contains a node with the key
		change(ptr, key, value); //change if true
	}
	else if (!this->contains(key)) { //if it does not contain key
		insert(key, value); //insert new node 
	}
	return true; //regardless return true
}
// If key is equal to a key currently in the map, then make that key no
// longer map to the value it currently maps to, but instead map to
// the value of the second parameter; return true in this case.
// If key is not equal to any key currently in the map then add it and
// return true. In fact this function always returns true.
//
//Node* parentNode(Node* ptr, const KeyType key) {
//
//}

bool parentNode(Node*& parent, Node* node) { //locates the parent node of any given node
	while (parent->left!= node && parent->right != node) { //starts at the root and finds it's way down to the parent of the given node
		if (node->key < parent->key) {
			parent = parent->left;
		}
		else if (node->key > parent->key) {
			parent = parent->right;
		}
	}
	return true;
}
bool leafNode(Node*& ptr) { //finds the leaf node is a decendant of the desired node to be deleted
	while (ptr->left!=nullptr||ptr->right!=nullptr) {
		if (ptr->left != nullptr) {
			ptr = ptr->left;
		}
		else if (ptr->right != nullptr) {
			ptr = ptr->right;
		}
	
	}
	return true;
}

bool Map::erase(const KeyType& key) {
	if (!contains(key)) { //checks to make sure this map contains the key
		return false;
	}
	else {
		Node* node = root; //node to be deleted
		Node* nodeParent = node; //parent of the node to be deleted
		Node* leafParent = node; //parent of the leaf that replaces the deleted node
		ValueType v{};
		node = locateNode(node, key, v); //locate the node
		if (node != root) {//checks if it is an internal node
			if (node->right != nullptr) { 
				Node* leaf = node->right; //find the next leaf
				leafNode(leaf);
				parentNode(nodeParent, node); //find parent of the node
				parentNode(leafParent, leaf); //find parent of the leaf
				if (node->right == leaf) { //check if leaf is child of the node to be deleted
					if (node == nodeParent->left) {
						nodeParent->left = leaf;
					}
					else {
						nodeParent->right = leaf;
					}

				}
				else {
					if (nodeParent->left == node) { //directly connect the node parent to the left node child
						nodeParent->left = leaf;
						leaf->left = node->left;
						leaf->right = node->right;
						if (leaf == leafParent->left) {
							leafParent->left = nullptr;
						}
						else {
							leafParent->right = nullptr;
						}


					}
					else if (nodeParent->right = node) { //directly connect the node parent to the right node child
						nodeParent->right = leaf;
						leaf->left = node->left;
						leaf->right = node->right;
						if (leaf == leafParent->left) {
							leafParent->left = nullptr;
						}
						else {
							leafParent->right = nullptr;
						}

					}
				}
				delete node;
				node = nullptr;


				
			}
			else if (node->right == nullptr && node->left != nullptr) { //check to see if node has a right child
				parentNode(nodeParent, node); //find the parent of the leaf
				nodeParent->left = node->left; //connect parent node to next child
				delete node;
				node = nullptr;
				return true;
				
			}
			else if (node->left == nullptr && node->right == nullptr) { //leaf node has no children
				if (node == root) { //
					delete root;
					root = nullptr;
				}
				else {
					parentNode(nodeParent, node); //find parent node
					if (nodeParent->left == node) {//remove accordingly 
						nodeParent->left = nullptr;
					}
					else {
						nodeParent->right = nullptr;
					}
					delete node; //delete leaf
					node = nullptr;

				}
				//findParent;
			}
		}
		else { //root node to be deleted
			if (root->right != nullptr) { //if root has a larger decendant
				Node* leaf = node->right; //find the leaf
				leafNode(leaf);
				if (leaf == root->right) { //check if the leaf is the child of the root
					leaf->left = root->left; 
					root = leaf;
					delete node;
					node = nullptr;
				}
				else {
					parentNode(leafParent, leaf); //find the parent of the leaf
					leaf->left = root->left;
					leaf->right = root->right;
					if (leaf == leafParent->left) {
						leafParent->left = nullptr; //dettach accordingly
					}
					else {
						leafParent->right = nullptr;
					}
					root = leaf; //leaf is now the root
					delete node; //delete desired node
					node, leaf,nodeParent,leafParent = nullptr;
				}
			}
			else {
				root = root->left;
				delete node;
				node = nullptr;
			}
		}
		return true;
	}


}


// If key is equal to a key currently in the map, remove the key/value
// pair with that key from the map and return true. Otherwise, make
// no change to the map and return false.
bool Map::contains(const KeyType& key) const {
	Node* ptr = root;
	ValueType temp{};
	Node* found = locateNode(ptr, key, temp); //locate a node
	if (found) {//if node is not null return true
		return true;
	}
	else { //if node is null return false
		return false;
	}
}
// Return true if key is equal to a key currently in the map, otherwise
// false.
bool Map::get(const KeyType& key, ValueType& value) const {
	Node* ptr = root;

	if (this->contains(key)) {//check if this map contains the key
		locateNode(ptr, key, value);//locate the node and its contents
		return true;
	}
	else { //otherwise return false
		return false;
	}

}
// If key is equal to a key currently in the map, set value to the
// value in the map that that key maps to, and return true. Otherwise,
// make no change to the value parameter of this function and return
// false.



bool Map::get(int i, KeyType& key, ValueType& value) const {
	Node* ptr = root;
	int iterator{};
	
	return iterate(ptr, i, key, value, iterator, this->size()); //iterate through the map using the size of the map and an iterator that keeps count
	

}
// If 0 <= i < size(), copy into the key and value parameters the
// key and value of one of the key/value pairs in the map and return
// true. Otherwise, leave the key and value parameters unchanged and
// return false. (See below for details about this function.)
void Map::swap(Map& other) {

	Node* ptr1 = this->root; //initialize a pointer at one root
	Node* ptr2 = other.root; //initialize a pointer at other root
	this->root = ptr2; //swap root
	other.root = ptr1; //swap root
	ptr1 = nullptr; //set temp pointers to null
	ptr2 = nullptr;

}
// Exchange the contents of this map with the other one.
void delete_nodes(Node*& ptr) {
	if (ptr != nullptr) { //post order delete nodes
		delete_nodes(ptr->left);
		delete_nodes(ptr->right);
		delete ptr;
		ptr = nullptr;
	}
}
Map::~Map() {
	Node* ptr = root;
	if (ptr != nullptr) {
		delete_nodes(ptr); //delete nodes until root == nullptr
	}
	root = nullptr;

}


bool largerTree(const Map& m1, const Map& m2, Map& result) {
	bool combined = true; //set to true until 2 values do not match
	for (int i{}; i < m1.size(); i++) { //larger map is compared 
		ValueType firstVal{};
		KeyType firstKey{};
		KeyType secondKey{};
		ValueType secondVal{};
		m1.get(i, firstKey, firstVal);	
		if (m2.contains(firstKey)) { //check if smaller map contains key of bigger map
			secondKey = firstKey;
			m2.get(secondKey, secondVal);
			if (secondVal!=firstVal) { //check if values are same
				combined = false; //values are not the same, therefore false
				m2.get(i, secondKey, secondVal);//get second map's key and value
				if (!m1.contains(secondKey)) { //check if first map contains
					result.insert(secondKey, secondVal);
				}
				continue;
			}
			else {
				if (!result.contains(firstKey)) {
					result.insert(firstKey, firstVal); //otherwise insert
				}
			}

		}
		else { //if value from first map is not in second map, add to result map
			result.insert(firstKey, firstVal);
			m2.get(i, secondKey, secondVal); //get next key and value from second map
			if (m1.contains(secondKey)) { //check that first map does not contain second map key and value
				ValueType temp{};
				m1.get(secondKey, temp);
				if (temp != secondVal) { //if it contains and values do not match, continue
					continue;
				}
			}
			else if (!result.contains(secondKey)) {
				result.insert(secondKey, secondVal); //otherwise insert
			}
			else {
				continue;
			}
		}
	}
 	return combined;
}
bool combine(const Map& m1, const Map& m2, Map& result) {
	bool m1Larger{};
	Map temp; //temporary tree
		if (m1.size() >= m2.size()) { //pass the largest tree to compare against
			m1Larger = largerTree(m1, m2, temp);
		}
		else {
			m1Larger = largerTree(m2, m1, temp);
		}
		if (&m1 == &result) { //if m1 and result are the same tree
			m1.~Map(); //erase contents of m1 first
			
	}
		result = temp; //assign result to temp
	return m1Larger;


	
}
void subtract(const Map& m1, const Map& m2, Map& result) {
	
	Map temp; //temporary map
	for (int i{}; i < m1.size(); i++) {
		KeyType firstKey;
		ValueType firstVal;
		m1.get(i, firstKey, firstVal);//iterate through m1
		if (m2.contains(firstKey)) { //check if m2 contains the key in m1
			continue; //if it does continue
		}
		else {
			temp.insert(firstKey, firstVal); //otherwise insert
		}	
	}
	if (!result.empty()) { //check if result has contents
		result.~Map(); //erase contents before adding new elements
	}
	result = temp;
	
	
}

void test()
{
	Map m; //initalize a tree 
	assert(m.insert("Fred", 123)); //node is properly created
	assert(m.insert("Ethel", 456)); //node is properly created
	assert(m.insert("Lucy", 789));//node is properly created
	assert(m.size() == 3);
	Map b; //initalize a tree 
	assert(b.insert("Lucy", 78)); //node is properly created
	assert(b.insert("Ricky", 321)); //node is properly created

	Map c; //initalize a tree 
	assert(!combine(m, b, c)); //Maps m and b contain one element that does not have the same value, therefore false
	subtract(c, b, m);
	assert(m.contains("Ethel") && m.contains("Fred")); //only two elements left from the subtract function
	assert(m.insert("Frank", 101)); //insert another element
	assert(m.size() == 3); //test size
	assert(m.erase("Ethel")); //erase an element
	assert(!m.contains("Ethel")); //does not contain "Ethel"
	assert(m.size() == 2); //size was reduced
	m.swap(c); //swap elements with c
	assert(m.size() == 3); //size is now 3
	assert(c.size() == 2);
	assert(!m.update("Frank", 102)); //contents were swapped with Map c, therefore "Frank" is not in m
	assert(m.insertOrUpdate("Lucky", 007)); //test to insert because "Lucky" is not in Map
	ValueType val;
	assert(m.insertOrUpdate("Lucky",777) && m.get("Lucky",val) && (val==777)); //test to update number
	m = b;
	assert(m.contains("Lucy")); //test copy operator 
	
		for (int i{}; i < m.size(); i++) {
			std::string k;
			double v;
			m.get(i, k, v);
			std::cout << k << v << std::endl;
		}
	
	
	
}

int main()
{
	test();
	
}