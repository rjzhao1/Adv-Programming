// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get"  , cix_command::GET  },
   {"put", cix_command::PUT},
   {"rm",cix_command::RM},
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

void cix_get (client_socket& server,string filename) {
   cix_header header;
   header.command = cix_command::GET;
   copy(filename.begin(), filename.end(), header.filename);
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::FILEOUT) {
       outlog << "sent GET, server did not return FILEOUT" << endl;
       outlog << "server returned " << header << endl;
    }
   else {
        char* buffer = new char [header.nbytes + 1];
        recv_packet (server, buffer, header.nbytes);
        std::ofstream outfile (header.filename,std::ofstream::binary);
        buffer[header.nbytes] = '\0';
        outlog << "received " << header.nbytes << " bytes" << endl;
        outfile.write(buffer,header.nbytes);
        outfile.close();
  }
}

void cix_put (client_socket& server,string filename) {
   cix_header header;
   header.command = cix_command::PUT;
   copy(filename.begin(), filename.end(), header.filename);
   ifstream is (header.filename, std::ifstream::binary);
   if (is) {
     // get length of file:
     is.seekg (0, is.end);
     int length = is.tellg();
     is.seekg (0, is.beg);

     char * buffer = new char [length];
     // read data as a block:
     is.read (buffer,length);
     header.nbytes = length;
     send_packet(server, &header, sizeof header);
     send_packet(server,buffer,length);
     is.close();
   }else{
     outlog <<filename << " file does not exist." << endl;
   }
   recv_packet (server, &header, sizeof header);
    if (header.command != cix_command::ACK) {
        outlog << "sent PUT, server did not return ACK" << endl;
        outlog << "server returned " << header << endl;
     }
}

void cix_rm(client_socket& server,string filename){
  cix_header header;
  header.command = cix_command::RM;
  copy(filename.begin(), filename.end(), header.filename);
  send_packet(server,&header,sizeof header);
  recv_packet (server, &header, sizeof header);
  if (header.command != cix_command::ACK) {
      outlog << "sent PUT, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();

         string::size_type pos = line.find(' ');
         string file_name;
         if(line.npos != pos) {
             file_name = line.substr(pos + 1);
             line = line.substr(0,pos);
         }
         outlog << "command " << line << endl;
         const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         outlog << "command " << file_name << endl;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
              cix_get(server,file_name);
              break;
            case cix_command::PUT:
              cix_put(server,file_name);
              break;
            case cix_command::RM:
              cix_rm(server,file_name);
              break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}
