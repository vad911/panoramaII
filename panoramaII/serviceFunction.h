#ifndef SERVICEFUNCTION_H
#define SERVICEFUNCTION_H


#include <opencv2/opencv.hpp>

//=======================================================================================
//
//		Description: ��������������� ������� �������
//		Auth:	Vadim Yarmushov
//		Date: 17/03/21
//		Ver: 0.0.1
//		
//=======================================================================================



namespace serviceFunction {


	void print_instruction();	// ����� ���������� �� �����



// ����������� ����������� � ��������� ������� �������� �����������
// cv::Mat mat - ������� ���� ����� ������������ ����������� �� cv::Mat in_mat
// cv::Mat in_mat - ������� �������� ������������, ������ ����� ���������� � ������� �����������  - mat
// � �������������� region
// ������ ���� ������� �������� cv::Mat mat( 600� 400) (w � h)
// �������� ���� �������� - cv::Mat in_mat  150�170 (w � h)
// � cv::Rect region (x,y, w,h)  x,y - ���������� ����� ������� ����� �������������� ������� � ������� �������� mat  ���� ����� ���������� -  region(200,200, 150,170)
// ������ in_mat ������ ���� ����� �� ��� � ������ region, 

	int copyToAreaInImage(cv::Mat& mat, cv::Mat& in_mat, cv::Rect region);






	// ����������� ����������� �� ������� src_region  ����������� src_mat � ��������� ������� dst_regio �������� ����������� -dst_mat
 // ������ src_region = dst_region, ���������� ������ ��������� ������� ����� �����
 // ������ ������� ���� �������� �� ����� ���� ������ ������� ��������� dst_region, � ����� �� ����� ���� ������ src_region
 // ���� ��� ��������� �������� 0, ����� ��� ������
	int copyToAreaInImage(cv::Mat& src_mat, cv::Rect src_region, cv::Mat& dst_mat, cv::Rect dst_region);







}








#endif
