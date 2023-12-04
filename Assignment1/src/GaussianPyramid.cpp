#include "Headers/Filters.h"
#include "Headers/GaussianPyramid.h"
#include "Headers/ImageOperations.h"
/*
Copyright (C) <2022> DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: IndexBuffer.cpp
Purpose: <Act as a index buffer manager>
Language: C++
Platform: Windows 32-bit, Microsoft C++ compiler (VS2019)
Project: <n.chungwatana_CS300_1>
Author: Nattamon Chungwatana, n.chungwatana, 0053759
Creation date: 29 Sep, 20222
*/

Image GaussianPyramid(Image* original)
{
	Image im;
	// Gaussian Smoothing the image
	im = ApplyFilter(original, GaussianFilter(5, 1)); 
	// Down Sampling
	Image result;
	result = DownSamplingPixel(&im);

	return result;
}

Image DownSamplingPixel(Image* smoothedimage)
{
	Image downsampled;

	int col = smoothedimage->GetWidth() / 2.0f;
	int row = smoothedimage->GetHeight() / 2.0f;
	int max = smoothedimage->GetMaxRGB();

	downsampled.SetBand(3);
	downsampled.SetWidth(col);
	downsampled.SetHeight(row);
	downsampled.SetMaxRGB(max);
	downsampled.CalcTotalPixels();
	downsampled.image = new unsigned char[downsampled.totalPixels]{ 0 };

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			float Halfx = smoothedimage->GetWidth() / 4.0f;
			float Halfy = smoothedimage->GetHeight() / 4.0f;

			float currentX = (i - Halfx) * 2;
			float currentY = (j - Halfy) * 2;

			currentX += smoothedimage->GetWidth() / 2.0f;
			currentY += smoothedimage->GetHeight() / 2.0f;

			RGB* reader = new RGB();
			bool Pixelavailable = smoothedimage->GetPixel(currentX + i, currentY + j, reader);
			downsampled.SetPixel(i, j, *reader);
		}
	}

	return downsampled;
}

Image LaplacianPyramid(Image* original, Image* gassianed)
{
	//Image im;
	//im = ApplyFilter(original, GaussianFilter(5, 1));
	Image result;
	result = ArithmeticOperation(original, gassianed, 1);

	return result;
}

Image UpSamplingPixel(Image* laplaicianedImage, Image* gaussianedImage)
{
	Image result;
	result = ArithmeticOperation(laplaicianedImage, gaussianedImage, 0);

	return result;
}

Image ImageFusion(Image* img1, float weight1, Image* img2, float weight2)
{
	//Image result;
	//result = ArithmeticOperation(img1, img2, 0); // plus

	Image gassian1_1;
	gassian1_1 = GaussianPyramid(img1);
	Image gassian1_2;
	gassian1_2 = GaussianPyramid(&gassian1_1);

	Image lap1_1;
	lap1_1 = LaplacianPyramid(img1, &gassian1_1);
	Image lap1_2;
	lap1_2 = LaplacianPyramid(&lap1_1, &gassian1_2);

	Image gassian2_1;
	gassian2_1 = GaussianPyramid(img2);
	Image gassian2_2;
	gassian2_2 = GaussianPyramid(&gassian2_1);

	Image lap2_1;
	lap2_1 = LaplacianPyramid(img2, &gassian2_1);
	Image lap2_2;
	lap2_2 = LaplacianPyramid(&lap2_1, &gassian2_2);

	Image result;
	//result = ArithmeticOperation(&gassian2_2, &gassian1_2, 0);
	result = ImageAdditionWeight(&gassian1_1, weight1, &gassian2_1, weight2);

	return result;
}