#include <bits/stdc++.h>
#include "LZW.hpp"

using namespace std;

int main() {
  string s;
  getline(cin,s);
  string compressed = LZW::compress_text(s);
  cout << compressed << endl;
  cout << LZW::uncompress_text(compressed) << endl;
  return 0;
}