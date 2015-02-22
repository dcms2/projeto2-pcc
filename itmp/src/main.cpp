#include <bits/stdc++.h>
#include <getopt.h>
#include <glob.h>
#include "matching/suffix_array.hpp"
#include "compression/LZW.hpp"

using std::string;

inline void help() {
  std::ifstream file_reader("../doc/help.txt");
  string line;
  while (std::getline(file_reader, line)) {
      printf("%s\n", line.c_str());
  }
}

inline string make_filename(string name, const string& ext) {
  int pos = name.find_last_of('.');
  if (pos == string::npos) {
    name += ext;
  } else {
    name.resize(pos);
    name += ext;
  }
  return std::move(name);
}

inline void create_indexfile(const string& filename, const string& input) {
  SuffixArray suffix_array = SuffixArray(input);
  string compressed = LZW::compress_all(input, suffix_array.get_pos(), suffix_array.get_Llcp(), suffix_array.get_Rlcp());
  std::ofstream file_writer(filename);
  file_writer << compressed << std::endl;
  file_writer.close();
}

inline void make_index(const std::vector<string>& filenames) {
  for (int i = 0; i < filenames.size(); ++i) {
    string new_name = make_filename(filenames[i], ".idx");
    std::ifstream file_reader(filenames[i]);
    if (!file_reader.is_open()) {
      std::cerr << "We couldn't open file \"" << filenames[i] << "\", maybe it doesn't exist." << std::endl;
    } else {
      string input = "", aux_input;
      bool first = true;
      while (std::getline(file_reader, aux_input)) {
        if (!first) input.push_back('\n');
        first = false;
        input += aux_input;
      }
      create_indexfile(new_name, input);
    }
  }
}

inline void read_index(const string& indexname,
                       string& text,
                       vector<int>& pos,
                       vector<int>& Llcp,
                       vector<int>& Rlcp) {
  std::ifstream file_reader(indexname);
  if (!file_reader.is_open()) {
    std::cerr << "We couldn't open file \"" << indexname << "\", maybe it doesn't exist." << std::endl;
  } else {
    string input = "", aux_input;
    bool first = true;
    while (std::getline(file_reader, aux_input)) {
      if (!first) input.push_back('\n');
      first = false;
      input += aux_input;
    }
    LZW::uncompress_all(input, text, pos, Llcp, Rlcp);
  }
}

char buffer[1000005], buffer2[1000005];

inline void search_number_occ(const std::vector<string>& indexes,
                              const std::vector<string>& patterns,
                              const bool together) {

  for (int i = 0; i < indexes.size(); ++i) {
    string text;
    vector<int> pos, Llcp, Rlcp;
    read_index(indexes[i], text, pos, Llcp, Rlcp);
    SuffixArray suffix_array = SuffixArray(text, pos, Llcp, Rlcp);
    if (!together) {
      printf("File: %s\n", indexes[i].c_str());
    }
    long long total = 0;
    for (int j = 0; j < patterns.size(); ++j) {
      char* pattern;
      if (patterns[j].size()+1 > 1000000) pattern = new char[patterns[j].size()+1];
      else pattern = buffer;
      strcpy(pattern, patterns[j].c_str());
      pair<int,int> par = suffix_array.findPattern(pattern);
      int x = par.second - par.first + 1;
      total += x;
      if (!together) {
        if (x != 1) printf("   found %s %d times\n", pattern, x);
        else printf("   found %s 1 time\n", pattern);
      }
      if (patterns[j].size()+1 > 100000) delete pattern;
    }
    if (together) {
      printf("%s:%lld\n", indexes[i].c_str(), total);
    }
  }
}

inline void print_line(const string& text, int p, const vector<int>& acc_lines) {
  int upb = upper_bound(acc_lines.begin(), acc_lines.end(), p) - acc_lines.begin();
  int begin = acc_lines[upb-1] + 1;
  int sz = 0;
  for (int i = begin; i < acc_lines[upb]; ++i) {
    buffer2[sz++] = text[i];
    if (sz == 1000000) {
      buffer2[sz] = 0;
      printf("%s", buffer2);
      sz = 0;
    }
  }
  buffer2[sz] = 0;
  printf("%s\n", buffer2);
}

