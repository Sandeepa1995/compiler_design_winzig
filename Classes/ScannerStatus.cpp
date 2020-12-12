//
// Created by damitha on 12/29/19.
//

#include "ScannerStatus.h"

ScannerStatus::ScannerStatus(int type1, int line1, int position1) {
    type = type1;
    line = line1;
    position = position1;
}

// Getter and setters for type
int ScannerStatus::getType(){
    return type;
};
void ScannerStatus::setType(int type1){
    type = type1;
}

// Getter and setters for line
int ScannerStatus::getLine(){
    return line;
}
void ScannerStatus::setLine(int line1){
    line = line1;
}
void ScannerStatus::incrementLine(int inc){
    line += inc;
}

// Getter and setters for position
int ScannerStatus::getPosition(){
    return position;
}
void ScannerStatus::setPosition(int position1){
    position = position1;
}
void ScannerStatus::incrementPositon(int inc){
    position += inc;
}
void ScannerStatus::resetPosition(){
    position = 0;
}