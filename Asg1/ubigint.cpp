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
  if(that==0){
    ubig_value.insert(ubig_value.begin(),0);
  }else{
    while (that>0) {
      ubig_value.insert(ubig_value.begin(),that%10);
      that /= 10;
    }
  }
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
  while (result.ubig_value.size() > 0 &&
         result.ubig_value.back() == 0){
    result.ubig_value.pop_back();
  }
  return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint diffResult;
   int len1 = this->ubig_value.size();
   int len2 = that.ubig_value.size();
   bool isCarry = false;
   int difference = 0;
   for(int i = 0; i < len1; i++){
      int num1 = this->ubig_value.at(i);
      if(i < len2){
        int num2 = that.ubig_value.at(i);
        if(isCarry == false){
          if(num1 >= num2){
            difference = num1 - num2;
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
        }else if(isCarry == true && num1 >= 1){
          isCarry = false;
          num1 -= 1;
          difference  = num1;
        }else{
          difference = num1;
        }
      }

      diffResult.ubig_value.push_back(difference);

   }
   while (diffResult.ubig_value.size() > 0 &&
          diffResult.ubig_value.back() == 0){
     diffResult.ubig_value.pop_back();
   }
   return diffResult;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint result;
   int len1 = this->ubig_value.size();
   int len2 = that.ubig_value.size();
   result.ubig_value.resize(len1+len2);
   int c = 0;
   int d = 0;
   for(int i = 0; i<len1;i++){
     c = 0;
     for(int j=0; j<len2;j++){
       d =result.ubig_value.at(i+j) +
          this->ubig_value.at(i)*that.ubig_value.at(j)+ c;
       result.ubig_value.at(i+j)=d%10;
       c = d/10;
     }
    result.ubig_value.at(i+len2) = c;
   }
   while (result.ubig_value.size() > 0 &&
          result.ubig_value.back() == 0){
     result.ubig_value.pop_back();
   }
   return result;
}

void ubigint::multiply_by_2() {
   int len = this->ubig_value.size();
   int carry = 0;
   int mult = 0;
   for(int i = 0; i< len;i++){
     mult = this->ubig_value.at(i)*2+carry;
     carry = 0;
     if(mult>=10){
       carry = mult/10;
       mult %=10;
     }
     ubig_value.at(i) = mult;
   }
   if(carry>=0){
     ubig_value.push_back(carry);
   }
   while (this->ubig_value.size() > 0 &&
          this->ubig_value.back() == 0){
     this->ubig_value.pop_back();
   }
}


  void ubigint::divide_by_2() {
     int length = this->ubig_value.size();
     int solNum = 0;
     int carryOver = 0;
     int nextNum = 0;
     for(int i = length-1; i >= 0; i--){
       int currentNum = this->ubig_value.at(i);
       nextNum = carryOver + currentNum;
       carryOver = 0;
         if(nextNum >= 2){
           solNum = nextNum/2;
           this->ubig_value.at(i) = solNum;
           if(nextNum%2 > 0){
             carryOver = 10;
           }
         }else if(nextNum == 1){
           this->ubig_value.at(i) = 0;
           carryOver = 10;
         }else{
           this->ubig_value.at(i) = 0;
         }
       }
     while (this->ubig_value.size() > 0 &&
            this->ubig_value.back() == 0){
       this->ubig_value.pop_back();
     }
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
  ubigint div = udivide (*this, that).quotient;
  while (div.ubig_value.size() > 0 &&
         div.ubig_value.back() == 0){
    div.ubig_value.pop_back();
  }
  return div;
}

ubigint ubigint::operator% (const ubigint& that) const {
  ubigint rem = udivide (*this, that).remainder;
  while (rem.ubig_value.size() > 0 &&
         rem.ubig_value.back() == 0){
    rem.ubig_value.pop_back();
  }
  return rem;
}

bool ubigint::operator== (const ubigint& that) const {
   int len1 = this->ubig_value.size();
   int len2 = that.ubig_value.size();
   if(len1==len2){
     for(int i = 0;i<len1;i++){
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
