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

CvPoint nextpoint(CvPoint cur, IplImage* ref) {				//붓질할 다음 좌표 구하는 함수
	int gx, gy;
	int cx1, cy1;
	//int cx2, cy2;
	CvSize size = cvGetSize(ref);

	if (cur.x + 1 < 0 || cur.x + 1 > size.width - 1) {			//좌표 벗어났을 시 방지
		cx1 = cur.x;
	}
	else {
		cx1 = cur.x + 1;
	}

	if (cur.y + 1 < 0 || cur.y + 1 > size.height - 1) {			//좌표 벗어났을 시 방지
		cy1 = cur.y;
	}
	else {
		cy1 = cur.y + 1;
	}


	//if (cur.x - 1 < 0 || cur.x - 1 > size.width - 1) {			//좌표 벗어났을 시 방지
	//	cx2 = cur.x;
	//}
	//else {
	//	cx2 = cur.x - 1;
	//}

	//if (cur.y - 1 < 0 || cur.y - 1 > size.height - 1) {			//좌표 벗어났을 시 방지
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

	//gx = (sx.val[0] - sx1.val[0]) * 0.11 + (sx.val[1] - sx1.val[1]) * 0.59 + (sx.val[2] - sx1.val[2]) * 0.3;  // 논문에 나온 비율로 변화율 구함 opencv는 bgr순서이므로 순서에 맞춤
	//gy = (sy.val[0] - sy1.val[0]) * 0.11 + (sy.val[1] - sy1.val[1]) * 0.59 + (sy.val[2] - sy1.val[2]) * 0.3;

	CvPoint next;

	if (gy * cur.x + -gx * cur.y > 0) {		//90도를 벗어나지 않은 경우 수직방향
		next = cvPoint(-gy, gx);
	}
	else {								//벗어났을 경우 반대방향의 수직방향
		next = cvPoint(gy, -gx);
	}
	return next;
}

void paintstroke(errorpoint e, IplImage* ref, IplImage* cvs) {
	CvSize size = cvGetSize(ref);
	int maxstrokelength = 10;		//최대 붓질 횟수
	int minxstrokelength = 5;		//최소 붓질 횟수

	CvPoint current = e.point;			// 현재좌표를 저장
	CvPoint next;

	for (int i = 0; i < maxstrokelength; i++) {
		CvScalar r = cvGet2D(ref, current.y, current.x);
		CvScalar c = cvGet2D(cvs, current.y, current.x);
		int rc = 0, rs = 0;

		for (int i = 0; i < 3; ++i) {						//원본과 변화했을 때 뭐가 더 괜찮은지 판단할 반복문
			rc = (r.val[i] - c.val[i]) * (r.val[i] - c.val[i]);
			rs = (r.val[i] - e.c.val[i]) * (r.val[i] - e.c.val[i]);
		}

		if (i > minxstrokelength && (rc < rs)) break;		//최소만큼 그렸을 때 변화한 것보다 원본이 낫다면 중지

		next = nextpoint(current, ref);							// 다음 지점의 좌표 가져올 함수 호출
		if (i != 0 && next.x == 0 && next.y == 0) break;					//  단색이라면 중지
		else if (i == 0 && next.x == 0 && next.y == 0) {	// 만약 배경이 다 같은 색이여서 못 그릴 수 있으므로 넣은 조건

			next.x = e.brush + current.x;				// 붓 크기만큼 한칸식 이동
			next.y = e.brush + current.y;

			if (next.x < 0 || next.x > size.width - 1) continue;	// 인덱스 벗어났을 시
			if (next.y < 0 || next.y > size.height - 1) continue;	// 인덱스 벗어났을 시

			cvLine(cvs, current, next, e.c, e.brush);		// 한 칸 씩은 칠하고 중지하게 해 배경이 안칠해지는 경우를 막음
			break;
		}

		float sum = next.x * next.x + next.y * next.y;			//법선벡터를 구하기 위한 계산	
		sum = sqrt(sum);										//법선벡터를 구하기 위한 계산
		next.x = next.x * e.brush / sum + current.x;				// 연결할 다음 좌표 이동할 수직 방향 법선 벡터에 현재 붓크기를 곱해주고 지금 좌표를 더해준다
		next.y = next.y * e.brush / sum + current.y;			// 연결할 다음 좌표 이동할 수직 방향 법선 벡터에 현재 붓크기를 곱해주고 지금 좌표를 더해준다
		if (next.x < 0 || next.x > size.width - 1) continue;	// 인덱스 벗어났을 시
		if (next.y < 0 || next.y > size.height - 1) continue;	// 인덱스 벗어났을 시

		cvLine(cvs, current, next, e.c, e.brush);		// 붓질

		current = next;		//현재좌표 변경


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
	else if (n == 1) {			//모드가 1이라면
		for (int j = 0; j < i; ++j) {
			paintstroke(e[j], ref, cvs);		//붓질하는 함수 호출
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

	while (1) {										//모드를 받을 반복문
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