#include "serviceFunction.h"
#include <iostream>
#include <string>





void serviceFunction::print_instruction()
{
	std::string str = R"RAW( using: > panorama  img_01 img_02 )RAW";
	std::cout << "\n "<< str << std::endl;
}



// ����������� ����������� � ��������� ������� �������� �����������
// cv::Mat mat - ������� ���� ����� ������������ ����������� �� cv::Mat in_mat
// cv::Mat in_mat - ������� �������� ������������, ������ ����� ���������� � ������� �����������  - mat
// � �������������� region
// ������ ���� ������� �������� cv::Mat mat( 600� 400) (w � h)
// �������� ���� �������� - cv::Mat in_mat  150�170 (w � h)
// � cv::Rect region (x,y, w,h)  x,y - ���������� ����� ������� ����� �������������� ������� � ������� �������� mat  ���� ����� ���������� -  region(200,200, 150,170)
// ������ in_mat ������ ���� ����� �� ��� � ������ region, 

int serviceFunction::copyToAreaInImage(cv::Mat& mat, cv::Mat& in_mat, cv::Rect region)
{
	int result = 0;


	// ������� ����������� ������� ���� ����������� � mat
	int width = region.width;
	int heigth = region.height;


	const int offset_x = region.x;	// P0 - ������� ����� ����� ������� ���� ��������
	const int offset_y = region.y;

	int type = mat.type();	// �������� ��� ������� �������

	for (int i = 0; i < heigth; ++i)
	{
		int it = offset_y + i;
		cv::Vec3b* ptr_mat = mat.ptr<cv::Vec3b>(it);					// ������� � ���������� ��������� �� ������� offset_y + i � ������� mat
		cv::Vec3b* ptr_in_mat = in_mat.ptr<cv::Vec3b>(i);				// ������� � ���������� ��������� �� ������� i � ������� in_mat

		for (int j = 0; j < width; ++j)
		{

			int itt = offset_x + j;

			ptr_mat[itt] = ptr_in_mat[j];


		}

	}

	return result;

}



// ����������� ����������� �� ������� src_region  ����������� src_mat � ��������� ������� dst_regio �������� ����������� -dst_mat
 // ������ src_region = dst_region, ���������� ������ ��������� ������� ����� �����
 // ������ ������� ���� �������� �� ����� ���� ������ ������� ��������� dst_region, � ����� �� ����� ���� ������ src_region
 // ���� ��� ��������� �������� 0, ����� ��� ������
int serviceFunction::copyToAreaInImage(cv::Mat& src_mat, cv::Rect src_region, cv::Mat& dst_mat, cv::Rect dst_region)
{

	int result = 0;

	// ������� ��������� ����������� ������� ���� ����������� � mat
	int src_width = src_region.width;
	int src_heigth = src_region.height;

	int dst_width = dst_region.width;	// �������� ����������� ������ ���� ����������� ��� � � ������� �� ������� �������� ��������
	int dst_heigth = dst_region.height;


	// ���������� ������� ����� ����� �������� ������� �� ������� ����� ���������� ��� ��������
	int src_Point_x = src_region.x;
	int src_Point_y = src_region.y;

	// ���������� ������� ����� ����� ������� � ������� ����� ���������� ��� ��������
	int dst_Point_x = dst_region.x;
	int dst_Point_y = dst_region.y;



	for (int i = 0; i < src_heigth; ++i)
	{
		int it_dst = dst_Point_y + i;
		cv::Vec3b* ptr_dst = dst_mat.ptr<cv::Vec3b>(it_dst);					// ������� � ���������� ��������� �� ������� it_dst = dst_Point_y + i � ������� dst_mat

		int it_src = src_Point_y + i;
		cv::Vec3b* ptr_src = src_mat.ptr<cv::Vec3b>(it_src);					// ������� � ���������� ��������� �� ������� it_src = src_Point_y + i � ������� src_mat

		for (int j = 0; j < src_width; ++j)
		{

			int itt_dst = dst_Point_x + j;				// ����������� ������� �� x ���� �������� 
			int itt_src = src_Point_x + j;				// ����������� ������� �� x ������ �������� 

			ptr_dst[itt_dst] = ptr_src[itt_src];


		}

	}

	return result;;

}