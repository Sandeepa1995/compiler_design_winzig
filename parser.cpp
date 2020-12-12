//
// Created by damitha on 12/30/19.
//

#include <map>
#include "parser.h"

// Map types as integers to stings
std::map<int, std::string> TYPE_MAP = { {1, "Identifier"},
                                        {2, "Integer"},
                                        {3, "Char"},
                                        {4, "String"},
                                        {7, "Keyword"}};

/* Read a given token (Remove from input token stream)
 * params: expectedType - Expected type
 *         expectedTerm - Expected term
 *         tokenStream - Pointer to the token vector
 *         stack - Pointer to the stack
 *         errors - Pointer to the error vector
 */
void readToken(int expectedType,
               std::string expectedTerm,
               std::vector<Token> *tokenStream,
               std::vector<Node> *stack,
               std::vector<std::string> *errors) {

    Token nextToken = tokenStream->at(0);
    if (expectedType == nextToken.getType()) {
        if (expectedType == 7) {
            if (expectedTerm == nextToken.getTerm()) {
                tokenStream->erase(tokenStream->begin());
            } else {
                errors->push_back("Expected the keyword '"
                                  + expectedTerm
                                  + "' but got '"
                                  + nextToken.getTerm()
                                  + "' at line "
                                  + std::to_string(nextToken.getLine())
                                  + ", and position "
                                  + std::to_string(nextToken.getPosition())
                                  + " in the line.");
                throw "Parser exception";
            }
        } else {
            tokenStream->erase(tokenStream->begin());
        }

        if (expectedType == 1
            || expectedType == 2
            || expectedType == 3
            || expectedType == 4) {
            Node newNode = Node(nextToken.getType(), nextToken.getTerm());
            stack->push_back(newNode);
        }
    } else {
        // Handle mismatch
        if(expectedType == 7){
            errors->push_back("Expected the keyword '"
                              + expectedTerm
                              + "' but got '"
                              + nextToken.getTerm()
                              + "' at line "
                              + std::to_string(nextToken.getLine())
                              + ", and position "
                              + std::to_string(nextToken.getPosition())
                              + " in the line.");
            throw "Parser exception";
        } else {
            errors->push_back("Expected the token of type "
                              + TYPE_MAP[expectedType]
                              + " but got "
                              + TYPE_MAP[nextToken.getType()]
                              + " at line "
                              + std::to_string(nextToken.getLine())
                              + ", and position "
                              + std::to_string(nextToken.getPosition())
                              + " in the line.");
            throw "Parser exception";
        }
    }
}

/* Build the AST
 * params: type - Type of the new node
 *         term - Term of the new node
 *         n - Number of nodes to pop and push to the new node
 *         stack - Pointer to the stack
 */
void buildTree(int type, std::string term, int n, std::vector<Node> *stack) {
    Node newNode = Node(type, term);

    for (int i = 0; i < n; i++) {
        newNode.addChiild(stack->back());
        stack->pop_back();
    }

    stack->push_back(newNode);
}


/* Handle Name Nonterminals
 */
void termName(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    readToken(1, "", tokenStream, stack, errors);
}

/* Handle ConstValue Nonterminals
 */
void termConstValue(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getType() == 1) {
        termName(tokenStream, stack, errors);
    } else if (nextToken.getType() == 2) {
        readToken(2, "", tokenStream, stack, errors);
    } else if (nextToken.getType() == 3) {
        readToken(3, "", tokenStream, stack, errors);
    }

}

/* Handle Const Nonterminals
 */
