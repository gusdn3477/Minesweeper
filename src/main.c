﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//수정을 하자 6시 43분
int len, col, num, seed, visi = 0, init = 0;
// Length，Column，Mines, Seed, Visible Grid, Initialized or not
unsigned char *m;
// 00000000
//        ^ Is mine or not
//       ^  Is visible or not
//     ^~   Mark and type
// ^~~~     Mines number around this grid
char p[] = { ' ', 'O', 'X', '_' };
// Mark characters

#define NONE 0
#define SAFE 1
#define MINE 2
#define QUERY 3

#define M(x, y) m[(x)*col+(y)]
#define IS_MINE(x, y) (M(x, y)&1)
#define IS_VISI(x, y) (M(x, y)&2)
// Visible or not
#define IS_MARK(x, y) (M(x, y)&12)
#define IS_NUM(x, y) (M(x, y)&240)
#define IS_OUT(x, y) (x>=len||y>=col||x<0||y<0)
// Out of range or notF
#define GET_MARK(x, y) ((M(x, y)&12)>>2)
#define GET_NUM(x, y) ((M(x, y)&240)>>4)
#define SET_MINE(x, y, s) M(x, y)=s?M(x, y)|1:M(x, y)&254
#define SET_VISI(x, y, s) M(x, y)=s?M(x, y)|2:M(x, y)&254
#define SET_MARK(x, y, s) M(x, y)=s&2?s&1?M(x, y)|12:(M(x, y)&243)|8:s&1?M(x, y)&243|4:M(x, y)&243
#define INC_NUM(x, y) if (!IS_OUT(x, y)) M(x, y)=(M(x, y)&15)|(GET_NUM(x, y)+1)<<4

void bfs(int x, int y) {
	if (IS_OUT(x, y) || IS_VISI(x, y))
		return;
	++visi;
	SET_VISI(x, y, 1);
	if (!GET_NUM(x, y)) {
		bfs(x - 1, y - 1);
		bfs(x - 1, y);
		bfs(x - 1, y + 1);
		bfs(x, y - 1);
		bfs(x, y + 1);
		bfs(x + 1, y - 1);
		bfs(x + 1, y);
		bfs(x + 1, y + 1);
	}
}

void print(int c) {
	int x, y;
	printf("  ");
	for (x = 0; x < len; ++x) {
		printf("%-2d", x);
	}
	printf("\n");
	for (y = col - 1; y >= 0; --y) {
		printf("%-2d", y);
		if (c)
			for (x = 0; x < len; ++x) {
				if (IS_MINE(x, y))
					printf("%-2c", '*');
				else
					printf("%-2c", GET_NUM(x, y) + '0');
			}
		else
			for (x = 0; x < len; ++x) {
				if (IS_VISI(x, y)) {
					if (IS_MINE(x, y))
						printf("%-2c", '*');
					else
						printf("%-2c", GET_NUM(x, y) + '0');
				}
				else
					printf("%-2c", p[GET_MARK(x, y)]);
			}
		printf("%-2d", y);
		printf("\n");
	}
	printf("  ");
	for (x = 0; x < len; ++x) {
		printf("%-2d", x);
	}
	printf("\n");
	if (c)
		printf("WARNING: CHAETING DETECTED\n");
}

void clear(void) {
	for (int i = 0; i < len*col; ++i)
		m[i] |= 2;
}

int input(void) {	//사용자의 입력, 입력값 검증, 지뢰 탐색 등의 여러 기능이 있는 함수.
	//TODO: 함수 기능 세분화(하나의 함수가 하나의 기능만 하도록 하기) 하기.
	int x, y, s;	//사용자로부터 입력받을 x,y좌표와 명령어(s)

	//사용자 입력 처리
	printf("Enter X coordinate, Y coordinate, and instruction\n");
	scanf("%d %d %d", &x, &y, &s);

	//입력값 검증 (좌표값 범위 검사)
	if (IS_OUT(x, y)) {
		print(0);
		printf("Invaild Command: Out of range\n");
		return 0;	//게임 계속 진행
	}

	//게임판 초기화
	if (!init) {
		for (int i = 0, a, b; i < num; ++i) {
			//반복해서 랜덤한 (a, b) 좌표 생성
			a = rand() % len;
			b = rand() % col;
			if (IS_MINE(a, b) || (a == x && b == y))
				continue;
			else {
				//(a,b)좌표에 지뢰 매설 및 주변 8개 칸의 안내숫자 1씩 증가
				INC_NUM(a - 1, b - 1);
				INC_NUM(a - 1, b);
				INC_NUM(a - 1, b + 1);
				INC_NUM(a, b - 1);
				SET_MINE(a, b, 1);
				INC_NUM(a, b + 1);
				INC_NUM(a + 1, b - 1);
				INC_NUM(a + 1, b);
				INC_NUM(a + 1, b + 1);
			}
		}
		init = 1;	//초기화 완료
	}

	//명령어 검사 및 분기처리
	if (s) {
		if (s == -seed) {	//-seed 값을 명령어로 입력할 경우 치트 동작
			print(1);
		}
		else if (s <= 4 && s > 0) {
			//해당 좌표에 메모하는 명령(1,2,3,4)를 입력했을 경우
			if (!IS_VISI(x, y))
				SET_MARK(x, y, s - 1);
			else {
				print(0);
				printf("Invaild Command: Already visible\n");
				return 0;	//게임 계속 진행
			}
			print(0);
		}
		else {
			//잘못된 명령어를 입력했을 경우
			print(0);
			printf("Invaild Command: Command does not exist\n");
		}
	}
	else {
		//지뢰를 파보는 명령(0)을 입력했을 경우
		SET_MARK(x, y, 0);
		if (IS_MINE(x, y)) {	//지뢰 밟았을 경우 패배 처리
			printf("You have lost\n");
			clear();
			print(0);
			return 1;	//패배
		}
		bfs(x, y);
		print(0);
	}
	if (visi == len * col - num) {	//남은 지뢰 갯수 검사
		//다 찾았을 경우 승리 처리
		printf("You win\n");
		clear();
		print(0);
		return 2;	//승리
	}
	return 0;	//게임 계속 진행
}

int main(int argc, char **argv) {
	if (argc > 3) {
		printf("Getting information from argument\n");
		len = atoi(argv[1]);
		col = atoi(argv[2]);
		num = atoi(argv[3]);
		if (argc > 4)
			seed = atoi(argv[4]);
		else seed = (int)time(NULL);
	}
	else {
		printf("Enter length, width, mineage, and seed(random seeds fill in -1)\n");
		scanf("%d %d %d %d", &len, &col, &num, &seed);
		if (seed < 0)
			seed = (int)time(NULL);
	}
	srand(seed);
	printf("Length: %d\n", len);
	printf("Column: %d\n", col);
	printf("Mines: %d\n", num);
	printf("Seed：%d\n", seed);
	m = (unsigned char *)malloc(len*col);
	memset(m, 0, len*col);
	print(0);
	for (; !input(););
}
