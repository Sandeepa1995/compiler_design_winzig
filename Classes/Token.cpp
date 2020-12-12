//
// Created by damitha on 12/29/19.
//

#include "Token.h"

Token::Token(int type1, std::string term1, int line1, int position1) {
    type = type1;
    term = term1;
    line = line1;
    position = position1;
}

Token::Token() {
    type = 0;
    term = "";
    line = 0;
    position = 0;
}

// Getter and setters for type
int Token::getType(){
    return type;
};
void Token::setType(int type1){
    type = type1;
}

// Getter and setters for term
std::string Token::getTerm(){
    return term;
};
void Token::setTerm(std::string term1){
    term = term1;
}

// Getter and setters for line
int Token::getLine(){
    return line;
}
void Token::setLine(int line1){
    line = line1;
}

// Getter and setters for position
int Token::getPosition(){
    return position;
}
void Token::setPosition(int position1){
    position = position1;
}
