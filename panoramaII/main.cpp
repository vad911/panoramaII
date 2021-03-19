#include <iostream>

//#include <opencv2/opencv.hpp>

#include <opencv2/features2d.hpp>				// ��������� ��� ������ ������ �����
#include <opencv2/highgui.hpp>					// ��� �������� ������
#include <opencv2/imgproc.hpp>					// ������� ��������� �����������
#include <opencv2/calib3d/calib3d.hpp>			// ���������� ������ � ������������
#include "serviceFunction.h"					// ��������������� �������



//=======================================================================================
//		
//		
//		Description: ������  panoramaII (������ ������) ���������� �������� ���� ����������� � �������������� OpenCV
//		c �������������� ������ �������� �����
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
	// ���� �� ����� ����� ������� ����������� � ������ 1 �������� � ���������� ������
	if (argc == 1)
	{
		print_instruction();	// ������� ����������
		return -1;
	}
	*/

	// ������� �����������
	// ����������� �������� �� ���� ����� �������, ������ ����
	// ��� ������, ��� ��� ����� image_01 - ��� ����� �����������, image_02 - ������ - ��� �������������� ����������,
	// ��� ������������ ���������� image_01 - ��� ������� �����������, image_02 - ������
//	cv::Mat image_01_origin = cv::imread(argv[1]);	// ������ ���������� ��� �������
//	cv::Mat image_02_origin = cv::imread(argv[2]); // ������ ���������� ��� �������

	// ������������ ������� �����������
	cv::Mat image_01_origin = cv::imread("Pan08_01.png");
	cv::Mat image_02_origin = cv::imread("Pan08_02.png");

	// �������� 256 ����� �����������
	cv::Mat image_01(image_01_origin.size(), CV_8U);
	cv::Mat image_02(image_02_origin.size(), CV_8U);
	//����������� � ������� ������ � ��������� �����������
	// cv::cvtColor(image_01_origin, image_01, CV_BGR2GRAY);
	cv::cvtColor(image_01_origin, image_01, cv::COLOR_BGR2GRAY);
	cv::cvtColor(image_02_origin, image_02, cv::COLOR_BGR2GRAY);

	// ������� ��������� �������� �����
	std::vector<cv::KeyPoint> keypoints_01;	// ��� ������� �����������
	std::vector<cv::KeyPoint> keypoints_02;	// ��� ������� �����������

	// === 1. ������ �������� ����� �� ������ ����������� � ������� ������� �� ������������ ������ FeatureDetector ===
		
	// �������� ��������� �� �������� ORB - ������������ ��� ��������� ����� � �������� �������
	// �� ����������� ������������ ��������
	// �������� ORB
	// static cv::Ptr<cv::BRISK> pDetector = cv::BRISK::create();
	 static cv::Ptr<cv::ORB> pDetector = cv::ORB::create();
	// static cv::Ptr<cv::SIFT> pDetector = cv::SIFT::create();	// �������� ��������, ��������� ������


	// ������ ������ �������� ����� �������� ����� ��������� detect()
	pDetector->detect(image_01, keypoints_01);		// ��� ������� �����������
	pDetector->detect(image_02, keypoints_02);		// ��� ������� �����������

	// === 2. �������� ����������� � ������� ������� �� ������ DescriptorExtractor ===
	// ������������ �������� ����� ���������� �������� ������, ��������������� 
	// ����������� ����������� � ����������� ������ �����.
	 cv::Ptr<cv::DescriptorExtractor> descriptorExtractor = cv::ORB::create();
	

	// �������� ������ ��� ����� ������� �����������
	cv::Mat descriptors_01;		// ��� ������� �����������
	cv::Mat descriptors_02;		// ��� ������� �����������

	// ������ ����������� - descriptors_01, descriptors_02
	descriptorExtractor->compute(image_01, keypoints_01, descriptors_01);
	descriptorExtractor->compute(image_02, keypoints_02, descriptors_02);

	// === 3. ������� ����������� � ������� ������� �� ������ DescriptorMatcher ===
	// ===    � ������� �� ����������� ��������� ����� ������� �����            ===

	
	cv::BFMatcher  matcher;							// ����� ������������ BFMatcher �������������
	//cv::BFMatcher  matcher(cv::NORM_HAMMING);		// ����� ������������ BFMatcher �������������
	std::vector<cv::DMatch> matches;				// matches - ����� ������ ���������� ������������� descriptoro' 

	// ��������� ����������� ������ matches
	matcher.match(descriptors_01, descriptors_02, matches);
	
	// === 4. ������� ����� ������� ����� �� ����������� ��������� ===
	// ������� �������
	double max_dist = 0; double min_dist = 100;
	
	
	//  ������ �����������, ������������ ��������� ����� ��������� �������
	for (int i = 0; i < descriptors_01.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) { min_dist = dist; }
	//	if (dist > max_dist) { max_dist = dist; }
	
	}


	// ������� ��� ������� ������ ������ ����������
	// ������� ������ �� ���������� ��� ��������� ������ 3*(min_dist+1)
	
	// ��������� ������ �������� �����
	std::vector< cv::DMatch > good_matches;	 


	for (int i = 0; i < descriptors_01.rows; i++)
	{
		if (matches[i].distance < (3 * (min_dist + 1)) )
		{
			good_matches.push_back(matches[i]);
		}
	}


	// �������� ���������� ������� ����������
	if (good_matches.size() < 4)
	{
		std::cout << "\nYou got insufficient number of matches! you need at least 4!" << std::endl;
		return -1;
	}



	std::vector<cv::Point2f> img1;		// ��������� ����� ������������ � ������ �����������
	std::vector<cv::Point2f> img2;		// ��������� ����� ������������ � ������� �����������, �������, ������������ � �����

	// ��������� �������������� ����� � 
	for (int i = 0; i < good_matches.size(); ++i)
	{
		img1.push_back(keypoints_01[good_matches[i].queryIdx].pt);
		img2.push_back(keypoints_02[good_matches[i].trainIdx].pt);
	}




	// �������� ����� � ������� ������������
	cv::Mat img_matches;	// �� ���� ����������� �������� ����� ����������
	cv::drawMatches(image_01, keypoints_01, image_02, keypoints_02,
		good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
		std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- ������� � ���� ��������� ��������� ��������� � ������� � ����
	
	
	cv::imwrite("best_matches.jpg", img_matches);
	cv::imshow("best matches ...", img_matches);
	// cv::waitKey(0);
	if (cv::waitKey(33) >= 0)
	{
		cv::imwrite("best_matches.jpg", img_matches);
	   
		cv::destroyWindow("best matches ...");
	
	}

	
	// ����������� ������ ������ �����
	for (int i = 0; i < (int)good_matches.size(); ++i)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}
	



	// ������ ������� ����������  - ������������� �����������, 	
	cv::Mat homography = cv::findHomography(img2, img1, cv::RANSAC);


	if (homography.empty())
	{
		std::cout << "\nCan't find homography matrix" << std::endl;
		return -10;
	}


	// ���� ����������� ����� �������� �� ������� temp_panorama

	// �������� ��������� ������� ������������� -  homography � ����������� image_02 -> ���������  �����������: panorama
	// ������� ����� ������� ��������� �������
	cv::Size sz(image_01.cols + image_02.cols, image_01.rows);
	int type = image_02.type();

	cv::Mat panorama;	// ���������� ������������ ����������� �  panorama;
	// �������� ��������� ������� ������������� -  homography � ����������� image_02 -> ���������  �����������: warpresult2
	cv::warpPerspective(image_02_origin, panorama, homography, sz);
		
	cv::Mat half(panorama, cv::Rect(0, 0, image_01.cols, image_01.rows));
	image_01_origin.copyTo(half);
	imwrite("panorama.jpg", panorama);


	std::cout << "\n End prog" << std::endl;
	return 0;
}


