#include <bits/stdc++.h>
#include "LZW.hpp"

using namespace std;

int main() {
  string input;
  getline(cin, input);
  cerr << "size input: " << input.size() << endl;
  
  string compressed = LZW::compress_text(input);
  cout << compressed << endl;
  cerr << "size compressed: " << compressed.size() << endl;

  string uncompressed = LZW::uncompress_text(compressed);

  assert(input == uncompressed);

  return 0;
}
