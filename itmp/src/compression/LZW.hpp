#ifndef LZW_H
#define LZW_H

#include <string>
#include <vector>
#include "trie.hpp"

using std::string;

#include <bits/stdc++.h>
using namespace std;

namespace LZW {
  string compress_text(const string& text) {
    Trie dictionary = Trie();
    string ret = "";
    bool reseted;

    for (char c : text) {
      int code_word = dictionary.add_or_contains(c,reseted);
    
      if (code_word) {
        ret.push_back(char(code_word>>8));
        ret.push_back(char(code_word&255));
        if (reseted) {
          ret.push_back(char(Trie::FLUSH>>8));
          ret.push_back(char(Trie::FLUSH&255));
        }
      }
    }
    
    int code_word = dictionary.get_code_word();

    if (code_word != -1) {
      ret.push_back(char(code_word>>8));
      ret.push_back(char(code_word&255));
    }
    
    return std::move(ret);
  }

  string uncompress_text(const string& text) {
    std::vector<string> dictionary(Trie::MAX_SIZE+3);
    std::vector<int> valid(Trie::MAX_SIZE+3, 0);
    int valid_step = 1;
    
    for (int i = 0; i < 256; ++i) {
      dictionary[i] = string(1, (unsigned char) i);
    }

    int size = 256;
    string ret, prev;    
    bool reseted = true;

    for (int i = 0; i < text.size(); i += 2) {
      int code_word = (((unsigned char) text[i]) << 8) | ((unsigned char) text[i^1]);

      if (reseted) {
        reseted = false;
        prev = string(1, (unsigned char) code_word);
        ret += prev;
        continue;
      }

      if (code_word == Trie::FLUSH) {
        size = 256;
        ++valid_step;
        reseted = true;
        continue;
      }

      string aux;
      if (code_word < 256 || valid[code_word] == valid_step) {
        aux = dictionary[code_word];
      } else if (code_word == size) {
        aux = prev + prev[0];
      } else {
        cerr << "i: " << i << endl;
        cerr << "chars: " << int(text[i]) << " " << int(text[i^1]) << endl;
        cerr << "code_word: " << code_word << endl;
        cerr << "size: " << size << endl;
        assert(false);
      }

      dictionary[size] = prev + aux[0];
      valid[size++] = valid_step;

      ret += aux;
      prev = aux;
    }

    return std::move(ret);
  }
}

#endif
