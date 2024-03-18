#include <algorithm>
#include <iostream>
#include "algo/algo.h"

QuickUnion::QuickUnion(int* id, int n) : id(id), n(n) {
    sz = new int[n];
    for (int i = 0; i < n; i++) {
        sz[i] = 1;
    }
    finished = new bool[n];
    for (int i = 0; i < n; i++) {
        finished[i] = false;
    }
}

QuickUnion::~QuickUnion() {
    delete[] id;
    delete[] sz;
    delete[] finished;
}

int* QuickUnion::get_id() {
    return id;
}

void QuickUnion::set_id(int* newId) {
    id = newId;
}

int QuickUnion::get_cluster_leader(int i) {
    return root(i);
}

void QuickUnion::set_finished(int i) {
    finished[root(i)] = true;
}

void QuickUnion::reset_finished() {
    for (int i = 0; i < n; i++) {
        finished[i] = false;
    }
}

int QuickUnion::root(int i) {
    while (i != id[i]) {
        i = id[i];
    }
    return i;
}

void QuickUnion::flatten() {
    int* newId = new int[n];
    for (int i = 0; i < n; i++) {
        newId[i] = root(i);
    }

    delete[] id;
    id = newId;
}

bool QuickUnion::is_finished(int i) {
    return finished[root(i)];
}

bool QuickUnion::safe_union(int p, int q) {
    int i = root(p);
    int j = root(q);

    if (i == j) {
        return false;
    }

    if (sz[i] < sz[j]) {
        id[i] = j;
        sz[j] += sz[i];
        return true;
    } else {
        id[j] = i;
        sz[i] += sz[j];
        return true;
    }
}
