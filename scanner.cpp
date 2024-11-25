#include <iostream>
#include <cstring>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s):input(s),first(0), current(0) { }


bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;
    while (current < input.length() &&  is_white_space(input[current]) ) current++;
    if (current >= input.length()) return new Token(Token::END);
    char c  = input[current];
    first = current;
    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        token = new Token(Token::NUM, input, first, current - first);
    }

    else if (isalpha(c)) {
        current++;
        while (current < input.length() && isalnum(input[current]))
            current++;
        string word = input.substr(first, current - first);
        if (word == "printf") {
            token = new Token(Token::PRINTF, word, 0, word.length());
        } else if (word == "if") {
            token = new Token(Token::IF, word, 0, word.length());
        } else if (word == "then") {
            token = new Token(Token::THEN, word, 0, word.length());
        } else if (word == "else") {
            token = new Token(Token::ELSE, word, 0, word.length());
        } else if (word == "endif") {
            token = new Token(Token::ENDIF, word, 0, word.length());
        } else if (word == "ifexp") {
            token = new Token(Token::IFEXP, word, 0, word.length());
        } else if (word == "while") {
            token = new Token(Token::WHILE, word, 0, word.length());
        } else if (word == "endwhile") {
            token = new Token(Token::ENDWHILE, word, 0, word.length());
        }
        else if (word == "do") {
            token = new Token(Token::DO, word, 0, word.length());
        }else if (word == "for") {
            token = new Token(Token::FOR, word, 0, word.length());
        }
        else if (word == "endfor") {
            token = new Token(Token::ENDFOR, word, 0, word.length());
        }
        else if (word == "true") {
            token = new Token(Token::TRUE, word, 0, word.length());
        }
        else if (word == "false") {
            token = new Token(Token::FALSE, word, 0, word.length());
        }
        else if (word == "return") {
            token = new Token(Token::RETURN, word, 0, word.length());
        }
        else if (word == "int") {
            token = new Token(Token::TYPE, word, 0, word.length());
        }
        else if (word == "main") {
            token = new Token(Token::MAIN, word, 0, word.length());
        }
        else if (word == "include") {
            token = new Token(Token::INCLUDE, word, 0, word.length());
        }
        else if (word == "long") {
            token = new Token(Token::TYPE, word, 0, word.length());
        }
        else {
            token = new Token(Token::ID, word, 0, word.length());
        }
    }

    else if (strchr("+-*/()=;,<>{}#", c)) {
        current++;  // Avanzar el cursor primero
        switch(c) {
            case '+':
                if (current < input.length() && input[current] == '+') {
                    token = new Token(Token::INCREMENT, "++", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::PLUS, c);
                }
                break;

            case '-': token = new Token(Token::MINUS, c); break;
            case '*': token = new Token(Token::MUL, c); break;
            case '/': token = new Token(Token::DIV, c); break;
            case ',': token = new Token(Token::COMA, c); break;
            case '(': token = new Token(Token::PI, c); break;
            case ')': token = new Token(Token::PD, c); break;
            case '{': token = new Token(Token::LBRACE, c); break ;
            case '}': token = new Token(Token::RBRACE, c); break ;
            case '#':
                while (current < input.length() && input[current] != '\n') {
                    current++;
                }
                return nextToken();
            case '=':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::EQ, "==", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::ASSIGN, c);
                }
                break;
            case '<':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::LE, "<=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::LT, c);
                }
                break;
            case '>':
                if (current < input.length() && input[current] == '=') {
                    token = new Token(Token::GE, input, first, 2);
                    current++;
                } else {
                    token = new Token(Token::GT, input, first, 1);
                }
                break;
            case ';': token = new Token(Token::PC, c); break;
            default:
                token = new Token(Token::ERR, c);
        }
        return token;  // Retornar el token aquí
    }

    else if (c == '"') {
        current++;  // Skip opening quote
        first = current;  // Start of string content

        while (current < input.length() && input[current] != '"') {
            if (input[current] == '%') {
                int format_start = current;
                current++;  // Move past %

                // Handle %ld\n
                if (current < input.length() && input[current] == 'l' &&
                    current + 1 < input.length() && input[current + 1] == 'd') {
                    current += 2;  // Move past 'ld'
                    if (current + 1 < input.length() &&
                        input[current] == '\\' && input[current + 1] == 'n') {
                        current += 2;  // Move past '\n'
                    }
                    current++;  // Skip closing quote
                    return new Token(Token::PRINTF_FORMAT, input, format_start, current - format_start - 1);
                }
                // Handle %d\n, %s\n, %f\n
                else if (current < input.length() &&
                        (input[current] == 'd' || input[current] == 's' || input[current] == 'f')) {
                    current++;  // Move past d/s/f
                    if (current + 1 < input.length() &&
                        input[current] == '\\' && input[current + 1] == 'n') {
                        current += 2;  // Move past '\n'
                    }
                    current++;  // Skip closing quote
                    return new Token(Token::PRINTF_FORMAT, input, format_start, current - format_start - 1);
                }
            }
            current++;
        }

        // Handle regular string (aunque en este caso no deberíamos llegar aquí para printf)
        int length = current - first;
        current++;  // Skip closing quote
        return new Token(Token::STRING, input, first, length);
    }

    else {
        token = new Token(Token::ERR, c);
        current++;
    }
    return token;
}

void Scanner::reset() {
    first = 0;
    current = 0;
}

Scanner::~Scanner() { }

void test_scanner(Scanner* scanner) {
    Token* current;
    while ((current = scanner->nextToken())->type != Token::END) {
        if (current->type == Token::ERR) {
            cout << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        } else {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END)" << endl;
    delete current;
}