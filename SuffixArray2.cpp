#include <bits/stdc++.h>

using namespace std;

/* This is a n * logn implementation strongly based on the original
article by Manber & Mayers. Unfortunately, I wasn't able to perfectly
understand how they implemented the Llcp and Rlcp arrays using only O(n) memory,
but I came out with a way to do it using O(nlogn) memory*/

class SuffixArray {
private:
    int* pos;
    int** step_bucket;
    int* Llcp;
    int* Rlcp;
    int finish;
    char* T; int n;

    int calcLog() {
        int lg;
        for(lg = 0; (1<<lg) < n; ++lg);
        return lg;
    }

    void sortIndex(int* arr) {
        pair<int,int>* v = new pair<int,int>[n];
        for(int i = 0; i < n; ++i)
            v[i] = make_pair(T[i],i);
        sort(v,v+n);
        for(int i = 0; i < n; ++i) {
            arr[i] = v[i].second;
        }
        delete v;
    }

    /* This is mainly a replication of the article. 
    I used bucket sort to generate de suffix array. At the first step,
    buckets contain suffixes that have the same first letter. At step 
    H, suffixes in the same bucket cointain the same first H letters.
    The modification I did was the following: create an array of memory
    O(nlogn) to store, at each step, in which bucket the suffixes were.
    This information will be used later to compute the LCP between different
    suffixes in O(logn) time.*/

    void buildSA() {
        this->pos = new int[n];
        this->step_bucket = new int*[calcLog()];
        this->finish = -1;
        int* prm = new int[n];
        int* count = new int[n];
        bool* bh = new bool[n];
        bool* b2h = new bool[n];
        int* next = new int[n];

        for(int i = 0; i < n; ++i)
            pos[i] = i;

        sortIndex(pos);

        bh[0] = 1; b2h[0] = 0;

        for(int i = 1; i < n; ++i) {
            bh[i] = (T[pos[i]] != T[pos[i-1]]);
            b2h[i] = 0;
        }

        int lim = calcLog();
        for(int k = 0; k < lim; ++k) {
            int h = (1<<k);
            int buckets = 0;
            for(int i = 0; i < n; i++) {
                int j = i+1;
                while(j < n && !bh[j]) j++;
                next[i] = j;
                buckets++;
                i = j-1;
            }
            if(buckets == n) break;
            this->finish = k;
            this->step_bucket[this->finish] = new int[n];

            for(int i = 0; i < n; i = next[i]) {
                count[i] = 0;
                for(int j = i; j < next[i]; ++j){
                    this->step_bucket[this->finish][pos[j]] = i;
                    prm[pos[j]] = i;
                }
            }

            count[prm[n-h]]++;
            b2h[prm[n-h]] = 1;

            for(int i = 0; i < n; i = next[i]) {
                for(int j = i; j < next[i]; ++j) {
                    int tail = pos[j]-h;
                    if(tail >= 0) {
                        int actBucket = prm[tail];
                        prm[tail] = actBucket + count[actBucket]++;
                        b2h[prm[tail]] = 1;
                    }
                }
                for(int j = i; j < next[i]; ++j) {
                    int tail = pos[j]-h;
                    if(tail >= 0 && b2h[prm[tail]])
                        for(int x = prm[tail]+1; !bh[x] && b2h[x]; ++x)
                            b2h[x] = 0;
                    }
                }

            for(int i = 0; i < n; ++ i) {
                bh[i] = b2h[i];
                pos[prm[i]] = i;
            }
        }
        delete prm;
        delete bh;
        delete b2h;
        delete count;
        delete next;
    }

    /*We know, by definition, that if two suffixes are in the same bucket
    at the H-th step, then their first H letters are the same. We then use
    this information to calculate the LCP between then in a greedy way.*/

    int precalcLCP(int i, int j) {
        int lcp;
        if(i == j)
            lcp = n-i;
        else {
            int k = this->finish;
            lcp = 0;
            while(k >= 0 && i < n && j < n) {
                if(this->step_bucket[k][i] == this->step_bucket[k][j]) {
                    lcp += (1<<k);
                    i += (1<<k); j += (1<<k);
                }
                k--;
            }
        }
        return lcp;
    }

    int computeLCP(char* str1, char* str2) {
        int i;
        for(i = 0; str1[i] && str2[i] && (str1[i] == str2[i]); ++i);
        return i;
    }

    void buildLCP(int b, int e) {
        if(e-b <= 1) return;
        int mid = (b+e)/2;
        Llcp[mid] = precalcLCP(pos[b], pos[mid]);
        Rlcp[mid] = precalcLCP(pos[e], pos[mid]);
        buildLCP(b, mid);
        buildLCP(mid, e);
    }

    /*These are the binary searches O(p + logn) that uses information
    stored in arrays Llcp and Rlcp to avoid recomparison of letters, 
    like taugh in class*/

    int computeLW(char* W, int p) {
        int Lw;
        int l = computeLCP(T+pos[0], W);
        int r = computeLCP(T+pos[n-1], W);
        if(l == p || W[l] <= T[pos[0]+l])
            Lw = 0;
        else if(r < p && W[r] > T[pos[n-1]+r])
            Lw = n;
        else {
            int L = 0, R = n-1;
            while(R-L > 1) {
                int M = (L+R)/2;
                int m;
                if(l >= r) {
                    if(Llcp[M] >= l)
                        m = l+computeLCP(T+(pos[M]+l), W+l);
                    else 
                        m = Llcp[M];
                } else {
                    if(Rlcp[M] >= r)
                        m = r+computeLCP(T+(pos[M]+r), W+r);
                    else 
                        m = Rlcp[M];
                }
                if(m == p || W[m] <= T[pos[M]+m]) {
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
        int l = computeLCP(T+pos[0], W);
        int r = computeLCP(T+pos[n-1], W);
        if(l < p && W[l] < T[pos[0]+l])
            Rw = -1;
        else if(r == p || W[r] >= T[pos[n-1]+r])
            Rw = n-1;
        else {
            int L = 0, R = n-1;
            while(R-L > 1) {
                int M = (L+R)/2;
                int m;
                if(l >= r) {
                    if(Llcp[M] >= l)
                        m = l+computeLCP(T+(pos[M]+l), W+l);
                    else 
                        m = Llcp[M];
                } else {
                    if(Rlcp[M] >= r)
                        m = r+computeLCP(T+(pos[M]+r), W+r);
                    else 
                        m = Rlcp[M];
                }
                if(m < p && W[m] < T[pos[M]+m]) {
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
        this->T = new char[n+1];
        strcpy(this->T, T);
        this->T[n] = '$';
        this->n = n+1;
        buildSA();
        this->Llcp = new int[this->n];
        this->Rlcp = new int[this->n];
        buildLCP(0, (this->n)-1);
    }

    pair<int,int> findPattern(char* W, int p) {
        int Lw = computeLW(W, p);
        int Rw = computeRW(W, p);
        return make_pair(Lw, Rw);
    }

    int getVal(int i) { return pos[i]; }
};

char W[10010], T[100010];

int main(){
    int k; scanf("%d", &k);
    while(k--) {
        scanf("%s", T);
        SuffixArray sa = SuffixArray(T, strlen(T));
        int q; scanf("%d", &q);
        while(q--) {
            scanf("%s", W);
            //printf("%s %s\n", T, W);
            pair<int,int> ans = sa.findPattern(W, strlen(W));
            if(ans.second-ans.first >= 0) puts("y");
            else puts("n");
        }
    }
    return 0;
}