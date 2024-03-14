#include <algorithm>
#include <iostream>

class QuickUnion {
private:
    int* id;
    int* sz;
    bool* finished;
    int n;

public:
    QuickUnion(int* id, int n) : id(id), n(n) {
        sz = new int[n];
        for (int i = 0; i < n; i++) {
            sz[i] = 1;
        }
        finished = new bool[n];
        for (int i = 0; i < n; i++) {
            finished[i] = 1;
        }
    }

    ~QuickUnion() {
        delete[] id;
        delete[] sz;
        delete[] finished;
    }

    int* get_id() {
        return id;
    }

    void set_id(int* newId) {
        id = newId;
    }

    int get_cluster_leader(int i) {
        return root(i);
    }

    void set_finished(int i) {
        finished[root(i)] = true;
    }

    void reset_finished() {
        for (int i = 0; i < n; i++) {
            finished[i] = false;
        }
    }

    int root(int i) {
        while (i != id[i]) {
            i = id[i];
        }
        return i;
    }

    int flatten() {
        int* newId = new int[n];
        for (int i = 0; i < n; i++) {
            newId[i] = root(i);
        }

        delete[] id;
        id = newId;
    }

    bool is_finished(int i) {
        return finished[root(i)];
    }

    bool connected(int p, int q) {
        return root(p) == root(q);
    }

    bool safe_union(int p, int q) {
        int i = root(p);
        int j = root(q);

        if (i == j) {
            return true;
        }

        if (sz[i] < sz[j]) {
            id[i] = j;
            sz[j] += sz[i];
            return false;
        } else {
            id[j] = i;
            sz[i] += sz[j];
            return false;
        }
    }
};
