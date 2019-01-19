#include<opencv2\core.hpp>
#include <opencv2/opencv.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2\video\tracking.hpp>
#include <shlobj.h>
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>  
#include <windows.h>
#include <process.h>
#include"beauty.h"
#pragma warning(disable:4996)
//打开命令窗
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME	"美颜相机"
using namespace std;
using namespace cv;
using namespace cvui;

int main(int argc, const char *argv[])
{
	//创建一个摄像头
	VideoCapture captue(0);
	double width = captue.get(CV_CAP_PROP_FRAME_WIDTH);  //帧宽度
	double height = captue.get(CV_CAP_PROP_FRAME_HEIGHT); //帧高度
	double frameRate = captue.get(CV_CAP_PROP_FPS);  //帧率 x frames/s
	double totalFrames = captue.get(CV_CAP_PROP_FRAME_COUNT); //总帧数
	//创建美颜前面原图
	Mat image;
	//测试
	cout << "视频宽度=" << width << endl;
	cout << "视频高度=" << height << endl;
	cout << "视频总帧数=" << totalFrames << endl;
	cout << "帧率=" << frameRate << endl;
	//窗体数值
	int win_width = width*2, win_height = height*2;
	//控件状态
	bool buffing = false, thinFace = false, bigEye = false;

	//控件数值
	int oldBuffing_num = 0;//旧磨皮数值
	int oldLightFace_num = 0; //旧美白数值
	int newBuffing_num = 0;//新磨皮数值
	int newLightFace_num = 0; //新美白数值
	int oldEffects_num = 0;//旧滤镜数值
	int newEffects_num = 0;//新滤镜数值

	//拍照按钮
	bool takeBtn = false;
	Mat start = imread("./src/start.png");
	Mat daily = imread("./src/daily.png");
	Mat final = imread("./src/final.png");

	//返回按钮
	bool backBtn = false;

	//退出按钮
	bool outBtn = false;

	//保存按钮
	bool saveBtn = false;

	//大眼
	bool BigEyes = false;
	bool BigEyes_status = false;

	//卡通
	bool cartoon = false;
	bool cartoon_status = false;

	//眼镜
	bool glasses = false;
	bool glasses_status = false;

	//拍照图片存储
	Mat img;
	char filename[200];
	//打开图片
	bool phoStatus = false;
	Mat photo_start = imread("./src/photo_start.png");
	Mat photo_end = imread("./src/photo_end.png");
	char szFilePath[1024] = { 0 };   // 所选择的文件最终的路径
	

	//创建组件
	Mat  frame = cv::Mat(win_width, win_height, CV_8UC3);
	Mat toolFrame_1 = cv::Mat(win_width, win_height, CV_8UC3);
	Mat toolFrame_2 = cv::Mat(win_width, win_height, CV_8UC3);

	//初始化
	cv::namedWindow(WINDOW_NAME, 0);
	cvResizeWindow(WINDOW_NAME, win_width, win_height);
	cvui::init(WINDOW_NAME);//UI依附的窗体


	if (!captue.isOpened())  //检测是否打开成功
	{
		cv::Mat frame = cv::Mat(200, 500, CV_8UC3);
		// 颜色
		frame = cv::Scalar(49, 52, 49);

		// 设置文字
		cvui::text(frame, 110, 80, "Your captue losed!");
		cvui::text(frame, 110, 120, "please open your captue!");
		//刷新
		cvui::imshow("错误", frame);
		cv::waitKey();
		return -1;
	}

	while (true) {
		if (phoStatus){
			char szFilePath[1024] = { 0 };   // 所选择的文件最终的路径
			BROWSEINFO bi;
			ZeroMemory(&bi, sizeof(BROWSEINFO));
			bi.hwndOwner = NULL;
			bi.pszDisplayName = szFilePath;
			bi.lpszTitle = ("从下面选择文件或文件夹:");
			bi.ulFlags = BIF_BROWSEINCLUDEFILES;
			LPITEMIDLIST idl = SHBrowseForFolder(&bi);

			SHGetPathFromIDList(idl, szFilePath);
			string std = szFilePath;
			if (std != ""){
				Mat img = imread(szFilePath);
				takeBtn = true;
				cv::resize(img, img, Size(width, height), 0, 0, CV_INTER_LINEAR);
				img.copyTo(frame);
			}
		}
		if (takeBtn){
			//初始化
			frame.copyTo(image);
			backBtn = false;
			saveBtn = false;
			newBuffing_num = 0;
			newLightFace_num = 0;
			newEffects_num = 0;
			BigEyes = false;
			cartoon = false;
			glasses = false;
			while (cv::waitKey(30) != 27){
				//如果某帧为空则退出循环
				if (frame.empty())
					break;
				if (backBtn)
				{
					backBtn = false;
					//返回相机
					break;
				}
				if (saveBtn)
				{
					time_t t;  //秒时间  
					tm* local; //本地时间   
					char buf[128] = { 0 };
					
					t = time(NULL); //获取目前秒时间  
					local = localtime(&t); //转为本地时间  
					strftime(buf, 64, "%Y_%m_%d_%H_%M_%S", local);//转为char数组
					sprintf(filename, "img\\%s.jpg", buf);//输出照片名字
					imwrite(filename, frame);//输出照片
				}

				//磨皮功能
				if (oldBuffing_num != newBuffing_num){
					//滤镜图像无法使用其他数值变化
					if (newEffects_num != 0)
					{
						newEffects_num = 0;
						image.copyTo(frame);
					}
					if (newBuffing_num == 0)
					{
						image.copyTo(frame);
						oldLightFace_num = 0;
						oldEffects_num = 0;
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					else
					{
						if (oldBuffing_num < newBuffing_num)
						{
							onbuffing(frame, frame, newBuffing_num + 5 - oldBuffing_num);
						}
						else{
							onbuffing(image, frame, newBuffing_num + 5);
							if (newLightFace_num != 0)
								BalanceColor(frame, frame, newLightFace_num);
						}
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					oldBuffing_num = newBuffing_num;
				}
				else{
					frame.copyTo(toolFrame_2);
				}
				//美白功能
				if (oldLightFace_num != newLightFace_num){
					//滤镜图像无法使用其他数值变化
					if (newEffects_num != 0)
					{
						newEffects_num = 0;
						image.copyTo(frame);
					}
					if (newLightFace_num == 0)
					{
						image.copyTo(frame);
						oldBuffing_num = 0;
						oldEffects_num = 0;
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					else{
						if (oldLightFace_num < newLightFace_num)
						{
							BalanceColor(frame, frame, newLightFace_num - oldLightFace_num);
						}
						else
						{
							BalanceColor(image, frame, newLightFace_num);
							if (newBuffing_num != 0)
								onbuffing(frame, frame, newBuffing_num + 5);
						}
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					oldLightFace_num = newLightFace_num;
				}
				else{
					frame.copyTo(toolFrame_2);
				}
				//滤镜功能
				if (oldEffects_num != newEffects_num){
					if (newEffects_num == 0)
					{
						image.copyTo(frame);
						newBuffing_num = 0;
						newLightFace_num = 0;
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					else{
						newBuffing_num = 0;
						newLightFace_num = 0;
						selectEffects(image, frame, newEffects_num);
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					oldEffects_num = newEffects_num;
				}
				else{
					frame.copyTo(toolFrame_2);
				}
				//大眼功能
				if (BigEyes){
					//滤镜图像无法使用其他数值变化
					if (newEffects_num != 0)
					{
						newEffects_num = 0;
						image.copyTo(frame);
						newBuffing_num = 0;
						newLightFace_num = 0;
						cartoon = false;
						glasses = false;
					}
					if (BigEyes_status == false)
					{
						detectAndDisplay(frame);
						BigEyes_status = !BigEyes_status;
					}
				}
				else{
					if (BigEyes_status == true)
					{
						BigEyes_status = !BigEyes_status;
						image.copyTo(frame);
						oldLightFace_num = 0;
						oldBuffing_num = 0;
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					frame.copyTo(toolFrame_2);
				}
				//卡通功能
				if (cartoon){
					//滤镜图像无法使用其他数值变化
					if (newEffects_num != 0)
					{
						newEffects_num = 0;
						image.copyTo(frame);
						newBuffing_num = 0;
						newLightFace_num = 0;
						BigEyes = false;
						glasses = false;
					}
					if (cartoon_status == false)
					{
						cartoonStatus(frame);
						cartoon_status = !cartoon_status;
					}
				}
				else{
					if (cartoon_status == true)
					{
						cartoon_status = !cartoon_status;
						image.copyTo(frame);
						oldLightFace_num = 0;
						oldBuffing_num = 0;
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					frame.copyTo(toolFrame_2);
				}
				//眼镜功能
				if (glasses){
					//滤镜图像无法使用其他数值变化
					if (newEffects_num != 0)
					{
						newEffects_num = 0;
						image.copyTo(frame);
						newBuffing_num = 0;
						newLightFace_num = 0;
						cartoon = false;
						BigEyes = false;
					}
					if (glasses_status == false)
					{
						AddGlasses(frame);
						glasses_status = !glasses_status;
					}
				}
				else{
					if (glasses_status == true)
					{
						glasses_status = !glasses_status;
						image.copyTo(frame);
						oldLightFace_num = 0;
						oldBuffing_num = 0;
						BigEyes = false;
						cartoon = false;
						glasses = false;
					}
					frame.copyTo(toolFrame_2);
				}
				//返回按钮设置
				backBtn = cvui::button(toolFrame_2, 10, 20, "BACK");
				//保存按钮设置
				saveBtn = cvui::button(toolFrame_2, 550, 240, "save");
				//磨皮组件
				cvui::printf(toolFrame_2, 30, 460, 0.5, 0xffffff, "Buffing");
				cvui::trackbar(toolFrame_2, 0, 430, 100, &newBuffing_num, 0, 10);
				//美白组件
				cvui::printf(toolFrame_2, 150, 460, 0.5, 0xffffff, "LightFace");
				cvui::trackbar(toolFrame_2, 120, 430, 100, &newLightFace_num, 0, 10);
				//滤镜组件
				cvui::printf(toolFrame_2, 270, 460, 0.5, 0xffffff, "Effects");
				cvui::trackbar(toolFrame_2, 240, 430, 100, &newEffects_num, 0, 4);
				//大眼组件
				cvui::checkbox(toolFrame_2, 350, 440, "BigEyes", &BigEyes);
				//卡通组件
				cvui::checkbox(toolFrame_2,430, 440, "Cartoon", &cartoon);
				//眼镜组件
				cvui::checkbox(toolFrame_2, 500, 440, "Glasses", &glasses);
				//刷新
				cvui::update();
				//显示工具
				cvui::imshow(WINDOW_NAME, toolFrame_2);
			}
			backBtn = false;
			takeBtn = false;
		}
		if (outBtn){
			//退出美颜相机
			frame.release();//释放资源
			captue.release();//释放资源
			break;
		}
		frame.copyTo(toolFrame_1);
		//创建窗体
		cv::namedWindow(WINDOW_NAME, 0);
		cvResizeWindow(WINDOW_NAME, win_width, win_height);
		//写入视频照片
		captue >> frame;
		if (frame.empty())//如果某帧为空则退出循环
			break;
		//显示图像
		cv::imshow(WINDOW_NAME, frame);
		//写入工具
		//拍照按钮设置
		takeBtn = cvui::button(toolFrame_1, 550, 240, daily, start, final);
		//退出按钮设置
		outBtn = cvui::button(toolFrame_1, 10, 20, "OUT");
		//图片库按钮设置
		phoStatus = cvui::button(toolFrame_1, 550, 300, photo_start, photo_start, photo_end);
		//显示工具
		cvui::imshow(WINDOW_NAME, toolFrame_1);
		if (cv::waitKey(20) == 27) {
			frame.release();//释放资源
			captue.release();//释放资源
			break;
		}
	}
	cv::destroyAllWindows();//关闭所有窗口
	return 0;
}