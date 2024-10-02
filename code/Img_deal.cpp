/*
 *                   江城子 . 程序员之歌
 * 
 *               十年生死两茫茫，写程序，到天亮。
 *                   千行代码，Bug何处藏。
 *               纵使上线又怎样，朝令改，夕断肠。
 * 
 *               领导每天新想法，天天改，日日忙。
 *                   相顾无言，惟有泪千行。
 *               每晚灯火阑珊处，夜难寐，加班狂。
 * 
 */
/*
 * @创建者: 半截诗 3492726416@qq.com
 * @创建日期: 2024-06-06 18:46:54
 * @上次编辑时间: 2024-06-09 14:17:54
 * @上次编辑者: 半截诗 3492726416@qq.com
 * @文件相对于项目的路径: \vscode-collaboration-opencv-visualsimulation\code\Img_deal.cpp
 * @描述: 
 */
#include "Img_deal.h"
#include "Prepared.h"
#include "ZZK_Threshold.h"
//int stop = 0;

//uint8 *IMG_data[60][94];
uint8_t IMG_DATA[IMG_ROW][IMG_COL]; // 图片数据
uint8_t* PerImg_ip[RESULT_ROW][RESULT_COL];
uint8_t PerImg[RESULT_ROW][RESULT_COL];
uint8_t BlackColor = 0;
#define USE_num RESULT_ROW*3    //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

#if ImagePerspective
void ImagePerspective_Init(void)
{
    ////// double change_un_Mat[3][3] = {{0.117206, -0.101210, 3.725771}, {0.005767, -0.002085, 2.745541}, {0.000099, -0.001054, 0.145553}};
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
//-----------------------以下代码--------------------------------------------------------------
uint8_t l_border[RESULT_ROW];//左线数组x
//uint8_t l_border_y[RESULT_ROW];//y
uint8_t r_border[RESULT_ROW];//右线数组x
//uint8_t r_border_y[RESULT_ROW];//y
uint8_t center_line[RESULT_ROW];//中线数组x
//uint8_t center_line_y[RESULT_ROW];//y


uint8_t start_point_l[2] = { 0 };//左边起点的x，y值
uint8_t start_point_r[2] = { 0 };//右边起点的x，y值

//存放点的x，y坐标
uint16_t points_l[(uint16_t)USE_num][2] = { {  0 } };//左线
uint16_t points_r[(uint16_t)USE_num][2] = { {  0 } };//右线
uint16_t dir_r[(uint16_t)USE_num] = { 0 };//用来存储右边生长方向
uint16_t dir_l[(uint16_t)USE_num] = { 0 };//用来存储左边生长方向
uint16_t data_stastics_l = 0;//统计左边找到点的个数
uint16_t data_stastics_r = 0;//统计右边找到点的个数
uint16_t data_stastics_l_in = 0;//统计左边找到点的个数
uint16_t data_stastics_r_in = 0;//统计右边找到点的个数
uint8_t hightest = 0;//最高点

uint8_t Lfound_1 = 0, Rfound_1 = 0, Lfound_2 = 0, Rfound_2 = 0, L_jieshu = 0, R_jieshu = 0;
uint8_t Lfound_1_in = 0, Rfound_1_in = 0, Lfound_2_in = 0, Rfound_2_in = 0, L_jieshu_in = 0, R_jieshu_in = 0;

//迷宫法巡线
//const signed char rowfront[4] = { -1, 0, 1, 0 };
//const signed char colfront[4] = { 0, 1, 0, -1 };
//const signed char rowleftfront[4] = { -1, -1, 1, 1 };
//const signed char colleftfront[4] = { -1, 1, 1, -1 };
//const signed char rowrightfront[4] = { -1, 1, 1, -1 };
//const signed char colrightfront[4] = { 1, 1, -1, -1 };

//enum track_type_e
//{
//  TRACK_NONE = 0,
//  TRACK_LEFT,
//  TRACK_RIGHT,
//  TRACK_NUM,
//};
//enum track_type_e track_type = TRACK_NONE;//巡线方式

enum cross_type_e
{
    CROSS_NONE = 0,     // 非十字模式
    CROSS_FOUND,        // 找到了左右前后两个L角点
    CROSS_BANRU,        //进入十字内部
    CROSS_IN,
    CROSS_FOUND_IN,
    CROSS_BANRU_IN,
};//枚举，CROSS_NONE = 0，CROSS_BEGIN = 1，CROSS_IN = 2，CROSS_NUM = 3...，
enum cross_type_e cross_type = CROSS_NONE;

enum circle_type_e
{
    CIRCLE_NONE=0,
    CIRCLE_1,
    CIRCLE_2,
    CIRCLE_3,
    CIRCLE_4,
    CIRCLE_5,
    CIRCLE_6,
    CIRCLE_7,
    CIRCLE_8,
    CIRCLE_9,
    CIRCLE_10,
    CIRCLE_11,
};
enum circle_type_e circle_type=CIRCLE_NONE;
/*
函数名称：void Img_Deal(void)
功能说明：最终处理函数
参数说明：无
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_process();
 */
void Img_Deal()
{
    memset(PerImg, 0, sizeof(PerImg));
    //cout << (int)GetOSTU() << endl;
#if ImagePerspective
    ImagePerspective_Init();
#endif
   // Get_OSTU_Img(zzkLQGetOSTU0200(LQOSTU_MIN02, LQOSTU_MAX02));
     Get_OSTU_Img(GetOSTU());
    uint16_t i;
    /*提取赛道边界*/
    image_filter(PerImg);//滤波
    image_draw_rectan(PerImg);//预处理
    //清零
    data_stastics_l = 0;
    data_stastics_r = 0;
    if (get_start_point(RESULT_ROW - 2, PerImg))//找到起点了，再执行八领域，没找到就一直找
    {
        //printf("正在开始八领域\n");
        search_l_r((uint16_t)USE_num, PerImg, &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
        //printf_s("jie shu\n");                                                  //zuo x y                            you x y
//printf_s("%d/n", data_stastics_l);
// 从爬取的边界线内提取边线 ， 这个才是最终有用的边线
        get_left(data_stastics_l);
        get_right(data_stastics_r);
        //printf_s("l=%d r=%d\n", data_stastics_l, data_stastics_r);
        //printf_s("hightest=%d\n", hightest);
        printf_s("CROSS_TYPE======  %d    \n", cross_type);
        printf_s("CIRCLE_TYPE=====  %d    \n", circle_type);
         //Crossing();
         //Check_Circle();
        
        Circle();
        //处理函数放这里，不要放到if外面去了，不要放到if外面去了，不要放到if外面去了，重要的事说三遍
    }

 //show_PerImg();

 //显示图像   显示关掉
 //ips154_displayimage032_zoom(bin_image[0], image_w, image_h, image_w, image_h, 0, 0);

 //根据最终循环次数画出边界点
        for (i = 0; i < data_stastics_l; i++)
        {
            Draw_Color(points_l[i][0] + 2, points_l[i][1], GREEN);//显示起点
            //printf_s("%d  ", dir_l[i]);
        }
        for (i = 0; i < data_stastics_r; i++)
        {
           // Draw_Color(points_r[i][0] - 2, points_r[i][1], RED);//显示起点
            //printf_s("%d  ", dir_r[i]);
            
        }


    for (i = hightest; i < RESULT_ROW - 1; i++)
    {
        center_line[i] = (l_border[i] + r_border[i]) >> 1;//求中线
        Draw_Color(center_line[i], i, BLUE);
       // Draw_Color(l_border[i], i, GREEN);
        Draw_Color(r_border[i], i, RED);
        //printf_s("%d ", l_border[i]);
      //  tft180_draw_point(l_border[i]-28, i, RGB565_GREEN);
      //  tft180_draw_point(r_border[i]-28, i, RGB565_BLUE);
       // tft180_draw_point(center_line[i], i, RGB565_RED);
    }

    //        if((PerImg[60][94]==0&&PerImg[58][94]==0&&PerImg[62][94]==0&&PerImg[60][96]==0&&PerImg[60][92]==0)||stop==1)
    //        {
    //
    //            stop=1;
    //        }
    //        else{
    //            stop=0;
    //        }
}

//---------------------------基本函数-------------------------------------------------------------
int min_value(int x, int y)//最小
{
    return (x) > (y) ? (y) : (x);
}
int median_value(int x, int low, int up)//返回x,low,up的中间值
{
    return x > up ? up : x < low ? low : x;
}
int my_abs(int value)//求绝对值
{
    if (value >= 0) return value;
    else return -value;
}
int limit_a_b(int x, int a, int b)//求x,y中的最小值
{
    if (x < a) x = a;
    if (x > b) x = b;
    return x;
}
int limit1(int x, int y)
{
    if (x > y)             return y;
    else if (x < -y)       return -y;
    else                return x;
}

void Draw_Board(uint8_t row[], uint8_t col[], uint8_t size)
{
    for (uint8_t i = size; i > 0; i--)
    {
        Draw_Color(col[i], row[i], BLUE);
        //tft180_draw_point(col[i], row[i], BLUE);
    }
}

uint8_t GetOSTU()//大津法
{
    uint16_t i, j;
    uint32_t Amount = 0;
    uint32_t PixelBack = 0;
    uint32_t PixelIntegralBack = 0;
    uint32_t PixelIntegral = 0;
    int32_t PixelIntegralFore = 0;
    int32_t PixelFore = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    int16_t MinValue, MaxValue;
    uint8_t Threshold = 0;
    uint8_t HistoGram[256] = { 0 }; //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; // 初始化灰度直方图

    for (j = 0; j < IMG_ROW; j++)
    {
        for (i = 0; i < IMG_COL; i++)
        {
            HistoGram[tmImage[j][i]]++; // 统计灰度级中每个像素在整幅图像中的个数
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++)
        ; // 获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--)
        ; // 获取最大灰度的值

    if (MaxValue == MinValue)
        return (uint8_t)MaxValue; // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)
        return (uint8_t)MinValue; // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j]; //  像素总数

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j; // 灰度值总数
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];                                              // 前景像素点数
        PixelFore = Amount - PixelBack;                                                    // 背景像素点数
        OmegaBack = (double)PixelBack / Amount;                                            // 前景像素百分比
        OmegaFore = (double)PixelFore / Amount;                                            // 背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;                                             // 前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             // 背景灰度值
        MicroBack = (double)PixelIntegralBack / PixelBack;                                 // 前景灰度百分比
        MicroFore = (double)PixelIntegralFore / PixelFore;                                 // 背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); // 计算类间方差
        if (Sigma > SigmaB)                                                                // 遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = (uint8_t)j;
        }
    }
    return Threshold; // 返回最佳阈值;
}