void termConst(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {

    termName(tokenStream, stack, errors);
    readToken(7, "=", tokenStream, stack, errors);
    termConstValue(tokenStream, stack, errors);

    buildTree(0, "const", 2, stack);
}

/* Handle Consts Nonterminals
 */
void termConsts(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {

    int constListLen = 1;
    Token nextToken = tokenStream->at(0);

    if (nextToken.getTerm() == "const") {
        readToken(7, "const", tokenStream, stack, errors);
        termConst(tokenStream, stack, errors);
        while (tokenStream->at(0).getTerm() == ",") {
            readToken(7, ",", tokenStream, stack, errors);
            termConst(tokenStream, stack, errors);
            constListLen += 1;
        }

        readToken(7, ";", tokenStream, stack, errors);

        buildTree(0, "consts", constListLen, stack);
    } else if (nextToken.getTerm() == "begin"
               || nextToken.getTerm() == "function"
               || nextToken.getTerm() == "var"
               || nextToken.getTerm() == "type") {

        buildTree(0, "consts", 0, stack);
    }
}

/* Handle LitList Nonterminals
 */
void termLitList(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int litListListLen = 1;

    readToken(7, "(", tokenStream, stack, errors);
    termName(tokenStream, stack, errors);
    while (tokenStream->at(0).getTerm() == ",") {
        readToken(7, ",", tokenStream, stack, errors);
        termName(tokenStream, stack, errors);
        litListListLen += 1;
    }
    readToken(7, ")", tokenStream, stack, errors);

    buildTree(0, "lit", litListListLen, stack);
}

/* Handle Type Nonterminals
 */
void termType(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    termName(tokenStream, stack, errors);
    readToken(7, "=", tokenStream, stack, errors);
    termLitList(tokenStream, stack, errors);

    buildTree(0, "type", 2, stack);
}

/* Handle Types Nonterminals
 */
void termTypes(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int typeListLen = 1;
    Token nextToken = tokenStream->at(0);

    if (nextToken.getTerm() == "type") {
        readToken(7, "type", tokenStream, stack, errors);
        termType(tokenStream, stack, errors);
        readToken(7, ";", tokenStream, stack, errors);
        while (tokenStream->at(0).getType() == 1) {
            termType(tokenStream, stack, errors);
            readToken(7, ";", tokenStream, stack, errors);
            typeListLen += 1;
        }

        buildTree(0, "types", typeListLen, stack);
    } else if (nextToken.getTerm() == "begin"
               || nextToken.getTerm() == "function"
               || nextToken.getTerm() == "var") {

        buildTree(0, "types", 0, stack);
    }
}

/* Handle Dcln Nonterminals
 */
void termDcln(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int dclnListLen = 1;
    termName(tokenStream, stack, errors);

    while (tokenStream->at(0).getTerm() == ",") {
        readToken(7, ",", tokenStream, stack, errors);
        termName(tokenStream, stack, errors);
        dclnListLen += 1;
    }
    readToken(7, ":", tokenStream, stack, errors);
    termName(tokenStream, stack, errors);

    buildTree(0, "var", dclnListLen + 1, stack);
}

/* Handle Dclns Nonterminals
 */
void termDclns(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getTerm() == "var") {
        int dclnsListLen = 1;

        readToken(7, "var", tokenStream, stack, errors);
        termDcln(tokenStream, stack, errors);
        readToken(7, ";", tokenStream, stack, errors);
        while (tokenStream->at(0).getType() == 1) {
            termDcln(tokenStream, stack, errors);
            readToken(7, ";", tokenStream, stack, errors);
            dclnsListLen += 1;
        }
        termConstValue(tokenStream, stack, errors);

        buildTree(0, "dclns", dclnsListLen, stack);

    } else if (nextToken.getTerm() == "begin"
               || nextToken.getTerm() == "function"
               || nextToken.getTerm() == ";") {

        buildTree(0, "dclns", 0, stack);
    }
}

/* Handle Params Nonterminals
 */
void termParams(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int paramsListLen = 1;
    termDcln(tokenStream, stack, errors);

    while (tokenStream->at(0).getTerm() == ";") {
        readToken(7, ";", tokenStream, stack, errors);
        termDcln(tokenStream, stack, errors);
        paramsListLen += 1;
    }

    buildTree(0, "params", paramsListLen, stack);
}

void termExpression(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors);

/* Handle Primary Nonterminals
 */
void termPrimary(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getTerm() == "-") {
        readToken(7, "-", tokenStream, stack, errors);
        termPrimary(tokenStream, stack, errors);

        buildTree(0, "-", 1, stack);

    } else if (nextToken.getTerm() == "+") {
        readToken(7, "+", tokenStream, stack, errors);
        termPrimary(tokenStream, stack, errors);

    } else if (nextToken.getTerm() == "not") {
        readToken(7, "not", tokenStream, stack, errors);
        termPrimary(tokenStream, stack, errors);

        buildTree(0, "not", 1, stack);

    } else if (nextToken.getTerm() == "eof") {
        readToken(7, "eof", tokenStream, stack, errors);

        buildTree(0, "eof", 0, stack);

    } else if (nextToken.getType() == 1) {
        termName(tokenStream, stack, errors);
        if (tokenStream->at(0).getTerm() == "(") {
            int expressionListLen = 1;

            readToken(7, "(", tokenStream, stack, errors);
            termExpression(tokenStream, stack, errors);
            while (tokenStream->at(0).getTerm() == ",") {
                readToken(7, ",", tokenStream, stack, errors);
                termExpression(tokenStream, stack, errors);
                expressionListLen += 1;
            }
            readToken(7, ")", tokenStream, stack, errors);

            buildTree(0, "call", 1 + expressionListLen, stack);
        }
    } else if (nextToken.getType() == 2) {
        readToken(2, "", tokenStream, stack, errors);
    } else if (nextToken.getType() == 3) {
        readToken(3, "", tokenStream, stack, errors);
    } else if (nextToken.getTerm() == "(") {
        readToken(7, "(", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, ")", tokenStream, stack, errors);

    } else if (nextToken.getTerm() == "succ") {
        readToken(7, "succ", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, ")", tokenStream, stack, errors);

        buildTree(0, "succ", 1, stack);

    } else if (nextToken.getTerm() == "pred") {
        readToken(7, "pred", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, ")", tokenStream, stack, errors);

        buildTree(0, "pred", 1, stack);

    } else if (nextToken.getTerm() == "chr") {
        readToken(7, "chr", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, ")", tokenStream, stack, errors);

        buildTree(0, "chr", 1, stack);

    } else if (nextToken.getTerm() == "ord") {
        readToken(7, "ord", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, ")", tokenStream, stack, errors);

        buildTree(0, "ord", 1, stack);
    }

}

