// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border"   , &interpreter::do_border   },
   {"move_by"   , &interpreter::do_move_by   },

};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"triangle"   , &interpreter::make_triangle   },
   {"equilateral", &interpreter::make_equilateral  },
   {"diamond"  , &interpreter::make_diamond },

};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}

void  interpreter::do_border(param begin, param end) {
  if(end-begin<2){
    throw runtime_error ( "border: not enough arguments");
  }

  shape::border_color =rgbcolor{begin[0]};
  shape::border_width =stof(begin[1]);
}

void  interpreter::do_move_by(param begin, param end) {
  DEBUGF ('f', range (begin, end));
  if(end-begin<1){
    throw runtime_error ( "move_by: not enough arguments");
  }
  window::pixels = stoi(begin[0]);
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   window::push_back (object (itor->second, where, color));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string words = "";
   auto font = fontcode.find(*begin);
   if (font == fontcode.end()) {
      throw runtime_error (*begin + ": no such font");
   }
   ++begin;
   while(begin!=end){
     words += (*begin);
     words += " ";
     ++begin;
   }
   return make_shared<text> (font->second, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = std::stof (*begin);
   ++begin;
   GLfloat height = std::stof (*begin);
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = std::stof (*begin);
   return make_shared<circle> (width);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vertex;
   while(begin!=end){
     GLfloat xpos = std::stof (*begin);
     ++begin;
     GLfloat ypos = std::stof (*begin);
     ++begin;
     vertex.push_back({xpos,ypos});
   }
   return make_shared<polygon> (vertex);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   GLfloat x_length = std::stof(*begin)/2.0;
   ++begin;
   GLfloat y_length = std::stof(*begin)/2.0;
   ++begin;
   DEBUGF ('f', range (begin, end));
   return make_shared<rectangle> (x_length, y_length);
}

shape_ptr interpreter::make_square (param begin, param end) {
  GLfloat length = stof(*begin)/2.0;
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (length);
}

shape_ptr interpreter::make_triangle (param begin, param end) {
  // auto temp = begin;
  // int length;
  // while (temp!=end) {
  //   ++length;++temp;
  // }
  if(end-begin<6){
    throw runtime_error ( "triangle: not enough arguments");
  }
  vertex_list vertex;
  for(int i = 0; i<3;i++){
    GLfloat xpos = std::stof (*begin);
    ++begin;
    GLfloat ypos = std::stof (*begin);
    ++begin;
    vertex.push_back({xpos,ypos});
  }
   DEBUGF ('f', range (begin, end));
   return make_shared<triangle> (vertex);
}
shape_ptr interpreter::make_equilateral (param begin, param end) {
   vertex_list vertex;
   GLfloat length = std::stof(*begin);
   GLfloat top_x = length/2.0;
   GLfloat top_y = sqrt(length*length-(length/2)*(length/2));
   vertex.push_back({0.0,0.0});
   vertex.push_back({length,0.0});
   vertex.push_back({top_x,top_y});
   DEBUGF ('f', range (begin, end));
   return make_shared<triangle> (vertex);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   GLfloat width = std::stof(*begin);
   ++begin;
   GLfloat height = std::stof(*begin);
   DEBUGF ('f', range (begin, end));
   return make_shared<diamond> (width, height);
}