void Get_OSTU_Img(uint8_t Thresh)//非黑即白
{
    for (uint8_t i = 0; i < IMG_ROW; i++)
        for (uint8_t j = 0; j < IMG_COL; j++)
            //#if ImagePerspective
             //           if (ImageUsed[i][j] > Thresh)//逆透视
            //#else
            if (PER_IMG[i][j] > Thresh)
                //#endif
                PerImg[i][j] = 255;
            else
                PerImg[i][j] = 0;
}

/*完成摄像头初始化后，调用一次ImagePerspective_Init，此后，直接调用ImageUsed   即为透视结果*/
void show_PerImg()//灰度图
{
    for (uint8_t i = 0; i < IMG_ROW; i++)
        for (uint8_t j = 0; j < IMG_COL; j++)
        {
            //  if (PerImg_ip[i][j] == &BlackColor)
            //      continue;
            //  else
             // {
                  //PerImg[i][j] = ImageUsed[i][j];
            PerImg[i][j] = PER_IMG[i][j];
            //  }
        }
}

/**
* @brief 最小二乘法
* @param uint8 begin                输入起点
* @param uint8 end                  输入终点
* @param uint8 *border              输入需要计算斜率的边界首地址
*  @see CTest       Slope_Calculate(start, end, border);//斜率
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
*/
float Slope_Calculate(uint8_t begin, uint8_t end, uint8_t* border)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16_t i = 0;
    float result = 0;
    static float resultlast;

    for (i = begin; i < end; i++)//y
    {
        xsum += i;
        ysum += border[i];
        xysum += i * (border[i]);
        x2sum += i * i;

    }
    if ((end - begin) * x2sum - xsum * xsum) //判断除数是否为零
    {
        result = ((end - begin) * xysum - xsum * ysum) / ((end - begin) * x2sum - xsum * xsum);
        resultlast = result;
    }
    else
    {
        result = resultlast;
    }
    return result;
}

