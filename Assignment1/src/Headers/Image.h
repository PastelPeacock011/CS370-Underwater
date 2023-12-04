#pragma once
#include <string>
#include <iostream>
#include "Pixel.h"
#include "Utilities.h"

struct Image {
private:
	std::string path;
	int header;
	int band;
	int width, height;
	int maxRGB;

	int* Histogram = new int[256]{ 0 }; //256 is INTENSITYLEVEL

public:
	unsigned char* image;
	//unsigned char* histrogram;
	int totalPixels = 0;
	int pixelCount = 0;

	/// <summary>
	/// Create image object
	/// </summary>
	/// <param name="Path"></param>
	/// <param name="Header (Pn)"></param>
	/// <param name="Band"></param>
	/// <param name="Width"></param>
	/// <param name="Height"></param>
	/// <param name="Max RGB"></param>
	Image(std::string p, int head, int b, int w, int h, int m) :
		path(p), header(head), band(b), width(w), height(h), maxRGB(m) {
		totalPixels = 0;
		image = nullptr;
	}

	Image() : Image("", 3, 3, 0, 0, 255) {}

	//Deep copy
	Image& operator=(const Image& rhs)
	{
		//if there is existing data
		if (totalPixels != 0) {
			delete[] image;
			image = nullptr;
		}

		delete[] Histogram;
		Histogram = nullptr;

		path = rhs.path;
		header = rhs.header;
		band = rhs.band;
		width = rhs.width;
		height = rhs.height;
		maxRGB = rhs.maxRGB;

		totalPixels = rhs.totalPixels;
		pixelCount = rhs.pixelCount;

		image = new unsigned char[totalPixels];
		for (int i = 0; i < totalPixels; i++)
			image[i] = rhs.image[i];

		Histogram = new int[256]{ 0 };
		for (int j = 0; j < 256; j++) //INTENSITYLEVEL
			Histogram[j] = rhs.Histogram[j];

		return *this;
	}

	Image(Image& i)
	{
		this->operator=(i);
	}

	~Image()
	{
		delete[] image;
		image = nullptr;

		delete[] Histogram;
		Histogram = nullptr;
	}

	//Getters
	int GetHeader() const { return header; }
	int GetBand() const { return band; }
	std::string GetPath() const { return path; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetMaxRGB() const { return maxRGB; }
	int* GetHistogram() const { return Histogram; }
	int GetValueHist(int greyScale) { return Histogram[greyScale]; }
	int GetGreyScale(int index)
	{
		if (!(index < 0 || index >= pixelCount))
		{
			return (int)std::round(image[index * 3] + image[(index * 3) + 1] + image[(index * 3) + 2] / 3.0f);
		}
	}

	//Setters
	void SetHeader(int t) { header = t; }
	void SetBand(int band) { this->band = band; }
	void SetPath(std::string path) { this->path = path; };
	void SetWidth(int w) { width = w; }
	void SetHeight(int h) { height = h; }
	void SetMaxRGB(int m) { maxRGB = m; }
	void SetHistogram(int intensity, int frequency)
	{
		Histogram[intensity] = frequency;
	}

	//Calc
	void CalcTotalPixels()
	{
		totalPixels = width * height * band;
		pixelCount = width * height;
	}

	//Print info
	void PrintInfo()
	{
		std::cout << "Path:\t\t" << path << std::endl;
		std::cout << "Header:\t\tP" << header << std::endl;
		std::cout << "Band:\t\t" << band << std::endl;
		std::cout << "Size:\t\t" << width << " x " << height << std::endl;
		std::cout << "Max RGB:\t" << maxRGB << std::endl;
		std::cout << "===========================================================" << std::endl;
	}

	void PrintData()
	{
		for (int i = 0; i < totalPixels; i += 3)
		{
			std::cout << "(" << (int)image[i] << ", " << (int)image[i + 1] << ", " << (int)image[i + 2] << ") ";
		}

		std::cout << std::endl;
	}

	void PrintHistogram()
	{
		int counting = 0;
		std::cout << "Total pixels: " << pixelCount << std::endl;
		for (int i = 0; i < 256; i++)
		{
			std::cout << i << "\t|  " << Histogram[i] << std::endl; 
			counting = counting + Histogram[i];
		}
		std::cout << "Total: " << counting << std::endl;
		return;
	}

	bool GetRGB(int index, RGB* color) const
	{
		if (index >= pixelCount || index < 0)
		{
			color->R = 0;
			color->G = 0;
			color->B = 0;
			return false;
		}
		
		//std::cout << index << std::endl;
		index = index * 3;
		color->R = (int)image[index];
		color->G = (int)image[index + 1];
		color->B = (int)image[index + 2];
		return true;
	}

	void SetPixel(int index, unsigned char r, unsigned char g, unsigned char b)
	{
		if (!(index < 0 || index >= pixelCount))
		{
			image[index * 3] = r;
			image[(index * 3) + 1] = g;
			image[(index * 3) + 2] = b;
		}
		return;
	}

	void SetPixel(int index, RGB color) 
	{
		if (!(index < 0 || index >= pixelCount))
		{
			image[index * 3] = color.R;
			image[(index * 3) + 1] = color.G;
			image[(index * 3) + 2] = color.B;
		}
		return;
	}

	bool GetPixel(int x, int y, RGB* color) const
	{
		int index = (x * 3) + (3 * y * (GetWidth()-1));
		if (index >= totalPixels || index < 0)
		{
			color->R = 0;
			color->G = 0;
			color->B = 0;
			return false;
		}

		color->R = (int)image[index];
		color->G = (int)image[index + 1];
		color->B = (int)image[index + 2];

		return true;
	}

	void SetPixel(int x, int y, RGB color) 
	{
		int index = (x * 3) + (3 * y * (GetWidth() - 1));
		if (index >= 0 && index < totalPixels)
		{
			image[index] = color.R;
			image[index + 1] = color.G;
			image[index + 2] = color.B;
		}
		return;
	}

	void CalcHistogram()
	{
		delete[] Histogram;
		Histogram = nullptr;
		Histogram = new int[256]{ 0 };

		int tmpGreyScale = 0;
		RGB tmpColor;
		//looping through color image and convert RGB to grey scale
		for (int i = 0; i < pixelCount; i++)
		{
			tmpColor.R = image[i * 3];
			tmpColor.G = image[(i * 3) + 1];
			tmpColor.B = image[(i * 3) + 2];

			tmpGreyScale = (int)std::round((tmpColor.R + tmpColor.G + tmpColor.B) / 3.0f);

			//insert greyscale value into the histogram
			if (tmpGreyScale >= 256) tmpGreyScale = 255;
			else if (tmpGreyScale < 0) tmpGreyScale = 0;

			Histogram[tmpGreyScale]++;
		}
	}
};