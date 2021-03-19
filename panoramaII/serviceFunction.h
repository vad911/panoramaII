#ifndef SERVICEFUNCTION_H
#define SERVICEFUNCTION_H


#include <opencv2/opencv.hpp>

//=======================================================================================
//
//		Description: Вспомогательные функции проекта
//		Auth:	Vadim Yarmushov
//		Date: 17/03/21
//		Ver: 0.0.1
//		
//=======================================================================================



namespace serviceFunction {


	void print_instruction();	// вывод инструкции на экран



// скопировать изображение в выбранную область Большего изображения
// cv::Mat mat - матрица куда будет копироваться изображение из cv::Mat in_mat
// cv::Mat in_mat - матрица фрагмент изображенияя, откуда будем копировать в большее изображение  - mat
// в соотвествующий region
// пример есть большая картинка cv::Mat mat( 600х 400) (w х h)
// копируем туда фрагмент - cv::Mat in_mat  150х170 (w х h)
// в cv::Rect region (x,y, w,h)  x,y - координаты левой верхней точки прямоугольника региона в большой картинке mat  куда будем копировать -  region(200,200, 150,170)
// размер in_mat должен быть такой же как и регион region, 

	int copyToAreaInImage(cv::Mat& mat, cv::Mat& in_mat, cv::Rect region);






	// скопировать изображение из области src_region  изображения src_mat в выбранную область dst_regio Большего изображения -dst_mat
 // размер src_region = dst_region, отличается только начальная верхняя левая точка
 // размер матрицы куда копируем не может быть меньше размера фрагмента dst_region, а также не может быть меньше src_region
 // если все нормально вернется 0, иначе код ошибки
	int copyToAreaInImage(cv::Mat& src_mat, cv::Rect src_region, cv::Mat& dst_mat, cv::Rect dst_region);







}








#endif
