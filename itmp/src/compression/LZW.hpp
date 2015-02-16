#ifndef LZW_H
#define LZW_H

#include <string>
#include "trie.hpp"

using std::string;

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

  //TODO make this better, i.e. not using map
  //stolen from rosettacode: http://rosettacode.org/wiki/LZW_compression#C.2B.2B
  string uncompress_text(const string& text) {
    std::map<int,string> dictionary;
    int dictSize = 256;
    for (int i = 0; i < 256; ++i) dictionary[i] = string(1,i);
    string ret = "", w, entry;
    int code_word = (text[0] << 8) | text[1];
    w = string(1, (unsigned char) code_word);
    ret += w;
    for (int i = 2; i < text.size(); i += 2) {
      int code_word = (text[i] << 8) | text[i+1];
      if (dictionary.count(code_word)) entry = dictionary[code_word];
      else if (code_word == dictSize) entry = w + w[0];
      else assert(false);
      ret += entry;
      dictionary[dictSize++] = w + entry[0];
      w = entry;
    }
    return ret;
  }
}

#endif
