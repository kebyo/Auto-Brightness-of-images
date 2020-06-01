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
    colorSpace = (config.mode == 0 || config.mode == 2 || config.mode == 4);//False is YCbCr601
    delete[] buffer;
    fclose(f);
}

void CImage::writeToFile(char *fileName) {
    FILE *f = fopen(fileName, "wb");
    if (!f) {
        throw CExpension("Input file didn't open");
    }
    fprintf(f, "P%i\n%i %i\n%i\n", version, width, height, max_val);
    unsigned char *buffer;
    if (version == 5) {
        buffer = new unsigned char[size];
        for (int i = 0; i < size; i++) {
            buffer[i] = (unsigned char) pix[i].red;
        }
        fwrite(buffer, sizeof(unsigned char), size, f);
    } else {
        buffer = new unsigned char[size * 3];
        for (int i = 0, j = 0; i < size * 3; i += 3, j++) {
            buffer[i] = (unsigned char) pix[j].red;
            buffer[i + 1] = (unsigned char) pix[j].green;
            buffer[i + 2] = (unsigned char) pix[j].blue;
        }
        fwrite(buffer, sizeof(unsigned char), size * 3, f);
    }
    delete[] buffer;
    fclose(f);
}

void CImage::settingUpOfBrightntess(SInput config) {
    if (config.mode == 0 || config.mode == 1) {
        userMode(config.offset, config.multiplier);
    } else {
        autoMode(config);
    }
}

void CImage::userMode(double offset, double multilier) {
    if (version == 5) {
        for (int i = 0; i < size; i++) {
            int newValue = cut((pix[i].red - offset) * multilier);
            pix[i].red = newValue;
            pix[i].green = newValue;
            pix[i].blue = newValue;
        }
    } else {
        if (colorSpace) {
            for (int i = 0; i < size; i++) {
                int newRed = cut((pix[i].red - offset) * multilier);
                pix[i].red = newRed;
                int newGreen = cut((pix[i].green - offset) * multilier);
                pix[i].green = newGreen;
                int newBlue = cut((pix[i].blue - offset) * multilier);
                pix[i].blue = newBlue;
            }
        } else {
            for (int i = 0; i < size; i++) {
                int newValue = cut((pix[i].red - offset) * multilier);
                pix[i].red = newValue;
            }
        }
    }
}

void CImage::autoMode(SInput config) {
    SMinMax tmp;
    tmp = getDarkestAndBrightest(config.mode == 4 || config.mode == 5);
    std::cout << tmp.min << " " << 255.0 / (tmp.max - tmp.min) << "\n";
    if (version == 5) {
        for (int i = 0; i < size; i++) {
            int newValue = cut((pix[i].red - tmp.min) * 255.0 / (tmp.max - tmp.min));
            pix[i].red = newValue;
            pix[i].green = newValue;
            pix[i].blue = newValue;
        }
    } else {
        if (colorSpace) {
            for (int i = 0; i < size; i++) {
                int newRed = cut((pix[i].red - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].red = newRed;
                int newGreen = cut((pix[i].green - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].green = newGreen;
                int newBlue = cut((pix[i].blue - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].blue = newBlue;
            }
        } else {
            for (int i = 0; i < size; i++) {
                int newValue = cut((pix[i].red - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].red = newValue;
            }
        }
    }
}

SMinMax CImage::getDarkestAndBrightest(bool is0039) {
    vector<int> p(256, 0);
    SMinMax tmp;
    tmp.min = 0;
    tmp.max = 255;
    for (int i = 0; i < size; i++) {
        if (version == 6 && colorSpace) {
            p[pix[i].red]++;
            p[pix[i].green]++;
            p[pix[i].blue]++;
        } else {
            p[pix[i].red]++;
        }
    }
    if (is0039) {
        int bright = 0, dark = 255;
        int j = size * 0.0039;
        if (version == 6 && colorSpace) {
            j *= 3;
        }
        int i = 0;
        while (i < j) {
            if (i % 2 == 0) {
                dark = getFirstD(dark, p);
                p[dark]--;
                i++;
                continue;
            }
            bright = getFirstB(bright, p);
            p[bright]--;
            i++;

        }
        bool flagB = false, flagD = false;
        for (int i = 0; i < p.size(); i++) {
            if (p[i] > 0 && !flagD) {
                tmp.min = i;
                flagD = true;
            }
            if (p[255 - i] > 0 && !flagB) {
                tmp.max = 255 - i;
                flagB = true;
            }
        }
    }
    return tmp;
}

int CImage::getFirstD(int i, vector<int> &p) {
    while (p[i] == 0) {
        i++;
    }
    return i;
}

int CImage::getFirstB(int i, vector<int> &p) {
    while (p[i] == 0) {
        i--;
    }
    return i;
}

double CImage::cut(double x) {
    x = x > 255.0 ? 255.0 : x;
    x = x < 0.0 ? 0.0 : x;
    return x;
}