/* Handle Factor Nonterminals
 */
void termFactor(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {

    termPrimary(tokenStream, stack, errors);

    Token nextToken = tokenStream->at(0);
    while (nextToken.getTerm() == "*"
           || nextToken.getTerm() == "/"
           || nextToken.getTerm() == "and"
           || nextToken.getTerm() == "mod") {
        if (nextToken.getTerm() == "*") {
            readToken(7, "*", tokenStream, stack, errors);
            termPrimary(tokenStream, stack, errors);

            buildTree(0, "*", 2, stack);
        } else if (nextToken.getTerm() == "/") {
            readToken(7, "/", tokenStream, stack, errors);
            termPrimary(tokenStream, stack, errors);

            buildTree(0, "/", 2, stack);
        } else if (nextToken.getTerm() == "and") {
            readToken(7, "and", tokenStream, stack, errors);
            termPrimary(tokenStream, stack, errors);

            buildTree(0, "and", 2, stack);
        } else if (nextToken.getTerm() == "mod") {
            readToken(7, "mod", tokenStream, stack, errors);
            termPrimary(tokenStream, stack, errors);

            buildTree(0, "mod", 2, stack);
        }
        nextToken = tokenStream->at(0);
    }
//    if (nextToken.getTerm() == "then"
//        || nextToken.getTerm() == "<="
//        || nextToken.getTerm() == "<"
//        || nextToken.getTerm() == ">="
//        || nextToken.getTerm() == ">"
//        || nextToken.getTerm() == "="
//        || nextToken.getTerm() == "<>"
//        || nextToken.getTerm() == "do"
//        || nextToken.getTerm() == "of"
//        || nextToken.getTerm() == ")"
//        || nextToken.getTerm() == ","
//        || nextToken.getTerm() == ";"
//        || nextToken.getTerm() == "end"
//        || nextToken.getTerm() == "else") {
//    }
}

/* Handle Term Nonterminals
 */
void termTerm(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    termFactor(tokenStream, stack, errors);

    Token nextToken = tokenStream->at(0);
    while (nextToken.getTerm() == "+"
           || nextToken.getTerm() == "-"
           || nextToken.getTerm() == "or") {
        if (nextToken.getTerm() == "+") {
            readToken(7, "+", tokenStream, stack, errors);
            termFactor(tokenStream, stack, errors);

            buildTree(0, "+", 2, stack);

        } else if (nextToken.getTerm() == "-") {
            readToken(7, "-", tokenStream, stack, errors);
            termFactor(tokenStream, stack, errors);

            buildTree(0, "-", 2, stack);

        } else if (nextToken.getTerm() == "or") {
            readToken(7, "or", tokenStream, stack, errors);
            termFactor(tokenStream, stack, errors);

            buildTree(0, "or", 2, stack);
        }
        nextToken = tokenStream->at(0);
    }
//    if (nextToken.getTerm() == "then"
//        || nextToken.getTerm() == "<="
//        || nextToken.getTerm() == "<"
//        || nextToken.getTerm() == ">="
//        || nextToken.getTerm() == ">"
//        || nextToken.getTerm() == "="
//        || nextToken.getTerm() == "<>"
//        || nextToken.getTerm() == "do"
//        || nextToken.getTerm() == "of"
//        || nextToken.getTerm() == ")"
//        || nextToken.getTerm() == ","
//        || nextToken.getTerm() == ";"
//        || nextToken.getTerm() == "end"
//        || nextToken.getTerm() == "else") {
//    }
}

/* Handle Expression Nonterminals
 */
void termExpression(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    termTerm(tokenStream, stack, errors);

    Token nextToken = tokenStream->at(0);
    if (nextToken.getTerm() == "<=") {
        readToken(7, "<=", tokenStream, stack, errors);
        termTerm(tokenStream, stack, errors);

        buildTree(0, "<=", 2, stack);

    } else if (nextToken.getTerm() == "<") {
        readToken(7, "<", tokenStream, stack, errors);
        termTerm(tokenStream, stack, errors);

        buildTree(0, "<", 2, stack);

    } else if (nextToken.getTerm() == ">=") {
        readToken(7, ">=", tokenStream, stack, errors);
        termTerm(tokenStream, stack, errors);

        buildTree(0, ">=", 2, stack);

    } else if (nextToken.getTerm() == ">") {
        readToken(7, ">", tokenStream, stack, errors);
        termTerm(tokenStream, stack, errors);

        buildTree(0, ">", 2, stack);

    } else if (nextToken.getTerm() == "=") {
        readToken(7, "=", tokenStream, stack, errors);
        termTerm(tokenStream, stack, errors);

        buildTree(0, "=", 2, stack);

    } else if (nextToken.getTerm() == "<>") {
        readToken(7, "<>", tokenStream, stack, errors);
        termTerm(tokenStream, stack, errors);

        buildTree(0, "<>", 2, stack);

    }
//    else if (nextToken.getTerm() == "then"
//               || nextToken.getTerm() == "do"
//               || nextToken.getTerm() == "of"
//               || nextToken.getTerm() == ")"
//               || nextToken.getTerm() == ","
//               || nextToken.getTerm() == ";"
//               || nextToken.getTerm() == "end"
//               || nextToken.getTerm() == "else") {
//    }
}

/* Handle StringNode Nonterminals
 */
void termStringNode(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    readToken(4, "", tokenStream, stack, errors);
}

/* Handle OutExp Nonterminals
 */
void termOutExp(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getTerm() == "+"
        || nextToken.getTerm() == "-"
        || nextToken.getTerm() == "not"
        || nextToken.getTerm() == "eof"
        || nextToken.getTerm() == "("
        || nextToken.getTerm() == "succ"
        || nextToken.getTerm() == "pred"
        || nextToken.getTerm() == "chr"
        || nextToken.getTerm() == "ord"
        || nextToken.getType() == 1
        || nextToken.getType() == 2
        || nextToken.getType() == 3) {
        termExpression(tokenStream, stack, errors);

        buildTree(0, "integer", 1, stack);
    } else if (nextToken.getType() == 4) {
        termStringNode(tokenStream, stack, errors);

        buildTree(0, "string", 1, stack);
    }
}

