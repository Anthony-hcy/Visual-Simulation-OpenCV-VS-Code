#ifndef IMG_DEAL_H_
#define IMG_DEAL_H_

#include <stdint.h>
#include "load.h"
using namespace std;
#define ImagePerspective (0) //逆透视开启

#define bin_jump_num    1//跳过的点数
#define border_max  RESULT_COL-2 //边界最大值
#define border_min  1   //边界最小值

#define WHITE    255
#define BLACK    0
#define RED      128
#define GREEN    100
#define BLUE     254

#define IMG_ROW 120     //行
#define IMG_COL 188    //列
//#define image_h 120//图像高度
//#define image_w 188//图像宽度
//#define RESULT_ROW 120 // 结果图行列
//#define RESULT_COL 188
#define image_h 120
#define image_w 188
#define RESULT_ROW IMG_ROW
#define RESULT_COL IMG_COL
#define USED_ROW IMG_ROW // 用于透视图的行列
#define USED_COL IMG_COL

#define PER_IMG IMG_DATA      // SimBinImage:用于透视变换的图像
#define ImageUsed *PerImg_ip //*PerImg_ip定义使用的图像，ImageUsed为用于巡线和识别的图像
#define showImg PerImg
#define tmImage IMG_DATA

//extern int stop;
extern uint8_t* PerImg_ip[RESULT_ROW][RESULT_COL];
extern uint8_t PerImg[RESULT_ROW][RESULT_COL];
extern uint8_t BlackColor;
extern uint8_t IMG_DATA[IMG_ROW][IMG_COL];
extern uint8_t center_line[RESULT_ROW];

extern uint8_t l_border[RESULT_ROW];//左线数组x
//uint8_t l_border_y[RESULT_ROW];//y
extern uint8_t r_border[RESULT_ROW];//右线数组x
//uint8_t r_border_y[RESULT_ROW];//y
extern uint8_t center_line[RESULT_ROW];//中线数组x
//uint8_t center_line_y[RESULT_ROW];//y
extern uint8_t hightest;//定义一个最高行，tip：这里的最高指的是y值的最小


void cross_fill(uint8_t(*image)[image_w], uint8_t* l_border,
    uint8_t* r_border, uint16_t total_num_l, uint16_t total_num_r,
    uint16_t* dir_l, uint16_t* dir_r, uint16_t(*points_l)[2],
    uint16_t(*points_r)[2]);
void cross_fill_in(uint8_t(*image)[image_w], uint8_t* l_border,
    uint8_t* r_border, uint16_t total_num_l, uint16_t total_num_r,
    uint16_t* dir_l, uint16_t* dir_r, uint16_t(*points_l)[2],
    uint16_t(*points_r)[2]);

void ImagePerspective_Init(void);
int min_value(int x, int y);
int median_value(int x, int low, int up);
int my_abs(int value);
int limit_a_b(int x, int a, int b);
int limit1(int x, int y);
void Draw_Board(uint8_t row[], uint8_t col[], uint8_t size);
uint8_t GetOSTU();
void Get_OSTU_Img(uint8_t Thresh);
void show_PerImg();
float Slope_Calculate(uint8_t begin, uint8_t end, uint8_t* border);
void calculate_s_i(uint8_t start, uint8_t end, uint8_t* border, float* slope_rate, float* intercept);
//void cross_fill();
//void cross_fill_in();

uint8_t get_start_point(uint8_t start_row, uint8_t(*image)[RESULT_COL]);
void search_l_r(uint16_t break_flag, uint8_t(*image)[RESULT_COL],
    uint16_t* l_stastic, uint16_t* r_stastic,
    uint8_t l_start_x, uint8_t l_start_y,
    uint8_t r_start_x, uint8_t r_start_y, uint8_t* hightest);
void get_left(uint16_t total_L);
void get_right(uint16_t total_R);
void Check_Cross();
void Check_Cross_In();
void Crossing();
void Check_Circle();
void Circle();

void image_filter(uint8_t(*PerImg)[RESULT_COL]);
void image_draw_rectan(uint8_t(*image)[RESULT_COL]);



//void ImageDeal_main(void);
void reset_staus(void);

void Img_Deal();

#endif /* IMG_DEAL_H_ */