/**
* @brief 计算斜率截距
* @param uint8 start                输入起点
* @param uint8 end                  输入终点
* @param uint8 *border              输入需要计算斜率的边界
* @param float *slope_rate          输入斜率地址
* @param float *intercept           输入截距地址
*  @see CTest       calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
*/
void calculate_s_i(uint8_t start, uint8_t end, uint8_t* border, float* slope_rate, float* intercept)
{
    uint16_t i, num = 0;
    uint16_t xsum = 0, ysum = 0;
    float y_average, x_average;

    num = 0;
    xsum = 0;
    ysum = 0;
    y_average = 0;
    x_average = 0;
    for (i = start; i < end; i++)
    {
        xsum += i;
        ysum += border[i];
        num++;
    }

    //计算各个平均数
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);

    }

    /*计算斜率*/
    *slope_rate = Slope_Calculate(start, end, border);//斜率
    *intercept = y_average - (*slope_rate) * x_average;//截距
}

uint8_t break_num_l_last = 0, break_num_r_last = 0;
/**
* @brief 十字补线函数
* @param uint8(*image)[image_w]     输入二值图像
* @param uint8 *l_border            输入左边界首地址
* @param uint8 *r_border            输入右边界首地址
* @param uint16 total_num_l         输入左边循环总次数
* @param uint16 total_num_r         输入右边循环总次数
* @param uint16 *dir_l              输入左边生长方向首地址
* @param uint16 *dir_r              输入右边生长方向首地址
* @param uint16(*points_l)[2]       输入左边轮廓首地址
* @param uint16(*points_r)[2]       输入右边轮廓首地址
*  @see CTest       cross_fill(image,l_border, r_border, data_statics_l, data_statics_r, dir_l, dir_r, points_l, points_r);
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
 */
void cross_fill(uint8_t(*image)[image_w], uint8_t* l_border,
    uint8_t* r_border, uint16_t total_num_l, uint16_t total_num_r,
    uint16_t* dir_l, uint16_t* dir_r, uint16_t(*points_l)[2],
    uint16_t(*points_r)[2])
{
    uint16_t i;
    uint8_t break_num_l = 0;
    uint8_t break_num_r = 0;
    uint8_t start, end;

    float slope_l_rate = 0, intercept_l = 0;
    //出十字
    for (i = 1; i < total_num_l; i++)
    {
        if (dir_l[i - 1] == 4 && dir_l[i] == 4 && dir_l[i + 3] == 6 && dir_l[i + 5] == 6 && dir_l[i + 7] == 6)
        {
            break_num_l = points_l[i][1];//传递y坐标
            if (break_num_l < break_num_l_last)
            {
                break_num_l = break_num_l_last;
            }
            break_num_l_last = break_num_l;

            //printf_s("brea_knum-L:%d\n", break_num_l);
            //printf_s("I:%d\n", i);
            //printf_s("十字标志位：1\n");
            break;
        }
    }
    for (i = 1; i < total_num_r; i++)
    {
        if (dir_r[i - 1] == 4 && dir_r[i] == 4 && dir_r[i + 3] == 6 && dir_r[i + 5] == 6 && dir_r[i + 7] == 6)
        {
            break_num_r = points_r[i][1];//传递y坐标
            if (break_num_r < break_num_r_last)
            {
                break_num_r = break_num_r_last;
            }
            break_num_r_last = break_num_r;
            //printf("brea_knum-R:%d\n", break_num_r);
            //printf("I:%d\n", i);
            //printf("十字标志位：1\n");
            break;
        }
    }
    if (break_num_l && break_num_r && image[image_h - 1][4] && image[image_h - 1][image_w - 4])//两边生长方向都符合条件
    {
        //计算斜率
        start = break_num_l - 10;//15
        start = limit_a_b(start, 0, image_h);
        end = break_num_l - 5;
        //printf_s("start=%d end=%d\n", start, end);
        calculate_s_i(start, end, l_border, &slope_l_rate, &intercept_l);
        //printf("slope_l_rate:%d\nintercept_l:%d\n", slope_l_rate, intercept_l);
        for (i = break_num_l - 5; i < image_h - 1; i++)
        {
            l_border[i] = slope_l_rate * (i)+intercept_l;//y = kx+b
            l_border[i] = limit_a_b(l_border[i], border_min, border_max);//限幅
        }

        //计算斜率
        start = break_num_r - 10;//起点
        start = limit_a_b(start, 0, image_h);//限幅
        end = break_num_r - 5;//终点
        calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
        //printf("slope_l_rate:%d\nintercept_l:%d\n", slope_l_rate, intercept_l);
        for (i = break_num_r - 5; i < image_h - 1; i++)
        {
            r_border[i] = slope_l_rate * (i)+intercept_l;
            r_border[i] = limit_a_b(r_border[i], border_min, border_max);
        }


    }

}

