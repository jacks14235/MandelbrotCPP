#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "CImg.h"
#include <chrono>
#include "Mandelbrot.h"
#include <string>

using namespace std::chrono;

using namespace cimg_library;



CImg<unsigned char> colorImage(CImg<unsigned char> image, int* imageArray, int xRes) {
	int colorNum;
	cimg_forXY(image, xC, yC) {
		colorNum = imageArray[xRes*xC + yC];
		unsigned char color[] = { colorNum % 5 * 51, colorNum % 7 * 36, colorNum % 11 * 23 };
		image.draw_point(xC, yC, color);
	}
	return(image);
}

CImg<unsigned char> bwImage(CImg<unsigned char> image, int* imageArray, int xRes) {
	int colorNum;
	cimg_forXY(image, xC, yC) {
		colorNum = imageArray[xRes*xC + yC];
		unsigned char color[] = { colorNum, colorNum/2, 0};
		image.draw_point(xC, yC, color);
	}
	return(image);
}



CImg<int> saveFast(int* imageArray, int xRes, int yRes, std::string filename) {
	CImg<int> img(xRes, yRes, 1, 3, 0);
	cimg_forXY(img, x, y) { img(x, y) = imageArray[x*xRes + y]; }
	img.save_pfm(filename.c_str());
	return(img);
}


void videoFrames(int xRes, int yRes, float centerX, float centerY, float sideLength, int maxIt) {
	double sl = 4;
	std::string name = "./frames/Frame_";
	std::string suffix = ".bmp";
	int count = 0;
	double zoomProp = 0.97;
	double xStart = centerX - sl / 2;
	double xEnd = centerX + sl / 2;
	double yStart = centerY - sl / 2;
	double yEnd = centerY + sl / 2;


	int* imageArray;
	while (sl >= sideLength) {

		CImg<unsigned char> img(xRes, yRes, 1, 3, 0);
		imageArray = makeImage(xRes, yRes, xStart, xEnd, yStart, yEnd, maxIt);
		std::string filename = name + std::to_string(count) + suffix;
		saveFast(imageArray, xRes, yRes, filename);
		count++;

		xStart = centerX - sl / 2;
		xEnd = centerX + sl / 2;
		yStart = centerY - sl / 2;
		yEnd = centerY + sl / 2;

		sl = sl * zoomProp;
		std::cout << sl;
	}
}


void updateImage(int xRes, 
	int yRes, 
	double xStart, 
	double xEnd, 
	double  yStart, 
	double yEnd, 
	int maxIt, 
	bool color,
	CImg<unsigned char> img
) {
	int* imageArray = makeImage(xRes, yRes, xStart, xEnd, yStart, yEnd, maxIt);
	if (color) {
		img = colorImage(img, imageArray, xRes);
	}
	else {
		img = bwImage(img, imageArray, xRes);
	}
	std::cout << "Called";
	img.save("test123.bmp");
}



int main() {
	//image resolution
	const int xRes = 1024;
	const int yRes = 1024;

	//center xy of image on complex plane
	double centerX = 0;
	double centerY = 0;

	//length of edges of image on complex plane
	double sideLength = 4;

	//x and y start and end coordinates in complex plane
	double xStart = centerX - sideLength / 2;
	double xEnd = centerX + sideLength / 2;
	double yStart = centerY - sideLength / 2;
	double yEnd = centerY + sideLength / 2;

	//number of iterations to test for divergence
	int maxIt = 1000;

	const std::string current = "current.pfm";

	bool color = false;
	CImg<unsigned char> img(xRes, yRes, 1, 3, 0);
	CImgDisplay main_disp(img, "Mandelbrot");
	//img.display(main_disp);
	//updateImage(xRes, yRes, xStart, xEnd, yStart, yEnd, maxIt, color, img);
	int* imageArray = makeImage(xRes, yRes, xStart, xEnd, yStart, yEnd, maxIt);
	img = saveFast(imageArray, xRes, yRes, current);
	img.display(main_disp);
	int x = 0;

	while (!main_disp.is_closed()) {
		main_disp.wait();

		//if either regular mouse button clicked and mouse is on display window
		if ((main_disp.button()&1 || main_disp.button()&2) && main_disp.mouse_y() >= 0) {
			const int x = main_disp.mouse_x();
			const int y = main_disp.mouse_y();
			 //if left click
			if (main_disp.button() & 1) {
				centerX = xStart + ((((double)x) / ((double)xRes)) * sideLength);
				centerY = yStart + ((((double)y) / ((double)yRes)) * sideLength);
				sideLength = sideLength / 2;
			}
			else {
				sideLength = sideLength * 2;
			}
			

			xStart = centerX - sideLength / 2;
			xEnd = centerX + sideLength / 2;
			yStart = centerY - sideLength / 2;
			yEnd = centerY + sideLength / 2;

			int* imageArray = makeImage(xRes, yRes, xStart, xEnd, yStart, yEnd, maxIt);

			auto start = high_resolution_clock::now();

			//updateImage(xRes, yRes, xStart, xEnd, yStart, yEnd, maxIt, color, img);
			//img.display(main_disp);
			saveFast(imageArray, xRes, yRes, current);
			CImg<unsigned char> temp("current.pfm");
			temp.display(main_disp);
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);
			std::cout << "\nImage Generation: ";
			std::cout << duration.count();
			std::cout << "\n\n";
		}

		//if v key clicked, make video frames zooming to this point
		if (main_disp.is_keyV()) {
			videoFrames(xRes, yRes, centerX, centerY, sideLength, maxIt);
		}

		//if n clicked, subtract 1000 from maxIt
		if (main_disp.is_keyN() && maxIt > 1000) {
			maxIt = maxIt - 1000;
			std::cout << "\nMax number of iterations: ";
			std::cout << maxIt;
		}
		
		//if m clicked, add 1000 to maxIt
		if (main_disp.is_keyM()) {
			maxIt = maxIt + 1000;
			std::cout << "\nMax number of iterations: ";
			std::cout << maxIt;
		}

	}

	std::cout << "Done";
}
