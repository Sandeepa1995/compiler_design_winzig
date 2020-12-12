//
// Created by damitha on 12/30/19.
//

#ifndef WINZIG_PARSER_H
#define WINZIG_PARSER_H

#include <string>
#include <vector>

#include "Classes/Node.h"
#include "Classes/Token.h"

Node parser(std::vector<Token> *tokenStream, std::vector<std::string> *errors);

#endif //WINZIG_PARSER_H
