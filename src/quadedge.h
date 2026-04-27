#include <iostream>

struct vertex {
    int id;
    double x, y;

    // TODO: this might be bad
    bool operator==(const vertex &other) const {
        return id == other.id && x == other.x && y == other.y;
    }

    bool operator!=(const vertex &other) const {
        return !(*this == other);
    }
};

struct quadedge;

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
    static void swap(edgeref e);

    static bool rightof(edgeref e, vertex v);  // v right of e
    static bool leftof(edgeref e, vertex v);   // v right of e

    bool operator==(const edgeref &other) const {
        return e == other.e && r == other.r;
    }

    bool operator!=(const edgeref &other) const {
        return !(*this == other);
    }
};

struct edgerecord {
    vertex data;
    edgeref next;
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
