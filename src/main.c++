extern "C" {
#define TRILIBRARY
#define REAL double
#include "predicates.h"
#include "triangle.h"
}

#include <cassert>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
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

void write(string path, triangulation &tr) {
    ofstream ele(path + "/out.ele");

    vector<array<int, 3>> triangles;
    set<array<int, 3>> seen;

    auto _create_face = [&](edgeref e) {
        edgeref a = e;
        edgeref b = a.lnext();
        edgeref c = b.lnext();

        if (c.lnext() != a || orient2d(&a.org().x, &b.org().x, &c.org().x) <= 0)
            return;  // not triangle OR outside face

        array<int, 3> t = {a.org().id, b.org().id, c.org().id};
        sort(t.begin(), t.end());
        if (seen.contains(t))
            return;

        triangles.push_back(t);
        seen.insert(t);
    };

    for (edgeref e : tr.es) {
        _create_face(e);
        _create_face(e.sym());
    }

    ele << format("{} 3 0", triangles.size()) << endl;

    for (int i = 0; i < triangles.size(); i++) {
        auto t = triangles[i];
        ele << format("{} {} {} {}", i + 1, t[0], t[1], t[2]) << endl;
    }

    ofstream node(path + "/out.node");

    node << format("{} 2 0 0", tr.vs.size()) << endl;

    for (vertex v : tr.vs) {
        node << format("{} {} {}", v.id, v.x, v.y) << endl;
    }
}

triangulation super_triangle(vector<vertex> vs) {
    // make super triangle around all the points
    double DMAX = numeric_limits<double>::max(),
           DMIN = numeric_limits<double>::lowest();
    REAL xmin = DMAX, xmax = DMIN, ymin = DMAX, ymax = DMIN;

    for (vertex v : vs) {
        if (v.x < xmin)
            xmin = v.x;
        if (v.x > xmax)
            xmax = v.x;
        if (v.y < ymin)
            ymin = v.y;
        if (v.y > ymax)
            ymax = v.y;
    }

    REAL dx = xmax - xmin, dy = ymax - ymin;
    REAL cx = (xmin + xmax) / 2;
    REAL pad = max(dx, dy) + 100;

    // A -> B -> C -> A (ccw)
    vertex A(vs.size() + 1, cx, ymax + pad),
        B(vs.size() + 2, xmin - pad, ymin - pad),
        C(vs.size() + 3, xmax + pad, ymin - pad);

    edgeref AB = edgeref::make_edge();
    AB.org() = A;
    AB.dest() = B;

    edgeref BC = edgeref::make_edge();
    BC.org() = B;
    BC.dest() = C;

    edgeref CA = edgeref::make_edge();
    CA.org() = C;
    CA.dest() = A;

    edgeref::splice(AB, CA.sym());
    edgeref::splice(BC, AB.sym());
    edgeref::splice(CA, BC.sym());

    vs.insert(vs.end(), {A, B, C});
    return triangulation{AB, vs, {AB, BC, CA}};
}

edgeref locate(vertex v, triangulation &tr) {
    edgeref e = tr.e;

    do {
        if (v == e.org() || v == e.dest())
            return e;

        if (edgeref::rightof(e, v))
            e = e.sym();
        else if (!edgeref::rightof(e.onext(), v))
            e = e.onext();
        else if (!edgeref::rightof(e.dprev(), v))
            e = e.dprev();
        else
            return e;
    } while (1);
}

void insert(vertex v, triangulation &tr) {
    edgeref e = locate(v, tr);

    if (v == e.org() || v == e.dest())
        return;

    edgeref t;
    if (orient2d(&e.org().x, &e.dest().x, &v.x) == 0) {
        // on edge e
        t = e.oprev();
		erase_if(tr.es, [&](edgeref x){ return x == e || x == e.sym(); });
        edgeref::delete_edge(e);
        e = t;
    }

    // connect vertices
    edgeref base = edgeref::make_edge();
    tr.es.push_back(base);
    vertex first = e.org();
    base.org() = first;
    base.dest() = v;
    edgeref::splice(base, e);

    do {
        base = edgeref::connect(e, base.sym());
        tr.es.push_back(base);
        e = base.oprev();
    } while (e.dest() != first);

    e = base.oprev();

    do {
        t = e.oprev();
        if (edgeref::rightof(e, t.dest()) &&
            incircle(&e.org().x, &t.dest().x, &e.dest().x, &v.x) > 0) {
            edgeref::swap(e);
            e = t;
        } else if (e.org() == first) {
            return;
        } else {
            e = e.onext().lprev();
        }
    } while (1);
}

int main(void) {
    exactinit();

    vector<vertex> vs = parse_nodes(
        "/Users/pdt/workspace/classes/274/project/voronoi/ex/box.node");

    cout << "parsed nodes" << endl;

    triangulation tr = super_triangle(vs);

    cout << "made super triangle" << endl;

    for (vertex v : vs) {
        insert(v, tr);
        cout << "inserted: " << v << endl;
    }

    cout << "writing output..." << endl;

    write("/Users/pdt/workspace/classes/274/project/voronoi/out", tr);

    return 0;
}
