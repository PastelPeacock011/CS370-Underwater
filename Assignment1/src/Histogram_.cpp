#include "Headers/Histogram_.h"
#include <map>
#include <vector>
#include <utility>

Image HistogramEqualization(Image* im, int* getSk)
{
	Image result;
	//map greyscale to index
	std::vector<int> mappedImage;

	//std::cout << "Start: HistogramEqualization()" << std::endl;
	RGB* tmpColor = new RGB();
	int tmpGreyScale = 0;

	//Nk
	im->CalcHistogram();
	//Pr
	float Pr[INTENSITYLEVEL] = { 0 };

	//Map greyscale to pixel index
	for (int i = 0; i < im->pixelCount; i++)
	{
		if (im->GetRGB(i, tmpColor))
		{
			tmpGreyScale = (int)std::round((tmpColor->R + tmpColor->G + tmpColor->B) / 3.0f);
			//mappedImage.push_back(tmpGreyScale);
			mappedImage.push_back(tmpColor->R);
			mappedImage.push_back(tmpColor->G);
			mappedImage.push_back(tmpColor->B);
		}
		else
			break;
	}

	int Sk[INTENSITYLEVEL] = { 0 };
	int L = INTENSITYLEVEL - 1;
	int prevSum = 0;
	float tmpF = 0.0f;
	//Getting Pr and Sk
	for (int j = 0; j < INTENSITYLEVEL; j++)
	{
		Pr[j] = (float)im->GetValueHist(j)/ (float)im->pixelCount;
		tmpF = prevSum + (L * (Pr[j]));
		prevSum = tmpF;

		//round current Sk
		Sk[j] = (int)std::round(tmpF);
	}

	//output intensity mapping
	if (getSk != nullptr)
	{
		for (int i = 0; i < INTENSITYLEVEL; i++)
			getSk[i] = Sk[i];
	}

	//Construct new image
	result = *im;
	//Pixel array
	unsigned char* pixelArr = new unsigned char[im->totalPixels];

	//Insert new pixels
	for (int index = 0; index < im->pixelCount; index++)
	{ 
		pixelArr[index*3] = (unsigned char)Sk[mappedImage[index * 3]];
		pixelArr[index*3 + 1] = (unsigned char)Sk[mappedImage[index * 3 + 1]];
		pixelArr[index*3 + 2] = (unsigned char)Sk[mappedImage[index * 3 + 2]];
	}

	result.image = pixelArr;
	//calculating new image histogram
	result.CalcHistogram();
	//clean map
	mappedImage.clear();

	//std::cout << "End: HistogramEqualization()" << std::endl;
	return result;
}

int GetClosestValue(int intensity, int* _array)
{
	int closestNum = 999, comp = 0, location = 0;
	for (int i = 0; i < 256; i++)
	{
		if (intensity == _array[i])
			return location;

		comp = std::abs(_array[i] - intensity);

		//find the smallest closest
		if (comp < closestNum)
		{
			closestNum = comp;
			location = i;
		}
	}

	return location;
}

Image HistogramMatching(Image* inputImage, Image* targetImage)
{
	Image result;
	result = *inputImage;
	//std::cout << "Start: HistogramMatching()" << std::endl;

	//Equalize both images first
	int* Sk1 = new int[INTENSITYLEVEL]{0};
	Image inputHistogram;
	inputHistogram = HistogramEqualization(inputImage, Sk1);
	
	int* Sk2 = new int[INTENSITYLEVEL]{0};
	Image targetHistogram;
	targetHistogram = HistogramEqualization(targetImage, Sk2);

	int value, intensity;

	//result must be the size of input image
	for (int i = 0; i < inputImage->totalPixels; i++)
	{
		value = (int)result.image[i];
		//matching
		intensity = GetClosestValue(value, Sk2);
		result.image[i] = (unsigned char)intensity;
	}
	
	result.CalcHistogram();

	//std::cout << "End: HistogramMatching()" << std::endl;
	return result;
}
