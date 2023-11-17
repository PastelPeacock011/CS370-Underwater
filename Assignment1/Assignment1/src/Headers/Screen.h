#pragma once
#include "Texture.h"
#include "Shader.h"
#include "ppmLoader.h"
#include "Plane.h"
#include "UI.h"

#define TOTAL_OPERATIONS 6
//window size
#define WIDTH	1280
#define HEIGHT	980

class Screen
{
public:
	void Init();
	void Draw();
	void DrawUI();
	void CleanUp();

	void UpdateWindowDimensions(const int w, const int h);

	static Screen* GetInstance() { return s_Instance = (s_Instance != nullptr) ? s_Instance : new Screen(); }
	~Screen() {}
private:
	//Essentials
	static Screen* s_Instance;
	Shader* shader = nullptr;

	//contents
	Texture tex1;

	Image img1;
	Image img2;
	Image outImage;

	Plane plane;

	//Matrices and transforms
	glm::mat4 projection;
	glm::mat4 MVP;
	glm::mat4 view;

	//Options
	const char* operations[TOTAL_OPERATIONS] = { nullptr };
	int currentOperation = 0;
	bool operateImg = false;
	bool displayResult = false;
	bool hasResult = false;
	
	float intensity = 0.0f;
	float gamma = 0.0f;

	//Input string (filename)
	char* file1;
	char* file2;

	//UI
	float img1_position[2];
	float img1_size = 700.0f;
	int windowWidth, windowHeight = 0;
	float ratio[2] = {1.0f};

	//Local functions
	void OperateImg();
};

