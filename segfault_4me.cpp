#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "gzstream/gzstream.h"
#include <algorithm>
#include <vector>
#include <sys/time.h>
#include <random>
#include <cstdlib>
#include <climits>
#include <sstream>
#include <new>
#include "pstreams/pstream.h"
#include <cstring>
#include <sys/stat.h>
#include <thread>
#include <atomic>

/*
 * Marshall Whittaker / oxagast
 * gcc segfault_4me.cpp -lstdc++ -lz -std=gnu++11 -L./gzstream -lgzstream -o segfault_4me
 * ./segfault_4me commandname commandpath buffersize
 */

std::atomic<bool> ready (false);
std::atomic_flag wins = ATOMIC_FLAG_INIT;

std::string remove_chars(const std::string& source, const std::string& chars) {
  std::string result="";
  for (unsigned int i=0; i<source.length(); i++) {
    bool foundany=false;
    for (unsigned int j=0; j<chars.length() && !foundany; j++) {
      foundany=(source[i]==chars[j]);
    }
    if (!foundany) {
      result+=source[i];
    }
  }
  return (result);
}


bool file_exists(const std::string& filen) {
  struct stat buf;
  return (stat(filen.c_str(), &buf) == 0);
}


int rand_me_plz (int rand_from, int rand_to) {
  std::random_device rd;
  std::default_random_engine generator(rd());   // seed random
  std::uniform_int_distribution<int> distribution(rand_from,rand_to);  // get a random
  auto roll = std::bind ( distribution, generator );  // bind it so we can do it multiple times
  return(roll());  
}


char fortune_cookie () {
  std::string rand_char;
  std::string byte;
  char chr;
  const char* hexdigit = "0123456789abcdef";  // hex character makeup
  for(int hex_out_curl=0; hex_out_curl< 2; hex_out_curl+=2) {
    char hexxy_1 = hexdigit[rand_me_plz(0,16) % 16];  // get 1 part of hex digit
    char hexxy_2 = hexdigit[rand_me_plz(0,16) % 16];  // get 2 part of hex digit
    rand_char = std::to_string(hexxy_1) + std::to_string(hexxy_2);  // put them together
    byte = rand_char.substr(hex_out_curl,2);  // pull out one hex
    chr = (char) (int)strtol(byte.c_str(), NULL, 16);  // get the char
  }
  return(chr);
}


std::vector<std::string> get_flags_man(char* cmd, std::string man_loc) {
  std::string cmd_name(cmd);
  std::string filename;
  std::vector<std::string> opt_vec;
  filename = "/usr/share/man/man" + man_loc + "/" + cmd_name + "." + man_loc + ".gz"; // put the filename back together
  if (file_exists(filename) == true) {
    char* chr_fn = strdup(filename.c_str()); // change the filename to a char pointer
    igzstream in(chr_fn);  //  gunzip baby
    std::string gzline;
    std::regex start_of_opt_1 ("^(\\.?\\\\?\\w{2} )*(\\\\?\\w{2} ?)*(:?\\.B )*((?:(?:\\\\-)+\\w+)(?:\\\\-\\w+)*).*"); // hella regex... why you do this to me manpage?
    std::smatch opt_part_1;
    std::regex start_of_opt_2 ("^\\.Op Fl (\\w+) Ar.*");
    std::smatch opt_part_2;
    std::regex start_of_opt_3 ("^\\\\fB(-.*)\\\\fB.*");
    std::smatch opt_part_3;
    while(std::getline(in, gzline)){
      if (std::regex_match(gzline, opt_part_1, start_of_opt_1)) {  // ring 'er out
        std::string opt_1 = opt_part_1[4];
        std::string opt_release = (remove_chars(opt_part_1[4], "\\"));  // remove the fucking backslashes plz
        opt_vec.push_back(opt_release);
      }
      if (std::regex_match(gzline, opt_part_2, start_of_opt_2)) {
        std::string opt_2 = opt_part_2[1];
        opt_vec.push_back("-" + opt_2);
      }
      if (std::regex_match(gzline, opt_part_3, start_of_opt_3)) {
        std::string opt_3 = opt_part_3[1];
        opt_vec.push_back(opt_3);
      }
    }
  }
  else {
    std::cerr << "Could not find a manpage for that command..." << std::endl;
    exit(1);
  }
  std::sort(opt_vec.begin(), opt_vec.end());
  opt_vec.erase(unique(opt_vec.begin(), opt_vec.end()), opt_vec.end());
  return(opt_vec);
}


