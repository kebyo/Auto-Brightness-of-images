//
// Created by ssiie on 01.06.2020.
//

#ifndef UNTITLED5_CIMAGE_H
#define UNTITLED5_CIMAGE_H

#include <iostream>
#include "CException.h"
#include <climits>
#include <algorithm>
#include <vector>

struct SInput {
    char *inputName;
    char *outputName;
    int mode;
    int offset;
    double multiplier;
};

struct SMinMax {
    int min;
    int max;
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
    int version;
    int width;
    int height;
    int max_val;
    int size;
    RGB *pix;
    bool colorSpace;

    int getFirstD(int i, vector<int> &p);

    int getFirstB(int i, vector<int> &p);

    void userMode(double offset, double multilier);

    void autoMode(SInput config);

    double cut(double x);

    SMinMax getDarkestAndBrightest(bool is0039);
};


#endif //UNTITLED5_CIMAGE_H
