// $Id: file_sys.cpp,v 1.6 2018-06-27 14:44:57-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   inode root_dir = (file_type::DIRECTORY_TYPE);
   root = make_shared<inode>(root_dir);
   cwd = root;
   root->contents->set_root(root,root);
 }

 void inode_state::set_prompt (const string& prompt){
   prompt_ = prompt;
 }

 void inode_state::set_cwd (const inode_ptr& new_cwd){
   cwd = new_cwd;
 }
const string& inode_state::prompt() const { return prompt_; }



ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

void plain_file::set_root (inode_ptr,inode_ptr){
  throw file_error ( "Error: no map in plain_file" );
}

void directory::set_root (inode_ptr self,inode_ptr parent){
  dirents.emplace(".",self);
  dirents.emplace("..",parent);
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
  data = words;
  DEBUGF ('i', words);
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

map<string,inode_ptr> plain_file::get_dir (){
  throw file_error ("No maps in a plain file");
}

inode_ptr inode_state::get_cwd() const{
  return cwd;
}

inode_ptr inode_state::get_root() const{
  return root;
}

size_t directory::size() const {
   size_t size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

base_file_ptr inode::get_contents(){
  return contents;
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode dir_file = (file_type::DIRECTORY_TYPE);
   inode_ptr dir = make_shared<inode>(dir_file);
   dir->get_contents()->set_root(dir,dirents.at("."));
   dirents.emplace(dirname,dir);
   // for(auto it = dirents.begin();it != dirents.end(); ++it)
   // {
   //     std::cout << it->first << " " << it->second << "\n";
   // }
   // for(auto it = (dir->get_contents()->get_dir().begin());it != (dir->get_contents()->get_dir().end()); ++it)
   // {
   //     std::cout << it->first << " " << it->second << "\n";
   // }
   return dir;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   inode p_file = (file_type::PLAIN_TYPE);
   inode_ptr plain = make_shared<inode>(p_file);
   dirents.emplace(filename,plain);
   return plain;
}

bool directory::exist(const string& name){
  if(dirents.find(name)==dirents.end()){
    return false;
  }
  return true;
}

bool plain_file::exist(const string&){
  throw file_error ("No maps in a plain file");
}
