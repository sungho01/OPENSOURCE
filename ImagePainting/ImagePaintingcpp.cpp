#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;


int main() {
	IplImage* src;
	int n;
	cout << "=============================================" << endl;
	cout << "Painterly Rendering" << endl;	
	cout << "=============================================" << endl;
	while (1) {									//�̹��� �̸� ���� �ݺ���
		char s[100];
		cout << "Input File Path: ";
		cin >> s;
		src = cvLoadImage(s);
		if (src == nullptr) {
			cout << "File Not Found!" << endl;
		}
		else break;
	}

	CvSize size = cvGetSize(src);
	IplImage* reference = cvCreateImage(size, 8, 3);
	IplImage* canvas = cvCreateImage(size, 8, 3);

	for (int y = 0; y < size.height; ++y)				//canvas�� ������� ĥ��
		for (int x = 0; x < size.width; ++x)
			cvSet2D(canvas, y, x, cvScalar(255, 255, 255));

	int brush[5] = { 30, 16, 8, 4, 2 };			//�� ũ�� �迭
	cvShowImage("src", src);
	cvShowImage("dst", canvas);

	cvWaitKey();

	return 0;
}