uint16_t break_num_l_last_in = 0, break_num_r_last_in = 0;
void cross_fill_in(uint8_t(*image)[image_w], uint8_t* l_border,
    uint8_t* r_border, uint16_t total_num_l, uint16_t total_num_r,
    uint16_t* dir_l, uint16_t* dir_r, uint16_t(*points_l)[2],
    uint16_t(*points_r)[2])
{
    uint16_t i;
    uint8_t break_num_l_in = 0;
    uint8_t break_num_r_in = 0;
    uint8_t start, end;

    float slope_l_rate = 0, intercept_l = 0;
    //出十字
    for (i = 1; i < total_num_l; i++)
    {
        if (dir_l[i - 1] == 4 && dir_l[i] == 4 && dir_l[i + 3] == 6 && dir_l[i + 5] == 6 && dir_l[i + 7] == 6)
        {
            //printf_s("Lfound_buxian\n");
            //Draw_Color(points_l[i][0], points_l[i][1], RED);
            break_num_l_in = points_l[i][1];//传递y坐标
            if (break_num_l_in < break_num_l_last_in)
            {
                break_num_l_in = break_num_l_last_in;
            }
            break_num_l_last_in = break_num_l_in;

            //printf_s("brea_knum-L:%d\n", break_num_l_in);
            //printf_s("I:%d\n", i);
            //printf_s("十字标志位：1\n");
            break;
        }
    }
    for (i = 1; i < total_num_r; i++)
    {
        if (dir_r[i - 1] == 4 && dir_r[i] == 4 && dir_r[i + 3] == 6 && dir_r[i + 5] == 6 && dir_r[i + 7] == 6)
        {
            //printf_s("Rfound_buxian\n");
           // Draw_Color(points_r[i][0], points_r[i][1], GREEN);
            break_num_r_in = points_r[i][1];//传递y坐标
            if (break_num_r_in < break_num_r_last_in)
            {
                break_num_r_in = break_num_r_last_in;
            }
            break_num_r_last_in = break_num_r_in;
            // printf("brea_knum-R:%d\n", break_num_r_in);
             //printf("I:%d\n", i);
             //printf("十字标志位：1\n");
            break;
        }
    }
    if (break_num_l_in && break_num_r_in /*&& image[image_h - 1][4] && image[image_h - 1][image_w - 4]*/)//两边生长方向都符合条件
    {
        //计算斜率
        //printf_s("jisuanxielv\n");
        start = break_num_l_in - 10;//15
        start = limit_a_b(start, 0, image_h);
        end = break_num_l_in - 5;
        //printf_s("start=%d end=%d\n", start, end);
        calculate_s_i(start, end, l_border, &slope_l_rate, &intercept_l);
        //printf("slope_l_rate:%d\nintercept_l:%d\n", slope_l_rate, intercept_l);
        for (i = break_num_l_in - 5; i < image_h - 1; i++)
        {
            l_border[i] = slope_l_rate * (i)+intercept_l;//y = kx+b
            l_border[i] = limit_a_b(l_border[i], border_min, border_max);//限幅
        }

        //计算斜率
        start = break_num_r_in - 10;//起点
        start = limit_a_b(start, 0, image_h);//限幅
        end = break_num_r_in - 5;//终点
        calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
        //printf("slope_l_rate:%d\nintercept_l:%d\n", slope_l_rate, intercept_l);
        for (i = break_num_r_in - 5; i < image_h - 1; i++)
        {
            r_border[i] = slope_l_rate * (i)+intercept_l;
            r_border[i] = limit_a_b(r_border[i], border_min, border_max);
        }


    }

}

/*
函数名称：void get_start_point(uint8 start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
修改时间：2022年9月8日
备    注：
example：  get_start_point(image_h-2)
 */
uint8_t get_start_point(uint8_t start_row, uint8_t(*image)[RESULT_COL])
{
    uint8_t i = 0, l_found = 0, r_found = 0;
    //清零
    start_point_l[0] = 0;//x
    start_point_l[1] = 0;//y

    start_point_r[0] = 0;//x
    start_point_r[1] = 0;//y

    //从中间往左边，先找起点
    for (i = RESULT_COL / 2; i > border_min; i--)  //i=120/2 i>1 i--
    {
        start_point_l[0] = i;//x
        start_point_l[1] = start_row;//y   120-5
        if (image[start_row][i] == 255 && image[start_row][i - 1] == 0)     //[188-5][i]   [188-5][i-1]
        {
            //printf_s("zuo qi dian[%d][%d]\n", start_row,i);
           // Draw_Color(i, start_row, RED);
            //cout << "zuo qi dian"<<endl;
            l_found = 1;
            break;
        }
    }

    for (i = RESULT_COL / 2; i < border_max; i++)      //i=120/2 i<188-2 i++
    {
        start_point_r[0] = i;//x
        start_point_r[1] = start_row;//y
        if (image[start_row][i] == 255 && image[start_row][i + 1] == 0)
        {
            //printf_s("you qi dian[%d][%d]\n",start_row, i);
            //Draw_Color(i, start_row, RED);
            //cout << "you qi dian"<<endl;
            r_found = 1;
            break;
        }
    }

    if (l_found && r_found)
    {
        //cout << " qi dian";
        return 1;
    }
    else {
        //printf("未找到起点\n");
        //cout << "no qi dian";
        return 0;
    }
}

/*
函数名称：void search_l_r(uint16_t break_flag, uint8_t(*image)[image_w],uint16_t *l_stastic, uint16_t *r_stastic,
                            uint8_t l_start_x, uint8_t l_start_y, uint8_t r_start_x, uint8_t r_start_y,uint8_t*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r            ：最多需要循环的次数
(*image)[image_w]       ：需要进行找点的图像数组，必须是二值图,填入数组名称即可
                       特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic              ：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic              ：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x               ：左边起点横坐标
l_start_y               ：左边起点纵坐标
r_start_x               ：右边起点横坐标
r_start_y               ：右边起点纵坐标
hightest                ：循环结束所得到的最高高度
函数返回：无
修改时间：2022年9月25日
备    注：
example：
    search_l_r((uint16_t)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
void search_l_r(uint16_t break_flag, uint8_t(*image)[RESULT_COL],
    uint16_t* l_stastic, uint16_t* r_stastic,
    uint8_t l_start_x, uint8_t l_start_y,
    uint8_t r_start_x, uint8_t r_start_y, uint8_t* hightest)
{

    uint8_t i = 0, j = 0;

    //左边变量
    uint8_t search_filds_l[8][2] = { {  0 } };
    uint8_t index_l = 0;
    uint8_t temp_l[8][2] = { {  0 } };
    uint8_t center_point_l[2] = { 0 };
    uint16_t l_data_statics = 0;//统计左边
    //定义八个邻域
    static int seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针

    //右边变量
    uint8_t search_filds_r[8][2] = { {  0 } };
    uint8_t center_point_r[2] = { 0 };//中心坐标点
    uint8_t index_r = 0;//索引下标
    uint8_t temp_r[8][2] = { {  0 } };
    uint16_t r_data_statics = 0;//统计右边
    //定义八个邻域
    static int seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针

    l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x  假设是80
    center_point_l[1] = l_start_y;//y       115
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

    //开启邻域循环
    while (break_flag--)//300--
    {

        //左边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//索引加一

        //右边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y

        index_l = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;//先清零，后使用
            temp_l[i][1] = 0;//先清零，后使用
        }

        //左边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//记录生长方向
            }

            if (index_l)
            {
                //更新坐标点
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y
                for (j = 0; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
            }

        }
        if ((points_r[r_data_statics][0] == points_r[r_data_statics - 1][0] && points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            || (points_l[l_data_statics - 1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics - 1][0] == points_l[l_data_statics - 3][0]
                && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics - 1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf_s("三次进入同一个点，退出\n");
            //printf_s("san ci jin yi ，tui chu\n");
            break;
        }
        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
            )
        {
            //printf("\n左右相遇退出\n");
            //printf_s("zuo you xiang yu\n");
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
            //printf("\n在y=%d处退出\n",*hightest);
            //printf_s("zai y=%d tui chu\n", *hightest);
            break;
        }
        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
        {
            //printf("\n如果左边比右边高了，左边等待右边\n");
            //printf_s("zuo gao deng you\n");
            continue;//如果左边比右边高了，左边等待右边
        }
        if (dir_l[l_data_statics - 1] == 7
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
        {
            //printf("\n左边开始向下了，等待右边，等待中... \n");
            //printf_s("zuo xia deng you\n");
            center_point_l[0] = points_l[l_data_statics - 1][0];//x
            center_point_l[1] = points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }
        r_data_statics++;//索引加一

        index_r = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;//先清零，后使用
            temp_r[i][1] = 0;//先清零，后使用
        }

        //右边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//索引加一
                dir_r[r_data_statics - 1] = (i);//记录生长方向
                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //更新坐标点
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                }

            }
        }


    }


    //取出循环次数
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;

}

/*
函数名称：void get_left(uint16 total_L)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_L ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example： get_left(data_stastics_l );
 */
