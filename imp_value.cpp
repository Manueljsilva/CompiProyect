#include "imp_value.hh"

ImpValue::ImpValue():type(NOTYPE) { }

ImpVType ImpValue::get_basic_type(string s) {
  ImpVType tt;
  if (s.compare("int")==0) tt = TINT;
  else if (s.compare("bool")==0) tt = TBOOL;
  else if (s.compare("long")==0) tt = TLONG;
  else tt = NOTYPE;
  return tt;
}
void ImpValue::set_default_value(ImpVType tt) {
  type = tt;
  if (tt == TINT) {
    int_value = 0;
  } else if (tt == TBOOL) {
    bool_value = true;
  } else if (tt == TLONG) {
    long_value = 0;
  }
  return;
}

std::ostream& operator << ( std::ostream& outs, const ImpValue & v )
{
  if (v.type == TINT)
    outs << v.int_value;
  else if (v.type == TBOOL) {
    if (v.bool_value)
      outs << "true";
    else
      outs << "false";
  } else if (v.type == TLONG){
    outs << v.long_value;
  } else {
    outs << "NOTYPE";
  }
  return outs;
}


