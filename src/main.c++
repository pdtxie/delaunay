extern "C" {
#define TRILIBRARY
#define REAL double
#include "predicates.h"
}

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <vector>

#include <argparse/argparse.hpp>

#include "quadedge.h"

bool DEBUG = 0;

using namespace std;
using namespace std::chrono;
using filesystem::path;

// parse input .node
vector<vertex> parse_nodes(string path) {
    ifstream infile(path);
    int n, dim, nattr, bs;
    infile >> n >> dim >> nattr >> bs;

    if (DEBUG)
        printf("[debug] parsing... n=%d, dim=%d, nattr=%d, bs=%d\n", n, dim, nattr, bs);

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

        // ignore super triangle
        auto _ignore_super = [&](edgeref e) { return e.org().id > tr.vs.size() - 3; };
        if (_ignore_super(a) || _ignore_super(b) || _ignore_super(c))
            return;

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
    double DMAX = numeric_limits<double>::max(), DMIN = numeric_limits<double>::lowest();
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
    REAL pad = max(dx, dy) * 10;

    // A -> B -> C -> A (ccw)
    vertex A(vs.size() + 1, cx, ymax + pad), B(vs.size() + 2, xmin - pad, ymin - pad),
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

edgeref locate_slow(vertex v, triangulation &tr) {
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

edgeref locate_fast(vertex v, triangulation &tr) {
    return edgeref(v.loce, v.locr);
}

void init_conflicts(triangulation &tr, int n) {
    // tr must be super triangle at this point TODO: add assert??
    trianglerecord *t = new trianglerecord;
    t->rep = tr.e;

    tr.e.assign_lrec(t);

    for (int i = 0; i < n; i++) {
        vertex &v = tr.vs[i];

        v.loce = tr.e.e;
        v.locr = tr.e.r;

        t->conflicts.push_back(&v);
    }
}

void insert(vertex &v, triangulation &tr, bool fast) {
    edgeref e = fast ? locate_fast(v, tr) : locate_slow(v, tr);

    vector<vertex *> old_conflicts;

    if (fast) {
        trianglerecord *old_t = e.lrec();
        old_conflicts = old_t->conflicts;
        old_t->alive = false;
        v.loce = nullptr;
        v.locr = 0;
    }

    if (v == e.org() || v == e.dest())
        return;

    edgeref t;
    if (orient2d(&e.org().x, &e.dest().x, &v.x) == 0) {
        if (fast) {
            trianglerecord *rt = e.rrec();

            if (rt && rt->alive) {
                old_conflicts.insert(old_conflicts.end(), rt->conflicts.begin(),
                                     rt->conflicts.end());
                rt->alive = false;
            }
        }

        // on edge e
        t = e.oprev();
        erase_if(tr.es, [&](edgeref x) { return x == e || x == e.sym(); });
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

    // create trianglerecords
    if (fast) {
        vector<trianglerecord *> new_ts;
        edgeref start = base.sym(), cur = start;

        do {
            trianglerecord *nt = new trianglerecord;
            cur.assign_lrec(nt);
            new_ts.push_back(nt);

            cur = cur.onext();
        } while (cur != start);

        edgeref::fix_conflicts(old_conflicts, new_ts);
    }

    e = base.oprev();

    do {
        t = e.oprev();

        REAL o = orient2d(&e.org().x, &t.dest().x, &e.dest().x);
        REAL det = incircle(&e.org().x, &t.dest().x, &e.dest().x, &v.x);
        bool inside = (o > 0 && det > 0) || (o < 0 && det < 0);

        if (edgeref::rightof(e, t.dest()) && inside) {
            edgeref::swap(e, fast);
            e = e.oprev();
        } else if (e.org() == first) {
            return;
        } else {
            e = e.onext().lprev();
        }
    } while (1);
}

int main(int argc, char **argv) {
    exactinit();

    argparse::ArgumentParser program("delaunay");
    program.add_argument("-f").required().help(".node file to triangulate");
    program.add_argument("-o").required().help("output directory (.ele and .node files will be output here)").default_value("out");
    program.add_argument("-d").flag().help("debug mode");
    program.add_argument("-p").flag().help("performance test / record time");
    program.add_argument("--fast").flag().help(
        "use fast point location. uses slow point location by default");
    program.add_argument("--random").flag().help("randomise input points");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    string file = program.get<string>("-f");
    string outdir = program.get<string>("-o");
    bool perf = program.get<bool>("-p");
    bool fast = program.get<bool>("--fast");
    bool random = program.get<bool>("--random");

    cout << format("running on file: {} with {} mode and {} points", file, fast ? "fast" : "slow",
                   random ? "randomised" : "non-randomised")
         << endl;
    if (program.get<bool>("-d")) {
        cout << "[debug] using debug mode" << endl;
        DEBUG = 1;
    }

    path inpath = filesystem::current_path() /= file;

    cout << "parsing nodes, shuffling + making super triangle..." << endl;

    /*[0]*/ chrono::steady_clock::time_point t0 = chrono::steady_clock::now();
    vector<vertex> vs = parse_nodes(inpath);
    if (random) {
        cout << "shuffling vertices..." << endl;
        random_device rd;
        mt19937 gen(rd());
        shuffle(vs.begin(), vs.end(), gen);
    }

    int n = vs.size();
    triangulation tr = super_triangle(vs);
    if (fast)
        init_conflicts(tr, n);
    /*[1]*/ chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

    if (perf)
        cout << format("[perf] parsed + made super triangle in {}ms",
                       duration_cast<milliseconds>(t1 - t0).count())
             << endl;

    cout << "inserting vertices..." << endl;

    /*[2]*/ chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    for (int i = 0; i < n; i++) {
        insert(tr.vs[i], tr, fast);
        if (DEBUG)
            cout << "[debug] inserted: " << tr.vs[i] << endl;
    }
    /*[3]*/ chrono::steady_clock::time_point t3 = chrono::steady_clock::now();

    if (perf)
        cout << format("[perf] inserted in {}ms", duration_cast<milliseconds>(t3 - t2).count())
             << endl;

    sort(tr.vs.begin(), tr.vs.end(),
         [](const vertex &v1, const vertex &v2) { return v1.id < v2.id; });

    path outpath = filesystem::current_path() /= outdir;
    cout << format("writing output to {} ...", outdir) << endl;

    /*[4]*/ chrono::steady_clock::time_point t4 = chrono::steady_clock::now();
    write(outpath, tr);
    /*[5]*/ chrono::steady_clock::time_point t5 = chrono::steady_clock::now();

    if (perf)
        cout << format("[perf] wrote output in {}ms", duration_cast<milliseconds>(t5 - t4).count())
             << endl;

    return 0;
}