void get_left(uint16_t total_L)
{
    uint8_t i = 0;
    uint16_t j = 0;
    uint8_t h = 0;
    //初始化
    for (i = 0; i < RESULT_ROW; i++)
    {
        l_border[i] = border_min;
    }
    h = RESULT_ROW - 2;//120-5
    //左边
    for (j = 0; j < total_L; j++)//j<200zuoyou
    {
        //printf_s("a=%d  h=%d\n", points_l[j][1],h);
        if (points_l[j][1] == h)
        {
            l_border[h] = points_l[j][0] + 1;
            // l_border_y[h] = h;
             //printf_s("OK");
        }
        else continue; //每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)
        {
            break;//到最后一行退出
        }
    }
}
/*
函数名称：void get_right(uint16 total_R)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_R  ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example：get_right(data_stastics_r);
 */
void get_right(uint16_t total_R)
{
    uint8_t i = 0;
    uint16_t j = 0;
    uint8_t h = 0;
    for (i = 0; i < RESULT_ROW; i++)
    {
        r_border[i] = border_max;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
    }
    h = RESULT_ROW - 2;
    //右边
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            r_border[h] = points_r[j][0] - 1;
        }
        else continue;//每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)break;//到最后一行退出
    }
}

//巡线找十字
uint8_t break_num_l_1 = 0, break_num_l_2 = 0;/*break_num_l_1_last=0, break_num_l_2_last=0*/
uint8_t break_num_r_1 = 0, break_num_r_2 = 0; /*break_num_r_1_last=0, break_num_r_2_last=0*/
void Check_Cross()
{
    uint16_t i;
    // printf_s("l=%d r=%d\n", data_stastics_l, data_stastics_r);
    data_stastics_l = 180;//180
    data_stastics_r = 180;
    for (i = 1; i < data_stastics_l; i++)
    {
        //printf("i=%d\n", i);
        if (dir_l[i - 1] == 2 && dir_l[i] == 2 && (dir_l[i + 3] == 4 || dir_l[i + 3] == 3) 
        && (dir_l[i + 5] == 4 || dir_l[i + 5] == 3) && dir_l[i + 7] == 4)
        {
            break_num_l_1 = points_l[i][1];//传递y坐标
            //Draw_Color(points_l[i][0], points_l[i][1], RED);
            /*if (break_num_l_1 < break_num_l_1_last)
            {
                break_num_l_1 = break_num_l_1_last;
            }
            break_num_l_1_last = break_num_l_1;*/
            Lfound_1 = true;
            // printf_s("Lfound_1\n");
            // printf_s("break_num_1=%d  I=%d\n", break_num_l_1, i);
             /*printf_s("brea_knum-L:%d\n", break_num_l);
             printf_s("I:%d\n", i);
             printf_s("十字标志位：1\n");*/
            break;
        }
    }
    if (Lfound_1 == 1)
    {
        for (i = 1; i < data_stastics_l; i++)
        {
            if (dir_l[i - 1] == 4 && dir_l[i] == 4 && dir_l[i + 3] == 6 && dir_l[i + 5] == 6 && dir_l[i + 7] == 6)
            {
                break_num_l_2 = points_l[i][1];//传递y坐标
                /*if (break_num_l_2 < break_num_l_2_last)
                {
                    break_num_l_2 = break_num_l_2_last;
                }
                break_num_l_2_last = break_num_l_2;*/
                Lfound_2 = true;
                //   printf_s("Lfound_2\n");
                  // Draw_Color(points_l[i][0], points_l[i][1], RED);
                   /*printf_s("brea_knum-L:%d\n", break_num_l);
                   printf_s("I:%d\n", i);
                   printf_s("十字标志位：1\n");*/
                break;

            }
        }
    }



    for (i = 1; i < data_stastics_r; i++)//data_stastics_r
    {
        if (dir_r[i - 1] == 2 && dir_r[i] == 2 && dir_r[i + 3] == 4 && dir_r[i + 5] == 4 && dir_r[i + 7] == 4)
        {
            break_num_r_1 = points_r[i][1];//传递y坐标
            /*if (break_num_r_1 < break_num_r_1_last)
            {
                break_num_r_1 = break_num_r_1_last;
            }
            break_num_r_1_last = break_num_r_1;*/
            Rfound_1 = true;
            // printf_s("Rfound_1\n");
             //Draw_Color(points_r[i][0], points_r[i][1], GREEN);
             /*printf("brea_knum-R:%d\n", break_num_r);
             printf("I:%d\n", i);
             printf("十字标志位：1\n");*/
            break;
        }
    }
    if (Rfound_1 == 1)
        for (i = 1; i < data_stastics_r; i++)
        {
            if (dir_r[i - 1] == 4 && dir_r[i] == 4 && (dir_r[i + 3] == 6 || dir_r[i + 3] == 5)
                && (dir_r[i + 5] == 6 || dir_r[i + 5] == 5) && dir_r[i + 7] == 6)
            {
                break_num_r_2 = points_r[i][1];//传递y坐标
                /*if (break_num_r_2 < break_num_r_2_last)
                {
                    break_num_r_2 = break_num_r_2_last;
                }
                break_num_r_2_last = break_num_r_2;*/
                Rfound_2 = true;
                // printf_s("Rfound_2\n");
                 /*printf("brea_knum-R:%d\n", break_num_r);
                 printf("I:%d\n", i);
                 printf("十字标志位：1\n");*/
                break;

            }
        }

}

