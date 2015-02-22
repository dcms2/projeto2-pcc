#ifndef TRIE_H
#define TRIE_H

#include <vector>

class Trie {
 public:
  static const int MAX_SIZE = 1 << 16;
  static const int FLUSH = MAX_SIZE-1;
  static const int END = MAX_SIZE-2;

  Trie() {
    trie.assign(MAX_SIZE + 3, Node()); // +3 due to cache issues
    init_trie();
  }

  void init_trie();
  int add_or_contains(unsigned char c, bool& reseted);
  int get_code_word();

 private:
  struct Node {
    Node() {}
    int code_word;
    int adj[256 + 3]; // +3 due to cache issues
  };

  void init_node(int node);

  int node_at, node_counter;
  std::vector<Node> trie;
};

void Trie::init_trie() {
  node_at = 0;
  node_counter = 1;
  init_node(0);
  bool trash;
  for (int i = 0; i < 256; ++i) {
    add_or_contains(i,trash);
    node_at = 0;
  }
}

int Trie::add_or_contains(unsigned char c, bool& reseted) {
  if (trie[node_at].adj[c] != -1) {
    node_at = trie[node_at].adj[c];
    reseted = false;
    return 0;
  } else {
    int code_word = trie[node_at].code_word;

    init_node(node_counter);
    trie[node_at].adj[c] = node_counter;
    trie[node_counter].code_word = node_counter-1;
    
    node_at = trie[0].adj[c];
    ++node_counter;
    
    if (node_counter == MAX_SIZE) {
      init_trie();
      node_at = trie[0].adj[c];
      reseted = true;
    } else {
      reseted = false;
    }
    
    return code_word;
  }
}

int Trie::get_code_word() {
  return trie[node_at].code_word;
}

void Trie::init_node(int node) {
  trie[node].code_word = -1;
  memset(trie[node].adj, -1, sizeof trie[node].adj);
}

#endif
