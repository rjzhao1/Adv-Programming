// $Id: bigint.cpp,v 1.78 2019-04-03 16:44:33-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   if((!this->is_negative && !that.is_negative)||
      (this->is_negative && that.is_negative)){
      bigint result = uvalue + that.uvalue;
      result.is_negative = this->is_negative;
      return result;
   }else{
     if(uvalue>that.uvalue){
       bigint result = uvalue - that.uvalue;
       result.is_negative = this->is_negative;
       return result;
     }else if(uvalue==that.uvalue){
       bigint result = 0;
       return result;
     }else{
      bigint result = uvalue - that.uvalue;
      result.is_negative = that.is_negative;
      return result;
      }

     }

   }

bigint bigint::operator- (const bigint& that) const {
  if((!this->is_negative && !that.is_negative)||
     (this->is_negative && that.is_negative)){
     if(uvalue>that.uvalue){
       bigint result = uvalue - that.uvalue;
       result.is_negative = this->is_negative;
      return result;
     }else if(uvalue==that.uvalue){
       bigint result = 0;
       return result;
     }else{
       bigint result = that.uvalue-uvalue;
       result.is_negative = !(this->is_negative);
       return result;
     }
  }else{
      bigint result = uvalue + that.uvalue;
      result.is_negative = this->is_negative;
      return result;
    }
}


bigint bigint::operator* (const bigint& that) const {
   bigint result = uvalue * that.uvalue;
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}
