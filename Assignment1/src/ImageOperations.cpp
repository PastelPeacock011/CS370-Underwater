#include "Headers/ImageOperations.h"
#include <math.h>

#define BiggerX 0
#define BiggerY 1
#define xDiff 2
#define yDiff 3
#define maxX 4
#define maxY 5

unsigned char (*operation) (const unsigned char u1, const unsigned char u2);

int* ImageDifference(const Image* img1, const Image* img2)
{
	//Layout: (	img with bigger x, img with bigger y, 
	//			xDiff, yDiff,
	//			maxX, maxY)

	int* imgDiff = new int[6]{ 0 };

	if (img1->GetWidth() > img2->GetWidth())
		imgDiff[BiggerX] = 1;
	else
		imgDiff[BiggerX] = 0;

	if (img1->GetHeight() > img2->GetHeight())
		imgDiff[BiggerY] = 1;
	else
		imgDiff[BiggerY] = 0;

	//xDiff and yDiff
	imgDiff[xDiff] = std::abs(img1->GetWidth()	- img2->GetWidth());
	imgDiff[yDiff] = std::abs(img1->GetHeight()	- img2->GetHeight());

	//maxX
	imgDiff[maxX] = (imgDiff[BiggerX] == 1) ? img2->GetWidth()	: img1->GetWidth();
	//maxY
	imgDiff[maxY] = (imgDiff[BiggerY] == 1) ? img2->GetHeight()	: img1->GetHeight();

	return imgDiff;
}

Image ArithmeticOperation(const Image* first, const Image* second, int op)
{
	Image im;
	
	//Select the operation
	if (op == 0)
		operation = &Addition;
	else if (op == 1)
		operation = &Subtraction;
	else if (op == 2)
		operation = &Product;
	else if (op == 3)
		operation = &Negative;

	//Assume that those two image is in the same size
	int* sizeInfo; //[6]
	sizeInfo = ImageDifference(first, second);

	//set image info
	im.SetBand(3);
	im.SetHeader(3);
	im.SetHeight(sizeInfo[maxY]);
	im.SetWidth(sizeInfo[maxX]);
	im.SetMaxRGB(255);
	im.CalcTotalPixels();

	unsigned char* result = new unsigned char[im.totalPixels] {0};

	int firstIndex = 0;
	int secondIndex = 0;
	int currentIndex = 0;
	
	for (int i = 0; i < sizeInfo[maxY]; i++)
	{
		for (int j = 0; j < (sizeInfo[maxX] * 3); j++)
		{
			result[currentIndex] = operation(first->image[firstIndex], second->image[secondIndex]);
			currentIndex++;
			firstIndex++;
			secondIndex++;
		}
		//after X reached maxX
		//See if we have to skip pixels in the bigger image
		if (sizeInfo[BiggerX] == 1) // first image is X bigger
			firstIndex += sizeInfo[xDiff] * 3;
		else
			secondIndex += sizeInfo[xDiff] * 3;
	}

	//free previous space
	if (im.totalPixels != 0)
	{
		delete [] im.image;
		im.image = nullptr;
	}

	im.image = result;

	//free sizeInfo
	delete[] sizeInfo;
	sizeInfo = nullptr;

	return im;
}

Image ImageTransform(const Image* img, int op, float intensity, float gamma)
{
	Image im;
	im = *img;

	if (op == 5) // Power transform
	{
		for (int i = 0; i < img->totalPixels; i++)
			im.image[i] = PowerTransform(img->image[i], intensity, gamma);
		std::cout << "Power transform" << std::endl;
	}
	else if (op == 4) // Log transform
	{
		for (int i = 0; i < img->totalPixels; i++)
			im.image[i] = LogTransform(img->image[i], intensity);
		std::cout << "Log transform" << std::endl;
	}

	return im;
}

unsigned char PowerTransform(const unsigned char u, float intensity, float gamma)
{
	double tmp = intensity * std::pow(u, gamma);
	return (unsigned char) tmp;
}

unsigned char LogTransform(const unsigned char inputR, float intensity)
{
	float tmp = intensity * (std::log(1.0f + inputR));
	unsigned char result = (unsigned char) tmp;
	return result;
}

unsigned char Addition(const unsigned char u1, const unsigned char u2)
{
	unsigned char res = 0;
	int tmp = (int)u1 + (int)u2;

	if (tmp > 255)
		res = 255;
	else
		res = tmp;

	return res;
}

unsigned char Subtraction(const unsigned char u1, const unsigned char u2)
{
	unsigned char res = 0;
	int tmp = (int)u1 - (int)u2;

	if(tmp < 0)
		res = 0;
	else
		res = tmp;

	return res;
}

unsigned char Product(const unsigned char u1, const unsigned char u2)
{
	unsigned char res = 0;
	int tmp = (int)u1 * (int)u2;
	
	if (tmp > 255)
		res = 255;
	else
		res = tmp;

	return res;
}

unsigned char Negative(const unsigned char u1, const unsigned char u2)
{
	unsigned char res = 0;
	float tmp = (float)u1 / (float)u2;

	if (tmp > 255)
		res = 255;
	else
		res = (unsigned char)tmp;

	return res;
}

Image ImageAdditionWeight(Image* img1, float weight1, Image* img2, float weight2)
{
	Image im;
	im = *img1;

	for (int i = 0; i < img1->totalPixels; i++)
		im.image[i] = AdditionWithWeight(img1->image[i], weight1, img2->image[i], weight2);

	return im;
}

unsigned char AdditionWithWeight(const unsigned char u1, float weight1, const unsigned char u2, float weight2)
{
	unsigned char res = 0;
	int tmp = (float)u1 * weight1 + (float)u2 * weight2;

	if (tmp > 255)
		res = 255;
	else
		res = tmp;

	return res;
}
