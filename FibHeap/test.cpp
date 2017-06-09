#include <iostream>
#include "FibHeap.h"
using namespace std;

void main()
{
	FibHeap h;
	for (int i = 1; i < 8; i++)	h.insert(i);
	Node* p = h.insert(8);
	h.insert(9);
	h.extract_min();
	h.decrease_key(p, 1);
}