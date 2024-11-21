#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        PLUS, MINUS, MUL, DIV, NUM, ERR, PD, PI, END, ID, ASSIGN,
        PC,LT, LE, GE, GT, EQ, IF, THEN, ELSE, ENDIF,WHILE,DO,ENDWHILE,COMA,IFEXP,
        FOR, ENDFOR, TRUE, FALSE, RETURN, MAIN, INCLUDE, HASH,
        PRINTF , LBRACE , RBRACE ,  STRING , INCREMENT , DECREMENT, PRINTF_FORMAT, TYPE
    };

    Type type;
    std::string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const std::string& source, int first, int last);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

#endif // TOKEN_H