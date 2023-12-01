#define _CRT_SECURE_NO_DEPRECATE
#include "Headers/ppmLoader.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iterator>





ppmLoader* ppmLoader::s_Instance = nullptr;

Image ppmLoader::LoadPPM(std::string path)
{
	Image im;
	im.SetPath(path);

	//file I/O
	FILE* file;

	//temporary vals
	char* str = new char[265]{1};
	char c;
	bool done = false;
	int col = 0, row = 0, max = 0;

	file = fopen(path.c_str(), "rb");
	if (file == NULL)
	{
		std::cout << "ppm:loader::LoadPPM() unable to open file " << path << std::endl;
		return im;
	}

	std::cout << "Reading: " << path << std::endl;

	//Reading header
	while (!done && (c = (char)fgetc(file)) != '\0')
		switch (c)
		{
		//get type
		case 'P':
			c = (char)fgetc(file);
			switch (c) {
			case '2':
				//pgm plain
				im.SetHeader(2);
				im.SetBand(1);
				break;
			case '5':
				//pgm normal
				im.SetHeader(5);
				im.SetBand(1);
				break;
			case '3':
				//ppm plain
				im.SetHeader(3);
				im.SetBand(3);
				break;
			case '6':
				//ppm normal
				im.SetHeader(6);
				im.SetBand(3);
				break;
			}

			c = (char)fgetc(file);
			//if found comment (#)
			if (c != 0x0A)
			{
				ungetc(c, file);
			}
			else
			{
				ungetc(c, file);
				fgets(str, 256, file);
			}
			break;
		case '#':
			fgets(str, 256, file);
			std::cout << "Note from ppm file: " << str << std::endl;
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ungetc(c, file);
			done = fscanf(file, "%d %d %d", &col, &row, &max);
			im.SetWidth(col);
			im.SetHeight(row);
			im.SetMaxRGB(max);
			done = true;
			fgetc(file);
			break;
		}
	
	im.CalcTotalPixels();
	im.image = new unsigned char[im.totalPixels]{0};
	
	int index = 0;
	int val = 0;

	while (!feof(file))
	{
		done = fscanf(file, "%hhu", &im.image[index]);
		index++;
	}
	
	//fread(im.image, 1, im.totalPixels, file);
	std::cout << "Reading the image was successful...\n" << std::endl;

	//delete
	delete[] str;
	str = NULL;

	//checks image info
	//im.PrintInfo();

	//return image info
	return im;
}

void ppmLoader::writePPM(std::string name, Image* im)
{
	FILE* outFile;
	outFile = fopen(("res/results/" + name + ".ppm").c_str(), "wb");
	if (outFile == NULL)
	{
		std::cout << "Error: couldn't write the image" << std::endl;
	}
	else
	{
		fprintf(outFile, "P%d\n%d %d\n%d\n ", im->GetHeader(), im->GetWidth(), im->GetHeight(), im->GetMaxRGB());

		for (int i = 0; i < im->totalPixels; i++)
			fprintf(outFile, "%i ", (int)(im->image[i]));

		std::cout << "Finish writing file ....\n" << std::endl;
	}
}

