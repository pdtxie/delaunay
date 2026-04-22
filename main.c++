#include <assert.h>
extern "C" {
#define TRILIBRARY
#define REAL double
#include "predicates.h"
#include "triangle.h"
}

#include <iostream>

struct quadedge;

struct edgeref {
    quadedge *e = nullptr;
    int r = 0;  // {0, 1, 2, 3}

    edgeref() = default;
    edgeref(quadedge *e, int r) : e(e), r(r) {}
    static edgeref makeedge();

    edgeref rot() {
        return {this->e, (this->r + 1) % 4};
    }

    edgeref sym() {
        return {this->e, (this->r + 2) % 4};
    }

    edgeref rotinv() {
        return {this->e, (this->r + 3) % 4};
    }

    edgeref &onext();
    edgeref oprev();

    static void splice(edgeref a, edgeref b) {
        edgeref alpha = a.onext().rot(), beta = b.onext().rot();

        edgeref ta = a.onext(), tb = b.onext(), talpha = alpha.onext(),
                tbeta = beta.onext();

        a.onext() = tb;
        b.onext() = ta;
        a.onext().rot().onext() = tbeta;
        b.onext().rot().onext() = talpha;
    }
};

struct edgerecord {
    float data;
    edgeref next;
};

struct quadedge {
    edgerecord es[4];
};

// need quadedge def
edgeref edgeref::makeedge() {
    quadedge *q = new quadedge;

    for (int i = 0; i < 4; i++) {
        q->es[i].next = edgeref(q, i);
    }

	return edgeref(q, 0);
}

edgeref &edgeref::onext() {
    return this->e->es[this->r].next;
}

edgeref edgeref::oprev() {
    return this->e->es[(this->r + 1) % 4].next.rot();
}

inline std::ostream& operator<<(std::ostream& os, const edgeref& ref) {
	return os << "edgeref(e=" << ref.e << ", r=" << ref.r << ")";
}

inline std::ostream& operator<<(std::ostream& os, const edgerecord& rec) {
	return os << "edgerecord(data=" << rec.data << ", next=" << rec.next;
}

inline std::ostream& operator<<(std::ostream& os, const quadedge& q) {
	for (int i = 0; i < 4; i++)
		os << "[" << i << "] " << q.es[i] << " / ";

	return os;
}

using namespace std;

int main(void) {
	edgeref e = edgeref::makeedge();

	cout << e << endl;
	cout << e.onext() << endl;
	cout << e.oprev() << endl;

	cout << *e.e << endl;

    return 0;
}
