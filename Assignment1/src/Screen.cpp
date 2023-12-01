#include "Headers/Screen.h"
#include "Headers/ImageOperations.h"
#include "Headers/CLAHE.h"

Screen* Screen::s_Instance = nullptr;

void Screen::Init()
{
	//Operation select options
	operations[0] = "Addition";
	operations[1] = "Subtraction";
	operations[2] = "Multiply";
	operations[3] = "Inversion";
	operations[4] = "Log Transform";
	operations[5] = "Power (Gamma) Transform";
	operations[6] = "Preview Image";

	//assignment 2
	ass2operations[0] = "Histogram Equalization";
	ass2operations[1] = "Histogram Matching";
	ass2operations[2] = "Gaussian Smoothing";
	ass2operations[3] = "Sobel edge detection";
	ass2operations[4] = "Unsharp-masking";

	//user input filename
	file1 = new char[256]{ "images/cameraman.ppm" };
	file2 = new char[256]{ "images/mandril_gray.ppm"};
	resFile1 = new char[256]{ 0 };

	img1 = ppmLoader::GetInstance()->LoadPPM("res/images/cameraman.ppm");
	tex1.Init(&img1);
	plane.Init();

	//Shader setup
	shader		= new Shader("res/shaders/Basic.shader");
	shader->Bind();

	tex1.Bind();
	shader->SetUniform1i("u_Texture", 0);

	//Static values
	projection	= glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);
	view		= glm::translate(glm::mat4(1.0f), glm::vec3(WIDTH/2.0f, HEIGHT/2.0, 0));

	shader->SetUniformMat4f("u_MVP", MVP);

	shader->UnBind();

	//test filters
	//outImage = ApplyFilter(&img1, SobelFilter(5, 1.0f));
	//outImage = ApplyFilter(&img1, GaussianFilter(3, 1.5f));
	outImage = UnsharpMasking(&img1, 10.0, 15);
	tex1.Clean();
	tex1.Init(&outImage);
	tex1.Bind();
}

void Screen::Draw()
{	
	projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(windowWidth / 2.0f, windowHeight / 2.0, 0));
	//Update window properties
	shader->Bind();

	//Update plane transforms
	plane.SetPosition({ img1_position[0], img1_position[1] });
	plane.SetScale({ img1_size, img1_size});
	MVP			= projection * view * plane.UpdateModelMatrix();

	shader->SetUniformMat4f("u_MVP", MVP);

	//Draws
	plane.Draw(*shader);

	if (operateImg)
	{
		//if artimetic operation
		if (currentOperation < 4)
		{
			img1 = ppmLoader::GetInstance()->LoadPPM("res/" + std::string(file1));
			img2 = ppmLoader::GetInstance()->LoadPPM("res/" + std::string(file2));
			if (img1.totalPixels == 0 || img2.totalPixels == 0)
				std::cout << "One or both file not found!!" << std::endl;
			else
			{
				std::cout << "Image processed" << std::endl;
				outImage = ArithmeticOperation(&img1, &img2, currentOperation);
				hasResult = true;
			}
		}
		else if (currentOperation == 6) //Preview Image
		{
			outImage = ppmLoader::GetInstance()->LoadPPM("res/" + std::string(file1));
			hasResult = true;
		}
		else if (currentOperation >= 4) //Image Transform
		{
			img1 = ppmLoader::GetInstance()->LoadPPM("res/" + std::string(file1));
			if (img1.totalPixels == 0)
				std::cout << "One or both file not found!!" << std::endl;
			else
			{
				std::cout << "Image processed" << std::endl;
				outImage = ImageTransform(&img1, currentOperation, intensity, gamma);
				hasResult = true;
			}
		}

		operateImg = false;
	}
	else if (ass2operateImg)
	{
		img1 = ppmLoader::GetInstance()->LoadPPM("res/" + std::string(file1));

		if (ass2currentOperation == 0) //Histogram Eq.
		{
			outImage = HistogramEqualization(&img1);

			//CLAHE clahe(&img1, 8, 8);
			//outImage = *clahe.image;
			hasResult = true;
		}
		else if (ass2currentOperation == 1) //Histogram Matching
		{
			img2 = ppmLoader::GetInstance()->LoadPPM("res/" + std::string(file2));
			outImage = HistogramMatching(&img1, &img2);
			hasResult = true;
		}
		else if (ass2currentOperation == 2) //Gaussian smoothing
		{
			outImage = ApplyFilter(&img1, GaussianFilter(kernelSize, sigma));
			hasResult = true;
		}
		else if (ass2currentOperation == 3) //Sobel Edge Detection
		{
			outImage = ApplySobel(&img1, sigma, kernelSize);
			hasResult = true;
		}
		else if (ass2currentOperation == 4) //Unsharp mask
		{
			outImage = UnsharpMasking(&img1, sigma, kernelSize);
			hasResult = true;
		}
	}

	if (displayResult)
	{
		//display result image
		hasResult = false;
		displayResult = false;
		tex1.Clean();
		tex1.Init(&outImage);
		tex1.Bind();

		//output file
		if (resFile1 != "")
			ppmLoader::GetInstance()->writePPM(resFile1, &outImage);
	}

	shader->UnBind();
}

