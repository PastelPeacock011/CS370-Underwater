#include "Headers/CLAHE.h"

//void bilinear_interpolation(Image* image, int x, int y, int dist = 1) {
//	int x_floor = x- dist;
//	x_floor = x_floor < 0 ? 0 : x_floor;
//	int y_floor = y- dist;
//	y_floor = y_floor < 0 ? 0 : y_floor;
//	int x_ceil = x_floor + dist;
//	x_ceil = x_ceil >= image->GetWidth() ? image->GetWidth() - 1 : x_ceil;
//	int y_ceil = y_floor + dist;
//	y_ceil = y_ceil >= image->GetHeight() ? image->GetHeight() - 1 : y_ceil;
//
//
//	unsigned char* P1 = Approach(image, y_floor, x_floor);
//	unsigned char* P2 = Approach(image, y_floor, x_ceil);
//	unsigned char* P3 = Approach(image, y_ceil, x_floor);
//	unsigned char* P4 = Approach(image, y_ceil, x_ceil);
//
//	//float w1 = (x_ceil - x) * (y_ceil - y);
//	//float w2 = (x - x_floor) * (y_ceil - y);
//	//float w3 = (x_ceil - x) * (y - y_floor);
//	//float w4 = (x - x_floor) * (y - y_floor);
//	float w1 = 0.25;
//	float w2 = 0.25;
//	float w3 = 0.25;
//	float w4 = 0.25;
//
//	char interpolated_value0 = w1 * P1[0] + w2 * P2[0] + w3 * P3[0] + w4 * P4[0];
//	char interpolated_value1 = w1 * P1[1] + w2 * P2[1] + w3 * P3[1] + w4 * P4[1];
//	char interpolated_value2 = w1 * P1[2] + w2 * P2[2] + w3 * P3[2] + w4 * P4[2];
//
//	unsigned char* point = Approach(image, y, x);
//	point[0] = interpolated_value0;
//	point[1] = interpolated_value1;
//	point[2] = interpolated_value2;
//}
void setNeighbor(int arr[4][2], int Num, int I, int J);
CLAHE::CLAHE(Image* img, int widthBlock, int HeightBlock) :image(img) {
	WN = widthBlock;
	HN = HeightBlock;
	if (image->GetWidth() % WN != 0 || image->GetHeight() % HN != 0) {
		//error
		std::cout << "Error: Image can't divide by "<< WN <<", " << HN << std::endl;
		return;
	}
	int BlockWidth{ image->GetWidth() / widthBlock };
	int BlockHeight{ image->GetHeight() / HeightBlock };

	std::vector<ImageBlock> blocks{};
	int WNHN{WN * HN};
	blocks.reserve(WNHN);

	for (int i = 0; i < image->GetHeight(); i += BlockHeight) {
		for (int j = 0; j < image->GetWidth(); j += BlockWidth) {
			blocks.emplace_back(image, BlockWidth, BlockHeight, i, j);
		}
	}

	for (auto& b : blocks) {
		b.block.CalcHistogram();
		int* hist = b.block.GetHistogram();
		float Threshold = 2;
		int limit = b.block.pixelCount / (INTENSITYLEVEL)*Threshold;
		Image temp;
		temp = b.block;
		for (int i = 0; i < temp.pixelCount; ++ i ) {
			int grey = (temp.image[i * 3] + temp.image[i * 3 + 1] + temp.image[i * 3 + 2])/3;
			temp.image[i * 3] = grey;
			temp.image[i * 3 + 1] = grey;
			temp.image[i * 3 + 2] = grey;
		}
		for (int j = 0; j < INTENSITYLEVEL; j++) {
			int diff = hist[j] - limit;
			char newValue{};
			for (int index = 0; diff > 0 && index < temp.pixelCount; index++)
			{
				if (temp.image[index * 3] == j) {
					int val = (newValue * (INTENSITYLEVEL / 10 - 1)) % INTENSITYLEVEL;
					temp.image[index * 3] = val;
					temp.image[index * 3 + 1] = val;
					temp.image[index * 3 + 2] = val;
					++newValue;
					diff -= 1;
				}
			}
		}
		HistogramEqualization(&(temp), b.Sk);
		//for (int index = 0; index < temp.pixelCount; index++)
		//{
		//	temp.image[index * 3] = (unsigned char)b.Sk[b.block.image[index * 3]];
		//	temp.image[index * 3 + 1] = (unsigned char)b.Sk[b.block.image[index * 3 + 1]];
		//	temp.image[index * 3 + 2] = (unsigned char)b.Sk[b.block.image[index * 3 + 2]];
		//}
		//b.block = temp;
		//temp = HistogramMatching(&b.block, &temp);
		//b.Reassign(&temp);
	}

	//interpolation

	int halfBlockHeight{ (BlockHeight / 2) + BlockHeight%2 };
	int halfBlockWidth{ (BlockWidth / 2) + BlockWidth%2};
	for (int i = halfBlockHeight; i < image->GetHeight() - halfBlockHeight; i += 1) {
		for (int j = halfBlockWidth; j < image->GetWidth() - halfBlockWidth; j += 1) {
			int blockI{i / BlockHeight };
			int blockJ{j / BlockWidth };

			int inBlockI{ (i % BlockHeight)};
			int inBlockJ{ (j % BlockWidth)};

			int inBlockPosI{ inBlockI / halfBlockHeight };
			int inBlockPosJ{ inBlockJ / halfBlockWidth };

			int neighborBlocks[4][2]{};
			if (inBlockPosI == 0 && inBlockPosJ == 0) {
				setNeighbor(neighborBlocks, 3, blockI, blockJ);
				setNeighbor(neighborBlocks, 2, blockI, blockJ - 1);
				setNeighbor(neighborBlocks, 1, blockI - 1, blockJ);
				setNeighbor(neighborBlocks, 0, blockI - 1, blockJ - 1);
			}
			else if (inBlockPosI == 0 && inBlockPosJ == 1) {
				setNeighbor(neighborBlocks, 2, blockI, blockJ);
				setNeighbor(neighborBlocks, 3, blockI, blockJ + 1);
				setNeighbor(neighborBlocks, 1, blockI - 1, blockJ + 1);
				setNeighbor(neighborBlocks, 0, blockI - 1, blockJ);
			}
			else if (inBlockPosI == 1 && inBlockPosJ == 0) {
				setNeighbor(neighborBlocks, 1, blockI, blockJ);
				setNeighbor(neighborBlocks, 3, blockI + 1, blockJ);
				setNeighbor(neighborBlocks, 2, blockI + 1, blockJ - 1);
				setNeighbor(neighborBlocks, 0, blockI, blockJ - 1);
			}
			else if (inBlockPosI == 1 && inBlockPosJ == 1) {
				setNeighbor(neighborBlocks, 0, blockI, blockJ);
				setNeighbor(neighborBlocks, 3, blockI + 1, blockJ + 1);
				setNeighbor(neighborBlocks, 2, blockI + 1, blockJ);
				setNeighbor(neighborBlocks, 1, blockI, blockJ + 1);
			}


			//unsigned char * centerPointLU = blocks[neighborBlocks[0][0] * WN + neighborBlocks[0][1]].GetCenter();
			//unsigned char * centerPointRU = blocks[neighborBlocks[1][0] * WN + neighborBlocks[1][1]].GetCenter();
			//unsigned char * centerPointLD = blocks[neighborBlocks[2][0] * WN + neighborBlocks[2][1]].GetCenter();
			//unsigned char * centerPointRD = blocks[neighborBlocks[3][0] * WN + neighborBlocks[3][1]].GetCenter();

			float x{ (float)(j - halfBlockWidth)/ BlockWidth };
			float y{ (float)(i - halfBlockHeight) / BlockHeight };

			float w1 = (neighborBlocks[3][1] - x) * (neighborBlocks[3][0] - y);
			float w2 = (x - neighborBlocks[0][1]) * (neighborBlocks[3][0] - y);
			float w3 = (neighborBlocks[3][1] - x) * (y - neighborBlocks[0][0]);
			float w4 = (x - neighborBlocks[0][1]) * (y - neighborBlocks[0][0]);

			//float GreyScaleLU{ (centerPointLU[0] + centerPointLU[1] + centerPointLU[2]) / 3.0f };
			//float GreyScaleRU{ (centerPointRU[0] + centerPointRU[1] + centerPointRU[2]) / 3.0f };
			//float GreyScaleLD{ (centerPointLD[0] + centerPointLD[1] + centerPointLD[2]) / 3.0f };
			//float GreyScaleRD{ (centerPointRD[0] + centerPointRD[1] + centerPointRD[2]) / 3.0f };

			//float interpolated_value = w1 * GreyScaleLU + w2 * GreyScaleRU + w3 * GreyScaleLD + w4 * GreyScaleRD;

			//unsigned char* point = Approach(image, i, j);
			//float pointGreyScale{(point[0] + point[1] + point[2])/3.0f};
			//float ratio{ interpolated_value / pointGreyScale };
			//point[0] *= ratio;
			//point[1] *= ratio;
			//point[2] *= ratio;

			unsigned char* point = Approach(image, i, j);
			int first{ neighborBlocks[0][0] * WN + neighborBlocks[0][1] };
			int second{ neighborBlocks[1][0] * WN + neighborBlocks[1][1] };
			int third{ neighborBlocks[2][0] * WN + neighborBlocks[2][1] };
			int fourth{ neighborBlocks[3][0] * WN + neighborBlocks[3][1] };
			float LU = (float)blocks[first ].Sk[point[0]];
			float RU = (float)blocks[second].Sk[point[0]];
			float LD = (float)blocks[third ].Sk[point[0]];
			float RD = (float)blocks[fourth].Sk[point[0]];
			float interpolated_value = w1 * LU + w2 * RU + w3 * LD + w4 * RD;
			point[0] = (unsigned char)(interpolated_value);

			LU = (float)blocks[first].Sk[point[1]];
			RU = (float)blocks[second].Sk[point[1]];
			LD = (float)blocks[third].Sk[point[1]];
			RD = (float)blocks[fourth].Sk[point[1]];
			interpolated_value = w1 * LU + w2 * RU + w3 * LD + w4 * RD;
			point[1] = (unsigned char)(interpolated_value);

			LU = (float)blocks[first].Sk[point[2]];
			RU = (float)blocks[second].Sk[point[2]];
			LD = (float)blocks[third].Sk[point[2]];
			RD = (float)blocks[fourth].Sk[point[2]];
			interpolated_value = w1 * LU + w2 * RU + w3 * LD + w4 * RD;
			point[2] = (unsigned char)(interpolated_value);

			//bilinear_interpolation(image, j, i,1);
		}
	}

	for (int i = 0; i < image->GetHeight(); i += 1) {
		if (i >= halfBlockHeight && i < image->GetHeight() - halfBlockHeight) continue;
		for (int j = 0; j < image->GetWidth(); j += 1) {
			if (j >= halfBlockWidth && j < image->GetWidth() - halfBlockWidth) continue;
			int blockI{ i / BlockHeight };
			int blockJ{ j / BlockWidth };
			unsigned char* point = Approach(image, i, j);
			int index{ blockI * WN + blockJ };
			point[0] = blocks[index].Sk[point[0]];
			point[1] = blocks[index].Sk[point[1]];
			point[2] = blocks[index].Sk[point[2]];
		}
	}

	for (int i = 0; i < image->GetHeight(); i += 1) {
		bool i_in = i >= halfBlockHeight && i < image->GetHeight() - halfBlockHeight;
		for (int j = 0; j < image->GetWidth(); j += 1) {
			bool j_in = j >= halfBlockWidth && j < image->GetWidth() - halfBlockWidth;
			int blockI{ i / BlockHeight };
			int blockJ{ j / BlockWidth };

			int inBlockI{ (i % BlockHeight) };
			int inBlockJ{ (j % BlockWidth) };

			unsigned char* point = Approach(image, i, j);

			if (i_in == false && j_in == true) {
				int inBlockPosJ{ inBlockJ / halfBlockWidth };
				float x{ (float)(j - halfBlockWidth) / BlockWidth };
				int neighborBlocks[2]{};
				if (inBlockPosJ == 1) {
					neighborBlocks[0] = blockJ;
					neighborBlocks[1] = blockJ + 1;
				}
				else {
					neighborBlocks[0] = blockJ - 1;
					neighborBlocks[1] = blockJ;
				}
				float w1 = (neighborBlocks[1] - x);
				float w2 = (x - neighborBlocks[0]);
				int first{ blockI * WN + neighborBlocks[0] };
				int second{ blockI * WN + neighborBlocks[1] };
				point[0] = (unsigned char)(w1 * blocks[first].Sk[point[0]] + w2 * blocks[second].Sk[point[0]]);
				point[1] = (unsigned char)(w1 * blocks[first].Sk[point[1]] + w2 * blocks[second].Sk[point[1]]);
				point[2] = (unsigned char)(w1 * blocks[first].Sk[point[2]] + w2 * blocks[second].Sk[point[2]]);
			}
			if (i_in == true && j_in == false) {
				int inBlockPosI{ inBlockI / halfBlockHeight };
				float y{ (float)(i - halfBlockHeight) / BlockHeight };
				int neighborBlocks[2]{};
				if (inBlockPosI == 1) {
					neighborBlocks[0] = blockI;
					neighborBlocks[1] = blockI + 1;
				}
				else {
					neighborBlocks[0] = blockI - 1;
					neighborBlocks[1] = blockI;
				}
				float w1 = (neighborBlocks[1] - y);
				float w2 = (y - neighborBlocks[0]);
				int first{ neighborBlocks[0] * WN + blockJ };
				int second{ neighborBlocks[1] * WN + blockJ };
				point[0] = (unsigned char)(w1 * blocks[first].Sk[point[0]] + w2 * blocks[second].Sk[point[0]]);
				point[1] = (unsigned char)(w1 * blocks[first].Sk[point[1]] + w2 * blocks[second].Sk[point[1]]);
				point[2] = (unsigned char)( w1 * blocks[first].Sk[point[2]] + w2 * blocks[second].Sk[point[2]]);
			}
		}
	}
}
void setNeighbor(int arr[4][2], int Num, int I, int J) {
	arr[Num][0] = I;
	arr[Num][1] = J;
}
