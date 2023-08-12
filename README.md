### 환경설정

소스코드는 리눅스 환경에서만 돌아가며, 리눅스 vscode & visual studio 에서 실행하시면 됩니다. 
.makefile 매크로로 실행을 시키셔도 되고, 아니면 makefile 안에 있는 소스코드로 실행 시키셔도 됩니다.

./makefile 소스 코드는 아래에 있습니다.

CC = gcc
OUTPUT = -o
EXTRA = -Wall -Wextra -Werror -pthread -lrt

all: simulator manager firealarm

simulator : simulator.c
	$(CC) $(OUTPUT) simulator simulator.c htable.c shm.c $(EXTRA)

manager : manager.c
	$(CC) $(OUTPUT) manager manager.c shm.c $(EXTRA)

firealarm : firealarm.c
	$(CC) $(OUTPUT) firealarm firealarm.c shm.c $(EXTRA)

clean:
	rm -f simulator manager firealarm *.o

.PHONY: all clean

### 조원 소개

총 4명 [ Luke, Joon , Mac[작성자], JuSung ]

Project Manager , Luke<br>
UI 수정 & Fire alarm 취약점 발견 및 수정 <br><br>
Leader & Coder, Joon<br>
Fire Alarm 소스 코드 수정 & 기획 및 설계  <br> <br>
Coder, Mac  <br>
시뮬레이터 파일, 매니저 파일 작성 & 기획 및 설계  <br> <br>
Coder, JuSung  <br>
Unit Test & 데이터 분석

### 소스코드 소개

총 3층 짜리 카팍 타워 가 존재하며, 출입구는 총 3개, 탈출구  총 3개로 이루어져 있습니다. 한 층마다 출입구 와 탈출구 가 하나씩 존재하며, 각 출입문 과 출입구에는 결제 시스템 및 바리게이트가 존재하며 출입문에는 [등록된 차량] 만 왔다갔다 할 수 있는 센서가 존재합니다. 등록되지 않은 차량은 출입문을 통과한뒤 바로 출입구로 나가야한다. 

등록된차량이 카팍 타워에 들어올 경우 어디에 주차했는지 매니저가 식별 할 수 있으며 매니저는 차량의 주차 공간을 실시간으로 확인 할 수 있다. 그리고 등록된 차량이 나가게 되면 들어오는 [들어온 시간과 나간시간]을 계산해서 자동적으로 영수증이 발급되게 된다. 
주차 타워 안에는 온도 센서가 존재하며, 일정 수준 보다 높은 온도가 측정 될 경우 모든 바리게이트가 열리게 되고, 이 온도센서의 역할은 불이 날 것을 대비해서 만들어 졌으며, 대물 피해가 발생하지 않도록 차량을 바로 나갈 수 있게끔 모든 층에 있는 바리게이트가 실시간으로 열리게 된다.  
### 소스코드 소개 - 시뮬레이터

시뮬레이터는 자동차 데이터만 계속해서 생성해 주는 역할을 한다.

### 소스코드 소개 - 매니저

매니저 데이터는 LinkedList 에 데이터가 담기게 되며, 데이터 삭제 & 추가 <LinkedList> 사용 한 이유는 자동차가 선입선출로 이루어지기 때문이다. 
하지만, 여기서 중간에 차량이 유턴해서 이탈하는 경우는 배제했고, 만약에 차량이 잘못들어올 경우 차량은 게이트를 통과해서 
Exit로 나가야 한다. 통행 비용은 발생하지 않는다. 등록된 차량은 .txt 파일에 저장되어있으며, .txt 파일을 읽어 와서 <HashTable> 에 값이 이동하게된다. 게이트웨이 앞에 있는 센서는 LinkedList의 값을 식별하고 HashTable 안에 있는 PlateNumber를 확인해서 등록된 차량인지 식별 할 수 있게끔 설계되어있다. 그리고, 게이트를 통과한 차량은 들어온 시간은 다른 배열 값에 저장이 되고, 차량이 게이트를 통해서 나갈 경우, [나간 시간 - 들어온 시간] * 시간 당 가격 으로 측정되서 .txt 파일에 저장된다. 그리고 난 뒤, 주차 타워로 인해서 번 수익 도한 측정되게 된다. 

그리고 데이터의 이동 경로를 UI를 통해서 실시간으로 확인 할 수 있다. 

### 소스코드 소개 - 화재알람

화재 알람은 절대 꺼지지 않게 설계되어있으며, 보안적으로 완벽하게 구비 되어 있어야합니다. 그래서 미스라 규칙에 의거해서 만들어 졌습니다. 


### C 프로그래밍 프로젝트 에서 만들었던 카팍 시뮬레이터 소스 코드와 영상입니다. 
시뮬레이션 동영상 보고 싶을 경우, https://drive.google.com/file/d/1ogEZhRamJTKE32GxDRcFoCbq0CQcBJZe/view?usp=share_link
