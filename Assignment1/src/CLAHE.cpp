#include "Headers/CLAHE.h"
#include "Headers/Histogram_.h"

CLAHE::CLAHE(Image* img, int widthBlockN, int HeightBlockN) :image(img), WN(widthBlockN), HN(HeightBlockN) {
	if (image->GetWidth() % WN != 0 || image->GetHeight() % HN != 0) {
		//error
	}
	int BlockWidth{ image->GetWidth() / WN };
	int BlockHeight{ image->GetHeight() / HN };

	std::vector<ImageBlock> blocks{};
	blocks.reserve(WN * HN);

	for (int i = 0; i < image->GetHeight(); i += BlockHeight) {
		for (int j = 0; j < image->GetWidth(); j += BlockWidth) {
			blocks.emplace_back(image, BlockWidth, BlockHeight, i, j);
		}
	}

	for (auto& b : blocks) {
		Image temp;
		temp = HistogramEqualization(&(b.block));
		b.Reassign(&temp);
	}

	//interpolation
}