extern "C" {
#include "predicates.h"
}
#include "quadedge.h"

#include <iostream>
#include <string>

using std::ostream;
using std::string;

vertex &edgeref::org() {
    return this->e->es[this->r].data;
}

vertex &edgeref::dest() {
    return this->sym().org();
}

edgeref edgeref::rot() {
    return {this->e, (this->r + 1) % 4};
}

edgeref edgeref::rotinv() {
    return {this->e, (this->r + 3) % 4};
}

edgeref edgeref::sym() {
    return {this->e, (this->r + 2) % 4};
}

// onext/oprev
edgeref &edgeref::onext() {
    return this->e->es[this->r].next;
}

edgeref edgeref::oprev() {
    return this->e->es[(this->r + 1) % 4].next.rot();
}

// convenience lnext/rnext/dnext
edgeref edgeref::lnext() {
    return this->rotinv().onext().rot();
}

edgeref edgeref::lprev() {
    return this->onext().sym();
}

edgeref edgeref::rprev() {
    return this->sym().onext();
}

edgeref edgeref::dprev() {
    return this->rotinv().onext().rotinv();
}

edgeref edgeref::rnext() {
    return this->rot().onext().rotinv();
}

edgeref edgeref::dnext() {
    return this->sym().onext().sym();
}

trianglerecord *&edgeref::lrec() {
    return this->e->es[this->r].face;
}

trianglerecord *&edgeref::rrec() {
    return this->sym().lrec();
}

void edgeref::assign_lrec(trianglerecord *t) {
    t->rep = *this;

    edgeref a = *this;
    edgeref b = a.lnext();
    edgeref c = b.lnext();

    a.lrec() = b.lrec() = c.lrec() = t;
}

bool edgeref::in_lrec(vertex &v) {
    edgeref a = *this;
    edgeref b = a.lnext();
    edgeref c = b.lnext();

    return !edgeref::rightof(a, v) && !edgeref::rightof(b, v) && !edgeref::rightof(c, v);
}

// topology methods
void edgeref::splice(edgeref a, edgeref b) {
    edgeref alpha = a.onext().rot(), beta = b.onext().rot();

    edgeref ta = a.onext(), tb = b.onext(), talpha = alpha.onext(), tbeta = beta.onext();

    a.onext() = tb;
    b.onext() = ta;
    alpha.onext() = tbeta;
    beta.onext() = talpha;
}

edgeref edgeref::connect(edgeref a, edgeref b) {
    edgeref e = edgeref::make_edge();
    e.org() = a.dest();
    e.dest() = b.org();
    splice(e, a.lnext());
    splice(e.sym(), b);
    return e;
}

void edgeref::fix_conflicts(std::vector<vertex *> &o, std::vector<trianglerecord *> n) {
    for (vertex *v : o) {
        // for each old vertex, reassign
        if (!v->loce)
            continue;

        for (trianglerecord *t : n) {
            if (t->rep.in_lrec(*v)) {
                // if in triangle
                t->conflicts.push_back(v);
                v->loce = t->rep.e;
                v->locr = t->rep.r;
                break;
            }
        }
    }
}

void edgeref::swap(edgeref e, bool fast) {
    std::vector<vertex *> old;

    if (fast) {
        trianglerecord *a = e.lrec();
        trianglerecord *b = e.rrec();

        if (a) {
            old.insert(old.end(), a->conflicts.begin(), a->conflicts.end());
            a->alive = false;
        }

        if (b) {
            old.insert(old.end(), b->conflicts.begin(), b->conflicts.end());
            b->alive = false;
        }
    }

    edgeref a = e.oprev();
    edgeref b = e.sym().oprev();

    splice(e, a);
    splice(e.sym(), b);

    splice(e, a.lnext());
    splice(e.sym(), b.lnext());

    e.org() = a.dest();
    e.dest() = b.dest();

    if (fast) {
        trianglerecord *c = new trianglerecord;
        trianglerecord *d = new trianglerecord;

        e.assign_lrec(c);
        e.sym().assign_lrec(d);

        edgeref::fix_conflicts(old, {c, d});
    }
}

edgeref edgeref::make_edge() {
    quadedge *q = new quadedge;

    // 0 = e, 2 = e.sym()
    // 1 = e.rot(), 3 = e.rotinv()
    q->es[0].next = edgeref(q, 0);
    q->es[1].next = edgeref(q, 3);
    q->es[2].next = edgeref(q, 2);
    q->es[3].next = edgeref(q, 1);

    return edgeref(q, 0);
}

void edgeref::delete_edge(edgeref e) {
    splice(e, e.oprev());
    splice(e.sym(), e.sym().oprev());
    delete e.e;
}

// geometric primitives
bool edgeref::rightof(edgeref e, vertex v) {
    // v right of e
    return orient2d(&v.x, &e.dest().x, &e.org().x) > 0;
}

bool edgeref::leftof(edgeref e, vertex v) {
    // v left of e
    return orient2d(&v.x, &e.org().x, &e.dest().x) > 0;
}

// debugging
ostream &operator<<(std::ostream &os, const vertex &v) {
    return os << "vertex(id=" << v.id << ", x=" << v.x << ", y=" << v.y << ")";
}

ostream &operator<<(std::ostream &os, const edgeref &ref) {
    return os << "edgeref(e=" << ref.e << ", r=" << ref.r << ")";
}

ostream &operator<<(std::ostream &os, const edgerecord &rec) {
    return os << "edgerecord(data=" << rec.data << ", next=" << rec.next;
}

ostream &operator<<(std::ostream &os, const quadedge &q) {
    for (int i = 0; i < 4; i++)
        os << "[" << i << "] " << q.es[i] << " / ";

    return os;
}