//十字环内找出口
uint16_t break_num_l_1_in = 0, break_num_l_2_in = 0;/*break_num_l_1_last=0, break_num_l_2_last=0*/
uint16_t break_num_r_1_in = 0, break_num_r_2_in = 0; /*break_num_r_1_last=0, break_num_r_2_last=0*/
void Check_Cross_In()
{
    uint16_t i;
    //printf_s("l=%d r=%d\n", data_stastics_l, data_stastics_r);
    data_stastics_l_in = 80;//80 180均可 180会提前进入type_4
    data_stastics_r_in = 80;
    for (i = 1; i < data_stastics_l_in; i++)
    {
        //printf("i=%d\n", i);
        if (dir_l[i - 1] == 2 && dir_l[i] == 2 && (dir_l[i + 3] == 4 || dir_l[i + 3] == 3) && (dir_l[i + 5] == 4 || dir_l[i + 5] == 3) && dir_l[i + 7] == 4)
        {
            break_num_l_1_in = points_l[i][1];//传递y坐标
            //Draw_Color(points_l[i][0], points_l[i][1], RED);
            Lfound_1_in = true;
            //  printf_s("Lfound_1_in\n");
              //printf_s("break_num_1=%d  I=%d\n", break_num_l_1_in, i);
              /*printf_s("brea_knum-L:%d\n", break_num_l);
              printf_s("I:%d\n", i);
              printf_s("十字标志位：1\n");*/
            break;
        }
    }
    if (Lfound_1_in == 1)
    {
        for (i = 1; i < data_stastics_l_in; i++)
        {
            if (dir_l[i - 1] == 4 && dir_l[i] == 4 && dir_l[i + 3] == 6 && dir_l[i + 5] == 6 && dir_l[i + 7] == 6)
            {
                break_num_l_2_in = points_l[i][1];//传递y坐标
                /*if (break_num_l_2 < break_num_l_2_last)
                {
                    break_num_l_2 = break_num_l_2_last;
                }
                break_num_l_2_last = break_num_l_2;*/
                Lfound_2_in = true;
                // printf_s("Lfound_2_in\n");
                 /*printf_s("brea_knum-L:%d\n", break_num_l);
                 printf_s("I:%d\n", i);
                 printf_s("十字标志位：1\n");*/
                break;

            }
        }
    }
    for (i = 1; i < data_stastics_r_in; i++)//data_stastics_r
    {
        if ((dir_r[i - 1] == 6 || dir_r[i - 1] == 5) && ((dir_r)[i] == 6 || dir_r[i] == 5)
            && (dir_r[i + 3] == 2 || dir_r[i + 3] == 3) && (dir_r[i + 5] == 2 || dir_r[i + 5] == 3)
            && (dir_r[i + 7] == 2 || dir_r[i + 7] == 3))
        {
            break_num_r_1_in = points_r[i][1];//传递y坐标
            /*if (break_num_r_1 < break_num_r_1_last)
            {
                break_num_r_1 = break_num_r_1_last;
            }
            break_num_r_1_last = break_num_r_1;*/
            Rfound_1_in = true;
            //  printf_s("Rfound_1_in\n");
              //Draw_Color(points_r[i][0], points_r[i][1], GREEN);
              /*printf("brea_knum-R:%d\n", break_num_r);
              printf("I:%d\n", i);
              printf("十字标志位：1\n");*/
            break;
        }
    }
    if (Rfound_1_in == 1)
        for (i = 1; i < data_stastics_r_in; i++)
        {
            if (dir_r[i - 1] == 4 && dir_r[i] == 4 && (dir_r[i + 3] == 6 || dir_r[i + 3] == 5)
                && (dir_r[i + 5] == 6 || dir_r[i + 5] == 5) && dir_r[i + 7] == 6)
            {
                break_num_r_2_in = points_r[i][1];//传递y坐标
                /*if (break_num_r_2 < break_num_r_2_last)
                {
                    break_num_r_2 = break_num_r_2_last;
                }
                break_num_r_2_last = break_num_r_2;*/
                Rfound_2_in = true;
                //  printf_s("Rfound_2_in\n");
                  //Draw_Color(points_r[i][0], points_r[i][1], GREEN);
                  /*printf("brea_knum-R:%d\n", break_num_r);
                  printf("I:%d\n", i);
                  printf("十字标志位：1\n");*/
                break;

            }
        }





}