/* Handle Assignment Nonterminals
 */
void termAssignment(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    termName(tokenStream, stack, errors);
    if (tokenStream->at(0).getTerm() == ":=") {
        readToken(7, ":=", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);

        buildTree(0, "assign", 2, stack);
    } else if (tokenStream->at(0).getTerm() == ":=:") {
        readToken(7, ":=:", tokenStream, stack, errors);
        termName(tokenStream, stack, errors);

        buildTree(0, "swap", 2, stack);
    }

}

/* Handle ForStat Nonterminals
 */
void termForStat(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getType() == 1) {
        termAssignment(tokenStream, stack, errors);
    } else if (nextToken.getTerm() == ")" || nextToken.getTerm() == ";") {
        buildTree(0, "<null>", 0, stack);
    }
}

/* Handle ForExp Nonterminals
 */
void termForExp(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getTerm() == "+"
        || nextToken.getTerm() == "-"
        || nextToken.getTerm() == "not"
        || nextToken.getTerm() == "eof"
        || nextToken.getTerm() == "("
        || nextToken.getTerm() == "succ"
        || nextToken.getTerm() == "pred"
        || nextToken.getTerm() == "chr"
        || nextToken.getTerm() == "ord"
        || nextToken.getType() == 1
        || nextToken.getType() == 2
        || nextToken.getType() == 3) {
        termExpression(tokenStream, stack, errors);
    } else if (nextToken.getTerm() == ";") {
        buildTree(0, "true", 0, stack);
    }
}

