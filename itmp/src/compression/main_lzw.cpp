#include <bits/stdc++.h>
#include "LZW.hpp"

using namespace std;

int main() {
  string input = "", s;
  bool first = true;
  while (getline(cin, s)) {
    if (!first) input += "\n";
    first = false;
    input += s;
  }
  cerr << "size input: " << input.size() << endl;
  string compressed = LZW::compress_text(input);
  //cout << compressed << endl;
  cerr << "size compressed: " << compressed.size() << endl;

  string uncompressed = LZW::uncompress_text(compressed);
  cout << uncompressed << endl;

//  assert(input == uncompressed);

  return 0;
}
