#include "serviceFunction.h"
#include <iostream>
#include <string>





void serviceFunction::print_instruction()
{
	std::string str = R"RAW( using: > panorama  img_01 img_02 )RAW";
	std::cout << "\n "<< str << std::endl;
}



// скопировать изображение в выбранную область Большего изображения
// cv::Mat mat - матрица куда будет копироваться изображение из cv::Mat in_mat
// cv::Mat in_mat - матрица фрагмент изображенияя, откуда будем копировать в большее изображение  - mat
// в соотвествующий region
// пример есть большая картинка cv::Mat mat( 600х 400) (w х h)
// копируем туда фрагмент - cv::Mat in_mat  150х170 (w х h)
// в cv::Rect region (x,y, w,h)  x,y - координаты левой верхней точки прямоугольника региона в большой картинке mat  куда будем копировать -  region(200,200, 150,170)
// размер in_mat должен быть такой же как и регион region, 

int serviceFunction::copyToAreaInImage(cv::Mat& mat, cv::Mat& in_mat, cv::Rect region)
{
	int result = 0;


	// размеры изображения которое надо скопировать в mat
	int width = region.width;
	int heigth = region.height;


	const int offset_x = region.x;	// P0 - верхняя левая точка области куда копируем
	const int offset_y = region.y;

	int type = mat.type();	// сохраним тип входной матрицы

	for (int i = 0; i < heigth; ++i)
	{
		int it = offset_y + i;
		cv::Vec3b* ptr_mat = mat.ptr<cv::Vec3b>(it);					// создали и установили указатель на строчку offset_y + i в массиве mat
		cv::Vec3b* ptr_in_mat = in_mat.ptr<cv::Vec3b>(i);				// создали и установили указатель на строчку i в массиве in_mat

		for (int j = 0; j < width; ++j)
		{

			int itt = offset_x + j;

			ptr_mat[itt] = ptr_in_mat[j];


		}

	}

	return result;

}



// скопировать изображение из области src_region  изображения src_mat в выбранную область dst_regio Большего изображения -dst_mat
 // размер src_region = dst_region, отличается только начальная верхняя левая точка
 // размер матрицы куда копируем не может быть меньше размера фрагмента dst_region, а также не может быть меньше src_region
 // если все нормально вернется 0, иначе код ошибки
int serviceFunction::copyToAreaInImage(cv::Mat& src_mat, cv::Rect src_region, cv::Mat& dst_mat, cv::Rect dst_region)
{

	int result = 0;

	// размеры фрагмента изображения которое надо скопировать в mat
	int src_width = src_region.width;
	int src_heigth = src_region.height;

	int dst_width = dst_region.width;	// габариты изображения должны быть одинаковыми как и у области из которой копируем фрагмент
	int dst_heigth = dst_region.height;


	// координаты верхней левой точки исходной области из которой будем копировать наш фрагмент
	int src_Point_x = src_region.x;
	int src_Point_y = src_region.y;

	// координаты верхней левой точки области в которую будем копировать наш фрагмент
	int dst_Point_x = dst_region.x;
	int dst_Point_y = dst_region.y;



	for (int i = 0; i < src_heigth; ++i)
	{
		int it_dst = dst_Point_y + i;
		cv::Vec3b* ptr_dst = dst_mat.ptr<cv::Vec3b>(it_dst);					// создали и установили указатель на строчку it_dst = dst_Point_y + i в массиве dst_mat

		int it_src = src_Point_y + i;
		cv::Vec3b* ptr_src = src_mat.ptr<cv::Vec3b>(it_src);					// создали и установили указатель на строчку it_src = src_Point_y + i в массиве src_mat

		for (int j = 0; j < src_width; ++j)
		{

			int itt_dst = dst_Point_x + j;				// расчитываем позицию по x куда копируем 
			int itt_src = src_Point_x + j;				// расчитываем позицию по x откуда копируем 

			ptr_dst[itt_dst] = ptr_src[itt_src];


		}

	}

	return result;;

}