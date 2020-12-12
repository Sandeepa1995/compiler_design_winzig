//
// Created by damitha on 12/29/19.
//

#ifndef WINZIG_SCANNER_H
#define WINZIG_SCANNER_H

#include <string>
#include <vector>

#include "Classes/Token.h"

std::vector<Token> scanner(std::string path, std::vector<std::string> *errors);

#endif //WINZIG_SCANNER_H
