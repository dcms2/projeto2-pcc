#include <bits/stdc++.h>

using namespace std;

class SuffixArray {
private:
    int* sa;
    char* T; int n;
    int* Llcp;
    int* Rlcp;

    void build_sa() {
        //TODO
    }

    void build_lcp(int b, int e) {
        //TODO
    }

    int computeLCP(char* str1, char* str2) {
        int i;
        for(i = 0; str1[i] && str2[i] && str1[i] == str2[i]; ++i);
        return i-1;
    }

    int computeLW(char* W, int p) {
        int Lw;
        int l = computeLCP(T+sa[0], W);
        int r = computeLCP(T+sa[n-1], W);
        if(l == p || W[l] <= T[sa[0]+l])
            Lw = 0;
        else if(r < p || W[r] <= T[sa[n-1]+r])
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
        //TODO
    }

public:
    SuffixArray(char* T, int n) {
        this->T = (char*) malloc(n);
        strcpy(this->T, T);
        this->n = n;
        build_sa();
        this->Llcp = (int*) malloc(n);
        this->Rlcp = (int*) malloc(n);
        build_lcp(0, n-1);
    }

    pair<int,int> findPattern(char* W, int p) {
        int Lw = computeLW(W, p);
        int Rw = computeRW(W, p);
        return make_pair(Lw, Rw);
    }
};

int main(){
    char T[100];
    scanf("%s", T);
    SuffixArray sa = SuffixArray(T, strlen(T));
    return 0;
}