/* Handle CaseExpression Nonterminals
 */
void termCaseExpression(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    termConstValue(tokenStream, stack, errors);
    if (tokenStream->at(0).getTerm() == "..") {
        readToken(7, "..", tokenStream, stack, errors);
        termConstValue(tokenStream, stack, errors);

        buildTree(0, "..", 2, stack);
    }
}

void termStatement(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors);

/* Handle Caseclause Nonterminals
 */
void termCaseclause(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int caseExpressionListLen = 1;

    termCaseExpression(tokenStream, stack, errors);
    while (tokenStream->at(0).getTerm() == ",") {
        readToken(7, ",", tokenStream, stack, errors);
        termCaseExpression(tokenStream, stack, errors);
        caseExpressionListLen += 1;
    }
    readToken(7, ":", tokenStream, stack, errors);
    termStatement(tokenStream, stack, errors);

    buildTree(0, "case_clause", caseExpressionListLen + 1, stack);

}

/* Handle Caseclauses Nonterminals
 */
int termCaseclauses(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int caseclauseListLen = 1;

    termCaseclause(tokenStream, stack, errors);
    readToken(7, ";", tokenStream, stack, errors);
    while (tokenStream->at(0).getType() == 1 || tokenStream->at(0).getType() == 2 ||
           tokenStream->at(0).getType() == 3) {
        termCaseclause(tokenStream, stack, errors);
        readToken(7, ";", tokenStream, stack, errors);
        caseclauseListLen += 1;
    }

    return caseclauseListLen;
}

