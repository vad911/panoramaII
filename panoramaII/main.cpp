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

	
	cv::BFMatcher  matcher(cv::NORM_HAMMING);		// ����� ������������ BFMatcher �������������
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



	std::vector<cv::Point2f> img1;		// ����� ����� 
	std::vector<cv::Point2f> img2;		// ������, ����������� � �����

	// �
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
	
	// ����������� ������ ������ �����
	for (int i = 0; i < (int)good_matches.size(); ++i)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}
	


	// === 5. ��������� ��� ���������� � ����. ===
	// ������ ������� ����������  - ������������� �����������, 	
	cv::Mat homography = cv::findHomography(img1, img2, cv::RANSAC);



	//Apply panorama correction
	int width = image_01.cols + image_02.cols;
	int height = image_01.rows + image_02.rows;

	cv::Size sz(width, height);
	int type = image_01.type();
	cv::Mat panorama(sz,type);
	
	cv::warpPerspective(image_02, panorama, homography, sz);	// �������� ���������� image_02 � panorama

	
	imwrite("panorama.jpg", panorama);
	/*
	cv::Mat roiImgResult_Left  = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, t_width, t_height));



	// ������ ������������ ��� ������������� ������� -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = panorama(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  ��������� ROI �������  roiImg1 � roiImgResult_Left � roiImg2 � roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - ����� ������� �����������  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - ������ ������� �����������  imgResult
	*/

