extern "C" {
#define TRILIBRARY
#define REAL double
#include "predicates.h"
#include "triangle.h"
}

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include "quadedge.h"

using namespace std;

// parse input .node
vector<vertex> parse_nodes(string path) {
	ifstream infile(path);
	int n, dim, nattr, bs;
	infile >> n >> dim >> nattr >> bs;

	printf("parsing... n=%d, dim=%d, nattr=%d, bs=%d\n", n, dim, nattr, bs);

	vector<vertex> v;

	for (int i = 0; i < n; i++) {
		int id;
		double x, y, b;
		vector<int> attrs(nattr);

		infile >> id >> x >> y;
		for (int j = 0; j < nattr; j++)
			infile >> attrs[j];

		if (bs)
			infile >> b;

		v.push_back(vertex(id, x, y));
	}

	return v;
}


int main(void) {
	vector<vertex> vs = parse_nodes("/Users/pdt/workspace/classes/274/project/voronoi/ex/box.node");

	for (vertex v : vs) {
		cout << v << endl;
	}

    return 0;
}

