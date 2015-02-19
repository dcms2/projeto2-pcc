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

    void build_sa() {
        this->sa = (int*) malloc(this->n);
        for(int i = 0; i < n; ++i) sa[i] = i;
        sort(sa, sa+n, cmp);
        for(int i = 0; i < n; ++i) printf("%d %d %s\n", i, sa[i], T+sa[i]);
    }

    int computeLCP(char* str1, char* str2) {
        int i;
        for(i = 0; str1[i] && str2[i] && str1[i] == str2[i]; ++i);
        return i;
    }

    void build_lcp(int b, int e) {
        if(e-b <= 1) return;
        int mid = (b+e)/2;
        printf("%d %d %d\n", b, mid, e);
        printf("before %d %d %d\n", sa[b], sa[mid], sa[e]);
        Llcp[mid] = computeLCP(T+sa[b], T+sa[mid]);
        Rlcp[mid] = computeLCP(T+sa[e], T+sa[mid]);
        printf("after %d %d %d\n", sa[b], sa[mid], sa[e]);
        build_lcp(b, mid);
        build_lcp(mid, e);
    }

    int computeLW(char* W, int p) {
        int Lw;
        //printf("%d %d %s\n", n-1, sa[n-1], T+sa[n-1]);
        int l = computeLCP(T+sa[0], W);
        int r = computeLCP(T+sa[n-1], W);
        //printf("%d %d %s\n", n-1, sa[n-1], T+sa[n-1]);
        if(l == p || W[l] <= T[sa[0]+l])
            Lw = 0;
        else if(r < p && W[r] > T[sa[n-1]+r])
            Lw = n;
        else {
            int L = 0, R = n-1;
            //cout << "oi" << endl;
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
        //TODO
    }

    

public:
    SuffixArray(char* T, int n) {
        this->T = (char*) malloc(n+2);
        strcpy(this->T, T);
        this->n = n;
        strcpy(A, this->T);
        build_sa();
        //cout << n << endl;
        this->Llcp = (int*) malloc(n);
        this->Rlcp = (int*) malloc(n);
        //printf("%s\n", T+5);
        cout << sa[n-1] << endl;
        build_lcp(0, n-1);
        cout << sa[n-1] << endl;
    }

    int findPattern(char* W, int p) {
        /*int Lw = computeLW(W, p);
        int Rw = computeRW(W, p);
        return make_pair(Lw, Rw);*/
        return computeLW(W, p);
    }

};

int main(){
    char T[100], W[100];
    scanf("%s", T);
    SuffixArray sa = SuffixArray(T, strlen(T));
    int q; scanf("%d", &q);
    while(q--) {
        scanf("%s", W);
        //sa.call(T, W);
        //printf("%s %s\n", T, W);
        printf("%d\n", sa.findPattern(W, strlen(W)));
    }
    return 0;
}