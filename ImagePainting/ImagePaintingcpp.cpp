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

CvPoint nextpoint(CvPoint cur, IplImage* ref) {				//������ ���� ��ǥ ���ϴ� �Լ�
	int gx, gy;
	int cx1, cy1;
	//int cx2, cy2;
	CvSize size = cvGetSize(ref);

	if (cur.x + 1 < 0 || cur.x + 1 > size.width - 1) {			//��ǥ ����� �� ����
		cx1 = cur.x;
	}
	else {
		cx1 = cur.x + 1;
	}

	if (cur.y + 1 < 0 || cur.y + 1 > size.height - 1) {			//��ǥ ����� �� ����
		cy1 = cur.y;
	}
	else {
		cy1 = cur.y + 1;
	}


	//if (cur.x - 1 < 0 || cur.x - 1 > size.width - 1) {			//��ǥ ����� �� ����
	//	cx2 = cur.x;
	//}
	//else {
	//	cx2 = cur.x - 1;
	//}

	//if (cur.y - 1 < 0 || cur.y - 1 > size.height - 1) {			//��ǥ ����� �� ����
	//	cy2 = cur.y;
	//}
	//else {
	//	cy2 = cur.y - 1;
	//}

	CvScalar s = cvGet2D(ref, cur.y, cur.x);
	CvScalar sx = cvGet2D(ref, cur.y, cx1);
	CvScalar sy = cvGet2D(ref, cy1, cur.x);
	/*CvScalar sx1 = cvGet2D(ref, cur.y, cx2);
	CvScalar sy1 = cvGet2D(ref, cy2, cur.x);*/

	gx = (sx.val[0] - s.val[0]) * 0.11 + (sx.val[1] - s.val[1]) * 0.59 + (sx.val[2] - s.val[2]) * 0.3;
	gy = (sy.val[0] - s.val[0]) * 0.11 + (sy.val[1] - s.val[1]) * 0.59 + (sy.val[2] - s.val[2]) * 0.3;

	//gx = (sx.val[0] - sx1.val[0]) * 0.11 + (sx.val[1] - sx1.val[1]) * 0.59 + (sx.val[2] - sx1.val[2]) * 0.3;  // ���� ���� ������ ��ȭ�� ���� opencv�� bgr�����̹Ƿ� ������ ����
	//gy = (sy.val[0] - sy1.val[0]) * 0.11 + (sy.val[1] - sy1.val[1]) * 0.59 + (sy.val[2] - sy1.val[2]) * 0.3;

	CvPoint next;

	if (gy * cur.x + -gx * cur.y > 0) {		//90���� ����� ���� ��� ��������
		next = cvPoint(-gy, gx);
	}
	else {								//����� ��� �ݴ������ ��������
		next = cvPoint(gy, -gx);
	}
	return next;
}

void paintstroke(errorpoint e, IplImage* ref, IplImage* cvs) {
	CvSize size = cvGetSize(ref);
	int maxstrokelength = 10;		//�ִ� ���� Ƚ��
	int minxstrokelength = 5;		//�ּ� ���� Ƚ��

	CvPoint current = e.point;			// ������ǥ�� ����
	CvPoint next;

	for (int i = 0; i < maxstrokelength; i++) {
		CvScalar r = cvGet2D(ref, current.y, current.x);
		CvScalar c = cvGet2D(cvs, current.y, current.x);
		int rc = 0, rs = 0;

		for (int i = 0; i < 3; ++i) {						//������ ��ȭ���� �� ���� �� �������� �Ǵ��� �ݺ���
			rc = (r.val[i] - c.val[i]) * (r.val[i] - c.val[i]);
			rs = (r.val[i] - e.c.val[i]) * (r.val[i] - e.c.val[i]);
		}

		if (i > minxstrokelength && (rc < rs)) break;		//�ּҸ�ŭ �׷��� �� ��ȭ�� �ͺ��� ������ ���ٸ� ����

		next = nextpoint(current, ref);							// ���� ������ ��ǥ ������ �Լ� ȣ��
		if (i != 0 && next.x == 0 && next.y == 0) break;					//  �ܻ��̶�� ����
		else if (i == 0 && next.x == 0 && next.y == 0) {	// ���� ����� �� ���� ���̿��� �� �׸� �� �����Ƿ� ���� ����

			next.x = e.brush + current.x;				// �� ũ�⸸ŭ ��ĭ�� �̵�
			next.y = e.brush + current.y;

			if (next.x < 0 || next.x > size.width - 1) continue;	// �ε��� ����� ��
			if (next.y < 0 || next.y > size.height - 1) continue;	// �ε��� ����� ��

			cvLine(cvs, current, next, e.c, e.brush);		// �� ĭ ���� ĥ�ϰ� �����ϰ� �� ����� ��ĥ������ ��츦 ����
			break;
		}

		float sum = next.x * next.x + next.y * next.y;			//�������͸� ���ϱ� ���� ���	
		sum = sqrt(sum);										//�������͸� ���ϱ� ���� ���
		next.x = next.x * e.brush / sum + current.x;				// ������ ���� ��ǥ �̵��� ���� ���� ���� ���Ϳ� ���� ��ũ�⸦ �����ְ� ���� ��ǥ�� �����ش�
		next.y = next.y * e.brush / sum + current.y;			// ������ ���� ��ǥ �̵��� ���� ���� ���� ���Ϳ� ���� ��ũ�⸦ �����ְ� ���� ��ǥ�� �����ش�
		if (next.x < 0 || next.x > size.width - 1) continue;	// �ε��� ����� ��
		if (next.y < 0 || next.y > size.height - 1) continue;	// �ε��� ����� ��

		cvLine(cvs, current, next, e.c, e.brush);		// ����

		current = next;		//������ǥ ����


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
	else if (n == 1) {			//��尡 1�̶��
		for (int j = 0; j < i; ++j) {
			paintstroke(e[j], ref, cvs);		//�����ϴ� �Լ� ȣ��
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

	while (1) {										//��带 ���� �ݺ���
		cout << "Select Drawing Mode(0 = circle, 1 = stroke) :";
		cin >> n;
		if (n != 0 && n != 1) {
			cout << "Wrong Drawing Mode!" << endl;
		}
		else
			break;
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