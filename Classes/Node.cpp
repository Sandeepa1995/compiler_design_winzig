//
// Created by damitha on 12/29/19.
//

#include "Node.h"

Node::Node(int type1, std::string term1) {
    type = type1;
    term = term1;
}

// Getter and setters for type
int Node::getType(){
    return type;
}
void Node::setType(int type1){
    type = type1;
}

// Getter and setters for term
std::string Node::getTerm(){
    return term;
}
void Node::setTerm(std::string term1){
    term = term1;
}

// Getter and setters for children
std::vector<Node> Node::getChildren(){
    return children;
}
void Node::addChiild(Node childNode){
    children.push_back(childNode);
}
int Node::numberOfChildren(){
    return (int)children.size();
}
Node Node::getChildAt(int i){
    return children.at(i);
}


