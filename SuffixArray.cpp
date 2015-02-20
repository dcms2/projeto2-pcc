#include <bits/stdc++.h>

using namespace std;

char A[1000];

bool cmp(int a, int b) {
    return strcmp(A+a, A+b) < 0;
}

class SuffixArray {
private:
    int* sa;
    char* T; int n;
    int* Llcp;
    int* Rlcp;

    struct tripla {
        int st, nd, idx;
        tripla() {}
        tripla(int st, int nd, int idx) : st(st), nd(nd), idx(idx) {}
        bool operator < (const tripla &rhs) const {
            if(st != rhs.st) return st < rhs.st;
            if(nd != rhs.nd) return nd < rhs.nd;
            return idx < rhs.idx;
        }
    };

    int calcLog() {
        int lg;
        for(lg = 0; (1<<lg) < n; ++lg);
        return lg;
    }

    void sort_index(int* p) {
        pair<int,int>* v = new pair<int,int>[n];
        for(int i = 0; i < n; ++i) 
            v[i] = make_pair(T[i], i);
        sort(v, v+n);
        int ord = 0;
        p[v[0].second] = ord;
        for(int i = 1; i < n; ++i) {
            if(v[i].first != v[i-1].first)
                ord++;
            p[v[i].second] = ord;
        }
        delete v;
    }   

    void build_sa() {
        this->sa = new int[n];
        int* p = new int[n];
        sort_index(p);
        tripla* v = new tripla[n];
        int lg = calcLog();
        for(int k = 1; k <= lg; ++k) {
            int j = (1<<(k-1));
            for(int i = 0; i < n; ++i) {
                if(i + j > n)
                    v[i] = tripla(p[i],0,i);
                else
                    v[i] = tripla(p[i], p[i+j], i);
            }
            sort(v, v+n);
            int ord = 0;
            p[v[0].idx] = ord;
            for(int i = 1; i < n; ++i) {
                if(!(v[i].st == v[i-1].st && v[i].nd == v[i-1].nd))
                    ord++;
                p[v[i].idx] = ord;
            }
        }
        for(int i = 0; i < n; ++i) sa[p[i]] = i;
        delete p;
        delete v;
    }

    int computeLCP(char* str1, char* str2) {
        int i;
        for(i = 0; str1[i] && str2[i] && str1[i] == str2[i]; ++i);
        return i;
    }

    void build_lcp(int b, int e) {
        if(e-b <= 1) return;
        int mid = (b+e)/2;
        Llcp[mid] = computeLCP(T+sa[b], T+sa[mid]);
        Rlcp[mid] = computeLCP(T+sa[e], T+sa[mid]);
        build_lcp(b, mid);
        build_lcp(mid, e);
    }

    int computeLW(char* W, int p) {
        int Lw;
        int l = computeLCP(T+sa[0], W);
        int r = computeLCP(T+sa[n-1], W);
        if(l == p || W[l] <= T[sa[0]+l])
            Lw = 0;
        else if(r < p && W[r] > T[sa[n-1]+r])
            Lw = n;
        else {
            int L = 0, R = n-1;
            while(R-L > 1) {
                int M = (L+R)/2;
                int m;
                if(l >= r) {
                    if(Llcp[M] >= l)
                        m = l+computeLCP(T+(sa[M]+l), W+l);
                    else 
                        m = Llcp[M];
                } else {
                    if(Rlcp[M] >= r)
                        m = r+computeLCP(T+(sa[M]+r), W+r);
                    else 
                        m = Rlcp[M];
                }
                if(m == p || W[m] <= T[sa[M]+m]) {
                    R = M;
                    r = m;
                } else {
                    L = M;
                    l = m;
                }
            }
            Lw = R;
        }
        return Lw;
    }   

    int computeRW(char* W, int p) {
        int Rw;
        int l = computeLCP(T+sa[0], W);
        int r = computeLCP(T+sa[n-1], W);
        if(l < p && W[l] < T[sa[0]+l])
            Rw = -1;
        else if(r == p || W[r] >= T[sa[n-1]+r])
            Rw = n-1;
        else {
            int L = 0, R = n-1;
            while(R-L > 1) {
                int M = (L+R)/2;
                int m;
                if(l >= r) {
                    if(Llcp[M] >= l)
                        m = l+computeLCP(T+(sa[M]+l), W+l);
                    else 
                        m = Llcp[M];
                } else {
                    if(Rlcp[M] >= r)
                        m = r+computeLCP(T+(sa[M]+r), W+r);
                    else 
                        m = Rlcp[M];
                }
                if(m < p && W[m] < T[sa[M]+m]) {
                    R = M;
                    r = m;
                } else {
                    L = M;
                    l = m;
                }
            }
            Rw = L;
        }
        return Rw;
    }

    

public:
    SuffixArray(char* T, int n) {
        this->T = new char[n];
        strcpy(this->T, T);
        this->n = n;
        strcpy(A, this->T);
        build_sa();
        this->Llcp = new int[n];
        this->Rlcp = new int[n];
        build_lcp(0, n-1);
    }

    pair<int,int> findPattern(char* W, int p) {
        int Lw = computeLW(W, p);
        int Rw = computeRW(W, p);
        return make_pair(Lw, Rw);
    }

};

int main(){
    char T[100010], W[100100];
    scanf("%s", T);
    SuffixArray sa = SuffixArray(T,  strlen(T));
    int q; scanf("%d", &q);
    while(q--) {
        scanf("%s", W);
        pair<int,int> ans = sa.findPattern(W, strlen(W));
        printf("%d\n", ans.second-ans.first+1);
    }
    return 0;
}