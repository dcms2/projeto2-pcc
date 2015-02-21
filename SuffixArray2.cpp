#include <bits/stdc++.h>

using namespace std;

class SuffixArray {
private:
    int* pos;
    char* T; int n;

    int calcLog() {
        int lg;
        for(lg = 0; (1<<lg) < n; ++lg);
        return lg;
    }

    void sort_index_fast(int* arr) {
        pair<int,int>* v = new pair<int,int>[n];
        for(int i = 0; i < n; ++i)
            v[i] = make_pair(T[i],i);
        sort(v,v+n);
        for(int i = 0; i < n; ++i) {
            arr[i] = v[i].second;
        }
        delete v;
    }


    void build_sa_fast() {
        this->pos = new int[n];
        int* prm = new int[n];
        int* count = new int[n];
        bool* bh = new bool[n];
        bool* b2h = new bool[n];
        int* next = new int[n];

        for(int i = 0; i < n; ++i)
            pos[i] = i;

        sort_index_fast(pos);

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

            for(int i = 0; i < n; i = next[i]) {
                count[i] = 0;
                for(int j = i; j < next[i]; ++j)
                    prm[pos[j]] = i;
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

    int computeLW(char* W, int p) {
        int Lw;
        if(strncmp(W, T+pos[0], p) <= 0)
            Lw = 0;
        else if(strncmp(W, T+pos[n-1], p) > 0)
            Lw = n;
        else {
            int L = 0, R = n-1;
            int M;
            while(R-L>1) {
                M = (L+R)/2;
                if(strncmp(W, T+pos[M], p) <= 0)
                    R = M;
                else
                    L = M;
            }
            Lw = R;
        }
        return Lw;
    } 

    int computeRW(char* W, int p) {
        int Rw;
        if(strncmp(W, T+pos[0], p) < 0)
            Rw = -1;
        else if(strncmp(W, T+pos[n-1], p) >= 0)
            Rw = n-1;
        else {
            int L = 0, R = n-1;
            int M;
            while(R-L > 1) {
                M = (L+R)/2;
                if(strncmp(W, T+pos[M], p) >= 0)
                    L = M;
                else
                    R = M;
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
        build_sa_fast();
    }

    pair<int,int> findPattern(char* W, int p) {
        int Lw = computeLW(W, p);
        int Rw = computeRW(W, p);
        return make_pair(Lw, Rw);
    }

    int getVal(int i) { return pos[i]; }
};

char W[1000100], T[1000100];
int out[1000100];

int main(){
    int n;
    while(scanf("%d", &n) == 1) {
        scanf("%s", W);
        scanf("%s", T);
        SuffixArray sa = SuffixArray(T, strlen(T));
        pair<int,int> ans = sa.findPattern(W, strlen(W));
        int k = 0;
        for(int i = ans.first; i <= ans.second; ++i) 
            out[k++] = sa.getVal(i);
        sort(out,out+k);
        for(int i = 0; i < k; ++i) printf("%d\n", out[i]);
        printf("\n");
    }
    return 0;
}