void termBody(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors);

/* Handle Statement Nonterminals
 */
void termStatement(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    Token nextToken = tokenStream->at(0);

    if (nextToken.getType() == 1) {
        termAssignment(tokenStream, stack, errors);
    } else if (nextToken.getTerm() == "output") {
        int outExpListLen = 1;

        readToken(7, "output", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termOutExp(tokenStream, stack, errors);
        while (tokenStream->at(0).getTerm() == ",") {
            readToken(7, ",", tokenStream, stack, errors);
            termOutExp(tokenStream, stack, errors);
            outExpListLen += 1;
        }
        readToken(7, ")", tokenStream, stack, errors);

        buildTree(0, "output", outExpListLen, stack);

    } else if (nextToken.getTerm() == "if") {
        readToken(7, "if", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, "then", tokenStream, stack, errors);
        termStatement(tokenStream, stack, errors);
        if (tokenStream->at(0).getTerm() == "else") {
            readToken(7, "else", tokenStream, stack, errors);
            termStatement(tokenStream, stack, errors);

            buildTree(0, "if", 3, stack);
        } else {
            buildTree(0, "if", 2, stack);
        }

    } else if (nextToken.getTerm() == "while") {
        readToken(7, "while", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, "do", tokenStream, stack, errors);
        termStatement(tokenStream, stack, errors);

        buildTree(0, "while", 2, stack);

    } else if (nextToken.getTerm() == "repeat") {
        int statementListLen = 1;

        readToken(7, "repeat", tokenStream, stack, errors);
        termStatement(tokenStream, stack, errors);
        while (tokenStream->at(0).getTerm() == ";") {
            readToken(7, ";", tokenStream, stack, errors);
            termStatement(tokenStream, stack, errors);
            statementListLen += 1;
        }
        readToken(7, "until", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);

        buildTree(0, "repeat", statementListLen + 1, stack);

    } else if (nextToken.getTerm() == "for") {
        readToken(7, "for", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termForStat(tokenStream, stack, errors);
        readToken(7, ";", tokenStream, stack, errors);
        termForExp(tokenStream, stack, errors);
        readToken(7, ";", tokenStream, stack, errors);
        termForStat(tokenStream, stack, errors);
        readToken(7, ")", tokenStream, stack, errors);
        termStatement(tokenStream, stack, errors);

        buildTree(0, "for", 4, stack);

    } else if (nextToken.getTerm() == "loop") {
        int statementListLen = 1;

        readToken(7, "loop", tokenStream, stack, errors);
        termStatement(tokenStream, stack, errors);
        while (tokenStream->at(0).getTerm() == ";") {
            readToken(7, ";", tokenStream, stack, errors);
            termStatement(tokenStream, stack, errors);
            statementListLen += 1;
        }
        readToken(7, "pool", tokenStream, stack, errors);

        buildTree(0, "loop", statementListLen, stack);

    } else if (nextToken.getTerm() == "case") {
        readToken(7, "case", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);
        readToken(7, "of", tokenStream, stack, errors);
        int numCases = termCaseclauses(tokenStream, stack, errors);
        if (tokenStream->at(0).getTerm() == "otherwise") {
            readToken(7, "otherwise", tokenStream, stack, errors);
            termStatement(tokenStream, stack, errors);

            buildTree(0, "otherwise", 1, stack);

            readToken(7, "end", tokenStream, stack, errors);

            buildTree(0, "case", 2 + numCases, stack);
        } else {
            readToken(7, "end", tokenStream, stack, errors);

            buildTree(0, "case", 1 + numCases, stack);
        }
    } else if (nextToken.getTerm() == "read") {
        int nameListLen = 1;

        readToken(7, "read", tokenStream, stack, errors);
        readToken(7, "(", tokenStream, stack, errors);
        termName(tokenStream, stack, errors);
        while (tokenStream->at(0).getTerm() == ",") {
            readToken(7, ",", tokenStream, stack, errors);
            termName(tokenStream, stack, errors);
            nameListLen += 1;
        }
        readToken(7, ")", tokenStream, stack, errors);

        buildTree(0, "read", nameListLen, stack);

    } else if (nextToken.getTerm() == "exit") {
        readToken(7, "exit", tokenStream, stack, errors);

        buildTree(0, "exit", 0, stack);

    } else if (nextToken.getTerm() == "return") {
        readToken(7, "return", tokenStream, stack, errors);
        termExpression(tokenStream, stack, errors);

        buildTree(0, "return", 1, stack);

    } else if (nextToken.getTerm() == "begin") {
        termBody(tokenStream, stack, errors);
    } else if (nextToken.getTerm() == ";" || nextToken.getTerm() == "end" || nextToken.getTerm() == "else") {
        buildTree(0, "<null>", 0, stack);
    }
}

/* Handle Body Nonterminals
 */
void termBody(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int bodyListLen = 1;

    readToken(7, "begin", tokenStream, stack, errors);
    termStatement(tokenStream, stack, errors);
    while (tokenStream->at(0).getTerm() != "end") {
        readToken(7, ";", tokenStream, stack, errors);
        termStatement(tokenStream, stack, errors);
        bodyListLen += 1;
    }
    readToken(7, "end", tokenStream, stack, errors);

    buildTree(0, "block", bodyListLen, stack);
}

/* Handle Fcn Nonterminals
 */
void termFcn(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    readToken(7, "function", tokenStream, stack, errors);
    termName(tokenStream, stack, errors);
    readToken(7, "(", tokenStream, stack, errors);
    termParams(tokenStream, stack, errors);
    readToken(7, ")", tokenStream, stack, errors);
    readToken(7, ":", tokenStream, stack, errors);
    termName(tokenStream, stack, errors);
    readToken(7, ";", tokenStream, stack, errors);
    termConsts(tokenStream, stack, errors);
    termTypes(tokenStream, stack, errors);
    termDclns(tokenStream, stack, errors);
    termBody(tokenStream, stack, errors);
    termName(tokenStream, stack, errors);
    readToken(7, ";", tokenStream, stack, errors);

    buildTree(0, "fcn", 8, stack);
}

/* Handle SubProgs Nonterminals
 */
void termSubProgs(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    int subProgsListLen = 0;

    while (tokenStream->at(0).getTerm() == "function") {
        termFcn(tokenStream, stack, errors);
        subProgsListLen += 1;
    }

    buildTree(0, "subprogs", subProgsListLen, stack);
}

/* Handle Program Nonterminals
 * params:  tokenStream - Pointer to the token vector
 *          stack - Pointer to the stack
 *          errors - Pointer to the error vector
 *
 */
void termProgram(std::vector<Token> *tokenStream, std::vector<Node> *stack, std::vector<std::string> *errors) {
    readToken(7, "program", tokenStream, stack, errors);
    termName(tokenStream, stack, errors);
    readToken(7, ":", tokenStream, stack, errors);
    termConsts(tokenStream, stack, errors);
    termTypes(tokenStream, stack, errors);
    termDclns(tokenStream, stack, errors);

    termSubProgs(tokenStream, stack, errors);

    termBody(tokenStream, stack, errors);

    termName(tokenStream, stack, errors);
    readToken(7, ".", tokenStream, stack, errors);

    buildTree(0, "program", 7, stack);
}

/* Parser
 * params:  tokenStream - Pointer to the token vector
 *          stack - Pointer to the stack
 *          errors - Pointer to the error vector
 */
Node parser(std::vector<Token> *tokenStream, std::vector<std::string> *errors) {
    std::vector<Node> stack;

    termProgram(tokenStream, &stack, errors);

    if (stack.size() == 1) {
        return stack.at(0);
    } else {
        errors->push_back("Unexpected error during parsing");
        throw "Parser exception";
    }
}
