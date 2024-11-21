#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first, last);
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    switch (tok.type) {
        case Token::PLUS: outs << "TOKEN(PLUS)"; break;
        case Token::MINUS: outs << "TOKEN(MINUS)"; break;
        case Token::MUL: outs << "TOKEN(MUL)"; break;
        case Token::DIV: outs << "TOKEN(DIV)"; break;
        case Token::NUM: outs << "TOKEN(NUM)"; break;
        case Token::ERR: outs << "TOKEN(ERR)"; break;
        case Token::PD: outs << "TOKEN(PD)"; break;
        case Token::PI: outs << "TOKEN(PI)"; break;
        case Token::END: outs << "TOKEN(END)"; break;
        case Token::ID: outs << "TOKEN(ID)"; break;
        case Token::ASSIGN: outs << "TOKEN(ASSIGN)"; break;
        case Token::PC: outs << "TOKEN(PC)"; break;
        case Token::LT: outs << "TOKEN(LT)"; break;
        case Token::LE: outs << "TOKEN(LE)"; break;
        case Token::EQ: outs << "TOKEN(EQ)"; break;
        case Token::GE: outs << "TOKEN(GE)"; break;
        case Token::GT: outs << "TOKEN(GT)"; break;
        case Token::IF: outs << "TOKEN(IF)"; break;
        case Token::THEN: outs << "TOKEN(THEN)"; break;
        case Token::ELSE: outs << "TOKEN(ELSE)"; break;
        case Token::ENDIF: outs << "TOKEN(ENDIF)"; break;
        case Token::WHILE: outs << "TOKEN(WHILE)"; break;
        case Token::COMA: outs << "TOKEN(COMA)"; break;
        case Token::IFEXP: outs << "TOKEN(IFEXP)"; break;
        case Token::DO: outs << "TOKEN(DO)"; break;
        case Token::ENDWHILE: outs << "TOKEN(ENDWHILE)"; break;
        case Token::FOR : outs << "TOKEN(FOR)"; break;
        case Token::ENDFOR : outs << "TOKEN(ENDFOR)"; break;
        case Token::TRUE : outs << "TOKEN(TRUE)"; break;
        case Token::FALSE : outs << "TOKEN(FALSE)"; break;
        case Token::RETURN : outs << "TOKEN(RETURN)"; break;
        case Token::PRINTF : outs << "TOKEN(PRINTF)"; break;
        case Token::PRINTF_FORMAT : outs << "TOKEN(PRINTF_FORMAT)"; break;
        case Token::LBRACE : outs << "TOKEN(LBRACE)"; break;
        case Token::RBRACE : outs << "TOKEN(RBRACE)"; break;
        case Token::STRING : outs << "TOKEN(STRING)"; break;
        case Token::INCREMENT : outs << "TOKEN(INCREMENT)"; break;
        case Token::DECREMENT : outs << "TOKEN(DECREMENT)"; break;
        case Token::TYPE : outs << "TOKEN(TYPE)"; break;
        case Token::MAIN : outs << "TOKEN(MAIN)"; break;
        case Token::INCLUDE : outs << "TOKEN(INCLUDE)"; break;
        case Token::HASH : outs << "TOKEN(HASH)"; break;
        default: outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}