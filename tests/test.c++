#include <iostream>
#include <cassert>
#include "../src/quadedge.h"

using namespace std;

bool eq(edgeref a, edgeref b) {
	return a.e == b.e && a.r == b.r;
}

bool eq(const vertex& a, const vertex& b) {
	return a.id == b.id && a.x == b.x && a.y == b.y;
}

int main() {
	vertex a(1, 0.0, 0.0);
	vertex b(2, 1.0, 0.0);

	edgeref e = edgeref::make_edge();
	e.org() = a; e.dest() = b;

	// 2.3 check properties
	assert(eq(e.rot().rot().rot().rot(), e));
	assert(eq(e.rot().onext().rot().onext(), e));
	assert(eq(
		e.rot().onext(),
		e.rotinv()
	));
	assert(eq(
		e.rot().onext().rot(),
		e
	));

	cout << "tests passed :D" << endl;

	return 0;
}
