//
// Created by damitha on 12/29/19.
//

#ifndef WINZIG_TOKEN_H
#define WINZIG_TOKEN_H

#include <string>

// Token class
class Token {
private:
    // Type of the token
    int type;
    // Term of the token
    std::string term;
    // Line the token is located in
    int line;
    // The position in the line the token is in
    int position;

public:
    Token(int type1, std::string term1, int line1, int postion1);
    Token();

    int getType();
    void setType(int type1);

    std::string getTerm();
    void setTerm(std::string term1);

    int getLine();
    void setLine(int line1);

    int getPosition();
    void setPosition(int position1);
};


#endif //WINZIG_TOKEN_H
