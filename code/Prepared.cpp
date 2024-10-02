/*
 * @创建者: 半截诗 3492726416@qq.com
 * @创建日期: 2024-06-06 17:53:32
 * @上次编辑时间: 2024-06-06 17:53:37
 * @上次编辑者: 半截诗 3492726416@qq.com
 * @文件相对于项目的路径: \try\Prepared.cpp
 * @描述: 
 */
#include "Prepared.h"

// Created by RUPC on 2022/9/20.

//uint8_t *PerImg_ip[RESULT_ROW][RESULT_COL];
//uint8_t PerImg[RESULT_ROW][RESULT_COL];
//uint8_t BlackColor = 0;

#if ImagePerspective
void ImagePerspective_Init(void)
{
   // double change_un_Mat[3][3] = {{0.117206, -0.101210, 3.725771}, {0.005767, -0.002085, 2.745541}, {0.000099, -0.001054, 0.145553}};
    double change_un_Mat[3][3] = { {0.003175,-0.002640,0.112073},{-0.000000,0.000004,0.124451},{0.000000,-0.000029,0.004393} };
    for (int i = 0; i < RESULT_COL; i++)
    {
        for (int j = 0; j < RESULT_ROW; j++)
        {
            #if ImagePerspective
            int local_x = (int)((change_un_Mat[0][0] * i + change_un_Mat[0][1] * j + change_un_Mat[0][2]) / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j + change_un_Mat[2][2]));
            int local_y = (int)((change_un_Mat[1][0] * i + change_un_Mat[1][1] * j + change_un_Mat[1][2]) / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j + change_un_Mat[2][2]));
            if (local_x >= 0 && local_y >= 0 && local_y < USED_ROW && local_x < USED_COL)
            {
                PerImg_ip[j][i] = &PER_IMG[local_y][local_x];
            }
            else
            {
                PerImg_ip[j][i] = &BlackColor; //&PER_IMG[0][0];
            }
            #else
            PerImg_ip[j][i] = &PER_IMG[j][i];
            #endif
        }
    }
}
#endif
//
//bool is_legal(int16_t row, int16_t col)
//{
//    if (row < 0 || row >= IMG_ROW)
//        return false;
//    if (col < 0 || col >= IMG_COL)
//        return false;
//    if (PerImg_ip[row][col] == &BlackColor)
//        return false;
//    return true;
//}