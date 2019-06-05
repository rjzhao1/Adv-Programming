#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

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


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height)
   : polygon({  {-width, height}, {width, height},
      {width, -height}, {-width, -height} }) {

}

square::square (GLfloat width): rectangle (width, width) {
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
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
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

   vertex_list list = vertices;
   GLfloat x;
   GLfloat y;
   glBegin(GL_POLYGON);
   glColor3ubv(color.ubvec);

   for(int i = 0; i < vertices.size(); ++i){
     x = (list[i].xpos/2 + center.xpos);
     y = (list[i].ypos/2 + center.ypos);
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
