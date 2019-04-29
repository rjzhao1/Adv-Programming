// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()<1){
     throw std::invalid_argument( "No Arguments" );
   }else{
     string contents="";
     inode_ptr cwd = state.get_cwd();
     for(unsigned int i = 1; i <words.size();++i){
       if(cwd->get_contents()->exist(words[i])){
          inode_ptr read_file =
          cwd->get_contents()->get_dir().at(words[i]);
          wordvec file_content = read_file->get_contents()->readfile();
          for(unsigned int j = 0;j<file_content.size();++j){
            contents += file_content[j];
            contents += " ";
          }
          cout <<contents<< "\n";
       }else{
         throw file_error("One or more files does not exist");
       }
     }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()<=1){
     throw std::invalid_argument( "No Arguments" );
   }else{
     string name = words[1];
     string contents = "";
     inode_ptr cwd = state.get_cwd();
     wordvec write_content;
     for(unsigned int i = 2; i<words.size();++i){
       write_content.push_back(words[i]);
     }
     if(cwd->get_contents()->exist(name)){
      inode_ptr file = cwd->get_contents()->get_dir().at(name);
      file->get_contents()->writefile(write_content);
     }else{
       cwd->get_contents()->mkfile(name);
       inode_ptr file = cwd->get_contents()->get_dir().at(name);
       file->get_contents()->writefile(write_content);
     }
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
  if(words.size()<=1){
    throw std::invalid_argument( "No Arguments" );
  }else{
    string name;
    for(unsigned int i = 1; i <words.size();++i){
      name+=words[i];
      name+=" ";
    }
    inode_ptr cwd = state.get_cwd();
    if(cwd->get_contents()->exist(name)){
        cwd->get_contents()->mkdir(name);
      }
  }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()<=1){
     throw std::invalid_argument( "No Arguments" );
   }else{
     string p;
     for(unsigned int i = 1; i <words.size();++i){
       p+=words[i];
       p+=" ";
     }
     state.set_prompt(p);
   }
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr cwd = state.get_cwd();
   inode_ptr prev =cwd->get_contents()->get_dir().at("..");
   string pwd = "";
   while (prev!=state.get_root()) {
     for(auto it = prev->get_contents()->get_dir().begin();
         it != prev->get_contents()->get_dir().end(); ++it){
           if(it->second==cwd){
             pwd.insert(0,it->first);
             pwd.insert(0,"/");
           }

     }
   }
   if(prev==state.get_root()){
     pwd.insert(0,"/");
   }
   cout << pwd << "\n";
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
