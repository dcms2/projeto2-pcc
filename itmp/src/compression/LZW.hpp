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

    ret.push_back(char(Trie::END>>8));
    ret.push_back(char(Trie::END&255));
    
    return std::move(ret);
  }

  string uncompress_text(const string& text, int& last_pos) {
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

      if (code_word == Trie::END) {
        last_pos = i+2;
        return std::move(ret);
      }

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
        std::cerr << "some of the files are not proper index files. Please check." << std::endl;
        return "";
      }

      dictionary[size] = prev + aux[0];
      valid[size++] = valid_step;

      ret += aux;
      prev = aux;
    }

    return std::move(ret);
  }

  string compress_all(const string& text, const int* pos, const int* Llcp, const int* Rlcp) {
    string compressed = compress_text(text);
    for (int i = 0; i < text.size()+1; ++i) {
      compressed.push_back(char(pos[i]>>24));
      compressed.push_back(char((pos[i]>>16)&255));
      compressed.push_back(char((pos[i]>>8)&255));
      compressed.push_back(char(pos[i]&255));
    }
    for (int i = 0; i < text.size()+1; ++i) {
      compressed.push_back(char(Llcp[i]>>24));
      compressed.push_back(char((Llcp[i]>>16)&255));
      compressed.push_back(char((Llcp[i]>>8)&255));
      compressed.push_back(char(Llcp[i]&255)); 
    }
    for (int i = 0; i < text.size()+1; ++i) {
      compressed.push_back(char(Rlcp[i]>>24));
      compressed.push_back(char((Rlcp[i]>>16)&255));
      compressed.push_back(char((Rlcp[i]>>8)&255));
      compressed.push_back(char(Rlcp[i]&255));
    }
    return std::move(compressed);
  }

  void uncompress_all(const string& compressed, string& text, vector<int>& pos, vector<int>& Llcp, vector<int>& Rlcp) {
    int last_pos = 0;
    text = uncompress_text(compressed, last_pos);
    int i = last_pos;
    for (int j = 0; j < text.size()+1; ++j) {
      int num = (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      pos.push_back(num);
    }
    for (int j = 0; j < text.size()+1; ++j) {
      int num = (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      Llcp.push_back(num);
    }
    for (int j = 0; j < text.size()+1; ++j) {
      int num = (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      num = (num << 8) | (unsigned char)(compressed[i++]);
      Rlcp.push_back(num);
    }
  }
}

#endif
