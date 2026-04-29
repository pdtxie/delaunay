#include <iostream>
#include <vector>

struct edgeref;
struct quadedge;
struct trianglerecord;

struct vertex {
    int id;
    double x, y;

    // NOTE: for fast point location
    // if null => inserted, none to point to
    // otherwise, oriented edge of containing triangle
    quadedge *loce = nullptr;
    int locr = 0;

    // TODO: this might be bad
    bool operator==(const vertex &other) const {
        return id == other.id && x == other.x && y == other.y;
    }

    bool operator!=(const vertex &other) const {
        return !(*this == other);
    }
};

struct edgeref {
    quadedge *e = nullptr;
    int r = 0;  // {0, 1, 2, 3}

    edgeref() = default;
    edgeref(quadedge *e, int r) : e(e), r(r) {
    }
    static edgeref make_edge();

    edgeref rot();
    edgeref rotinv();
    edgeref sym();

    edgeref &onext();
    edgeref oprev();

    edgeref lnext();
    edgeref lprev();

    edgeref rnext();
    edgeref rprev();

    edgeref dprev();
    edgeref dnext();

    vertex &org();
    vertex &dest();

    static void splice(edgeref a, edgeref b);
    static edgeref connect(edgeref a, edgeref b);
    static void delete_edge(edgeref e);
    static void swap(edgeref e, bool fast);

    static bool rightof(edgeref e, vertex v);  // v right of e
    static bool leftof(edgeref e, vertex v);   // v right of e

    bool operator==(const edgeref &other) const {
        return e == other.e && r == other.r;
    }

    bool operator!=(const edgeref &other) const {
        return !(*this == other);
    }

    // NOTE: fast point loc
    trianglerecord *&lrec();
    trianglerecord *&rrec();

    void assign_lrec(trianglerecord *t);
    bool in_lrec(vertex &v);

    static void fix_conflicts(std::vector<vertex *> &o, std::vector<trianglerecord *> n);
};

// for fast point location
struct trianglerecord {
    edgeref rep;
    std::vector<vertex *> conflicts;  // uninserted
    bool alive = true;
};

struct edgerecord {
    vertex data;
    edgeref next;

    trianglerecord *face = nullptr;
};

struct quadedge {
    edgerecord es[4];
};

struct triangulation {
    edgeref e;
    std::vector<vertex> vs;
    std::vector<edgeref> es;
};

// debugging
std::ostream &operator<<(std::ostream &os, const vertex &v);
std::ostream &operator<<(std::ostream &os, const edgeref &ref);
std::ostream &operator<<(std::ostream &os, const edgerecord &rec);
std::ostream &operator<<(std::ostream &os, const quadedge &q);
