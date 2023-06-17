# ImpagePainting 코드

----------

## 코드 기능

이미지를 그림으로 변환시켜주는 프로그램이다.

해당 코드는 2가지 모드가 있으며, 0번 모드는 점들을 찍어서 그림으로 변환시키고 1번 모드는 점들을 선으로 연결해서 
그림으로 변환시켜준다. 

## 사용법

해당 프로그램 실행 후 콘솔창에 이미지의 절대 경로를 입력 후 원하는 모드 0,1중 하나릅 입력하면 된다.

이 코드는 opencv 2.3.0 라이브러를 이용한 코드로 해당 라이브러리를 다운받은 후 셋팅이 필요하다.

1.	디렉토리 설정 
-	Include 디렉토리 설정 : 프로젝트 속성에서 VC++디텍토리 칸으로 들어가 포함 디렉토리에 C:\\OpenCV-2.3.0\include를 기입한다.

-	라이브러리 디렉토리 설정 : 프로젝트 설정에서 VC++디텍토리 칸으로 들어가 라이브러리 디렉토리에 C:\\OpenCV-2.3.0\lib를 기입한다.

2.	라이브러리 파일 설정
-	프로젝트 속성에서 링커의 입력칸에 있는 추가 종속성에 opencv_core230.lib, opencv_imgproc230.lib, opencv_highgui230.lib를 기입한다.

3.	dll파일 설정
-	dll파일들을 현재 프로젝트 내의 debug폴더에 넣어준다.

------------------

## 참고 논문

해당 코드는 아래 논문을 참고해서 작성한 코드이다.

### Painterly Rendering with Curved Brush Strokes of Multiple Sizes


Aaron Hertzmann

Media Research Laboratory

Department of Computer Science

New York University

