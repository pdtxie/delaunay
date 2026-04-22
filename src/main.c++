extern "C" {
#define TRILIBRARY
#define REAL double
#include "predicates.h"
#include "triangle.h"
}

#include <cassert>
#include <iostream>

#include "quadedge.h"

using namespace std;

int main(void) {
    edgeref e = edgeref::make_edge();

    cout << e << endl;
    cout << e.onext() << endl;
    cout << e.oprev() << endl;

    cout << *e.e << endl;

    return 0;
}