//十字程序
void Crossing()
{

    uint8_t start, end, i;
    float slope_l_rate = 0, intercept_l = 0;


    switch (cross_type)
    {
    case CROSS_NONE:
    {
        Check_Cross();
        if (Lfound_1 == 1 && Rfound_1 == 1 && Lfound_2 == 1 && Rfound_2 == 1)
        {
            cross_type = CROSS_FOUND;
            //printf_s("jinru cross\n");
        }
        else {
            cross_type = CROSS_NONE;
            Lfound_1 = 0; Lfound_2 = 0;
            Rfound_1 = 0; Rfound_2 = 0;
        }
        break;
    }
    case CROSS_FOUND:
    {
        Check_Cross();
        //LEFT
        start = limit_a_b(break_num_l_1 + 2, 1, image_h - 2);
        end = limit_a_b(break_num_l_1 + 15, 1, image_h - 2);
        if (end - start >= 6)
        {
            // printf_s("jin xian\n");
            // printf_s("start=%d  end=%d\n", start, end);
            calculate_s_i(start, end, l_border, &slope_l_rate, &intercept_l);
            for (i = 10; i < break_num_l_1 + 2; i++)
            {
                l_border[i] = slope_l_rate * (i)+intercept_l;//y = kx+b
                l_border[i] = limit_a_b(l_border[i], border_min, border_max);//限幅
            }
        }
        else {
            L_jieshu = 1;
        }

        ////计算斜率 RIGHT
        start = limit_a_b(break_num_r_1 + 2, 1, image_h - 2);//限幅
        end = limit_a_b(break_num_r_1 + 15, 1, image_h - 2);//终点
        if (end - start >= 6 && L_jieshu == 0)
        {
            calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
            for (i = 10; i < break_num_r_1 + 2; i++)
            {
                r_border[i] = slope_l_rate * (i)+intercept_l;
                r_border[i] = limit_a_b(r_border[i], border_min, border_max);
            }
        }
        else {
            R_jieshu = 1;
        }
        //printf("slope_l_rate:%d\nintercept_l:%d\n", slope_l_rate, intercept_l);

        //printf_s("[116][4]=%d [115][185]=%d [116][5]=%d [115][184]=%d", PerImg[116][6],PerImg[115][185],PerImg[116][7],PerImg[115][184]);
        if ((L_jieshu == 1 && R_jieshu == 1) && (PerImg[116][3] == 255/*||PerImg[116][6]==255)*/ && PerImg[115][185] == 255
            && PerImg[116][2] == 255/*||PerImg[116][7]==255)*/ && PerImg[115][184] == 255))
        {
            cross_type = CROSS_BANRU;
            // printf_s("banru\n");
            Lfound_1 = 0; Lfound_2 = 0;
            Rfound_1 = 0; Rfound_2 = 0;
            L_jieshu = 0; R_jieshu = 0;
        }
        else
        {
            cross_type = CROSS_FOUND;
        }
        break;
    }
    case CROSS_BANRU:
    {
        cross_fill(PerImg, l_border, r_border, data_stastics_l, data_stastics_r, dir_l, dir_r, points_l, points_r);
        // printf_s("buxian\n");
        if (PerImg[116][3] == 0 && PerImg[116][185] == 0 && PerImg[116][3] == 0 && PerImg[116][185] == 0)
        {
            cross_type = CROSS_IN;
            // printf_s("jinru shizi\n");
        }
        break;
    }
    case CROSS_IN:
    {
        Check_Cross_In();
        if ((Lfound_1_in == 1 && Lfound_2_in == 1)
            || (Lfound_1_in == 1 && Rfound_1_in == 1) || (Rfound_1_in == 1 && Rfound_2_in == 1) /*&& Rfound_2_in == 1*/)
        {
            cross_type = CROSS_FOUND_IN;
            //printf_s("jinru cross\n");
        }
        else if ((Lfound_1_in == 1 && PerImg[115][185] == 255 && PerImg[115][184] == 255)
            /*||(Rfound_1_in==1&& PerImg[116][7] == 255&& PerImg[116][8] == 255)*/)
        {
            cross_type = CROSS_BANRU_IN;
            Lfound_1_in = 0; Lfound_2_in = 0;
            Rfound_1_in = 0; Rfound_2_in = 0;
        }
        else {
            cross_type = CROSS_IN;
            Lfound_1_in = 0; Lfound_2_in = 0;
            Rfound_1_in = 0; Rfound_2_in = 0;
        }
        break;
    }
    case CROSS_FOUND_IN:
    {
        Check_Cross_In();
        //LEFT
        start = limit_a_b(break_num_l_1_in + 2, 1, image_h - 6);
        end = limit_a_b(break_num_l_1_in + 10, 1, image_h - 6);
        // printf_s("start=%d  end=%d\n", start, end);
        if (end != image_h - 6)
        {
            //printf_s("chu shizi found\n");

            calculate_s_i(start, end, l_border, &slope_l_rate, &intercept_l);
            for (i = 10; i < break_num_l_1_in + 2; i++)
            {
                l_border[i] = slope_l_rate * (i)+intercept_l;//y = kx+b
                l_border[i] = limit_a_b(l_border[i], border_min, border_max);//限幅
            }
        }
        else {
            L_jieshu_in = 1;
        }

        ////计算斜率 RIGHT
        start = limit_a_b(break_num_r_1_in + 2, 1, image_h - 2);//限幅
        end = limit_a_b(break_num_r_1_in + 10, 1, image_h - 2);//终点
        if (end - start >= 3 && L_jieshu_in == 0)
        {
            calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
            for (i = 10; i < break_num_r_1_in + 2; i++)
            {
                r_border[i] = slope_l_rate * (i)+intercept_l;
                r_border[i] = limit_a_b(r_border[i], border_min, border_max);
            }
        }
        else {
            R_jieshu_in = 1;
        }
        //printf("slope_l_rate:%d\nintercept_l:%d\n", slope_l_rate, intercept_l);

        //printf_s("[116][4]=%d [115][185]=%d [116][5]=%d [115][184]=%d", PerImg[116][6],PerImg[115][185],PerImg[116][7],PerImg[115][184]);
        if ((L_jieshu_in == 1 && R_jieshu_in == 1) && (PerImg[116][7] == 255/*||PerImg[116][6]==255)*/ && PerImg[115][185] == 255
            && PerImg[116][8] == 255/*||PerImg[116][7]==255)*/ && PerImg[115][184] == 255))
        {
            cross_type = CROSS_BANRU_IN;

            Lfound_1_in = 0; Lfound_2_in = 0;
            Rfound_1_in = 0; Rfound_2_in = 0;
            L_jieshu_in = 0; R_jieshu_in = 0;
        }
        else
        {
            cross_type = CROSS_FOUND_IN;
        }
        break;
    }
    case CROSS_BANRU_IN:
    {
        // printf_s("banru_in\n");
        cross_fill_in(PerImg, l_border, r_border, data_stastics_l_in, data_stastics_r_in, dir_l, dir_r, points_l, points_r);
        // printf_s("buxian_in\n");
        if (PerImg[116][3] == 0 && PerImg[116][185] == 0 && PerImg[116][3] == 0 && PerImg[116][185] == 0)
        {
            cross_type = CROSS_NONE;
            //  printf_s(" shizi jieshu \n");
        }
        break;
    }

    }

}

