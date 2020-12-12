//
// Created by damitha on 12/29/19.
//
#include "scanner.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>

#include "Classes/ScannerStatus.h"

// Assume number of spaces in a tab to be 4 by default
int SPACES_IN_TAB = 4;

/* Function that checks the possible types a character could be and returns a possible type
 * params:  c - Character to check
 *          line - Line the character is found in
 *          position - Position in the line
 *          errorPointer - Pointer to the error vector
 *          tokenPointer - Pointer to the token vector
 */
ScannerStatus scannerTransition(char c, int line, int position,
                                std::vector<std::string> *errorPointer,
                                std::vector<Token> *tokenPointer) {

    ScannerStatus currentStatus = ScannerStatus(0, line, position);

    if (isalpha(c) || c == '_') {
        // If the character is the start of an identifier or keyword
        currentStatus.setType(1);
    } else if (isdigit(c)) {
        // If the character is the start of a Integer
        currentStatus.setType(2);
    } else if (c == '\'') {
        // If the character is the start of a char
        currentStatus.setType(3);
    } else if (c == '"') {
        // If the character is the start of a string
        currentStatus.setType(4);
    } else if (c == '#') {
        // If the character is the start of a single line comment
        currentStatus.setType(5);
    } else if (c == '{') {
        // If the character is the start of a comment block
        currentStatus.setType(6);
    } else if (c == ':' || c == '.' ||
               c == '<' || c == '>') {
        // If the character is the start of a non-alphabetical keyword that can have more than one character
        currentStatus.setType(7);
    } else if (c == ';' || c == ',' || c == '(' || c == ')' ||
               c == '+' || c == '-' || c == '*' || c == '/' ||
               c == '=') {
        // If the character is the start of a non-alphabetical keyword that has one character
        std::stringstream bufferStream;

        bufferStream << c;
        Token newToken = Token(7, bufferStream.str(), line, position);
        tokenPointer->push_back(newToken);

        currentStatus.setType(0);
    } else if (isspace(c)) {
        // If the character is a space
        if (c == '\n') {
            currentStatus.incrementLine(1);
            currentStatus.resetPosition();
        } else if (c == '\t') {
            int tabOffset = currentStatus.getPosition() / SPACES_IN_TAB;
            currentStatus.setPosition((tabOffset + 1) * SPACES_IN_TAB + 1);
        }
        currentStatus.setType(0);
    } else if (c == '}') {
        // ERROR - Closing of a comment section without an opening
        errorPointer->push_back(
                "Closing comment character '}' found without a starting comment character ('{') at line "
                + std::to_string(line)
                + ", and position "
                + std::to_string(position) + " in the line.");
    } else {
        // ERROR - Unknown character
        errorPointer->push_back("Unknown character '"
                                + std::string(1, c)
                                + "' in line "
                                + std::to_string(line)
                                + ", and position "
                                + std::to_string(position) + " in the line.");
    }
    return currentStatus;
}

// Check if a word is a keyword
std::string keywordFromIdenti(std::string token) {
    if (token == "program" ||
        token == "var" ||
        token == "const" ||
        token == "type" ||
        token == "function" ||
        token == "return" ||
        token == "begin" ||
        token == "end" ||
        token == "output" ||
        token == "if" ||
        token == "then" ||
        token == "else" ||
        token == "while" ||
        token == "do" ||
        token == "case" ||
        token == "of" ||
        token == "otherwise" ||
        token == "repeat" ||
        token == "for" ||
        token == "until" ||
        token == "loop" ||
        token == "pool" ||
        token == "exit" ||
        token == "mod" ||
        token == "and" ||
        token == "or" ||
        token == "not" ||
        token == "read" ||
        token == "succ" ||
        token == "pred" ||
        token == "chr" ||
        token == "ord" ||
        token == "eof") {
        return token;
    } else {
        return "<identifier>";
    }
}

/* Scanner
 * params:  path - Path to the input file
 *          error - Pointer to the error vector
 */
