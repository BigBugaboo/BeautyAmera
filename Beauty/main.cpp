#include<opencv2\core.hpp>
#include <opencv2/opencv.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2\video\tracking.hpp>
#include <shlobj.h>
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#pragma warning(disable:4996)
//打开命令窗
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME	"美颜相机"
using namespace std;
using namespace cv;
using namespace cvui;
//延时拍照回调函数

int main(int argc, const char *argv[])
{
	//窗体数值
	int win_width = 1200, win_height = 800;
	//控件状态
	bool buffing = false, thinFace = false, bigEye = false;

	//控件数值
	int buffing_num = 0;//磨皮数值
	int thinFace_num = 0; //瘦脸数值
	int bigEye_num = 0; //大眼数值

	//拍照按钮
	bool btnStatus = false;
	int flag = 0;
	Mat start = imread("./src/start.png");
	Mat daily = imread("./src/daily.png");
	Mat final = imread("./src/final.png");

	//拍照图片存储
	Mat img;
	int count = 0;
	char filename[200];
	//打开图片
	bool phoStatus = false;
	Mat photo_start = imread("./src/photo_start.png");
	Mat photo_end = imread("./src/photo_end.png");
	char szFilePath[1024] = { 0 };   // 所选择的文件最终的路径
	

	//创建组件
	Mat * frame = new cv::Mat(win_width, win_height, CV_8UC3);
	Mat * toolFrame = new  cv::Mat(win_height, win_width, CV_8UC3);

	//初始化
	cv::namedWindow(WINDOW_NAME, 0);
	cvResizeWindow(WINDOW_NAME, win_width, win_height);
	cvui::init(WINDOW_NAME);

	//创建一个摄像头
	VideoCapture captue(0);
	double width = captue.get(CV_CAP_PROP_FRAME_WIDTH);  //帧宽度
	double height = captue.get(CV_CAP_PROP_FRAME_HEIGHT); //帧高度
	double frameRate = captue.get(CV_CAP_PROP_FPS);  //帧率 x frames/s
	double totalFrames = captue.get(CV_CAP_PROP_FRAME_COUNT); //总帧数
	//测试
	cout << "视频宽度=" << width << endl;
	cout << "视频高度=" << height << endl;
	cout << "视频总帧数=" << totalFrames << endl;
	cout << "帧率=" << frameRate << endl;

	if (!captue.isOpened())  //检测是否打开成功
	{
		cv::Mat frame = cv::Mat(200, 500, CV_8UC3);
		// 颜色
		frame = cv::Scalar(49, 52, 49);

		// 设置文字
		cvui::text(frame, 110, 80, "Your captue losed!");
		cvui::text(frame, 110, 120, "please open your captue!");
		//刷新
		cvui::imshow(WINDOW_NAME, frame);
		cv::waitKey();
		return -1;
	}

	while (true) {
		if (buffing) {
			frame->copyTo(*toolFrame);
		}
		else if (thinFace){
			frame->copyTo(*toolFrame);
		}
		else if (bigEye){
			frame->copyTo(*toolFrame);
		}
		else if (btnStatus && flag == 0){
			sprintf(filename, "img\\%d.jpg", ++count);
			imwrite(filename, *frame);
		}
		else if (btnStatus && flag > 0){
			frame->copyTo(*toolFrame);
		}
		else if (btnStatus && flag < 0){
			frame->copyTo(*toolFrame);
		}
		else if (phoStatus){
			//char szFilePath[1024] = { 0 };   // 所选择的文件最终的路径
			//BROWSEINFO bi;
			//ZeroMemory(&bi, sizeof(BROWSEINFO));
			//bi.hwndOwner = NULL;
			//bi.pszDisplayName = szFilePath;
			//bi.lpszTitle = ("从下面选择文件或文件夹:");
			//bi.ulFlags = BIF_BROWSEINCLUDEFILES;
			//LPITEMIDLIST idl = SHBrowseForFolder(&bi);

			//SHGetPathFromIDList(idl, szFilePath);
			//Mat img = imread(szFilePath);
			//cv::imshow(szFilePath, img);
		}
		else {
			frame->copyTo(*toolFrame);
		}
		//创建窗体
		cv::namedWindow(WINDOW_NAME, 0);
		cvResizeWindow(WINDOW_NAME, win_width, win_height);
		//写入图像
		captue >> *frame;
		if (frame->empty())//如果某帧为空则退出循环
			break;
		//显示图像
		cv::imshow(WINDOW_NAME, *frame);
		//写入工具
		//拍照按钮设置
		flag = cvui::counter(*toolFrame, 525, 200, &flag);
		btnStatus = cvui::button(*toolFrame, 550, 240, daily, start, final);
		//图片库按钮设置
		phoStatus = cvui::button(*toolFrame, 550, 300, photo_start, photo_start, photo_end);
		//磨皮功能
		cvui::checkbox(*toolFrame, 20, 400, "buffing", &buffing);
		cvui::trackbar(*toolFrame, 0, 430, 100, &buffing_num, 0, 10);
		//瘦脸功能
		cvui::checkbox(*toolFrame, 140, 400, "thin face", &thinFace);
		cvui::trackbar(*toolFrame, 120, 430, 100, &thinFace_num, 0, 10);
		//大眼功能
		cvui::checkbox(*toolFrame, 260, 400, "Big eye", &bigEye);
		cvui::trackbar(*toolFrame, 240, 430, 100, &bigEye_num, 0, 10);
		//显示工具
		cvui::imshow(WINDOW_NAME, *toolFrame);
		if (cv::waitKey(20) == 27) {
			frame->release();//释放资源
			captue.release();//释放资源
			break;
		}
	}
	destroyAllWindows();//关闭所有窗口
	return 0;
}