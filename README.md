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

총 4명에서 프로젝트를 진행했으며, UI 수정 & Fire alarm 취약점 발견 및 수정 은 Luke 그리고 Joon 은 프로젝트 매니저 및 Fire alarm 코드 수정 및 전반적인 소스 알고리즘 작성
저는 전반적인 소스 알고리즘 작성 및 연동 과 소스코드 작성 맡았습니다. 

### 소스코드 소개

총 3층 짜리 카팍 타워 가 존재하며, 출입구는 총 3개, 탈출구  총 3개로 이루어져 있습니다. 한 층마다 출입구 와 탈출구 가 하나씩 존재하며, 각 출입문 과 출입구에는 바리게이트가 존재하며 출입문에는 등록된 차량만 왔다갔다 할 수 있는 센서가 존재합니다. 등록되지 않는 차량은 출입문을 통과하지 못합니다. 

등록된차량이 카팍 타워에 들어오게 되면 몇층에 주차했는지 그리고 주차된 층 차량 갯수가 올라가게 됩니다. 그리고 등록된 차량이 나가게 되면 들어오는 시간 과 나간 시간을 계산해서 자동적으로 영수증이 생성되게 됩니다. 

카팍은 온도 센서가 존재해서 일정 수준 보다 높은 온도가 측정 될 경우 모든 바리게이트가 열리게 되고, 차량을 바로 나갈 수 있도록 환경을 구축해 줍니다. 온도센서 프로세스는 절대 꺼지면 안됩니다. 

### 소스코드 소개 - 시뮬레이터

시뮬레이터는 데이터를 자동으로 생성해 주는 역할을 합니다. 데이터는 LinkedList 에 데이터가 담기게 되며, 담긴 데이터는 20개 이상 채워질 경우, 잠깐동안 멈추게 됩니다. (하지만, 이 기능은 구현하지 못했습니다.)


### 소스코드 소개 - 매니저


매니저 프로세스는 모든 데이터를 실시간으로 볼 수 있으며, 센서 프로세스도 관리 하게 됩니다. 그리고 자동차의 빌을 생성 할 수 있게 도와줍니다.


### 소스코드 소개 - 화재알람

화재 알람은 절대 꺼지지 않게 설계되어있으며, 보안적으로 완벽하게 구비 되어 있어야합니다. 그래서 미스라 규칙에 의거해서 만들어 졌습니다. 




### C 프로그래밍 프로젝트 에서 만들었던 카팍 시뮬레이터 소스 코드와 영상입니다. 
시뮬레이션 동영상 보고 싶을 경우, (If you want to watch the Demo, click the link) https://drive.google.com/file/d/1ogEZhRamJTKE32GxDRcFoCbq0CQcBJZe/view?usp=share_link