//	imwrite("panorama.jpg", panorama);





	/*
	//############################################ [1]
	// === 5. ��������� ��� ���������� � ����. ===


	
	//�������  ������ ������� - �������� ����� �� �������� �����
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


	//cv::Mat panorama;	 // ����� ������ ���������� �������� - ���������
	cv::Mat warpedImage02(image_02.rows, 2 * image_02.cols, image_02.type());

	
	// �������� ��������� ����� �������� ��������,
	// �� ������ � ������
	std::vector<cv::Point2f> inputCorners(4);
	inputQuad[0] = cv::Point2f(0, 0);
	inputQuad[1] = cv::Point2f(image_02.cols, 0);
	inputQuad[2] = cv::Point2f(image_02.cols, image_02.rows);
	inputQuad[3] = cv::Point2f(0, image_02.rows);
	
	// ��������, ��� ��� ����� - ��������� �������������
	cv::Point2f outputQuad[4];
	outputQuad[0] = cv::Point2f(0, 0);
	outputQuad[1] = cv::Point2f(panorama.cols, 0);
	outputQuad[2] = cv::Point2f(panorama.cols, panorama.rows);
	outputQuad[3] = cv::Point2f(0, panorama.rows);
	

	// ������ ������� ����������  - ������������� �����������, 	
	cv::Mat homography = cv::findHomography(img1, img2, cv::RANSAC);

	// �������� ������� �������������
	cv::Mat M = getPerspectiveTransform(inputQuad, outputQuad);


	// ���� ����������� �� ������ �������������
	std::vector<cv::Point2f> inputCorners(4);
	inputCorners[0] = cv::Point2f(0, 0);
	inputCorners[1] = cv::Point2f(image_02.cols, 0);
	inputCorners[2] = cv::Point2f(0, image_02.rows);
	inputCorners[3] = cv::Point2f(image_02.cols, image_02.rows);

	// ���� ����������� ����� �������������, ��� ����� ���� ����� ������������� ����������
	std::vector<cv::Point2f> outputCorners(4);

	// ������ �������� ���� - outputCorners
	perspectiveTransform(inputCorners, outputCorners, M);

	cv::Rect br = cv::boundingRect(outputCorners);

	for (int i = 0; i < 4; i++) {
		outputQuad[i] += cv::Point2f(-br.x, -br.y);
	}

	M = getPerspectiveTransform(inputQuad, outputQuad);

	cv::warpPerspective(image_02, warpedImage02, homography, br.size());
		
	imwrite("warpedImage02.jpg", warpedImage02);
	

	// -------- ������ ������ ����������� � ����
	cv::Mat panorama(image_01.rows, 3 * image_01.cols, image_01.type());

	cv::Mat roiImgResult_Left = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, image_02.cols, image_02.rows));



	// ������ ������������ ��� ������������� ������� -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = warpedImage02(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  ��������� ROI �������  roiImg1 � roiImgResult_Left � roiImg2 � roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - ����� ������� �����������  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - ������ ������� �����������  imgResult

	
	imwrite("panorama.jpg", panorama);

	//############################################ [2]
	*/

	/*
	cv::Mat lambda = cv::getPerspectiveTransform(inputQuad, outputQuad);


	// ������ ������� ����������  - ������������� �����������, 	
	cv::Mat homography = cv::findHomography(img1, img2, cv::RANSAC);


	if (homography.empty())
	{
		std::cout << "\nCan't find homography matrix" << std::endl;
		return -10;
	}


	// ���� ����������� ����� �������� �� ������� temp_panorama

	// �������� ��������� ������� ������������� -  homography � ����������� image_01 -> ���������  �����������: panorama
	// ������� ����� ������� ��������� �������
	
	cv::Mat warpresult2;	// ���������� ������������ ����������� �  warpresult2;
	// �������� ��������� ������� ������������� -  homography � ����������� image_02 -> ���������  �����������: warpresult2
	cv::warpPerspective(image_02, warpresult2, lambda, cv::Size(2 * image_02.cols, image_02.rows));
	//cv::warpPerspective(image_02, warpresult2, homography, cv::Size(2 * image_02.cols, image_02.rows));


			
	cv::Mat roiImgResult_Left = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, image_02.cols, image_02.rows));

	

	// ������ ������������ ��� ������������� ������� -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = warpresult2(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  ��������� ROI �������  roiImg1 � roiImgResult_Left � roiImg2 � roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - ����� ������� �����������  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - ������ ������� �����������  imgResult

	imwrite("warpresult2.jpg", warpresult2);

	imwrite("panorama.jpg", panorama);
	*/
	

	/*
	// === 5. ��������� ��� ���������� � ����. ===
	cv::Mat warpresult2;	// ���������� ������������ ����������� �  warpresult2;

	// ������ ������� ����������  - ������������� �����������
	// ���������� ����� � keypoints_01 � keypoints_02  �� ������ ���� ������ 4.
	//
	cv::Mat homography = cv::findHomography( img1, img2, cv::RANSAC);


	// �������� ��������� ������� ������������� -  homography � ����������� image_02 -> ���������  �����������: warpresult2
	cv::warpPerspective(image_02, warpresult2, homography, cv::Size(2 * image_02.cols, image_02.rows));

	//cv::Mat panorama;						// ����� ������ ���������� �������� - ���������
	cv::Mat panorama(image_01.rows, 3 * image_01.cols, image_01.type());


	cv::Mat roiImgResult_Left  = panorama(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImgResult_Right = panorama(cv::Rect(image_01.cols, 0, image_02.cols, image_02.rows));

	// ������ ������������ ��� ������������� ������� -  ROI: roiImg1, roiImg2
	cv::Mat roiImg1 = image_01(cv::Rect(0, 0, image_01.cols, image_01.rows));
	cv::Mat roiImg2 = warpresult2(cv::Rect(0, 0, image_02.cols, image_02.rows));

	//  ��������� ROI �������  roiImg1 � roiImgResult_Left � roiImg2 � roiImgResult_Right
	roiImg1.copyTo(roiImgResult_Left);	//Img1  - ����� ������� �����������  imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2  - ������ ������� �����������  imgResult

	imshow("result", panorama);
	imwrite("panorama.jpg", panorama);

	*/


	std::cout << "\n End prog" << std::endl;
	return 0;
}


