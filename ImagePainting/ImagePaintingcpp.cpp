#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;

typedef struct {
	CvPoint point;
	int brush;
	CvScalar c;
}errorpoint;			// 좌표, 색, 붓 크기 저장할 구조체

int error(CvScalar r, CvScalar c) {					// 에러 함수
	int sum = 0;
	for (int i = 0; i < 3; ++i) {
		sum += sqrt((r.val[i] - c.val[i]) * (r.val[i] - c.val[i]));			//에러 구함
	}
	return sum / 3;		//평균을 반환
}


errorpoint maxerror(IplImage* ref, IplImage* cvs, int y, int x, int brush) {			//그리드 내부의 에러 가장 큰 좌표 구하는 함수
	CvSize size = cvGetSize(ref);
	errorpoint e;
	int max_err = 0;
	for (int y1 = y; y1 < y + brush; ++y1)
		for (int x1 = x; x1 < x + brush; ++x1) {
			if (x1 < 0 || x1 > size.width - 1) continue;				//인덱스 벗어나는 것을 방지
			if (y1 < 0 || y1 > size.height - 1) continue;				//인덱스 벗어나는 것을 방지
			CvScalar r = cvGet2D(ref, y1, x1);
			CvScalar c = cvGet2D(cvs, y1, x1);
			int err = error(r, c);			//에러 함수 호출
			if (err > max_err) {			//가장 큰 에러로 계속 최신화
				max_err = err;
				e.point = cvPoint(x1, y1);		//구조체에 좌표 저장
				e.c = r;				// 색 저장
				e.brush = brush;		//붓 크기 저장
			}
		}
	return e;			//구조체 반환
}

int areaError(IplImage* ref, IplImage* cvs, int y, int x, int brush) {			//그리드 내부의 에러 검출하는 함수
	int errorSum = 0, cnt = 0;
	CvSize size = cvGetSize(ref);
	for (int y1 = y; y1 < y + brush; ++y1)
		for (int x1 = x; x1 < x + brush; ++x1) {
			if (x1 < 0 || x1 > size.width - 1) continue;				//인덱스 벗어나는 것 방지
			if (y1 < 0 || y1 > size.height - 1) continue;				//인덱스 벗어나는 것 방지
			CvScalar r = cvGet2D(ref, y1, x1);
			CvScalar c = cvGet2D(cvs, y1, x1);
			errorSum += error(r, c);	//에러함수 호출
			cnt++;
		}
	return errorSum / cnt;			//그리드 내부 에러 평균을 구한 후 반환
}

void shuffle(errorpoint* e, int cnt) {			//셔플 함수
	srand(time(NULL));			//srand함수를 time을 통해 난수 얻어옴
	int rn;
	errorpoint tmp;
	for (int i = 0; i < cnt - 1; i++) {
		rn = rand() % cnt;
		tmp = e[i];		//무작위로 인덱스 바꿔줌
		e[i] = e[rn];
		e[rn] = tmp;

	}
}

void paintLayer(IplImage* ref, IplImage* cvs, int brush, int n) {
	CvSize size = cvGetSize(ref);
	int i = 0, cnt = 0;
	int max_err = 0, max_x = 0, max_y = 0;
	for (int y = 0; y < size.height; y += brush)
		for (int x = 0; x < size.width; x += brush) {				//이미지를 붓 크기만큼 그리드로 나눔 각 x,y를 붓 크기만큼 더해서 반복으로 구현
			if (x < 0 || x > size.width - 1) continue;				// 인덱스 벗어나는 것을 방지
			if (y < 0 || y > size.height - 1) continue;				// 인덱스 벗어나는 것을 방지
			int	errorSum = areaError(ref, cvs, y, x, brush);		// 그리드 면적의 레퍼런스 이미지와 컨버스 이미지의 다른 정도 계산하는 함수 호출
			if (errorSum > 15) {								// 위 계산 값이 15보다 크다면
				++cnt;						//증가
			}
		}

	errorpoint* e = new errorpoint[cnt];		//좌표와, 붓 크기, 색상 저장할 구조체 동적할당 
	for (int y = 0; y < size.height; y += brush)
		for (int x = 0; x < size.width; x += brush) {
			if (x < 0 || x > size.width - 1) continue;
			if (y < 0 || y > size.height - 1) continue;
			int errorSum = areaError(ref, cvs, y, x, brush);
			if (errorSum > 15) {
				e[i] = maxerror(ref, cvs, y, x, brush);		// 유효한 그리드 중 가장 다른 좌표 구하는 함수 호출 후 구조체에 저장
				i++;
			}
		}

	shuffle(e, cnt);		// 구조체배열의 순서를 랜덤으로 셔플하는 함수 호출

	if (n == 0) {		//모드가 0이면
		for (int j = 0; j < i; ++j) {
			cvCircle(cvs, e[j].point, e[j].brush, e[j].c, -1);			//원으로 찍음
		}
	}
	
	delete[] e;		//동적할당 해제

}

void paint(IplImage* src, IplImage* cvs, IplImage* ref, int brush[5], int size, int n) {		// 페인트 함수

	for (int i = 0; i < size; i++) {							// 붓 크기 배열 사이즈만큼 반복
		cvWaitKey(1000);
		cvSmooth(src, ref, CV_GAUSSIAN, 2 * brush[i] + 1);		//가우시안필터로 이미지를 한번 뭉갠다 커널은 붓크기의 두배 +1로 함
		paintLayer(ref, cvs, brush[i], n);			// 페인트레이어 함수 호출
		cvShowImage("dst", cvs);
	}
}

int main() {
	IplImage* src;
	int n = 0;
	cout << "=============================================" << endl;
	cout << "Painterly Rendering" << endl;	
	cout << "=============================================" << endl;
	while (1) {									//이미지 이름 받을 반복문
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

	for (int y = 0; y < size.height; ++y)				//canvas를 흰색으로 칠함
		for (int x = 0; x < size.width; ++x)
			cvSet2D(canvas, y, x, cvScalar(255, 255, 255));

	int brush[5] = { 30, 16, 12, 8, 4};			//붓 크기 배열
	cvShowImage("src", src);
	cvShowImage("dst", canvas);

	paint(src, canvas, reference, brush, 5, n); //paint 함수 호출

	cvWaitKey();

	return 0;
}