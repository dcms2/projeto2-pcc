#include <bits/stdc++.h>
#include <getopt.h>
#include <glob.h>

using std::string;

inline void help() {
  std::ifstream file_reader("../doc/help.txt");
  string line;
  while (std::getline(file_reader, line)) {
      printf("%s\n", line.c_str());
  }
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
        } else {
          fprintf(stderr, ">>> %d\n", op);
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

    std::cerr << "search_mode = " << search_mode << std::endl;

    std::vector<string> patterns;

    if (has_pattern_file) {
        std::ifstream file_reader(pattern_filename);
        string pattern;
        while (std::getline(file_reader, pattern)) {
            patterns.push_back(pattern);
        }
    } else {
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
            filenames.push_back(vector_ptr->gl_pathv[i]);
        }

        ++optind;
    }

    delete vector_ptr;

    if (search_mode) {
      //@TODO
    } else {
      //@TODO
    }

    /*

      //filenames in vector filenames
      //pattern in vector patterns
      //only number of occurrences iff has_count is 1

      std::cerr << "Files:" << std::endl;

      for (string file : fileNames) {
        std::cerr << file << std::endl;
      }

      std::cerr << std::endl;

      std::cerr << "Patterns:" << std::endl;

      for (string pattern : patterns) {
        std::cerr << pattern << std::endl;
      }

      std::cerr << std::endl;

      if (has_count) std::cerr << "Only count" << std::endl;
      else std::cerr << "All occurrences" << std::endl;

    */

    return 0;
}
