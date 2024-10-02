/*
 * @创建者: 半截诗 3492726416@qq.com
 * @创建日期: 2024-06-06 17:52:34
 * @上次编辑时间: 2024-06-06 17:52:40
 * @上次编辑者: 半截诗 3492726416@qq.com
 * @文件相对于项目的路径: \try\load.cpp
 * @描述: 
 */
#include "load.h"
#include "Prepared.h"
#include "Img_deal.h"
//#include "IMG.h"
//uint8_t IMG_DATA[IMG_ROW][IMG_COL]; // 图片数据
//#define showImg PerImg
cv::String folder = "C:\\Users\\34927\\Desktop\\Image\\Image_2"; // 默认播放文件夹的路径
//cv::String folder = "C:\\Users\\34927\\Desktop\\aa";
std::vector<cv::String> imagePathList;					   // 存放图片的序列
typedef std::pair<cv::Point, uint8_t> Pix;
std::queue<Pix> ColorPix;

void Draw_Color(uint16_t col, uint16_t row, uint8_t Color)
{
	ColorPix.push({{col, row}, Color});
}

/**
 * @描述: 读取一张图像在Mat矩阵，转换为数组
 * @传入参数 {String} addr
 * @返回 {*}
 */
void Imgread_Mat2Buffer(cv::String addr)
{
	cv::Mat img(IMG_ROW, IMG_COL, CV_8UC1);
	img = cv::imread(addr, 0);
	if (img.rows != IMG_ROW || img.cols != IMG_COL)
	{
		printf_s("图片大小不适合！！！\n");
		return;
	}
	for (uint16_t i = 0; i < IMG_ROW; i++)
		for (uint16_t j = 0; j < IMG_COL; j++)
			IMG_DATA[i][j] = img.at<uchar>(i, j);

	cv::imshow("befor", img);
	//ImageDeal_main();
	Img_Deal();
}

/**
 * @描述: 将数组读取为Mat矩阵，并显示
 * 后续需要添加处理部分显示彩色标注
 * @返回 {*}
 */

void Imshow_Buffer2Mat()
{
	int i, j;
	cv::Mat img(IMG_ROW, IMG_COL, CV_8UC1);
	cv::Mat show(IMG_ROW, IMG_COL, CV_8UC3);

	for (uint16_t i = 0; i < IMG_ROW; i++)
		for (uint16_t j = 0; j < IMG_COL; j++)
			img.at<uchar>(i, j) = showImg[i][j];

	cv::cvtColor(img, show, cv::COLOR_GRAY2RGB);
	while (ColorPix.size())
	{
		Pix px= ColorPix.front();
		ColorPix.pop();
		if(px.second == BLACK)cv::circle(show, px.first, 0, cv::Scalar(0, 0, 0), -1);
		else if (px.second == WHITE)cv::circle(show, px.first, 0, cv::Scalar(255, 255, 255), -1);
		else if (px.second == BLUE)cv::circle(show, px.first, 0, cv::Scalar(255, 0, 0), -1);
		else if (px.second == RED)cv::circle(show, px.first, 0, cv::Scalar(0, 0, 255), -1);
		else if (px.second == GREEN)cv::circle(show, px.first, 0, cv::Scalar(0, 255, 0), -1);
	}
	cv::resize(show, show, cv::Size(IMG_COL * 4, IMG_ROW * 4), cv::INTER_LINEAR);
	cv::imshow("after", show);
}

/**
 * @描述: 读取文件夹的图片，中间通过算法处理数据并显示结果
 * @返回 {*}
 */
void Read_folder(cv::String folder_addr, int FramTime)
{
	cv::glob(folder_addr, imagePathList);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		std::cout << imagePathList[i] << "\n";
		Imgread_Mat2Buffer(imagePathList[i]);
		Imshow_Buffer2Mat();
		cv::waitKey(FramTime);
	}
	reset_staus();
	cv::waitKey(1000);
	cv::destroyAllWindows();
}