std::vector<std::string> get_flags_template(std::string filename) {
  int man_num;
  std::vector<std::string> opt_vec;
  std::string line;
  std::ifstream template_file (filename);
  if (template_file.is_open())
  {
    while (std::getline (template_file,line))
    {
      opt_vec.push_back(line);
    }
    template_file.close();
  }
  else { 
    std::cerr << "Could not open template file..." << std::endl;
    exit (1);
  }
  return(opt_vec);  
}

std::string trash_generator(int trash, int buf, std::string user_junk) {
  std::string junk = "";
  std::string hex_stuff;
  int trash_num;
  if (trash == 0) {                                            // kosher
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = "A" + junk; // put lots of As
    }
  }
  if (trash == 1) {
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = "9" + junk; // yadda yadda
    }
  }
  if (trash == 2) {
    char fortune = fortune_cookie(); // ditto for random
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = junk += fortune;
    }
  }
  if (trash == 3) {
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = junk += fortune_cookie();
    }
  }
  if (trash == 4) {                                            // front
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = "A" + junk; // put lots of As
    }
    junk = user_junk + junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(0,buf);
    else return ("OOR");
  }
  if (trash == 5) {
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = "9" + junk; // yadda yadda
    }
    junk = user_junk + junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(0,buf);
    else return ("OOR");
  }
  if (trash == 6) {
    char fortune = fortune_cookie(); // ditto for random
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = junk += fortune;
    }
    junk = user_junk + junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(0,buf);
    else return ("OOR");
  }
  if (trash == 7) {
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = junk += fortune_cookie();
    }
    junk = user_junk + junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(0,buf);
    else return ("OOR");
  }
  if (trash == 8) {
    for (trash_num = 0; trash_num < buf; trash_num++) {  // back
      junk = "A" + junk; // put lots of As
    }
    junk = junk + user_junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(junk.length()-buf);
    else return ("OOR");
  }
  if (trash == 9) {
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = "9" + junk; // yadda yadda
    }
    junk = junk + user_junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(junk.length()-buf);
    else return ("OOR");
  }
  if (trash == 10) {
    char fortune = fortune_cookie(); // ditto for random
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = junk += fortune;
    }
    junk = junk + user_junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(junk.length()-buf);
    else return ("OOR");
  }
  if (trash == 11) {
    for (trash_num = 0; trash_num < buf; trash_num++) {
      junk = junk += fortune_cookie();
    }
    junk = junk + user_junk;
    if (buf-user_junk.length() < junk.size()) junk = junk.substr(junk.length()-buf);
    else return ("OOR");
  }
  return(junk);
}



std::string make_garbage(int trash, int buf) {
  buf = buf-1;
  std::string all_junk;
  if (isatty(STDIN_FILENO)) {
    std::string user_stuff = "";
    all_junk = trash_generator(trash, buf, user_stuff);
  }
  else {
    std::string user_stuff;
    getline(std::cin, user_stuff);
    all_junk = trash_generator(trash, buf, user_stuff);
  }
  return(all_junk);
}

std::string execer(std::string the_cmd_str) {
  std::vector<std::string> errors;
  redi::ipstream in(the_cmd_str + " >&2", redi::pstreambuf::pstderr); // gotta put them to stderr
  std::string errmsg;                                                      // some os thing
  while (std::getline(in, errmsg)) {
    errors.push_back(errmsg);
  }
  if (errors.size() > 0) {
    return (errors[errors.size() - 1]);
  }
  else return ("NOSEG");                                                // damn it...
}


