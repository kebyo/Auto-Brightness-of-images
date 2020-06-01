//
// Created by ssiie on 01.06.2020.
//

#ifndef UNTITLED5_CIMAGE_H
#define UNTITLED5_CIMAGE_H

#include <iostream>
#include "CException.h"

struct SInput {
    char *inputName;
    char *outputName;
    int mode;
    int offset;
    double multiplier;
};

struct RGB {
    double red;
    double green;
    double blue;
};

class CImage {
public:
    CImage(SInput config);

    void settingUpOfBrightntess(SInput config);

    void writeToFile(char *fileName);

private:
    FILE *file;
    int version;
    int width;
    int height;
    int max_val;
    int size;
    RGB *pix;
    bool colorSpace;

    void userMode(double offset, double multilier);

    void autoMode(SInput config);

    double cut(double x);

    int getMin();

    int getMax();
};


#endif //UNTITLED5_CIMAGE_H
