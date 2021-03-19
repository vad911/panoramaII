#include <iostream>

//#include <opencv2/opencv.hpp>

#include <opencv2/features2d.hpp>				// алгоритмы для поиска особых точек
#include <opencv2/highgui.hpp>					// для загрузки файлов
#include <opencv2/imgproc.hpp>					// функции обработки изображений
#include <opencv2/calib3d/calib3d.hpp>			// калибровка камеры и стереозрение
#include "serviceFunction.h"					// вспомогательные функции



//=======================================================================================
//		
//		
//		Description: Проект  panoramaII (вторая версия) занимается склейкой двух изображений с использованием OpenCV
//		c использованием поиска ключевых точек
//		Auth:	Vadim Yarmushov
//		Date: 17/03/21
//		Ver: 0.0.1
//		
//=======================================================================================


int main(int argc, char *argv[])
{

	using namespace serviceFunction;
	

	std::cout << "\n";
	std::cout << "\nInitialization system OpenCV..." << std::endl;
	std::cout << "Description: application panoramaII  (ver2 with keypoints ) for composite two images in the one panorama" << std::endl;
	std::cout << "Author: Vadim Yarmushov" << std::endl;
	std::cout << "Date: 17/03/2021" << std::endl;
	std::cout << "ver: 0.0.2 " << std::endl;
	
	
	/*
	// если не ввели имена входных изображений и только 1 аргумент в коммандной строке
	if (argc == 1)
	{
		print_instruction();	// выведем инструкцию
		return -1;
	}
	*/

	// входные изображения
	// изображения подавать на вход слева направо, сверху вниз
	// это значит, что при вводе image_01 - это левое изображение, image_02 - правое - при горизонтальном совмещении,
	// при вертикальном совмещении image_01 - это верхнее изображение, image_02 - нижнее
//	cv::Mat image_01_origin = cv::imread(argv[1]);	// первая фотография для слияния
//	cv::Mat image_02_origin = cv::imread(argv[2]); // вторая фотография для слияния

	// полноцветные входные изображения
	cv::Mat image_01_origin = cv::imread("Pan08_01.png");
	cv::Mat image_02_origin = cv::imread("Pan08_02.png");

	// создадим 256 тоное изображение
	cv::Mat image_01(image_01_origin.size(), CV_8U);
	cv::Mat image_02(image_02_origin.size(), CV_8U);
	//Преобразуем в оттенки серого и выполняем бинаризацию
	// cv::cvtColor(image_01_origin, image_01, CV_BGR2GRAY);
	cv::cvtColor(image_01_origin, image_01, cv::COLOR_BGR2GRAY);
	cv::cvtColor(image_02_origin, image_02, cv::COLOR_BGR2GRAY);

	// заведем хранилище ключевых точек
	std::vector<cv::KeyPoint> keypoints_01;	// для первого изображения
	std::vector<cv::KeyPoint> keypoints_02;	// для второго изображения

	// === 1. Найдем ключевые точки на каждом изображении с помощью методов из абстрактного класса FeatureDetector ===
		
	// создадим указатель на детектор ORB - используется для обработки видео в реальном времени
	// он инвариантен относительно поворота
	// Детектор ORB
	// static cv::Ptr<cv::BRISK> pDetector = cv::BRISK::create();
	 static cv::Ptr<cv::ORB> pDetector = cv::ORB::create();
	// static cv::Ptr<cv::SIFT> pDetector = cv::SIFT::create();	// работает медленно, результат плохой


	// теперь найдем ключевые точки применяя метод детектора detect()
	pDetector->detect(image_01, keypoints_01);		// для первого изображения
	pDetector->detect(image_02, keypoints_02);		// для второго изображения

	// === 2. Вычислим дескрипторы с помощью методов из класса DescriptorExtractor ===
	// Дескриптором ключевой точки называется числовой вектор, характеризующий 
	// особенности изображения в окрестности данной точки.
	 cv::Ptr<cv::DescriptorExtractor> descriptorExtractor = cv::ORB::create();
	

	// создадим массив где будем хранить дескрипторы
	cv::Mat descriptors_01;		// для первого изображения
	cv::Mat descriptors_02;		// для второго изображения

	// найдем дескрипторы - descriptors_01, descriptors_02
	descriptorExtractor->compute(image_01, keypoints_01, descriptors_01);
	descriptorExtractor->compute(image_02, keypoints_02, descriptors_02);

	// === 3. Сравним дескрипторы с помощью методов из класса DescriptorMatcher ===
	// ===    и выберем из результатов сравнения самые похожие точки            ===

	
	cv::BFMatcher  matcher(cv::NORM_HAMMING);		// будем использовать BFMatcher сопоставитель
	//cv::BFMatcher  matcher(cv::NORM_HAMMING);		// будем использовать BFMatcher сопоставитель
	std::vector<cv::DMatch> matches;				// matches - здесь храним результаты сопоставления descriptoro' 

	// используя дескрипторы найдем matches
	matcher.match(descriptors_01, descriptors_02, matches);
	
	// === 4. Выберем самые похожие точки из результатов сравнения ===
	// зададим границы
	double max_dist = 0; double min_dist = 100;
	
	
	//  найдем минимальную, максимальную дистанцию между ключевыми точками
	for (int i = 0; i < descriptors_01.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) { min_dist = dist; }
	//	if (dist > max_dist) { max_dist = dist; }
	
	}


	// оставим для расчета только лучшие совпадения
	// оставим только те рузультаты чья дистанция меньше 3*(min_dist+1)
	
	// хранилище лучших ключевых точек
	std::vector< cv::DMatch > good_matches;	 


	for (int i = 0; i < descriptors_01.rows; i++)
	{
		if (matches[i].distance < (3 * (min_dist + 1)) )
		{
			good_matches.push_back(matches[i]);
		}
	}


	// проверим количество хороших совпадений
	if (good_matches.size() < 4)
	{
		std::cout << "\nYou got insufficient number of matches! you need at least 4!" << std::endl;
		return -1;
	}



	std::vector<cv::Point2f> img1;		// общая сцена 
	std::vector<cv::Point2f> img2;		// объект, добавляемый в сцену

	// к
	for (int i = 0; i < good_matches.size(); ++i)
	{
		img1.push_back(keypoints_01[good_matches[i].queryIdx].pt);
		img2.push_back(keypoints_02[good_matches[i].trainIdx].pt);
	}




	// Нарисуем точки с лучшими совпадениеми
	cv::Mat img_matches;	// на этом изображении нарисуем точки совпажения
	cv::drawMatches(image_01, keypoints_01, image_02, keypoints_02,
		good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
		std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Покажем в окне результат найденных сопадений и выведем в файл
	
	// cv::waitKey(0);

	cv::imwrite("best_matches.jpg", img_matches);
	//cv::imshow("best matches ...", img_matches);
	/*
	if (cv::waitKey(33) >= 0)
	{
		cv::imwrite("best_matches.jpg", img_matches);
	   
		cv::destroyWindow("best matches ...");
	
	}

	*/
	
	// распечатаем список лучших точек
	for (int i = 0; i < (int)good_matches.size(); ++i)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}
	


	// === 5. Совместим две фотографии в одну. ===
	// Найдем матрицу гомографии  - трансформации перспективы, 	
	cv::Mat homography = cv::findHomography(img1, img2, cv::RANSAC);



	//Apply panorama correction
	int width = image_01.cols + image_02.cols;
	int height = image_01.rows + image_02.rows;

	cv::Size sz(width, height);
	int type = image_01.type();
	cv::Mat panorama(sz,type);
	
	cv::warpPerspective(image_02, panorama, homography, sz);	// записали измененное image_02 в panorama

	
	imwrite("panorama.jpg", panorama);
	/*
	cv::Mat roiImgResult_Left  = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, t_width, t_height));



	// укажем интересующие нас прямоугольные области -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = panorama(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  скопируем ROI регионы  roiImg1 в roiImgResult_Left и roiImg2 в roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - левая сторона изображения  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - правая сторона изображения  imgResult
	*/

