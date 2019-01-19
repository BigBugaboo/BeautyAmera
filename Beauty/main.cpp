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
//�����
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME	"�������"
using namespace std;
using namespace cv;
using namespace cvui;

int main(int argc, const char *argv[])
{
	//����һ������ͷ
	VideoCapture captue(0);
	double width = captue.get(CV_CAP_PROP_FRAME_WIDTH);  //֡���
	double height = captue.get(CV_CAP_PROP_FRAME_HEIGHT); //֡�߶�
	double frameRate = captue.get(CV_CAP_PROP_FPS);  //֡�� x frames/s
	double totalFrames = captue.get(CV_CAP_PROP_FRAME_COUNT); //��֡��
	//��������ǰ��ԭͼ
	Mat image;
	//����
	cout << "��Ƶ���=" << width << endl;
	cout << "��Ƶ�߶�=" << height << endl;
	cout << "��Ƶ��֡��=" << totalFrames << endl;
	cout << "֡��=" << frameRate << endl;
	//������ֵ
	int win_width = width*2, win_height = height*2;
	//�ؼ�״̬
	bool buffing = false, thinFace = false, bigEye = false;

	//�ؼ���ֵ
	int oldBuffing_num = 0;//��ĥƤ��ֵ
	int oldLightFace_num = 0; //��������ֵ
	int newBuffing_num = 0;//��ĥƤ��ֵ
	int newLightFace_num = 0; //��������ֵ
	int oldEffects_num = 0;//���˾���ֵ
	int newEffects_num = 0;//���˾���ֵ

	//���հ�ť
	bool takeBtn = false;
	Mat start = imread("./src/start.png");
	Mat daily = imread("./src/daily.png");
	Mat final = imread("./src/final.png");

	//���ذ�ť
	bool backBtn = false;

	//�˳���ť
	bool outBtn = false;

	//���水ť
	bool saveBtn = false;

	//����
	bool BigEyes = false;
	bool BigEyes_status = false;

	//��ͨ
	bool cartoon = false;
	bool cartoon_status = false;

	//�۾�
	bool glasses = false;
	bool glasses_status = false;

	//����ͼƬ�洢
	Mat img;
	char filename[200];
	//��ͼƬ
	bool phoStatus = false;
	Mat photo_start = imread("./src/photo_start.png");
	Mat photo_end = imread("./src/photo_end.png");
	char szFilePath[1024] = { 0 };   // ��ѡ����ļ����յ�·��
	

	//�������
	Mat  frame = cv::Mat(win_width, win_height, CV_8UC3);
	Mat toolFrame_1 = cv::Mat(win_width, win_height, CV_8UC3);
	Mat toolFrame_2 = cv::Mat(win_width, win_height, CV_8UC3);

	//��ʼ��
	cv::namedWindow(WINDOW_NAME, 0);
	cvResizeWindow(WINDOW_NAME, win_width, win_height);
	cvui::init(WINDOW_NAME);//UI�����Ĵ���


	if (!captue.isOpened())  //����Ƿ�򿪳ɹ�
	{
		cv::Mat frame = cv::Mat(200, 500, CV_8UC3);
		// ��ɫ
		frame = cv::Scalar(49, 52, 49);

		// ��������
		cvui::text(frame, 110, 80, "Your captue losed!");
		cvui::text(frame, 110, 120, "please open your captue!");
		//ˢ��
		cvui::imshow("����", frame);
		cv::waitKey();
		return -1;
	}

	while (true) {
		if (phoStatus){
			char szFilePath[1024] = { 0 };   // ��ѡ����ļ����յ�·��
			BROWSEINFO bi;
			ZeroMemory(&bi, sizeof(BROWSEINFO));
			bi.hwndOwner = NULL;
			bi.pszDisplayName = szFilePath;
			bi.lpszTitle = ("������ѡ���ļ����ļ���:");
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
			//��ʼ��
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
				//���ĳ֡Ϊ�����˳�ѭ��
				if (frame.empty())
					break;
				if (backBtn)
				{
					backBtn = false;
					//�������
					break;
				}
				if (saveBtn)
				{
					time_t t;  //��ʱ��  
					tm* local; //����ʱ��   
					char buf[128] = { 0 };
					
					t = time(NULL); //��ȡĿǰ��ʱ��  
					local = localtime(&t); //תΪ����ʱ��  
					strftime(buf, 64, "%Y_%m_%d_%H_%M_%S", local);//תΪchar����
					sprintf(filename, "img\\%s.jpg", buf);//�����Ƭ����
					imwrite(filename, frame);//�����Ƭ
				}

				//ĥƤ����
				if (oldBuffing_num != newBuffing_num){
					//�˾�ͼ���޷�ʹ��������ֵ�仯
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
				//���׹���
				if (oldLightFace_num != newLightFace_num){
					//�˾�ͼ���޷�ʹ��������ֵ�仯
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
				//�˾�����
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
				//���۹���
				if (BigEyes){
					//�˾�ͼ���޷�ʹ��������ֵ�仯
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
				//��ͨ����
				if (cartoon){
					//�˾�ͼ���޷�ʹ��������ֵ�仯
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
				//�۾�����
				if (glasses){
					//�˾�ͼ���޷�ʹ��������ֵ�仯
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
				//���ذ�ť����
				backBtn = cvui::button(toolFrame_2, 10, 20, "BACK");
				//���水ť����
				saveBtn = cvui::button(toolFrame_2, 550, 240, "save");
				//ĥƤ���
				cvui::printf(toolFrame_2, 30, 460, 0.5, 0xffffff, "Buffing");
				cvui::trackbar(toolFrame_2, 0, 430, 100, &newBuffing_num, 0, 10);
				//�������
				cvui::printf(toolFrame_2, 150, 460, 0.5, 0xffffff, "LightFace");
				cvui::trackbar(toolFrame_2, 120, 430, 100, &newLightFace_num, 0, 10);
				//�˾����
				cvui::printf(toolFrame_2, 270, 460, 0.5, 0xffffff, "Effects");
				cvui::trackbar(toolFrame_2, 240, 430, 100, &newEffects_num, 0, 4);
				//�������
				cvui::checkbox(toolFrame_2, 350, 440, "BigEyes", &BigEyes);
				//��ͨ���
				cvui::checkbox(toolFrame_2,430, 440, "Cartoon", &cartoon);
				//�۾����
				cvui::checkbox(toolFrame_2, 500, 440, "Glasses", &glasses);
				//ˢ��
				cvui::update();
				//��ʾ����
				cvui::imshow(WINDOW_NAME, toolFrame_2);
			}
			backBtn = false;
			takeBtn = false;
		}
		if (outBtn){
			//�˳��������
			frame.release();//�ͷ���Դ
			captue.release();//�ͷ���Դ
			break;
		}
		frame.copyTo(toolFrame_1);
		//��������
		cv::namedWindow(WINDOW_NAME, 0);
		cvResizeWindow(WINDOW_NAME, win_width, win_height);
		//д����Ƶ��Ƭ
		captue >> frame;
		if (frame.empty())//���ĳ֡Ϊ�����˳�ѭ��
			break;
		//��ʾͼ��
		cv::imshow(WINDOW_NAME, frame);
		//д�빤��
		//���հ�ť����
		takeBtn = cvui::button(toolFrame_1, 550, 240, daily, start, final);
		//�˳���ť����
		outBtn = cvui::button(toolFrame_1, 10, 20, "OUT");
		//ͼƬ�ⰴť����
		phoStatus = cvui::button(toolFrame_1, 550, 300, photo_start, photo_start, photo_end);
		//��ʾ����
		cvui::imshow(WINDOW_NAME, toolFrame_1);
		if (cv::waitKey(20) == 27) {
			frame.release();//�ͷ���Դ
			captue.release();//�ͷ���Դ
			break;
		}
	}
	cv::destroyAllWindows();//�ر����д���
	return 0;
}