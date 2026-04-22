#include <assert.h>
extern "C" {
#define TRILIBRARY
#define REAL double
#include "predicates.h"
#include "triangle.h"
}

#include <iostream>

struct vertex {
	int id;
	REAL x, y;
};

struct quadedge;

struct edgeref {
    quadedge *e = nullptr;
    int r = 0;  // {0, 1, 2, 3}

    edgeref() = default;
    edgeref(quadedge *e, int r) : e(e), r(r) {
    }
    static edgeref make_edge();

    edgeref rot() {
        return {this->e, (this->r + 1) % 4};
    }

    edgeref sym() {
        return {this->e, (this->r + 2) % 4};
    }

    edgeref rotinv() {
        return {this->e, (this->r + 3) % 4};
    }

	edgeref lnext() {
		return this->rotinv().onext().rot();
	}

	edgeref rnext() {
		return this->rot().onext().rotinv();
	}

	edgeref dnext() {
		return this->sym().onext().sym();
	}

    vertex &org();
    vertex &dest();

    edgeref &onext();
    edgeref oprev();

    static void splice(edgeref a, edgeref b) {
        edgeref alpha = a.onext().rot(), beta = b.onext().rot();

        edgeref ta = a.onext(), tb = b.onext(), talpha = alpha.onext(),
                tbeta = beta.onext();

        a.onext() = tb;
        b.onext() = ta;
        alpha.onext() = tbeta;
        beta.onext() = talpha;
    }

    static edgeref connect(edgeref a, edgeref b) {
        edgeref e = make_edge();
		e.org() = a.dest();
		e.dest() = b.org();
		splice(e, a.lnext());
		splice(e.sym(), b);
        return e;
    }

	static void delete_edge(edgeref e);

	static void swap(edgeref e) {
		edgeref a = e.oprev();
		edgeref b = e.sym().oprev();

		splice(e, a);
		splice(e.sym(), b);

		splice(e, a.lnext());
		splice(e.sym(), b.lnext());

		e.org() = a.dest();
		e.dest() = b.dest();
	}
};

struct edgerecord {
    vertex data;
    edgeref next;
};

struct quadedge {
    edgerecord es[4];
};

// need quadedge def
edgeref edgeref::make_edge() {
    quadedge *q = new quadedge;

    for (int i = 0; i < 4; i++) {
        q->es[i].next = edgeref(q, i);
    }

    return edgeref(q, 0);
}

void edgeref::delete_edge(edgeref e) {
	splice(e, e.oprev());
	splice(e.sym(), e.sym().oprev());
	delete e.e;
}

edgeref &edgeref::onext() {
    return this->e->es[this->r].next;
}

edgeref edgeref::oprev() {
    return this->e->es[(this->r + 1) % 4].next.rot();
}

vertex &edgeref::org() {
    return this->e->es[this->r].data;
}

vertex &edgeref::dest() {
    return this->sym().org();
}

// debugging
inline std::ostream &operator<<(std::ostream &os, const vertex &v) {
	return os << "vertex(id=" << v.id << ", x=" << v.x << ", y=" << v.y << ")";
}

inline std::ostream &operator<<(std::ostream &os, const edgeref &ref) {
    return os << "edgeref(e=" << ref.e << ", r=" << ref.r << ")";
}

inline std::ostream &operator<<(std::ostream &os, const edgerecord &rec) {
    return os << "edgerecord(data=" << rec.data << ", next=" << rec.next;
}

inline std::ostream &operator<<(std::ostream &os, const quadedge &q) {
    for (int i = 0; i < 4; i++)
        os << "[" << i << "] " << q.es[i] << " / ";

    return os;
}

using namespace std;

int main(void) {
    edgeref e = edgeref::make_edge();

    cout << e << endl;
    cout << e.onext() << endl;
    cout << e.oprev() << endl;

    cout << *e.e << endl;

    return 0;
}
