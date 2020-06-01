//
// Created by ssiie on 01.06.2020.
//

#include "CImage.h"

CImage::CImage(SInput config) {
    FILE *f = fopen(config.inputName, "rb");
    file = f;
    if (fscanf(f, "P%i%i%i%i\n", &this->version, &this->width, &this->height, &max_val) != 4) {
        throw CExpension("Wrong amount data in file", f);
    }
    size = width * height;
    unsigned char *buffer;
    if (version == 6) {
        buffer = new unsigned char[size * 3];
        pix = new RGB[size];
        fread(buffer, sizeof(unsigned char), size * 3, f);
        for (int i = 0, j = 0; i < size * 3; i += 3, j++) {
            pix[j].red = (double) buffer[i];
            pix[j].green = (double) buffer[i + 1];
            pix[j].blue = (double) buffer[i + 2];
        }
    } else if (version == 5) {
        buffer = new unsigned char[size];
        pix = new RGB[size];
        fread(buffer, sizeof(unsigned char), size, f);
        for (int i = 0; i < size; i++) {
            pix[i].red = (double) buffer[i];
            pix[i].green = (double) buffer[i];
            pix[i].blue = (double) buffer[i];
        }
    } else {
        throw CExpension("Wrong version of file", f);
    }
    delete[] buffer;
    fclose(f);
}