void Screen::DrawUI()
{
	ImGui::Text("Image Transform");
	ImGui::Spacing();

	ImGui::Text("Position (x, y)");
	ImGui::SliderFloat2("pos", img1_position, -500.0f, 500.0f);

	ImGui::Text("Size");
	ImGui::SliderFloat("scale", &img1_size, 0.0f, 2000.0f);
	ImGui::Spacing();
	//Select operations
	ImGui::Separator();

	//result name
	ImGui::Spacing();
	ImGui::Text("Leave blank if not wanting to write output file\nResult file name (exclude .ppm): ");
	ImGui::InputText("resFile name", resFile1, 256);

	if(ImGui::CollapsingHeader("Assignment 1"))
	{
		ImGui::Spacing();
		ImGui::Text("List of avaliable image operations");
		ImGui::Combo("operation", &currentOperation, operations, IM_ARRAYSIZE(operations), TOTAL_OPERATIONS);

		//Select Images (Drop down menu)
		ImGui::Spacing();
		ImGui::Text("Import image(s) example: images/<filename>.ppm\nor /results/<filename>.ppm): ");
		ImGui::InputText("Image #1", file1, 256);
		if (currentOperation < 4)
		{
			ImGui::Text("Import second image: ");
			ImGui::InputText("Image #2", file2, 256);
		}
		else if (currentOperation == 6)
		{
		}
		else if (currentOperation >= 4)
		{
			ImGui::InputFloat("Intensity Value", &intensity);
			if (currentOperation == 5)
				ImGui::InputFloat("Gamma Value", &gamma);
		}

		//Operate button
		operateImg = ImGui::Button("Operate(Assignment1)");
	}

	if(ImGui::CollapsingHeader("Assignment 2"))
	{
		ImGui::Text("List of avaliable image operations");
		ImGui::Combo("op list", &ass2currentOperation, ass2operations, IM_ARRAYSIZE(ass2operations), TOTAL_OPERATIONS);
		ImGui::Spacing();

		//Select Images (Drop down menu)
		ImGui::Spacing();
		ImGui::Text("Import image(s) example: images/<filename>.ppm\nor results/<filename>.ppm): ");
		ImGui::InputText("Ass2 Image #1", file1, 256);

		if (ass2currentOperation == 1) //Histogram Matching.
		{
			ImGui::Text("Import second image: ");
			ImGui::InputText("Ass2 Image #2", file2, 256);
		}
		else if (ass2currentOperation >= 2) //Gaussain + Sobel + unsharp
		{
			//Input kernel size and sigma
			ImGui::InputInt("Filter (kernel) size", &kernelSize);
			ImGui::InputFloat("Sigma Value", &sigma);
		}
		
		//second operate button
		ass2operateImg = ImGui::Button("Operate(Assignment2)");
	}

	//Display result image
	if (hasResult)
		displayResult = ImGui::Button("Display & Write Result");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / 
		ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void Screen::CleanUp()
{
	delete file1;
	file1 = nullptr;
	delete file2;
	file2 = nullptr;
	delete resFile1;
	resFile1 = nullptr;
	
	delete shader;
}

void Screen::UpdateWindowDimensions(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
}
