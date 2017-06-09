#pragma once
#include <cmath>

struct Node
{
	friend class FibHeap;
	explicit Node(const int& k) :
		key(k), parent(nullptr), child(nullptr), left(nullptr), right(nullptr),
		degree(0), mark(false) {};
	Node(const Node&) = delete;
	Node(Node&&) = delete;
private:
	void clear() {
		parent = child = left = right = nullptr;
		degree = 0;
		mark = false;
	}
	int key;
	Node* parent, *child, *left, *right;
	unsigned int degree;
	bool mark;
};

class FibHeap
{
public:
	FibHeap() :min(nullptr), n(0) {};
	explicit FibHeap(FibHeap& h1, FibHeap& h2);
	FibHeap(const FibHeap&) = delete;
	FibHeap& operator=(const FibHeap&) = delete;
	inline Node* insert(const int& k);
	Node* get_min() { return min; }
	Node* extract_min();
	bool decrease_key(Node* x, const int& k);
	Node* erase(Node* x) { decrease_key(x, min->key - 1); return extract_min(); }
	~FibHeap() { if (min != nullptr)destroy(min); }

private:
	void cut(Node* x, Node* y)
	{
		//remove x from it's original list
		if (x->right != x) {
			x->left->right = x->right;
			x->right->left = x->left;
		}
		//plug x into list y
		y->right->left = x;
		x->right = y->right;
		x->left = y;
		y->right = x;
	}

	void destroy(Node* x)
	{
		if (x->child == nullptr) {
			if (x->right != x) {
				x->left->right = x->right;
				x->right->left = x->left;
			}
			delete x;
		}
		else {
			Node* p = x->child;
			do {
				destroy(p);
			} while (p != x->child);
		}
	}

	Node* min;
	int n;
	const double phi=(1+sqrt(5))/2;
};

FibHeap::FibHeap(FibHeap& h1, FibHeap& h2)
{
	min = h1.min;
	if (h1.min != nullptr && h2.min != nullptr) {
		Node* y1 = min->right;
		Node* y2 = h2.min->right;
		y1->left = h2.min;
		h2.min->right = y1;
		y2->left = min;
		min->right = y2;
	}
	if (min == nullptr || 
		(h2.min != nullptr && h2.min->key < min->key) )	min = h2.min;
	n = h1.n + h2.n;
	h1.min = h2.min = nullptr;
	h1.n = h2.n = 0;
}

inline Node* FibHeap::insert(const int& k)
{
	Node* x = new Node(k);
	if (min == nullptr) {
		min = x;
		min->left = min->right = min;
	}
	else {
		min->left->right = x;
		x->left = min->left;
		min->left = x;
		x->right = min;
	}
	if (x->key < min->key)	min = x;
	++n;
	return x;
}

Node* FibHeap::extract_min()
{
	//extract min
	if (min == nullptr)	return nullptr;
	Node* x = min;
	if (x->child != nullptr) {
		Node* y = x->child;
		Node* yr = y->right;
		Node* xr = x->right;
		yr->left = x;
		x->right = yr;
		y->right = xr;
		xr->left = y;
		for (Node* p = y; p != x; p = p->left)	p->parent = nullptr;
	}
	if (x->right == x) {
		x->clear();
		min = nullptr;
		return x;
	}
	else {
		x->left->right = x->right;
		x->right->left = x->left;
	}
	min = x->left;
	--n;
	
	//consolidate
	int dn = log(n) / log(phi);
	Node** A = new Node*[dn + 1]{ nullptr };
	Node* p = min;
	bool fin = false;
	while (!fin) {  //for every node p in the root list
		unsigned int& d = p->degree;
		for (Node* z = A[d]; (z = A[d]) != nullptr && d <= dn; d++) {  //when p and z share the same degree
			if (z->key < p->key) { //ensure that z has a larger key than p
				Node* t = p;
				p = z;
				z = t;
			}
			//remove z from the root list
			if (z == min)	min = z->right;
			z->left->right = z->right;
			z->right->left = z->left;
			//add z into p's child
			z->parent = p;
			if (p->child == nullptr) {
				p->child = z;
				z->left = z->right = z;
			}
			else {
				Node* c = p->child;
				c->right->left = z;
				z->right = c->right;
				c->right = z;
				z->left = c;			
			}
			z->mark = false;
			A[d] = nullptr;
		}
		A[d] = p;
		p = p->right;
		if (p == min)	fin = true;
	}
	//find the min node
	for (int i = 0; i != dn + 1; ++i) {
		if (A[i] == nullptr)	continue;
		if (min == nullptr || A[i]->key < min->key)	min = A[i];
	}
	x->clear();
	return x;
}

bool FibHeap::decrease_key(Node* x, const int& k)
{
	if (k > x->key)	return false;
	if (k == x->key)	return true;
	x->key = k;
	Node* y = x->parent;
	if (y != nullptr && x->key < y->key) {
		//cut x
		cut(x, min);
		--y->degree;
		x->parent = nullptr;
		x->mark = false;
		//cascading cut
		Node* z;
		for (z = y->parent; z != nullptr; y = z, z = y->parent) {
			if (y->mark == false) {
				y->mark = true;
				break;
			}
			else {
				cut(y, min);
				--z->degree;
				y->parent = nullptr;
				y->mark = false;
			}
		}
	}
	if (x->key < min->key)	min = x;
	return true;
}