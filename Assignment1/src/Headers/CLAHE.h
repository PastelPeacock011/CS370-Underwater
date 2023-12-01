#pragma once
#include "Image.h"
#include <vector>

inline unsigned char* Approach(Image * img, int i, int j) {
	int index = i*(img->GetWidth()*3) + j * 3;

	return img->image + index;
}

struct ImageBlock {
	Image* image{};
	int width{};
	int height{};
	int startI{};
	int startJ{};

	Image block{};

	ImageBlock(Image* img, int w, int h, int stI, int stJ) : image(img), width(w), height(h), startI(stI), startJ(stJ), block("", 3, 3, w, h, 255) {
		block.CalcTotalPixels();
		block.image = new unsigned char[block.totalPixels];
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width;++j) {
				unsigned char* blockValue = Approach(&block,i,j);
				unsigned char* imageValue = Approach(image, startI+i, startJ+j);

				blockValue[0] = imageValue[0];
				blockValue[1] = imageValue[1];
				blockValue[2] = imageValue[2];
			}
		}
	}
	ImageBlock& operator=(const ImageBlock& rhs) {
		image = rhs.image;
		width = rhs.width;
		height = rhs.height;
		startI = rhs.startI;
		startJ = rhs.startJ;
		block = rhs.block;
		return *this;
	}
	ImageBlock(const ImageBlock& block) {
		*this = block;
	}

	void Reassign(Image* img) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				unsigned char* blockValue = Approach(img, i, j);
				unsigned char* imageValue = Approach(image, startI + i, startJ + j);

				imageValue[0] = blockValue[0];
				imageValue[1] = blockValue[1];
				imageValue[2] = blockValue[2];
			}
		}
	}
};

class CLAHE {
public:
	Image* image{};
	int WN{};
	int HN{};
	CLAHE(Image* img, int widthBlockN, int HeightBlockN);
};