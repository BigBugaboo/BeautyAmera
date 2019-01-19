#pragma once
using namespace cv;
using namespace std;

void skin(Mat &image,Mat &result){
	Mat temp;
	Mat Y, Cr, Cb;
	Mat channels[3];
	image.copyTo(temp);                  //备份  
	/*转换颜色空间并分割颜色通道*/
	cvtColor(temp, temp, CV_BGR2YCrCb);
	split(temp, channels);
	Y = channels[0];
	Cr = channels[1];
	Cb = channels[2];
	result.create(image.rows, image.cols, CV_8UC1);//创建等大小图像
	for (int j = 1; j < Y.rows - 1; j++)
	{
		uchar* currentCr = Cr.ptr< uchar>(j);
		uchar* currentCb = Cb.ptr< uchar>(j);
		uchar* current = result.ptr< uchar>(j);
		for (int i = 1; i < Y.cols - 1; i++)
		{ // 这个Cr、Cb的范围根据实际环境要进行调节效果会好点
			if ((currentCr[i] > 125) && (currentCr[i] < 155) && (currentCb[i] > 105) && (currentCb[i] < 125))
			{
				current[i] = 255;
			}
			else
			{
				current[i] = 0;
			}
		}
	}
}
//磨皮功能
void onbuffing(const cv::Mat &image, cv::Mat &result,int value){
	Mat mask, temp;
	image.copyTo(result);
	cv::Mat detected(result.size(), CV_8UC3, cv::Scalar(0, 0, 0));
	skin(result, mask);

	result.copyTo(detected, mask);

	bilateralFilter(detected, temp, value, value * 2, value / 2);
	temp.copyTo(result, mask);
}
//大眼功能
void detectAndDisplay(cv::Mat &frame)
{
	string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier eyes_cascade;
	eyes_cascade.load(eyes_cascade_name);
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	std::vector<Rect> eyes;
	eyes_cascade.detectMultiScale(frame_gray, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (int j = 0; j < eyes.size(); j++)
	{
		int width = frame.cols;
		int heigh = frame.rows;
		Mat img1(frame.size(), CV_8UC3);
		frame.copyTo(img1);
		Point center(eyes[j].x + eyes[j].width*0.5, eyes[j].y + eyes[j].height*0.5);
		int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
		//circle(img1, center, radius, Scalar(255, 0, 0), 4, 8, 0);
		int R1 = sqrtf(eyes[j].width*eyes[j].width + eyes[j].height*eyes[j].height) / 8; //直接关系到放大的力度,与R1成正比;

		for (int y = 0; y < heigh; y++)
		{
			uchar *img1_p = img1.ptr<uchar>(y);
			for (int x = 0; x < width; x++)
			{
				int dis = norm(Point(x, y) - center);
				if (dis < R1)
				{
					int newX = (x - center.x)*dis / R1 + center.x;
					int newY = (y - center.y)*dis / R1 + center.y;

					img1_p[3 * x] = frame.at<uchar>(newY, newX * 3);
					img1_p[3 * x + 1] = frame.at<uchar>(newY, newX * 3 + 1);
					img1_p[3 * x + 2] = frame.at<uchar>(newY, newX * 3 + 2);
				}
			}
		}
		img1.copyTo(frame);
	}
}
//卡通效果
void cartoonStatus(cv::Mat &image){
	Mat gray_Image;
	cvtColor(image, gray_Image, CV_BGR2GRAY);
	const int MEDIAN_BLUR_FILTER_SIZE = 7;
	medianBlur(gray_Image, gray_Image, MEDIAN_BLUR_FILTER_SIZE);
	Mat edges_Image;
	const int LAPLACIAN_FILTER_SIZE = 5;
	Laplacian(gray_Image, edges_Image, CV_8U, LAPLACIAN_FILTER_SIZE);
	//均衡化前
	Mat mask_Image;
	const int EDGES_THRESHOLD = 80;
	threshold(edges_Image, mask_Image, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);
	//素描图
	Size size = image.size();
	Size smallSize;
	smallSize.width = size.width / 2;
	smallSize.height = size.height / 2;
	Mat small_Image = Mat(smallSize, CV_8UC3);
	resize(image, small_Image, smallSize, 0, 0, INTER_LINEAR);
	Mat tmp_Image = Mat(smallSize, CV_8UC3);
	int repetitions = 7;
	for (int i = 0; i < repetitions; i++)
	{
		int ksize = 9;
		double sigmaColor = 9;
		double sigmaSpace = 7;
		bilateralFilter(small_Image, tmp_Image, ksize, sigmaColor, sigmaSpace);
		bilateralFilter(tmp_Image, small_Image, ksize, sigmaColor, sigmaSpace);
	}
	Mat big_Image;
	resize(small_Image, big_Image, size, 0, 0, INTER_LINEAR);
	Mat dst_Image = Mat(size, CV_8UC3);
	dst_Image.setTo(0);
	big_Image.copyTo(image, mask_Image);
}
//取最小值
int FMax(const int X, const int Y)
{
	return (X < Y ? Y : X);
}

//取最大值
int FMin(const int X, const int Y)
{
	return (Y < X ? Y : X);
}
//美白功能
void BalanceColor(Mat& frame, Mat& bitmap, int value)
{
	frame.copyTo(bitmap);

	value = (value * 7) + 30;
	//定义转换数组
	double  highlights_add[256], highlights_sub[256];
	double  midtones_add[256], midtones_sub[256];
	double  shadows_add[256], shadows_sub[256];

	//初始化转换数组
	for (int i = 0; i < 256; i++)
	{
		highlights_add[i] = shadows_sub[255 - i] = (1.075 - 1 / ((double)i / 16.0 + 1));
		midtones_add[i] = midtones_sub[i] = 0.667 * (1 - (((double)i - 127.0) / 127.0)*(((double)i - 127.0) / 127.0));
		shadows_add[i] = highlights_sub[i] = 0.667 * (1 - (((double)i - 127.0) / 127.0)*(((double)i - 127.0) / 127.0));
	}
	int red, green, blue;
	unsigned char r_lookup[256], g_lookup[256], b_lookup[256];

	for (int i = 0; i < 256; i++)
	{
		red = i;
		green = i;
		blue = i;

		red += (int)(0.0 * shadows_sub[red] + value * midtones_add[red] + 0.0 * highlights_sub[red]);
		red = FMax(0, FMin(0xFF, red));

		green += (int)(0.0 * shadows_sub[green] + value * midtones_add[green] + 0.0 * highlights_sub[green]);
		green = FMax(0, FMin(0xFF, green));

		blue += (int)(0.0 * shadows_sub[blue] + value * midtones_add[blue] + 0.0 * highlights_sub[blue]);
		blue = FMax(0, FMin(0xFF, blue));

		r_lookup[i] = (unsigned char)red;
		g_lookup[i] = (unsigned char)green;
		b_lookup[i] = (unsigned char)blue;
	}

	for (int row = 0; row < bitmap.rows; row++)
	{
		for (int col = 0; col < bitmap.cols; col++)
		{
			bitmap.at<Vec3b>(row, col)[0] = b_lookup[bitmap.at<Vec3b>(row, col)[0]];
			bitmap.at<Vec3b>(row, col)[1] = g_lookup[bitmap.at<Vec3b>(row, col)[1]];
			bitmap.at<Vec3b>(row, col)[2] = r_lookup[bitmap.at<Vec3b>(row, col)[2]];
		}
	}
}
//眼镜功能
void AddGlasses(cv::Mat &frame)
{
	string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier eyes_cascade;
	RNG rng(12345);
	eyes_cascade.load(eyes_cascade_name);
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	std::vector<Rect> eyes;
	eyes_cascade.detectMultiScale(frame_gray, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	int x1, x2, y1, y2;
	int flag = 1;
	Point start, end;
	int r1, r2;
	for (int j = 0; j < eyes.size(); j++)
	{
		if (eyes.size() % 2 == 1)
			return;
		else{
			int width = frame.cols;
			int heigh = frame.rows;
			Mat img1(frame.size(), CV_8UC3);
			frame.copyTo(img1);
			Point center(eyes[j].x + eyes[j].width*0.5, eyes[j].y + eyes[j].height*0.5);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.36);
			circle(img1, center, radius, Scalar(0, 0, 0), 2, 8, 0);
			if (flag == 1)
			{
				start = Point(eyes[j].x + eyes[j].width*0.5, eyes[j].y + eyes[j].height*0.5); //直线起点
				r1 = cvRound((eyes[j].width + eyes[j].height)*0.36);
				flag = 2;
			}
			else if (flag == 2)
			{
				r2 = cvRound((eyes[j].width + eyes[j].height)*0.36);
				end = Point(eyes[j].x + eyes[j].width*0.5, eyes[j].y + eyes[j].height*0.5); //直线起点


				if (start.x > end.x)
				{
					start.x -= r1;
					end.x += r2;
					start.y = end.y = (start.y + end.y) / 2;
					if (end.x - start.x >= -30){
						cv::line(img1, start, end, cv::Scalar(0, 0, 0), 1);
					}
				}

				else
				{
					start.x += r1;
					end.x -= r2;
					start.y = end.y = (start.y + end.y) / 2;
					if (end.x - start.x <= 30){
						cv::line(img1, start, end, cv::Scalar(0, 0, 0), 1);
					}
				}

				flag = 1;
			}

			img1.copyTo(frame);
		}
	}
}
//木刻
void muKeFilter(Mat &srcImage, Mat &muke){
	Mat dstImage = srcImage;
	cvtColor(dstImage, dstImage, CV_BGR2GRAY);
	threshold(dstImage, dstImage, 127, 255, THRESH_BINARY);
	dstImage.copyTo(muke);
}
//怀旧
void huaiJiuFilter(Mat &srcImage, Mat &huaijiu){
	srcImage.copyTo(huaijiu);
	Mat dstImage = huaijiu;
	int rowNum = dstImage.rows;
	int colNum = dstImage.cols;

	for (int j = 0; j<rowNum; j++){
		uchar* data = dstImage.ptr<uchar>(j);
		for (int i = 0; i<colNum; i++){
			int b = data[i * 3];
			int g = data[i * 3 + 1];
			int r = data[i * 3 + 2];

			int R = static_cast<int>(0.393*r + 0.769*g + 0.189*b);
			int G = static_cast<int>(0.349*r + 0.686*g + 0.168*b);
			int B = static_cast<int>(0.272*r + 0.534*g + 0.131*b);

			data[i * 3 + 2] = max(0, min(R, 255));
			data[i * 3 + 1] = max(0, min(G, 255));
			data[i * 3] = max(0, min(B, 255));
		}
	}
}
//线条
void xianTiaoFilter(Mat &srcImage, Mat &xiantiao){
	Mat grayImage;

	//灰度化
	cvtColor(srcImage, grayImage, CV_BGR2GRAY);

	// 设置中值滤波器参数
	medianBlur(grayImage, grayImage, 7);

	// Laplacian边缘检测
	Mat edge;
	Laplacian(grayImage, edge, CV_8U, 5);

	// 对边缘检测结果进行二值化
	Mat dstImage;
	threshold(edge, dstImage, 127, 255, THRESH_BINARY_INV);// >127 ? 0:255,用黑色描绘轮廓

	dstImage.copyTo(xiantiao);
}
//素描
void suMiaoFilter(Mat &srcImage, Mat &sumiao){
	Mat gray0, gray1;
	//去色，灰度化
	cvtColor(srcImage, gray0, CV_BGR2GRAY);

	//反色
	addWeighted(gray0, -1, NULL, 0, 255, gray1);

	//高斯模糊,高斯核的Size与最后的效果有关  
	GaussianBlur(gray1, gray1, Size(11, 11), 0);

	//融合：颜色减淡  
	Mat dstImage(gray1.size(), CV_8UC1);

	for (int y = 0; y<srcImage.rows; y++)
	{
		uchar* P0 = gray0.ptr<uchar>(y);
		uchar* P1 = gray1.ptr<uchar>(y);
		uchar* P = dstImage.ptr<uchar>(y);
		for (int x = 0; x<srcImage.cols; x++)
		{
			int tmp0 = P0[x];
			int tmp1 = P1[x];
			P[x] = (uchar)min((tmp0 + (tmp0*tmp1) / (256 - tmp1)), 255);
		}
	}
	dstImage.copyTo(sumiao);
}
//滤镜功能
void selectEffects(Mat &srcImage, Mat &muke,int flag){
	switch (flag){
		case 1:
			muKeFilter(srcImage, muke);
			break;
		case 2:
			huaiJiuFilter(srcImage, muke);
			break;
		case 3:
			xianTiaoFilter(srcImage, muke);
			break;
		case 4:
			suMiaoFilter(srcImage, muke);
			break;
		default:
			break;
	}

}