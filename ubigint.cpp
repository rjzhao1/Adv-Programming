// $Id: ubigint.cpp,v 1.16 2019-04-02 16:28:42-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"



ubigint::ubigint (unsigned long that) {
   DEBUGF ('~', this << " -> " << 0);

}

ubigint::ubigint (const string& that){
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      int dig = digit - '0';
      ubig_value.insert(ubig_value.begin(),dig);
   }

}

ubigint ubigint::operator+ (const ubigint& that) const {
  ubigint result;
  int len1 = this->ubig_value.size();
  int len2 = that.ubig_value.size();
  int sum = 0;
  int carry = 0;
  if(len1>len2){
    for(int i = 0; i<len2;i++){
      sum = this->ubig_value.at(i)+that.ubig_value.at(i)+carry;
      carry = 0;
      if(sum>=10){
        carry = 1;
        sum -= 10;
      }
      result.ubig_value.push_back(sum);
    }

    for(int i = len2; i<len1; i++){
      sum = this->ubig_value.at(i)+carry;
      carry = 0;
      if(sum>=10){
        carry = 1;
        sum -= 10;
      }
      result.ubig_value.push_back(sum);
    }
    if(carry>0){
      result.ubig_value.push_back(carry);
    }
  }else if(len1==len2){
    for(int i = 0; i<len2;i++){
      sum = this->ubig_value.at(i)+that.ubig_value.at(i)+carry;
      carry = 0;
      if(sum>=10){
        carry = 1;
        sum -= 10;
      }
      result.ubig_value.push_back(sum);
    }
    if(carry>0){
      result.ubig_value.push_back(carry);
    }
  }else{
    for(int i = 0; i<len1;i++){
      sum = this->ubig_value.at(i)+that.ubig_value.at(i)+carry;
      carry = 0;
      if(sum>=10){
        carry = 1;
        sum -= 10;
      }
      result.ubig_value.push_back(sum);
    }

    for(int i = len1; i<len2; i++){
      sum = that.ubig_value.at(i)+carry;
      carry = 0;
      if(sum>=10){
        carry = 1;
        sum -= 10;
      }
      result.ubig_value.push_back(sum);
    }
    if(carry>0){
      result.ubig_value.push_back(carry);
    }
  }

    return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");

   ubigint diffResult;
   int len1 = this->ubig_value.size();
   int len2 = that.ubig_value.size();
   bool isCarry = false;
   bool lastNumCarry = false;
   int nextNum;
   int num = this->ubig_value.at(0);
   int difference = 0;
   int carry = 0;
   int furthestInd;

   for(int i = 0; i < len1; i++){
      int num1 = this->ubig_value.at(i);
      //int num2 = that.ubig_value.at(i);
      if(i < len2){
        int num2 = that.ubig_value.at(i);
        if(isCarry == false){
          if(num1 > num2){
          }else if (num1 < num2){
            num1 += 10;
            isCarry = true;
            difference = num1 - num2;
          }
        }else{
          int tempDiff = (num1-1) - num2;
          if(num1 > num2 && tempDiff >= 0){
            difference = (num1-1) - num2;
            isCarry = false;
          }else{
            num1 += 9;
            isCarry = true;
            difference = num1 - num2;
          }
        }
      }else{
        if(isCarry == true && num1 == 0){
          difference = num1 += 9;
          lastNumCarry = true;
        }else if(isCarry == true && num1 >= 1){
          isCarry = false;
          num1 -= 1;
          difference  = num1;
        }else{
          difference = num1;
        }
      }

      cout << endl;
      diffResult.ubig_value.push_back(difference);

   }
   return diffResult;

}

ubigint ubigint::operator* (const ubigint& that) const {

}

void ubigint::multiply_by_2() {
   cout<<"hi"<<"\n";
}

void ubigint::divide_by_2() {
   cout<<"hi"<<"\n";
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {

}

ubigint ubigint::operator% (const ubigint& that) const {

}

bool ubigint::operator== (const ubigint& that) const {
   if(this->ubig_value.size()==that.ubig_value.size()){
     for(int i = 0;i<this->ubig_value.size();i++){
       if(this->ubig_value.at(i)!=that.ubig_value.at(i)){
         return false;
       }
     }
     return true;
   }else{
     return false;
   }
}

bool ubigint::operator< (const ubigint& that) const {
  if(this->ubig_value.size()<that.ubig_value.size()){
    return true;
  }else if(this->ubig_value.size()>that.ubig_value.size()){
    return false;
  }else{
    for(int i = this->ubig_value.size()-1;i>=0;i--){
      if(this->ubig_value.at(i)>that.ubig_value.at(i)){
        return false;
      }
      if(this->ubig_value.at(i)<that.ubig_value.at(i)){
        return true;
      }
    }
    return false;
  }
}

ostream& operator<< (ostream& out, const ubigint& that) {
  string output;
  for(int i = that.ubig_value.size()-1; i>=0;i--){
    output += that.ubig_value.at(i)+'0';
  }
  return out << output;
}
