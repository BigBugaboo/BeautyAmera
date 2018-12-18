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
//�����
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME	"�������"
using namespace std;
using namespace cv;
using namespace cvui;
//��ʱ���ջص�����

int main(int argc, const char *argv[])
{
	//������ֵ
	int win_width = 1200, win_height = 800;
	//�ؼ�״̬
	bool buffing = false, thinFace = false, bigEye = false;

	//�ؼ���ֵ
	int buffing_num = 0;//ĥƤ��ֵ
	int thinFace_num = 0; //������ֵ
	int bigEye_num = 0; //������ֵ

	//���հ�ť
	bool btnStatus = false;
	int flag = 0;
	Mat start = imread("./src/start.png");
	Mat daily = imread("./src/daily.png");
	Mat final = imread("./src/final.png");

	//����ͼƬ�洢
	Mat img;
	int count = 0;
	char filename[200];
	//��ͼƬ
	bool phoStatus = false;
	Mat photo_start = imread("./src/photo_start.png");
	Mat photo_end = imread("./src/photo_end.png");
	char szFilePath[1024] = { 0 };   // ��ѡ����ļ����յ�·��
	

	//�������
	Mat * frame = new cv::Mat(win_width, win_height, CV_8UC3);
	Mat * toolFrame = new  cv::Mat(win_height, win_width, CV_8UC3);

	//��ʼ��
	cv::namedWindow(WINDOW_NAME, 0);
	cvResizeWindow(WINDOW_NAME, win_width, win_height);
	cvui::init(WINDOW_NAME);

	//����һ������ͷ
	VideoCapture captue(0);
	double width = captue.get(CV_CAP_PROP_FRAME_WIDTH);  //֡���
	double height = captue.get(CV_CAP_PROP_FRAME_HEIGHT); //֡�߶�
	double frameRate = captue.get(CV_CAP_PROP_FPS);  //֡�� x frames/s
	double totalFrames = captue.get(CV_CAP_PROP_FRAME_COUNT); //��֡��
	//����
	cout << "��Ƶ���=" << width << endl;
	cout << "��Ƶ�߶�=" << height << endl;
	cout << "��Ƶ��֡��=" << totalFrames << endl;
	cout << "֡��=" << frameRate << endl;

	if (!captue.isOpened())  //����Ƿ�򿪳ɹ�
	{
		cv::Mat frame = cv::Mat(200, 500, CV_8UC3);
		// ��ɫ
		frame = cv::Scalar(49, 52, 49);

		// ��������
		cvui::text(frame, 110, 80, "Your captue losed!");
		cvui::text(frame, 110, 120, "please open your captue!");
		//ˢ��
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
			//char szFilePath[1024] = { 0 };   // ��ѡ����ļ����յ�·��
			//BROWSEINFO bi;
			//ZeroMemory(&bi, sizeof(BROWSEINFO));
			//bi.hwndOwner = NULL;
			//bi.pszDisplayName = szFilePath;
			//bi.lpszTitle = ("������ѡ���ļ����ļ���:");
			//bi.ulFlags = BIF_BROWSEINCLUDEFILES;
			//LPITEMIDLIST idl = SHBrowseForFolder(&bi);

			//SHGetPathFromIDList(idl, szFilePath);
			//Mat img = imread(szFilePath);
			//cv::imshow(szFilePath, img);
		}
		else {
			frame->copyTo(*toolFrame);
		}
		//��������
		cv::namedWindow(WINDOW_NAME, 0);
		cvResizeWindow(WINDOW_NAME, win_width, win_height);
		//д��ͼ��
		captue >> *frame;
		if (frame->empty())//���ĳ֡Ϊ�����˳�ѭ��
			break;
		//��ʾͼ��
		cv::imshow(WINDOW_NAME, *frame);
		//д�빤��
		//���հ�ť����
		flag = cvui::counter(*toolFrame, 525, 200, &flag);
		btnStatus = cvui::button(*toolFrame, 550, 240, daily, start, final);
		//ͼƬ�ⰴť����
		phoStatus = cvui::button(*toolFrame, 550, 300, photo_start, photo_start, photo_end);
		//ĥƤ����
		cvui::checkbox(*toolFrame, 20, 400, "buffing", &buffing);
		cvui::trackbar(*toolFrame, 0, 430, 100, &buffing_num, 0, 10);
		//��������
		cvui::checkbox(*toolFrame, 140, 400, "thin face", &thinFace);
		cvui::trackbar(*toolFrame, 120, 430, 100, &thinFace_num, 0, 10);
		//���۹���
		cvui::checkbox(*toolFrame, 260, 400, "Big eye", &bigEye);
		cvui::trackbar(*toolFrame, 240, 430, 100, &bigEye_num, 0, 10);
		//��ʾ����
		cvui::imshow(WINDOW_NAME, *toolFrame);
		if (cv::waitKey(20) == 27) {
			frame->release();//�ͷ���Դ
			captue.release();//�ͷ���Դ
			break;
		}
	}
	destroyAllWindows();//�ر����д���
	return 0;
}