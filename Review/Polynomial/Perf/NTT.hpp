#pragma once
#include "TestLib.hpp"
#include <algorithm>
#include <cstring>
#include <vector>
using Int64 = long long;
#define asInt64 static_cast<Int64>
constexpr int mod = 998244353, size = 1 << 23;
int add(int a, int b) {
    a += b;
    return a < mod ? a : a - mod;
}
int sub(int a, int b) {
    a -= b;
    return a >= 0 ? a : a + mod;
}
int clamp(int a) {
    return a >= 0 ? a : a + mod;
}
Int64 powm(Int64 a, int k) {
    Int64 res = 1;
    while(k) {
        if(k & 1)
            res = res * a % mod;
        k >>= 1, a = a * a % mod;
    }
    return res;
}
typedef std::vector<int> Poly;
Poly rc[25], irc[25], rvc[25];
const Poly& getRev(int k) {
    if(rvc[k].empty()) {
        int n = 1 << k, off = k - 1;
        Poly& rev = rvc[k];
        rev.resize(n);
        for(int i = 1; i < n; ++i)
            rev[i] =
                rev[i >> 1] >> 1 | ((i & 1) << off);
    }
    return rvc[k];
}
const Poly& getRoot(int k) {
    if(rc[k].empty()) {
        int n = 1 << k;
        Int64 base = powm(3, (mod - 1) / n);
        Poly& root = rc[k];
        root.resize(n);
        root[0] = 1;
        for(int i = 1; i < n; ++i)
            root[i] = root[i - 1] * base % mod;
    }
    return rc[k];
}
const Poly& getInvRoot(int k) {
    if(irc[k].empty()) {
        int n = 1 << k;
        Int64 base = powm(3, (mod - 1) / n);
        Int64 invBase = powm(base, mod - 2);
        Poly& invR = irc[k];
        invR.resize(n);
        invR[0] = 1;
        for(int i = 1; i < n; ++i)
            invR[i] = invR[i - 1] * invBase % mod;
    }
    return irc[k];
}
void init() {
    for(int i = 0; i < 24; ++i) {
        getRev(i);
        getRoot(i);
        getInvRoot(i);
    }
}
typedef const Poly& (*Func)(int);
void NTT(int n, Poly& A, Func w) {
    int p = 0;
    while((1 << p) != n)
        ++p;
    const Poly& rev = getRev(p);
    for(int i = 0; i < n; ++i)
        if(i < rev[i])
            std::swap(A[i], A[rev[i]]);
    for(int i = 2, cp = 1; i <= n; i <<= 1, ++cp) {
        int m = i >> 1;
        const Poly& cw = w(cp);
        for(int j = 0; j < n; j += i)
            for(int k = 0; k < m; ++k) {
                int &x = A[j + k], &y = A[j + m + k];
                int t = asInt64(cw[k]) * y % mod;
                y = sub(x, t);
                x = add(x, t);
            }
    }
}
int* data(Poly& A) {
    return &*A.begin();
}
const int* data(const Poly& A) {
    return &*A.begin();
}
void copyPoly(Poly& dst, const Poly& src, int siz) {
    memcpy(data(dst), data(src), sizeof(int) * siz);
}
int getSize(int n) {
    n <<= 1;
    int p = 1;
    while(p < n)
        p <<= 1;
    return p;
}
void DFT(int n, Poly& A) {
    NTT(n, A, getRoot);
}
void IDFT(int n, Poly& A, int b, int e,
          bool clear = true) {
    NTT(n, A, getInvRoot);
    Int64 div = mod - (mod - 1) / n;
    for(int i = b; i < e; ++i)
        A[i] = A[i] * div % mod;
    if(clear) {
        memset(data(A), 0, sizeof(int) * b);
        memset(data(A) + e, 0, sizeof(int) * (n - e));
    }
}
Duration benchmarkNTT(int n) {
    int p = getSize(n);
    puts("Generating input data for NTT...");
    Poly A = genData(p, p, mod);
    puts("Calculating NTT...");
    Duration t = time([&] { NTT(p, A, getRoot); });
    puts("Done.");
    return t;
}
struct WrongAnswer {};
