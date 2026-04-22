#include <iostream>

struct vertex {
	int id;
	double x, y;
};

struct quadedge;

struct edgeref {
    quadedge *e = nullptr;
    int r = 0;  // {0, 1, 2, 3}

    edgeref() = default;
    edgeref(quadedge *e, int r) : e(e), r(r) {}
    static edgeref make_edge();

    edgeref rot();
    edgeref rotinv();
	edgeref sym();

	edgeref lnext();
	edgeref rnext();
	edgeref dnext();

    vertex &org();
    vertex &dest();

    edgeref &onext();
    edgeref oprev();

    static void splice(edgeref a, edgeref b);
    static edgeref connect(edgeref a, edgeref b);
	static void delete_edge(edgeref e);
	static void swap(edgeref e);
};

struct edgerecord {
    vertex data;
    edgeref next;
};

struct quadedge {
    edgerecord es[4];
};

// debugging
std::ostream &operator<<(std::ostream &os, const vertex &v);
std::ostream &operator<<(std::ostream &os, const edgeref &ref);
std::ostream &operator<<(std::ostream &os, const edgerecord &rec);
std::ostream &operator<<(std::ostream &os, const quadedge &q);