inline void search_print_lines(const std::vector<string>& indexes,
                               const std::vector<string>& patterns,
                               const bool sorted) {

  for (int i = 0; i < indexes.size(); ++i) {
    string text;
    vector<int> pos, Llcp, Rlcp, acc_lines;
    read_index(indexes[i], text, pos, Llcp, Rlcp);
    
    acc_lines.push_back(-1);
    for (int j = 0; j < text.size(); ++j) {
      if (text[j] == '\n') acc_lines.push_back(j);
    }
    acc_lines.push_back(text.size());

    SuffixArray suffix_array = SuffixArray(text, pos, Llcp, Rlcp);
    for (int j = 0; j < patterns.size(); ++j) {
      char* pattern;
      if (patterns[j].size()+1 > 1000000) pattern = new char[patterns[j].size()+1];
      else pattern = buffer;
      strcpy(pattern, patterns[j].c_str());
      pair<int,int> par = suffix_array.findPattern(pattern);
      if (sorted) {
        vector<int> positions;
        for (int k = par.first; k <= par.second; ++k) {
          positions.push_back(suffix_array.get_pos()[k]);
        }
        sort(positions.begin(), positions.end());
        for (int k = 0; k < positions.size(); ++k) {
          printf("%s(%s):", indexes[i].c_str(), pattern);
          print_line(text,positions[k],acc_lines);
        }
      } else {
        for (int k = par.first; k <= par.second; ++k) {
          printf("%s(%s):", indexes[i].c_str(), pattern);
          print_line(text,suffix_array.get_pos()[k],acc_lines); 
        }
      }
      if (patterns[j].size()+1 > 1000000) delete pattern;
    }
  }
}

inline bool is_index_file(const string& s) {
  if (s.size() < 4) return false;
  int sz = s.size();
  return s[sz-1] == 'x' && s[sz-2] == 'd' && s[sz-3] == 'i' && s[sz-4] == '.';
}

int main(int argc, char **argv) {
  static struct option long_options[] = {
    {"help"   , no_argument      , 0, 'h'},
    {"count"  , no_argument      , 0, 'c'},
    {"all"    , no_argument      , 0, 'a'},
    {"sort"   , no_argument      , 0, 's'},
    {"pattern", required_argument, 0, 'p'},
    {0, 0, 0, 0}
  };

  int op;
  int has_help = 0, has_count = 0, has_pattern_file = 0, has_all = 0, sorted = 0;
  char* pattern_filename;

  while ((op = getopt_long(argc, argv, "hcasp:", long_options, NULL)) != -1) {
    if (op == 'h') {
      has_help = 1;
    } else if (op == 'c') {
      has_count = 1;
    } else if (op == 'a') {
      has_all = 1;
    } else if (op == 's') {
      sorted = 1;
    } else if (op == 'p') {
      pattern_filename = optarg;
      has_pattern_file = 1;
    }
  }

  if (has_help) {
    help();
    return 0;
  }

  bool search_mode = false;

  if (optind == argc) {
    std::cerr << "Please specify a mode: index or search." << std::endl;
    help();
    return 1;
  } else {
    if (strcmp(argv[optind], "search") == 0) {
      search_mode = true;
    } else if (strcmp(argv[optind], "index") != 0) {
      std::cerr << "Please specify a mode: index or search." << std::endl;
    }
    ++optind;
  }

  std::vector<string> patterns;

  if (has_pattern_file && search_mode) {
    std::ifstream file_reader(pattern_filename);
    string pattern;
    if (!file_reader.is_open()) {
      std::cerr << "We couldn't open file \"" << pattern_filename << "\", maybe it doesn't exist." << std::endl;
    } else {
      while (std::getline(file_reader, pattern)) {
        patterns.push_back(pattern);
      }
    }
  } else if (search_mode) {
    if (optind == argc) {
      std::cerr << "Please provide a pattern or a patternfile." << std::endl;
      help();
      return 1;
    }
    patterns.push_back(argv[optind]);
    ++optind;
  }

  if (optind == argc) {
    std::cerr << "Please provide a textfile." << std::endl;
    help();
    return 1;
  }

  std::vector<string> filenames;

  glob_t *vector_ptr = new glob_t;

  while (optind < argc) {
    glob(argv[optind], GLOB_NOSORT|GLOB_MARK, NULL, vector_ptr);

    for (int i = 0; i < vector_ptr->gl_pathc; i++) {
      if (!is_index_file(vector_ptr->gl_pathv[i]) && search_mode) {
        std::cerr << string(vector_ptr->gl_pathv[i]) << " doesn't have a .idx extension, it won't be used to search for patterns." << std::endl;
      } else {
        filenames.push_back(vector_ptr->gl_pathv[i]);
      }
    }

    ++optind;
  }

  delete vector_ptr;

  if (search_mode) {
    if (has_count || has_all) search_number_occ(filenames, patterns, has_all);
    else search_print_lines(filenames, patterns, sorted);
  } else {
    make_index(filenames);
  }

  return 0;
}