std::vector<Token> scanner(std::string path, std::vector<std::string> *errors) {
    // Vector to keep the tokens identified
    std::vector<Token> tokenStream;
    // Input file stream
    std::ifstream ifs(path.c_str());

    // Variables needed to create tokens with positioning
    std::stringstream bufferStream;
    int type = 0;
    char c;
    int line = 1;
    int position = 0;
    int lineStart = 0;
    int positionStart = 0;
    bool stateChange = false;

    while (ifs.get(c)) {
        position += 1;
        switch (type) {
            // Identifier or Aplhabetic keyword
            case 1: {
                if (!isalnum(c) && c != '_') {
                    std::string term = keywordFromIdenti(bufferStream.str());
                    Token newToken = Token(0, bufferStream.str(), lineStart, positionStart);
                    if (term == "<identifier>") {
                        newToken.setType(1);
                    } else {
                        newToken.setType(7);
                    }
                    tokenStream.push_back(newToken);

                    stateChange = true;
                } else {
                    bufferStream << c;
                }
                break;
            }
            // Integer
            case 2: {
                if (!isdigit(c)) {
                    Token newToken = Token(2, bufferStream.str(), lineStart, positionStart);
                    tokenStream.push_back(newToken);

                    stateChange = true;
                } else {
                    bufferStream << c;
                }
                break;
            }
            // Char
            case 3: {
                bufferStream << c;
                if (c == '\'') {
                    if (bufferStream.str().length() == 3) {
                        Token newToken = Token(3, bufferStream.str(), lineStart, positionStart);
                        tokenStream.push_back(newToken);
                    } else if (bufferStream.str().length() < 3) {
                        if(ifs.get(c)){
                            if (c == '\'') {
                                errors->push_back("Quote character defined in line " + std::to_string(lineStart) +
                                                  ", and position " + std::to_string(positionStart) + " in the line.");
                            } else {
                                errors->push_back("Empty character in line " + std::to_string(lineStart) +
                                                  ", and position " + std::to_string(positionStart) + " in the line.");
                                stateChange = true;
                            }
                        } else {
                            errors->push_back("Empty character in line " + std::to_string(lineStart) +
                                              ", and position " + std::to_string(positionStart) + " in the line.");
                        }
                    } else {
                        errors->push_back("String defined as character in line " + std::to_string(lineStart) +
                                          ", and position " + std::to_string(positionStart) + " in the line.");
                    }
                    type = 0;
                }
                break;
            }
            // String
            case 4: {
                bufferStream << c;
                if (c == '"') {
                    Token newToken = Token(4, bufferStream.str(), lineStart, positionStart);
                    tokenStream.push_back(newToken);

                    type = 0;
                }
                break;
            }
            // Single line comment
            case 5: {
                if (c == '\n') {
                    type = 0;
                    line += 1;
                    position = 0;
                }
                break;
            }
            // Comment block
            case 6: {
                if (c == '\n') {
                    line += 1;
                    position = 0;
                } else if (c == '\t'){
                    int tabOffset = position / SPACES_IN_TAB;
                    position = (tabOffset + 1) * SPACES_IN_TAB + 1;
                } else if (c == '}') {
                    type = 0;
                }
                break;
            }
            // Non-alphabetic keywords
            case 7: {
                bool addToken = false;
                if (bufferStream.str() == ":=") {
                    addToken = true;
                    if (c == ':') {
                        bufferStream << c;

                        type = 0;
                    } else {
                        stateChange = true;
                    }
                } else if (bufferStream.str() == ":") {
                    if (c == '=') {
                        bufferStream << c;
                    } else {
                        addToken = true;

                        stateChange = true;
                    }
                } else if (bufferStream.str() == ".") {
                    addToken = true;
                    if (c == '.') {
                        bufferStream << c;
                        type = 0;
                    } else {
                        stateChange = true;
                    }
                } else if (bufferStream.str() == "<") {
                    addToken = true;
                    if (c == '=' || c == '>') {
                        bufferStream << c;

                        type = 0;
                    } else {
                        stateChange = true;
                    }
                } else if (bufferStream.str() == ">") {
                    addToken = true;
                    if (c == '=') {
                        bufferStream << c;

                        type = 0;
                    } else {
                        stateChange = true;
                    }
                }

                if (addToken) {
                    Token newToken = Token(7, bufferStream.str(), lineStart, positionStart);
                    tokenStream.push_back(newToken);
                }
                break;
            }
            // Starting a new token
            default: {
                stateChange = true;
            }
        }
        if (stateChange) {
            bufferStream.str("");
            bufferStream << c;
            lineStart = line;
            positionStart = position;
            ScannerStatus currentStatus = scannerTransition(c, line, position,
                                                            errors, &tokenStream);
            type = currentStatus.getType();
            if (type == 0) {
                bufferStream.str("");
            }
            line = currentStatus.getLine();
            position = currentStatus.getPosition();
            stateChange = false;
        }
    }

    // Error handling regarding unclosed char, strings or comments
    if(type == 3){
        errors->push_back("Unclosed character starting at line " + std::to_string(lineStart) +
                          ", and position " + std::to_string(positionStart) + " in the line.");
    } else if(type == 4){
        errors->push_back("Unclosed string starting at line " + std::to_string(lineStart) +
                          ", and position " + std::to_string(positionStart) + " in the line.");
    } else if(type == 6){
        errors->push_back("Unclosed comment starting at line " + std::to_string(lineStart) +
                          ", and position " + std::to_string(positionStart) + " in the line.");
    }

    return tokenStream;
}