//	imwrite("panorama.jpg", panorama);





	/*
	//############################################ [1]
	// === 5. Совместим две фотографии в одну. ===


	
	//получим  четыре входные - выходные точки из ключевых точек
	cv::Point2f inputQuad[4] ;
	cv::Point2f outputQuad[4];

	inputQuad[0] = img1[0];
	inputQuad[1] = img1[1];
	inputQuad[2] = img1[2];
	inputQuad[3] = img1[3];


	outputQuad[0] = img2[0];
	outputQuad[1] = img2[1];
	outputQuad[2] = img2[2];
	outputQuad[3] = img2[3];
	

//	cv::Point2f inputQuad[4];
//	cv::Point2f outputQuad[4];


	//cv::Mat panorama;	 // здесь храним полученную панораму - результат
	cv::Mat warpedImage02(image_02.rows, 2 * image_02.cols, image_02.type());

	
	// Выясняем положение углов исходной картинке,
	// по ширине и высоте
	std::vector<cv::Point2f> inputCorners(4);
	inputQuad[0] = cv::Point2f(0, 0);
	inputQuad[1] = cv::Point2f(image_02.cols, 0);
	inputQuad[2] = cv::Point2f(image_02.cols, image_02.rows);
	inputQuad[3] = cv::Point2f(0, image_02.rows);
	
	// Выясняем, где они будут - применяем трансформацию
	cv::Point2f outputQuad[4];
	outputQuad[0] = cv::Point2f(0, 0);
	outputQuad[1] = cv::Point2f(panorama.cols, 0);
	outputQuad[2] = cv::Point2f(panorama.cols, panorama.rows);
	outputQuad[3] = cv::Point2f(0, panorama.rows);
	

	// Найдем матрицу гомографии  - трансформации перспективы, 	
	cv::Mat homography = cv::findHomography(img1, img2, cv::RANSAC);

	// получаем матрицу трансформации
	cv::Mat M = getPerspectiveTransform(inputQuad, outputQuad);


	// углы изображение до начала трансформации
	std::vector<cv::Point2f> inputCorners(4);
	inputCorners[0] = cv::Point2f(0, 0);
	inputCorners[1] = cv::Point2f(image_02.cols, 0);
	inputCorners[2] = cv::Point2f(0, image_02.rows);
	inputCorners[3] = cv::Point2f(image_02.cols, image_02.rows);

	// углы изображения после трансформации, они могут быть иметь отрицательные координаты
	std::vector<cv::Point2f> outputCorners(4);

	// найдем выходные углы - outputCorners
	perspectiveTransform(inputCorners, outputCorners, M);

	cv::Rect br = cv::boundingRect(outputCorners);

	for (int i = 0; i < 4; i++) {
		outputQuad[i] += cv::Point2f(-br.x, -br.y);
	}

	M = getPerspectiveTransform(inputQuad, outputQuad);

	cv::warpPerspective(image_02, warpedImage02, homography, br.size());
		
	imwrite("warpedImage02.jpg", warpedImage02);
	

	// -------- теперь сольем изображения в одно
	cv::Mat panorama(image_01.rows, 3 * image_01.cols, image_01.type());

	cv::Mat roiImgResult_Left = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, image_02.cols, image_02.rows));



	// укажем интересующие нас прямоугольные области -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = warpedImage02(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  скопируем ROI регионы  roiImg1 в roiImgResult_Left и roiImg2 в roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - левая сторона изображения  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - правая сторона изображения  imgResult

	
	imwrite("panorama.jpg", panorama);

	//############################################ [2]
	*/

	/*
	cv::Mat lambda = cv::getPerspectiveTransform(inputQuad, outputQuad);


	// Найдем матрицу гомографии  - трансформации перспективы, 	
	cv::Mat homography = cv::findHomography(img1, img2, cv::RANSAC);


	if (homography.empty())
	{
		std::cout << "\nCan't find homography matrix" << std::endl;
		return -10;
	}


	// наше изображение может выходить за пределы temp_panorama

	// применим найденную матрицу трансформации -  homography к изображению image_01 -> результат  изображение: panorama
	// матрица будет хранить результат склейки
	
	cv::Mat warpresult2;	// сохраниним исправленное изображение в  warpresult2;
	// применим найденную матрицу трансформации -  homography к изображению image_02 -> результат  изображение: warpresult2
	cv::warpPerspective(image_02, warpresult2, lambda, cv::Size(2 * image_02.cols, image_02.rows));
	//cv::warpPerspective(image_02, warpresult2, homography, cv::Size(2 * image_02.cols, image_02.rows));


			
	cv::Mat roiImgResult_Left = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, image_02.cols, image_02.rows));

	

	// укажем интересующие нас прямоугольные области -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = warpresult2(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  скопируем ROI регионы  roiImg1 в roiImgResult_Left и roiImg2 в roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - левая сторона изображения  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - правая сторона изображения  imgResult

	imwrite("warpresult2.jpg", warpresult2);

	imwrite("panorama.jpg", panorama);
	*/
	

	/*
	// === 5. Совместим две фотографии в одну. ===
	cv::Mat warpresult2;	// сохраниним исправленное изображение в  warpresult2;

	// Найдем матрицу гомографии  - трансформации перспективы
	// количество точек в keypoints_01 и keypoints_02  не должно быть меньше 4.
	//
	cv::Mat homography = cv::findHomography( img1, img2, cv::RANSAC);


	// применим найденную матрицу трансформации -  homography к изображению image_02 -> результат  изображение: warpresult2
	cv::warpPerspective(image_02, warpresult2, homography, cv::Size(2 * image_02.cols, image_02.rows));

	//cv::Mat panorama;						// здесь храним полученную панораму - результат
	cv::Mat panorama(image_01.rows, 3 * image_01.cols, image_01.type());


	cv::Mat roiImgResult_Left  = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, image_02.cols, image_02.rows));

	// укажем интересующие нас прямоугольные области -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = warpresult2(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  скопируем ROI регионы  roiImg1 в roiImgResult_Left и roiImg2 в roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - левая сторона изображения  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - правая сторона изображения  imgResult

	imshow("result", panorama);
	imwrite("panorama.jpg", panorama);

	*/


	std::cout << "\n End prog" << std::endl;
	return 0;
}


