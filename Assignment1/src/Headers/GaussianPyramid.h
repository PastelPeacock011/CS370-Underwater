#pragma once

/*
Copyright (C) <2022> DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: IndexBuffer.h
Purpose: <Act as a index buffer manager>
Language: C++
Platform: Windows 32-bit, Microsoft C++ compiler (VS2019)
Project: <n.chungwatana_CS300_1>
Author: Nattamon Chungwatana, n.chungwatana, 0053759
Creation date: 29 Sep, 20222
*/

Image GaussianPyramid(Image* original);
Image DownSamplingPixel(Image* smoothedimage);
//RGB GaussianPyramidPixel(Image* original, int outputX, int outputY);
Image LaplacianPyramid(Image* original, Image* gassianed);
Image UpSamplingPixel(Image* laplaicianedImage, Image* gaussianedImage);

Image ImageFusion(Image* img1, float weight1, Image* img2, float weight2);