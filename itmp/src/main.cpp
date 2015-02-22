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
    while (name.size() > pos) name.pop_back();
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

inline void search_number_occ(const std::vector<string>& indexes,
                              const std::vector<string>& patterns) {

  for (int i = 0; i < indexes.size(); ++i) {
    string text;
    vector<int> pos, Llcp, Rlcp;
    read_index(indexes[i], text, pos, Llcp, Rlcp);
    SuffixArray suffix_array = SuffixArray(text, pos, Llcp, Rlcp);
    std::cout << "File: " << indexes[i] << std::endl;
    for (int j = 0; j < patterns.size(); ++j) {
      char* pattern = new char[patterns[j].size()+1];
      strcpy(pattern, patterns[j].c_str());
      pair<int,int> par = suffix_array.findPattern(pattern);
      int x = par.second - par.first + 1;
      if (x != 1) std::cerr << "    found " << patterns[j] << " " << x << " times" << std::endl;
      else std::cerr << "    found " << patterns[j] << " 1 time" << std::endl;
      delete pattern;
    }
  }
}

inline void search_print_lines(const std::vector<string>& indexes,
                               const std::vector<string>& patterns) {

  std::cerr << "@TODO" << std::endl;
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
    {"pattern", required_argument, 0, 'p'},
    {0, 0, 0, 0}
  };

  int op;
  int has_help = 0, has_count = 0, has_pattern_file = 0;
  char* pattern_filename;

  while ((op = getopt_long(argc, argv, "hcp:", long_options, NULL)) != -1) {
    if (op == 'h') {
      has_help = 1;
    } else if (op == 'c') {
      has_count = 1;
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
      if (!is_index_file(vector_ptr->gl_pathv[i])) {
        std::cerr << string(vector_ptr->gl_pathv[i]) << " doesn't have a .idx extension, it won't be used to search for patterns." << std::endl;
      } else {
        filenames.push_back(vector_ptr->gl_pathv[i]);
      }
    }

    ++optind;
  }

  delete vector_ptr;

  if (search_mode) {
    if (has_count) search_number_occ(filenames, patterns);
    else search_print_lines(filenames, patterns);
  } else {
    make_index(filenames);
  }

  return 0;
}
