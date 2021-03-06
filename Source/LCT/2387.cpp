#include <algorithm>
#include <cstdio>
int read() {
    int res = 0, c;
    do
        c = getchar();
    while (c < '0' || c > '9');
    while ('0' <= c && c <= '9') {
        res = res * 10 + c - '0';
        c = getchar();
    }
    return res;
}
const int size = 50005;
struct Node {
    int maxi, val, p, c[2];
    bool rev;
} T[size * 3];
int getPos(int u) {
    return u == T[T[u].p].c[1];
}
bool isRoot(int u) {
    int p = T[u].p;
    return T[p].c[0] != u && T[p].c[1] != u;
}
#define ls T[u].c[0]
#define rs T[u].c[1]
void pushDown(int u) {
    if (T[u].rev) {
        std::swap(ls, rs);
        T[ls].rev ^= 1;
        T[rs].rev ^= 1;
        T[u].rev = false;
    }
}
int choose(int u, int v) {
    return T[u].val > T[v].val ? u : v;
}
void update(int u) {
    T[u].maxi = choose(choose(T[ls].maxi, T[rs].maxi), u);
}
void connect(int u, int p, int c) {
    T[u].p = p;
    T[p].c[c] = u;
}
void rotate(int u) {
    int ku = getPos(u);
    int p = T[u].p;
    int kp = getPos(p);
    int pp = T[p].p;
    int t = T[u].c[ku ^ 1];
    T[u].p = pp;
    if (!isRoot(p))
        connect(u, pp, kp);
    connect(t, p, ku);
    connect(p, u, ku ^ 1);
    update(p);
    update(u);
}
void push(int u) {
    if (!isRoot(u)) push(T[u].p);
    pushDown(u);
}
void splay(int u) {
    push(u);
    while (!isRoot(u)) {
        int p = T[u].p;
        if (!isRoot(p)) rotate((getPos(u) == getPos(p)) ? p : u);
        rotate(u);
    }
}
void access(int u) {
    int v = 0;
    while (u) {
        splay(u);
        rs = v;
        update(u);
        v = u;
        u = T[u].p;
    }
}
void makeRoot(int u) {
    access(u);
    splay(u);
    T[u].rev ^= 1;
    pushDown(u);
}
void link(int u, int v) {
    makeRoot(u);
    access(v);
    splay(v);
    T[u].p = v;
}
void cut(int u, int v) {
    makeRoot(u);
    access(v);
    splay(v);
    T[u].p = T[v].c[0] = 0;
    update(v);
}
struct Edge {
    int u, v, a, b;
    bool operator<(const Edge &rhs) const {
        return a < rhs.a;
    }
} E[size * 2];
int fa[size];
int find(int u) {
    return fa[u] == u ? u : fa[u] = find(fa[u]);
}
int query(int u, int v) {
    makeRoot(u);
    access(v);
    splay(v);
    return T[v].maxi;
}
int main() {
    int n = read();
    int m = read();
    for (int i = 1; i <= n; ++i) fa[i] = i;
    for (int i = 1; i <= m; ++i) {
        E[i].u = read();
        E[i].v = read();
        E[i].a = read();
        E[i].b = read();
    }
    std::sort(E + 1, E + m + 1);
    int ans = 1 << 30;
    for (int i = 1; i <= m; ++i) {
        int u = E[i].u, v = E[i].v;
        int fu = find(u), fv = find(v);
        if (fu == fv) {
            int e = query(u, v);
            if (T[e].val > E[i].b) {
                int eid = e - n;
                cut(e, E[eid].u);
                cut(e, E[eid].v);
            } else
                continue;
        } else
            fa[fu] = fv;
        int id = n + i;
        T[id].val = E[i].b, T[id].maxi = id;
        link(id, u);
        link(id, v);
        if (find(1) == find(n)) ans = std::min(ans, E[i].a + T[query(1, n)].val);
    }
    if (ans == (1 << 30))
        puts("-1");
    else
        printf("%d\n", ans);
    return 0;
}
