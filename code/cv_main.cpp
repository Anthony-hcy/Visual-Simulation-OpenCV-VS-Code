/*
 * @创建者: 半截诗 3492726416@qq.com
 * @创建日期: 2024-06-06 18:46:54
 * @上次编辑时间: 2024-06-06 19:09:29
 * @上次编辑者: 半截诗 3492726416@qq.com
 * @文件相对于项目的路径: \vscode-collaboration-opencv-visualsimulation\code\cv_main.cpp
 * @描述: 
 */
#include "cv_main.h"
#include "load.h"
#include "Prepared.h"

int input, runing_flag = 1;
int frame_delay;
char input_str[1000];
cv::String img_addr;

int main()
{
#if ImagePerspective
	ImagePerspective_Init();
#endif
	while (runing_flag)
	{ 
		printf_s("这是基于opencv开发的一个仿真代码\n");
		printf_s("请输入数字以进行你想要进行的操作\n");
		printf_s("1、处理单张图片\n");
		printf_s("2、播放整个文件夹的图片\n");
		printf_s("3、播放默认文件夹\n");
		printf_s("4、退出\n");
		std::cin >> input;
		switch (input)
		{
		case 1:
			printf_s("请输入图片地址\n");
			std::cin >> input_str;
			img_addr = input_str;
			Imgread_Mat2Buffer(img_addr);
			Imshow_Buffer2Mat();
			cv::waitKey(0);
			cv::destroyAllWindows();
			break;
		case 2:
			printf_s("请输入文件夹地址\n");
			std::cin >> input_str;
			img_addr = input_str;
			Read_folder(img_addr, 1);
			break;
		case 3:
			printf_s("请输入每张图片处理间隔（0为按下按键后才切换）\n");
			std::cin >> frame_delay;
			Read_folder(folder, frame_delay);
			break;
		case 4:
			runing_flag = 0;
			break;
		default:
			break;
		}
	}
	cv::waitKey(0);
	return 0;
}