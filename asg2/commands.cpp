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
     throw command_error( "No Arguments" );
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
          contents = "";
       }else{
         throw command_error("One or more files does not exist");
       }
     }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr cwd = state.get_cwd();
   if(words.size()==1||words[1]=="/"){
     inode_ptr root = state.get_root();
     state.set_cwd(root);
   }else{
     string dir_name = "";
     wordvec path = split (words[1], "/");
     for(unsigned int i = 1; i< words.size();++i){
       dir_name+=words[i];
     }
     if(path.size()<=1){
       if(cwd->get_contents()->exist(dir_name)){
         inode_ptr new_dir =
            cwd->get_contents()->get_dir().at(dir_name);
         state.set_cwd(new_dir);
       }else{
         throw command_error("One or more files does not exist");
       }
     }else{
       if(cwd->get_contents()->has_path(path)){
         inode_ptr dir_loc = cwd;
         for(unsigned int i = 0; i < path.size()-1;i++){
           dir_loc = dir_loc->get_contents()->get_dir().at(path[i]);
         }
         if(dir_loc->get_contents()->exist(path[path.size()-1])){
           dir_loc =
             dir_loc->get_contents()->get_dir().at(path[path.size()-1]);
           state.set_cwd(dir_loc);
         }else{
           throw command_error("Directory does  exist");
         }
       }
     }
   }

}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   exit_status exit_stat;
   if(words.size()==1||words[1]=="0"){
     exit_stat.set(0);
   }else{
    int exit_num = atoi (words[1].c_str());
    exit_stat.set(exit_num);
   }
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
     throw command_error( "No Arguments" );
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
    throw command_error( "No Arguments" );
  }else{
    string name;
    wordvec path = split (words[1], "/");
    for(unsigned int i = 1; i <words.size();++i){
      name+=words[i];
    }
    inode_ptr cwd = state.get_cwd();
    std::cout << path << '\n';
    if(path.size()>1 && cwd->get_contents()->has_path(path)){
      inode_ptr dir_loc = cwd;
      for(unsigned int i = 0; i < path.size()-1;i++){
        dir_loc = dir_loc->get_contents()->get_dir().at(path[i]);
      }
      if(!dir_loc->get_contents()->exist(path[path.size()-1])){
        dir_loc->get_contents()->mkdir(path[path.size()-1]);
      }else{
        throw command_error("Directory already exist");
      }
    }else if(!cwd->get_contents()->exist(name)&&path.size()<=1){
        cwd->get_contents()->mkdir(name);
      }else if(cwd->get_contents()->exist(name)){
        throw command_error("Directory already exist");
      }else{
        throw command_error("No path found");
      }

  }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()<=1){
     throw command_error( "No Arguments" );
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
   if(cwd==state.get_root()){
     pwd.insert(0,"/");
   }else{
     while (cwd!=state.get_root()) {
     for(auto it = prev->get_contents()->get_dir().begin();
         it != prev->get_contents()->get_dir().end(); ++it){
           if(it->second==cwd){
             pwd.insert(0,it->first);
             pwd.insert(0,"/");
           }
         }
         cwd = prev;
         prev =cwd->get_contents()->get_dir().at("..");
       }
     }
     cout << pwd << "\n";
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()<=1){
     throw command_error( "No Arguments" );
   }else{
     string name = "";
     for(unsigned int i = 1; i <words.size()-1;++i){
       name+=words[i];
       name+="/";
     }
     wordvec path = split (words[1], "/");
     inode_ptr cwd = state.get_cwd();
     if(path.size()>1){
       if(cwd->get_contents()->has_path(path)){
         inode_ptr dir_loc = cwd;
         for(unsigned int i = 0; i < path.size()-1;i++){
           dir_loc = dir_loc->get_contents()->get_dir().at(path[i]);
         }
         if(dir_loc->get_contents()->exist(path[path.size()-1])){
          map<string,inode_ptr> sub_dir = dir_loc->get_contents()->get_dir();
          for(auto it = sub_dir.begin();it!=sub_dir.end();++it){
            if(it->first!="."&&it->first!=".."){
              if(it->second->get_contents()->is_dir()){
                if(it->second->get_contents()->size()>2){
                  name+=it->first;
                  name="rm/"+name;
                  path = split (name, "/");
                  fn_rm(state,path);
                }
              }
            }
          }
          dir_loc->get_contents()->remove(path[path.size()-1]);
         }else{
           throw command_error("Directory already exist");
         }

       }else{
         throw command_error( "No path found" );
       }
     }else{
       if(cwd->get_contents()->exist(path[0])){
         map<string,inode_ptr> sub_dir = cwd->get_contents()->get_dir();
         for(auto it = sub_dir.begin();it!=sub_dir.end();++it){
           if(it->first!="."&&it->first!=".."){
             if(it->second->get_contents()->is_dir()){
               if(it->second->get_contents()->size()>2){
                 name+=it->first;
                 name="rm/"+name;
                 path = split (name, "/");
                 fn_rm(state,path);
               }
             }
           }
         }
         cwd->get_contents()->remove(path[0]);
       }
     }
   }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()<=1){
     throw command_error( "No Arguments" );
   }else{
     string name = "";
     for(unsigned int i = 1; i <words.size()-1;++i){
       name+=words[i];
       name+="/";
     }
     wordvec path = split (words[1], "/");
     inode_ptr cwd = state.get_cwd();
     if(path.size()>1){
       if(cwd->get_contents()->has_path(path)){
         inode_ptr dir_loc = cwd;
         for(unsigned int i = 0; i < path.size()-1;i++){
           dir_loc = dir_loc->get_contents()->get_dir().at(path[i]);
         }
         if(dir_loc->get_contents()->exist(path[path.size()-1])){
          map<string,inode_ptr> sub_dir = dir_loc->get_contents()->get_dir();
          for(auto it = sub_dir.begin();it!=sub_dir.end();++it){
            if(it->first!="."&&it->first!=".."){
              if(it->second->get_contents()->is_dir()){
                if(it->second->get_contents()->size()>2){
                  name+=it->first;
                  name="rm/"+name;
                  path = split (name, "/");
                  fn_rm(state,path);
                }
              }
            }
          }
          dir_loc->get_contents()->remove(path[path.size()-1]);
         }else{
           throw command_error("Directory already exist");
         }

       }else{
         throw command_error( "No path found" );
       }
     }else{
       if(cwd->get_contents()->exist(path[0])){
         map<string,inode_ptr> sub_dir = cwd->get_contents()->get_dir();
         for(auto it = sub_dir.begin();it!=sub_dir.end();++it){
           if(it->first!="."&&it->first!=".."){
             if(it->second->get_contents()->is_dir()){
               if(it->second->get_contents()->size()>2){
                 name+=it->first;
                 name="rm/"+name;
                 path = split (name, "/");
                 fn_rm(state,path);
               }
             }
           }
         }
         cwd->get_contents()->remove(path[0]);
       }
     }
   }
}