int match_seg(int buf_size, std::vector<std::string> opts, std::vector<std::string> spec_env, std::string path_str) {
  bool segged = false;
  if (file_exists(path_str) == true) {
    while (segged != true) {
      std::vector<std::string> junk_opts_env;
      std::vector<std::string> junk_opts;
      for( int cmd_flag_l = 0; cmd_flag_l < opts.size(); cmd_flag_l++) {  // loop around the options
        if (rand_me_plz(0,1) == 1) {   // roll tha die
          junk_opts.push_back(opts.at(cmd_flag_l));  // put the random arg in the vector
        }
      }
      for( int cmd_flag_l = 0; cmd_flag_l < spec_env.size(); cmd_flag_l++) {  // loop around the options
        if (rand_me_plz(0,1) == 1) {   // roll tha die
          junk_opts_env.push_back(spec_env.at(cmd_flag_l));  // put the random arg in the vector
        }
      }
      std::string env_str;
      for( std::vector<std::string>::const_iterator junk_opt_env = junk_opts_env.begin(); junk_opt_env != junk_opts_env.end(); ++junk_opt_env) { // loop through the vector of junk envs
        std::string oscar_env = make_garbage(rand_me_plz(0,11),buf_size);
        if (oscar_env != "OOR") {
          env_str = env_str + *junk_opt_env + oscar_env + " ";
        }
      }
      std::string sys_str = path_str + " ";
      for( std::vector<std::string>::const_iterator junk_opt = junk_opts.begin(); junk_opt != junk_opts.end(); ++junk_opt) { // loop through the vector of junk opts
        std::string oscar = make_garbage(rand_me_plz(0,11),buf_size);
        if (oscar != "OOR") {
          sys_str = sys_str + *junk_opt + " " + oscar + " ";  // add options and garbage
        }
      }
      sys_str = env_str + " " + sys_str;
      junk_opts.clear();
      junk_opts.shrink_to_fit();
      std::istringstream is_it_segfault(execer(sys_str)); // run it and grab stdout and stderr
      std::string sf_line;
      while (std::getline(is_it_segfault, sf_line)) {
        std::regex sf_reg ("(.*Segmentation fault.*|.*core dump.*)"); // regex for the sf
        std::smatch sf;
        if (regex_match(sf_line, sf, sf_reg)) {  // match segfault
          std::cout << "Segfaulted with: " << sys_str << std::endl; 
          segged = true;
        }
      }
    }
    return(0);
  }
  else {
    std::cerr << "Command not found at path..." << std::endl;
    exit(1);
  }
}


void thread_me (int id, int buf_size_int, std::vector<std::string> opts, std::vector<std::string> spec_env, std::string path_str) {
  while(!ready) {
    std::this_thread::yield();
  }      // wait for sig
  match_seg(buf_size_int, opts, spec_env, path_str);  // start in parallel
  if (!wins.test_and_set()) {
    exit(0);  // exit all threads cleanly on a segfault
  }
}


int main (int argc, char* argv[]) {
  std::string buf_size;
  char* man_chr;
  std::vector<std::string> opts;
  std::string path_str;
  std::vector<std::string> spec_env;
  int opt;
  std::string man_loc = "8";
  std::string mp;
  std::string template_file;
  bool template_opt = false;
  bool man_opt = false;
  int num_threads = 2;
  while ((opt = getopt(argc, argv, "m:p:t:e:c:f:b:")) != -1) {
    switch (opt) {
      case 't':
        template_opt = true;
        template_file = optarg;
        break;
      case 'c':
        path_str = optarg;
        break;
      case 'b':
        buf_size = optarg;
        break;
      case 'e':
        spec_env = get_flags_template(optarg);
        break;
      case 'p':
        man_loc = optarg;
        break;
      case 'm':
        man_opt = true;
        man_chr = optarg;
        break;
      case 'f':
        num_threads = std::atoi(optarg);
        break;
      default:
        std::cout
        << "Usage:" << std::endl
        << " " << argv[0] << " -t|m template_or_manpage -e env_var_file -c commandpath -b bufsize" << std::endl
        << " " << argv[0] << " -t template -c ./faulty -b 2048" << std::endl;
        exit(1);
    }  
  }
  if ((man_opt == true) && (template_opt == false)) {
    opts = get_flags_man(man_chr, man_loc);
  }
  else if ((man_opt == false) && (template_opt == true)) {
    opts = get_flags_template(template_file);
  }
  else if ((man_opt == true) && (template_opt == true)) {
    std::cerr << "The -t and -m options cannot be specified simultaniously..." << std::endl;
    exit(1);
  }
  else {
    std::cerr << "You must specify -t or -m..." << std::endl;
    exit(1);
  }
  std::istringstream b_size(buf_size);
  int is_int;
  if (!(b_size >> is_int)) {
    std::cerr << "Buffer size must be an integer..." << std::endl;
    exit(1);
  }
  char buf_char_maybe;
  if (b_size >> buf_char_maybe) {
    std::cerr << "Buffer size must be an integer..." << std::endl;
    exit(1);
  }
  else {
    int buf_size_int = std::stoi(buf_size);
    std::vector<std::thread> threads;
    for (int cur_thread=1; cur_thread <= num_threads; ++cur_thread) threads.push_back(std::thread(thread_me,cur_thread, buf_size_int, opts, spec_env, path_str));  // Thrift Shop
    ready = true;  // bout to go get me some threads
    for (auto& all_thread : threads) all_thread.join();  // is that your grandma's coat?
    return (0);
  }
}
