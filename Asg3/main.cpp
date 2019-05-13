// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"
#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

int main (int argc, char** argv) {


   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   str_str_map test;

   regex empty_regex {R"(^\s*?$)"};
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex find_regex {R"(^\s*([^=]+?)\s*$)"};
   regex eqv_regex {R"(^\s*[=]\s*$)"};
   regex key_regex {R"(^\s*(.+?)\s*=$)"};
   regex value_regex {R"(^=\s*(.+?)\s*$)"};

   smatch result;
   fstream infile;


   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      // str_str_pair pair (*argp, to_string<int> (argp - argv));
      // cout << "Before insert: " << pair << endl;

      infile.open(*argp);
      string str;
      if(infile.fail()){
        std::cout <<"File \""<< argv << "\" does not exist.\n";
      }
      while(getline(infile, str)){
        if(regex_search (str, result, comment_regex)){
          cout << "comment_regex" << '\n';
        }else if(regex_search (str, result, key_regex)){
          std::cout << "key_regex" << '\n';
        }else if(regex_search (str, result, find_regex)){
          std::cout << "find_regex" << '\n';
        }else if(regex_search (str, result, eqv_regex)){
          std::cout << "eqv_regex" << '\n';
        }else if(regex_search (str, result, value_regex)){
          std::cout << "value_regex" << '\n';
        }else if(regex_search (str, result, key_value_regex)){
          string key = result[1];
          string value = result[2];
          str_str_pair pair (key, value);
          test.insert(pair);
          std::cout << "key_value_regex" << '\n';
        }
      }
      infile.close();
   }

   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }
   std::cout << *test.end() << '\n';

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}
