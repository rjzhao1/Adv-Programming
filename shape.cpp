// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

rgbcolor shape::border_color{255,0,0};
GLfloat shape::border_width = 4;

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height)
   : polygon({  {height, 0}, {0, width},
      {-height, 0}, {0, -width} }) {

}



polygon::polygon (const vertex_list& vertices_):
   vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height)
   : polygon({  {-width, height}, {width, height},
      {width, -height}, {-width, -height} }) {

}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

triangle::triangle (const vertex_list& vertices_)
 :polygon (vertices_) {
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   const GLubyte* words =
         reinterpret_cast<const GLubyte*> (textdata.c_str());
   void* font = glut_bitmap_font;
   glColor3ubv (color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (font, words);
   draw_border(center,border_color);

}

void text::draw_border (const vertex& center,
  const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   const GLubyte* words =
         reinterpret_cast<const GLubyte*> (textdata.c_str());
   void* font = glut_bitmap_font;
   glBegin (GL_LINE_LOOP);
   glLineWidth(border_width);
   glColor3ubv (color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (font, words);
   glEnd();
}



void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv (color.ubvec);
   const GLfloat delta = 2 * M_PI / 64;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = dimension.xpos * cos (theta) + center.xpos;
      GLfloat ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   draw_border(center,border_color);
   glEnd();
}

void ellipse::draw_border (const vertex& center,
 const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_LINE_LOOP);
   glLineWidth(border_width);
   glColor3ubv (color.ubvec);
   const GLfloat delta = 2 * M_PI / 64;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = dimension.xpos * cos (theta) + center.xpos;
      GLfloat ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
    glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {

   GLfloat x;
   GLfloat y;
   GLfloat x_sum = 0.0;
   GLfloat y_sum = 0.0;
   GLfloat x_avg;
   GLfloat y_avg;

   for(unsigned int i = 0; i < vertices.size(); i++){
     x_sum += vertices[i].xpos;
     y_sum += vertices[i].ypos;
   }

   x_avg = x_sum/vertices.size();
   y_avg = y_sum/vertices.size();

   glBegin(GL_POLYGON);
   glColor3ubv(color.ubvec);

   for(unsigned int i = 0; i < vertices.size(); ++i){
     x = vertices[i].xpos - x_avg + center.xpos;
     y = vertices[i].ypos - y_avg + center.ypos;
     glVertex2f(x, y);
   }
   glEnd();
   draw_border(center,border_color);
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw_border (const vertex& center,
 const rgbcolor& color) const {
   GLfloat x;
   GLfloat y;
   GLfloat x_sum = 0.0;
   GLfloat y_sum = 0.0;
   GLfloat x_avg;
   GLfloat y_avg;

   for(unsigned int i = 0; i < vertices.size(); i++){
     x_sum += vertices[i].xpos;
     y_sum += vertices[i].ypos;
   }

   x_avg = x_sum/vertices.size();
   y_avg = y_sum/vertices.size();
   glLineWidth(border_width);
   glBegin (GL_LINE_LOOP);
   glColor3ubv(color.ubvec);

   for(unsigned int i = 0; i < vertices.size(); ++i){
     x = vertices[i].xpos - x_avg + center.xpos;
     y = vertices[i].ypos - y_avg + center.ypos;
     glVertex2f(x, y);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}
