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
    
    for (char c : text) {
      int code_word = dictionary.add_or_contains(c);
    
      if (code_word) {
        ret.push_back(char(code_word>>8));
        ret.push_back(char(code_word&255));
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
      dictionary[i] = string(1,i);
    }

    int size = 256;
    int code_word = (text[0] << 8) | text[1];

    string prev = string(1, (unsigned char) code_word);
    string ret = prev;

    for (int i = 2; i < text.size(); i += 2) {
      int code_word = (text[i] << 8) | text[i^1];
      string aux;
      if (code_word < 256 || valid[code_word] == valid_step) {
        aux = dictionary[code_word];
      } else if (code_word == size) {
        aux = prev + prev[0];
      } else {
        assert(false);
      }
      dictionary[size] = prev + aux[0];
      valid[size++] = valid_step;
      if (size == Trie::MAX_SIZE) {
        size = 256;
        ++valid_step;
      }
      ret += aux;
      prev = aux;
    }

    return std::move(ret);
  }
}

#endif
