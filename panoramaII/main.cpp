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

	
	cv::BFMatcher  matcher;							// будем использовать BFMatcher сопоставитель
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



	std::vector<cv::Point2f> img1;		// хранилище точек относящиехся к левому изображению
	std::vector<cv::Point2f> img2;		// хранилище точек относящиехся к правому изображению, объекту, добавляемому в сцену

	// скопируем соотвествующие точки в 
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
	
	
	cv::imwrite("best_matches.jpg", img_matches);
	cv::imshow("best matches ...", img_matches);
	// cv::waitKey(0);
	if (cv::waitKey(33) >= 0)
	{
		cv::imwrite("best_matches.jpg", img_matches);
	   
		cv::destroyWindow("best matches ...");
	
	}

	
	// распечатаем список лучших точек
	for (int i = 0; i < (int)good_matches.size(); ++i)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}
	



	// Найдем матрицу гомографии  - трансформации перспективы, 	
	cv::Mat homography = cv::findHomography(img2, img1, cv::RANSAC);


	if (homography.empty())
	{
		std::cout << "\nCan't find homography matrix" << std::endl;
		return -10;
	}


	// наше изображение может выходить за пределы temp_panorama

	// применим найденную матрицу трансформации -  homography к изображению image_02 -> результат  изображение: panorama
	// матрица будет хранить результат склейки
	cv::Size sz(image_01.cols + image_02.cols, image_01.rows);
	int type = image_02.type();

	cv::Mat panorama;	// сохраниним исправленное изображение в  panorama;
	// применим найденную матрицу трансформации -  homography к изображению image_02 -> результат  изображение: warpresult2
	cv::warpPerspective(image_02_origin, panorama, homography, sz);
		
	cv::Mat half(panorama, cv::Rect(0, 0, image_01.cols, image_01.rows));
	image_01_origin.copyTo(half);
	imwrite("panorama.jpg", panorama);


	std::cout << "\n End prog" << std::endl;
	return 0;
}


