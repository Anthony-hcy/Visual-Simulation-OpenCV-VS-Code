/*
 * @创建者: 半截诗 3492726416@qq.com
 * @创建日期: 2024-06-06 17:52:49
 * @上次编辑时间: 2024-06-06 17:53:09
 * @上次编辑者: 半截诗 3492726416@qq.com
 * @文件相对于项目的路径: \try\load.h
 * @描述: 
 */
#ifndef LOAD_H_
#define LOAD_H_

#include <opencv2/opencv.hpp>
#include "Img_deal.h"
//#include "IMG.h"
//#define IMG_ROW 120   //行
//#define IMG_COL 188		//列

//extern uint8_t IMG_DATA[IMG_ROW][IMG_COL];
extern cv::String folder;
extern uint8_t IMG[120][188];
void Imgread_Mat2Buffer(cv::String addr);
void Imshow_Buffer2Mat();
void Read_folder(cv::String folder_addr, int FramTime);
void Draw_Color(uint16_t x, uint16_t y, uint8_t Color);

#endif // !LOAD_H_