bool Zhidao=false;
void Check_Zhidao()//只根据左边的线斜率判断直道（待完善）
{
    uint16_t i;
    uint8_t zhidao_jishu=0;
    uint8_t nozhidao_jishu=0;
    for (i = 10; i < 80; i++)//判断直道？可能没用
    {
        // if(l_border[i]-l_border[i+1]<=2&&abs(points_l[i][0]-points_l[i+1][0])<=1)
        // {
        if(abs((double(points_l[i][1]-points_l[i+10][1]))/double((points_l[i][0]-points_l[i+10][0])))>=1)
        {  
            zhidao_jishu++;
            //printf_s("zhidao=%d ",zhidao_jishu);
            //printf_s("k=%lf ",(double(points_l[i][1]-points_l[i+10][1]))/double((points_l[i][0]-points_l[i+10][0])));
        }
        else
        {
            nozhidao_jishu++;
            //printf_s("nozhidao=%d ",nozhidao_jishu);
        }             
    }
        if(zhidao_jishu>nozhidao_jishu)
        {
            Zhidao=true;
            printf_s("直道AAAAAAAAAAAAAA ");
        }else{
            Zhidao=false;
            printf_s("不是直道BBBBBBBBBBBBBB ");
        }
}

uint8_t A=0;
uint8_t C=0;
uint8_t C_x=0;
bool Afound=false;
bool Cfound=false;
void Check_Circle()//先写一边的，右环岛
{
    // printf_s("l=%d r=%d\n", data_stastics_l, data_stastics_r);
    uint16_t i;
    data_stastics_l = 180;//180
    data_stastics_r = 180;
    
    Check_Zhidao();
    if(circle_type==CIRCLE_NONE||circle_type==CIRCLE_1)
    {
    for (i = 1; i < data_stastics_r; i++)//data_stastics_r
    {
        // if ((dir_r[i - 1] == 5||dir_r[i - 1] == 4) && (dir_r[i] == 4||dir_r[i]==5) 
        // && (dir_r[i + 3] == 2||dir_r[i + 3] == 1) && (dir_r[i + 5] == 1||dir_r[i + 5] == 2) /* && dir_r[i + 7] == 4 */)
        // {
        if(/* (l_border[i]>l_border[i+1])&&(l_border[i]>l_border[i+15])&&(l_border[i]>l_border[i+30])&&(l_border[i]>l_border[i+45]) */
        Zhidao&&(points_r[i][1]<points_r[i-2][1])&&(points_r[i][1]<points_r[i+2][1])&&(points_r[i][0]<points_r[i+2][0])&&(points_r[i][0]>=points_r[i-2][0]))
        {           
            A = points_r[i][1];//传递y坐标    
            Afound=true;      
             printf_s("Afound\n");
            Draw_Color(points_r[i][0]-2, points_r[i][1], GREEN);
            
            break;           
        }   
    }
    }

    if(circle_type==CIRCLE_1||circle_type==CIRCLE_2)
    {
        for(i=10;i<data_stastics_r; i++)
        {
            if((points_r[i][1]<points_r[i-2][1])&&(points_r[i][1]>points_r[i+2][1])
            &&(points_r[i][0]<points_r[i+2][0])&&(points_r[i][0]<points_r[i-2][0]))
            {
                C=points_r[i][1];//y
                C_x=points_r[i][0];
                Cfound=true;
                printf_s("Cfound\n");
                printf_s("x=%d y=%d",C_x,C);
                Draw_Color(points_r[i][0]-2, points_r[i][1], GREEN);
                break;
            }

        }

    }


    

}

void Circle()
{
    uint8_t start, end, i;
    float slope_l_rate = 0, intercept_l = 0;
    switch(circle_type)
    {
        case CIRCLE_NONE:
        {
            Check_Circle();
            if(Afound)
            {
                circle_type=CIRCLE_1;
            }else
            {
                circle_type=CIRCLE_NONE;
            }
            break;
        }
        case CIRCLE_1:
        {
            Check_Circle();
           
        start =limit_a_b(A + 2, 1, image_h - 2);//限幅
        end = limit_a_b(A + 15, 1, image_h - 2);//终点
         if (PerImg[115][185] == 0&&PerImg[115][184]==0)
         {
            calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
            for (i = C; i < A + 2; i++)
            {
                r_border[i] = slope_l_rate * (i)+intercept_l;
                r_border[i] = limit_a_b(r_border[i], border_min, border_max);
            }
        }
        else
        {
            circle_type=CIRCLE_2;
        }
            break;
        }
        case CIRCLE_2:
        {
            double k,b=0;
            Check_Circle();

            start =limit_a_b(C, 1, image_h - 2);//限幅
            end = limit_a_b(117, 1, image_h - 2);//终点
            calculate_s_i(start, end, r_border, &slope_l_rate, &intercept_l);
            for(i = 1; i < 117; i++)
            {        
                r_border[i] = slope_l_rate * (i)+intercept_l;
                r_border[i] = limit_a_b(r_border[i], border_min, border_max);
            }
            break;
        }

    }

}



//定义膨胀和腐蚀的阈值区间   滤波
#define threshold_max   255*5//可调节
#define threshold_min   255*2
void image_filter(uint8_t(*PerImg)[RESULT_COL])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16_t i, j;
    uint32_t num = 0;


    for (i = 1; i < RESULT_ROW - 1; i++)
    {
        for (j = 1; j < (RESULT_COL - 1); j++)
        {
            //统计八个方向的像素值
            num =
                PerImg[i - 1][j - 1] + PerImg[i - 1][j] + PerImg[i - 1][j + 1]
                + PerImg[i][j - 1] + PerImg[i][j + 1]
                + PerImg[i + 1][j - 1] + PerImg[i + 1][j] + PerImg[i + 1][j + 1];


            if (num >= threshold_max /*&& PerImg[i][j] == 0*/)
            {

                PerImg[i][j] = 255;//白  可以搞成宏定义，方便更改

            }
            if (num <= threshold_min /*&& PerImg[i][j] == 255*/)
            {

                PerImg[i][j] = 0;//黑

            }

        }
    }

}


/*
函数名称：void image_draw_rectan(uint8(*image)[image_w])
功能说明：给图像画一个黑框
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_draw_rectan(bin_image);
 */
void image_draw_rectan(uint8_t(*image)[RESULT_COL])
{

    uint8_t i = 0;
    for (i = 0; i < RESULT_ROW; i++)
    {
        image[i][0] = 0;//第0列 第1列
        image[i][1] = 0;
        image[i][RESULT_COL - 1] = 0;//倒数两列
        image[i][RESULT_COL - 2] = 0;

    }
    for (i = 0; i < RESULT_COL; i++)
    {
        image[0][i] = 0;//最上面两行
        image[1][i] = 0;
        //image[image_h-1][i] = 0;

    }
}


void reset_staus()
{
}