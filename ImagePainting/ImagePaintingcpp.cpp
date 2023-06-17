#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;

typedef struct {
	CvPoint point;
	int brush;
	CvScalar c;
}errorpoint;			// ��ǥ, ��, �� ũ�� ������ ����ü

int error(CvScalar r, CvScalar c) {					// ���� �Լ�
	int sum = 0;
	for (int i = 0; i < 3; ++i) {
		sum += sqrt((r.val[i] - c.val[i]) * (r.val[i] - c.val[i]));			//���� ����
	}
	return sum / 3;		//����� ��ȯ
}


errorpoint maxerror(IplImage* ref, IplImage* cvs, int y, int x, int brush) {			//�׸��� ������ ���� ���� ū ��ǥ ���ϴ� �Լ�
	CvSize size = cvGetSize(ref);
	errorpoint e;
	int max_err = 0;
	for (int y1 = y; y1 < y + brush; ++y1)
		for (int x1 = x; x1 < x + brush; ++x1) {
			if (x1 < 0 || x1 > size.width - 1) continue;				//�ε��� ����� ���� ����
			if (y1 < 0 || y1 > size.height - 1) continue;				//�ε��� ����� ���� ����
			CvScalar r = cvGet2D(ref, y1, x1);
			CvScalar c = cvGet2D(cvs, y1, x1);
			int err = error(r, c);			//���� �Լ� ȣ��
			if (err > max_err) {			//���� ū ������ ��� �ֽ�ȭ
				max_err = err;
				e.point = cvPoint(x1, y1);		//����ü�� ��ǥ ����
				e.c = r;				// �� ����
				e.brush = brush;		//�� ũ�� ����
			}
		}
	return e;			//����ü ��ȯ
}

int areaError(IplImage* ref, IplImage* cvs, int y, int x, int brush) {			//�׸��� ������ ���� �����ϴ� �Լ�
	int errorSum = 0, cnt = 0;
	CvSize size = cvGetSize(ref);
	for (int y1 = y; y1 < y + brush; ++y1)
		for (int x1 = x; x1 < x + brush; ++x1) {
			if (x1 < 0 || x1 > size.width - 1) continue;				//�ε��� ����� �� ����
			if (y1 < 0 || y1 > size.height - 1) continue;				//�ε��� ����� �� ����
			CvScalar r = cvGet2D(ref, y1, x1);
			CvScalar c = cvGet2D(cvs, y1, x1);
			errorSum += error(r, c);	//�����Լ� ȣ��
			cnt++;
		}
	return errorSum / cnt;			//�׸��� ���� ���� ����� ���� �� ��ȯ
}

void shuffle(errorpoint* e, int cnt) {			//���� �Լ�
	srand(time(NULL));			//srand�Լ��� time�� ���� ���� ����
	int rn;
	errorpoint tmp;
	for (int i = 0; i < cnt - 1; i++) {
		rn = rand() % cnt;
		tmp = e[i];		//�������� �ε��� �ٲ���
		e[i] = e[rn];
		e[rn] = tmp;

	}
}

void paintLayer(IplImage* ref, IplImage* cvs, int brush, int n) {
	CvSize size = cvGetSize(ref);
	int i = 0, cnt = 0;
	int max_err = 0, max_x = 0, max_y = 0;
	for (int y = 0; y < size.height; y += brush)
		for (int x = 0; x < size.width; x += brush) {				//�̹����� �� ũ�⸸ŭ �׸���� ���� �� x,y�� �� ũ�⸸ŭ ���ؼ� �ݺ����� ����
			if (x < 0 || x > size.width - 1) continue;				// �ε��� ����� ���� ����
			if (y < 0 || y > size.height - 1) continue;				// �ε��� ����� ���� ����
			int	errorSum = areaError(ref, cvs, y, x, brush);		// �׸��� ������ ���۷��� �̹����� ������ �̹����� �ٸ� ���� ����ϴ� �Լ� ȣ��
			if (errorSum > 15) {								// �� ��� ���� 15���� ũ�ٸ�
				++cnt;						//����
			}
		}

	errorpoint* e = new errorpoint[cnt];		//��ǥ��, �� ũ��, ���� ������ ����ü �����Ҵ� 
	for (int y = 0; y < size.height; y += brush)
		for (int x = 0; x < size.width; x += brush) {
			if (x < 0 || x > size.width - 1) continue;
			if (y < 0 || y > size.height - 1) continue;
			int errorSum = areaError(ref, cvs, y, x, brush);
			if (errorSum > 15) {
				e[i] = maxerror(ref, cvs, y, x, brush);		// ��ȿ�� �׸��� �� ���� �ٸ� ��ǥ ���ϴ� �Լ� ȣ�� �� ����ü�� ����
				i++;
			}
		}

	shuffle(e, cnt);		// ����ü�迭�� ������ �������� �����ϴ� �Լ� ȣ��

	if (n == 0) {		//��尡 0�̸�
		for (int j = 0; j < i; ++j) {
			cvCircle(cvs, e[j].point, e[j].brush, e[j].c, -1);			//������ ����
		}
	}
	
	delete[] e;		//�����Ҵ� ����

}

void paint(IplImage* src, IplImage* cvs, IplImage* ref, int brush[5], int size, int n) {		// ����Ʈ �Լ�

	for (int i = 0; i < size; i++) {							// �� ũ�� �迭 �����ŭ �ݺ�
		cvWaitKey(1000);
		cvSmooth(src, ref, CV_GAUSSIAN, 2 * brush[i] + 1);		//����þ����ͷ� �̹����� �ѹ� ������ Ŀ���� ��ũ���� �ι� +1�� ��
		paintLayer(ref, cvs, brush[i], n);			// ����Ʈ���̾� �Լ� ȣ��
		cvShowImage("dst", cvs);
	}
}

int main() {
	IplImage* src;
	int n = 0;
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

	int brush[5] = { 30, 16, 12, 8, 4};			//�� ũ�� �迭
	cvShowImage("src", src);
	cvShowImage("dst", canvas);

	paint(src, canvas, reference, brush, 5, n); //paint �Լ� ȣ��

	cvWaitKey();

	return 0;
}