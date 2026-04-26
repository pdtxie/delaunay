#include "../src/quadedge.h"
#include <assert.h>
#include <cassert>
#include <iostream>

using namespace std;

void test_edge_algebra() {
    vertex a(1, 0.0, 0.0);
    vertex b(2, 1.0, 0.0);

    edgeref e = edgeref::make_edge();
    e.org() = a;
    e.dest() = b;

    // 2.3 check properties
    assert(e.rot().rot().rot().rot() == e);
    assert(e.rot().onext().rot().onext() == e);
    assert(e.rot().onext() == e.rotinv());
    assert(e.rot().onext().rot() == e);
    assert(e.rot().rot() == e.sym());
    assert(e.oprev() == e.rot().onext().rot());
    assert(e.lnext() == e.rotinv().onext().rot());
    assert(e.rnext() == e.rot().onext().rotinv());
    assert(e.dnext() == e.sym().onext().sym());
}

void test_operators() {
	vertex v1(1, -1, 0);
	vertex v2(2, 0, -1);
	vertex v3(3, 1, 0);
	vertex v4(4, 0, 1);

	edgeref e1 = edgeref::make_edge();
	edgeref e2 = edgeref::make_edge();
	edgeref e3 = edgeref::make_edge();
	edgeref e4 = edgeref::make_edge();

	e1.org() = v1; e1.dest() = v2;
	e2.org() = v2; e2.dest() = v3;
	e3.org() = v3; e3.dest() = v4;
	e4.org() = v4; e4.dest() = v1;

	edgeref::splice(e1.sym(), e2);
	edgeref::splice(e2.sym(), e3);
	edgeref::splice(e3.sym(), e4);
	edgeref::splice(e4.sym(), e1);

	assert(e1.lnext() == e2);
	assert(e1.lnext().lnext() == e3);
	assert(e1.lnext().lnext().lnext() == e4);

	assert(e1.rnext() == e4);
	assert(e2.rnext() == e1);
	assert(e3.rnext() == e2);
	assert(e4.rnext() == e3);

	edgeref e = edgeref::connect(e4, e3);

	assert(e.org() == v1);
	assert(e.dest() == v3);

	assert(e.lnext() == e3);
	assert(e4.lnext() == e);
}

int main() {
	test_edge_algebra();
	test_operators();

    cout << "tests passed :D" << endl;

    return 0;
}
