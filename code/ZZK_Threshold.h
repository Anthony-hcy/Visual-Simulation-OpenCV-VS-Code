/*
 * @创建者: 半截诗 3492726416@qq.com
 * @创建日期: 2024-06-06 17:54:33
 * @上次编辑时间: 2024-06-06 17:55:10
 * @上次编辑者: 半截诗 3492726416@qq.com
 * @文件相对于项目的路径: \try\ZZK_Threshold.h
 * @描述: 
 */
#ifndef _ZZK_Threshold_H
#define _ZZK_Threshold_H
//#include "zf_common_headfile.h"
#include <stdint.h>
#define ROW 120
#define COL 188

#define LQOSTU_MIN02         0   //大津法计算阈值时的最小行数
#define LQOSTU_MAX02         (120-1) //大津法计算阈值时的最大行数
#define LQOSTU_THRERR        3     //动态，跳变沿高度取值
#define LQOSTU_QIANGGUANG    200   //强光阈值，大于阈值的亮度认为是强光
#define LQOSTU_QIANGNUM      500   //强光总点数的阈值

extern uint8_t cam_static;//当赛道出界，或者计算的灰度阈值异常时使用
extern uint8_t cam_thr_mid, cam_thr_max, cam_thr_min;
extern uint8_t cam_thr, cam_thr0;//摄像头的阈值
extern uint8_t HistoGram[256]; //在进行大津的计算阈值时用于统计赛道指定区域的各个阈值的个数数组
extern int LQ_ary[256];

extern uint8_t cam_strong_light;//强光标志位
extern uint8_t cam_normal_light;//正常光标志位
extern uint8_t cam_wesk_light;  //弱光标志位

uint8_t zzkLQGetOSTU0200(uint8_t LQOSTU_MIN, uint8_t LQOSTU_MAX);
uint8_t zzkLQGetOSTU0200_col(uint8_t LQOSTU_MIN, uint8_t LQOSTU_MAX, uint8_t lie_min, uint8_t lie_max);
#endif