#ifndef LZW_H
#define LZW_H

#include <string>
#include <vector>
#include "trie.hpp"

using std::string;

namespace LZW {
  string compress_text(const string& text) {
    Trie dictionary = Trie();
    string ret = "";
    bool reseted;

    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
      int code_word = dictionary.add_or_contains(*it,reseted);
      if (code_word) {
        ret.push_back(char(code_word >> 8));
        ret.push_back(char(code_word & 255));
        if (reseted) {
          ret.push_back(char(Trie::FLUSH >> 8));
          ret.push_back(char(Trie::FLUSH & 255));
        }
      }
    }
    
    int code_word = dictionary.get_code_word();

    if (code_word != -1) {
      ret.push_back(char(code_word >> 8));
      ret.push_back(char(code_word & 255));
    }

    ret.push_back(char(Trie::END >> 8));
    ret.push_back(char(Trie::END & 255));

    return std::move(ret);
  }

  string uncompress_text(const string& text, std::string::const_iterator& it) {
    std::vector<string> dictionary(Trie::MAX_SIZE+3);
    std::vector<int> valid(Trie::MAX_SIZE+3, 0);
    int valid_step = 1;
    
    for (int i = 0; i < 256; ++i) {
      dictionary[i] = string(1, (unsigned char) i);
    }

    int size = 256;
    string ret, prev;    
    bool reseted = true;

    while (it != text.end()) {
      int code_word = (unsigned char) *it; ++it;
      code_word = (code_word << 8) | (unsigned char) *it; ++it;

      if (code_word == Trie::END) {
        return std::move(ret);
      }

      if (reseted) {
        reseted = false;
        prev = string(1, (unsigned char) code_word);
        ret.append(prev);
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
        aux = prev;
        aux.push_back(*prev.begin());
      } else {
        std::cerr << "some of the files are not proper index files. Please check." << std::endl;
        return "";
      }

      dictionary[size] = prev;
      dictionary[size].push_back(*aux.begin());
      valid[size++] = valid_step;
      
      ret.append(aux);
      prev = aux;
    }

    return std::move(ret);
  }

  inline void serialize(string& s, const int* arr, int arr_size) {
    s.clear();
    int temp[10];
    for (int i = 0; i < arr_size; ++i) {
      int sz = 0, x = arr[i];
      while (x >= 10) {
        temp[sz++] = x % 10;
        x /= 10;
      }
      temp[sz++] = x;
      for (int j = sz-1; j >= 0; --j) s.push_back(char(temp[j] + '0'));
      s.push_back(' ');
    }
  }

  string compress_all(const string& text, const int* pos, const int* Llcp, const int* Rlcp) {
    string compressed = compress_text(text);
    string aux = "";
    serialize(aux, pos, text.size()+1);
    compressed += compress_text(aux);
    serialize(aux, Llcp, text.size()+1);
    compressed += compress_text(aux);
    serialize(aux, Rlcp, text.size()+1);
    compressed += compress_text(aux);
    return std::move(compressed);
  }

  inline void deserialize(const string& s, vector<int>& vec) {
    for (std::string::const_iterator it = s.begin(); it != s.end();) {
      int num = 0;
      while (*it != ' ') {
        num = (num << 3) + (num << 1) + *it - '0';
        ++it;
      }
      ++it;
      vec.push_back(num);
    }
  }

  void uncompress_all(const string& compressed, string& text, vector<int>& pos, vector<int>& Llcp, vector<int>& Rlcp) {
    std::string::const_iterator compressed_it = compressed.begin();
    text = uncompress_text(compressed, compressed_it);
    string aux = uncompress_text(compressed, compressed_it);
    deserialize(aux, pos);
    aux = uncompress_text(compressed, compressed_it);
    deserialize(aux, Llcp);
    aux = uncompress_text(compressed, compressed_it);
    deserialize(aux, Rlcp);
  }
}

#endif
