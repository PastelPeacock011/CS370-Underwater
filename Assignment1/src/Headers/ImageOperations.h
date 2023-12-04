#pragma once
#include "Image.h"
#include "Utilities.h"

Image ArithmeticOperation(const Image* first, const Image* second, int op);

int* ImageDifference(const Image* img1, const Image* img2);

unsigned char Addition(const unsigned char, const unsigned char);

unsigned char Subtraction(const unsigned char, const unsigned char);

unsigned char Product(const unsigned char, const unsigned char);

unsigned char Negative(const unsigned char, const unsigned char);

Image ImageTransform(const Image*, int, float, float);

//TODO: Power (Gamma) Transform
unsigned char PowerTransform(const unsigned char u, float intensity, float gamma);

//TODO: Log Transform
unsigned char LogTransform(const unsigned char inputR, float intensity);

Image ImageAdditionWeight(Image* img1, float weight1, Image* img2, float weight2);
unsigned char AdditionWithWeight(const unsigned char u1, float weight1, const unsigned char u2, float weight2);