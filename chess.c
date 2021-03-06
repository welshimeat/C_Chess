#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#define _CRT_SECURE_NO_WARNINGS
enum name { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE };
int turn = 0;
int isEnpassantTurn[2][8]; // 행 인덱스는 앙파상을 할 수 있는 쪽, 열 인덱스는 앙파상을 실행해 이동하는 열
typedef struct piece
{
	_Bool isempty;//말이 있으면 0, 없으면 1.
	enum name n;
	int color;//백이면 0, 흑이면 1, 없으면 2
	int count;//이동한 횟수
	_Bool alive;//사망이면 0, 생존이면 1
	_Bool isMove;//갈 수 있으면 1, 없으면 0
}Piece;
Piece enemy;
int enemy_x;
int enemy_y;
int check(Piece* (*p)[8]);

void initialize_Move(Piece* (*p)[8], int x, int y) // 장애물을 확인하고 갈 수 있으면 isMove를 1로, 갈 수 없으면 0으로 바꾼다.
{
	int i, j, blockDR_X = 7, blockDR_Y = 7, blockUL_X = 0, blockUL_Y = 0, blockDL_X = 7, blockDL_Y = 7, blockUR_X = 0, blockUR_Y = 7, blockD = 7, blockU = 0, blockR = 7, blockL = 0;

	if (x == -1 && y == -1)
		return;

	else
	{
		switch (p[x][y]->n) // ★☆★☆ x-1, y-1로 수정해야 할수도
		{
		case 0: // PAWN
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++) {
					if (p[x][y]->color == 1) // 흑이면
					{
						if ((i == (x + 1)) && j == y && p[i][j]->isempty == 1)
							p[i][j]->isMove = 1;
						else if (i == (x + 2) && j == y && p[x][y]->count == 0 && p[i][j]->isempty == 1)
							p[i][j]->isMove = 1;
						else if (i == (x + 1) && ((j == (y + 1)) || (j == (y - 1))) && p[i][j]->color == 0)
							p[i][j]->isMove = 1;
						else if (isEnpassantTurn[1][j] == turn - 1 && turn > 1 && i == x + 1 && p[i][j]->color != 1) // 앙파상 가능한 턴이면
							p[i][j]->isMove = 1;
						else
							p[i][j]->isMove = 0;
					}
					else
					{
						if ((i == (x - 1)) && j == y && p[i][j]->isempty == 1)
							p[i][j]->isMove = 1;
						else if (i == (x - 2) && j == y && p[x][y]->count == 0 && p[i][j]->isempty == 1)
							p[i][j]->isMove = 1;
						else if (i == (x - 1) && ((j == (y + 1)) || (j == (y - 1))) && p[i][j]->color == 1)
							p[i][j]->isMove = 1;
						else if (isEnpassantTurn[0][j] == turn - 1 && turn > 1 && i == x - 1 && p[i][j]->color != 0) // 앙파상 가능한 턴이면
							p[i][j]->isMove = 1;
						else
							p[i][j]->isMove = 0;
					}
				}
			break;
		case 1: // KNIGHT
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
					if ((((abs(x - i) == 2) && abs(y - j) == 1) || ((abs(x - i) == 1) && abs(y - j) == 2)) && (p[i][j]->color != p[x][y]->color))
						p[i][j]->isMove = 1;
					else
						p[i][j]->isMove = 0;
			break;
		case 2: // BISHOP
			for (i = x + 1, j = y + 1; i <= 7 && j <= 7; i++, j++) //아래 오른쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockDR_X = i;
					blockDR_Y = j;
					break;
				}
			for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--) //위 왼쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockUL_X = i;
					blockUL_Y = j;
					break;
				}
			for (i = x + 1, j = y - 1; i <= 7 && j >= 0; i++, j--) //아래 왼쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockDL_X = i;
					blockDL_Y = j;
					break;
				}
			for (i = x - 1, j = y + 1; i >= 0 && j <= 7; i--, j++) //위 오른쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockUR_X = i;
					blockUR_Y = j;
					break;
				}

			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (i < blockUL_X && j < blockUL_Y) // 위 왼쪽 장애물
						p[i][j]->isMove = 0;
					else if (i<blockUR_X && j>blockUR_Y) // 위 오른쪽 장애물
						p[i][j]->isMove = 0;
					else if (i > blockDL_X && j < blockDL_Y) // 아래 왼쪽 장애물
						p[i][j]->isMove = 0;
					else if (i > blockDR_X && j > blockDR_Y) // 아래 오른쪽 장애물
						p[i][j]->isMove = 0;
					else if (p[i][j]->color == p[x][y]->color)
						p[i][j]->isMove = 0;
					else if (i == x || j == y)
						p[i][j]->isMove = 0;
					else if (abs(i - x) == abs(j - y))
						p[i][j]->isMove = 1;
					else
						p[i][j]->isMove = 0;
				}
			}

			break;
		case 3: // ROOK
			for (i = x + 1; i <= 7; i++) // 아래쪽 장애물
				if (p[i][y]->isempty == 0) {
					blockD = i;
					break;
				}
			for (i = x - 1; i >= 0; i--) // 위쪽 장애물
				if (p[i][y]->isempty == 0) {
					blockU = i;
					break;
				}
			for (i = y + 1; i <= 7; i++) // 오른쪽 장애물
				if (p[x][i]->isempty == 0) {
					blockR = i;
					break;
				}
			for (i = y - 1; i >= 0; i--) // 왼쪽 장애물
				if (p[x][i]->isempty == 0) {
					blockL = i;
					break;
				}

			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (j == y && ((i >= blockU) && (i <= blockD))) // 위와 아래 장애물이 있을 때
						p[i][j]->isMove = 1;
					else if (i == x && ((j >= blockL) && (j <= blockR))) // 왼쪽과 오른쪽 장애물이 있을 때
						p[i][j]->isMove = 1;
					else
						p[i][j]->isMove = 0;
				}
			}

			break;
		case 4: // QUEEN
			for (i = x + 1, j = y + 1; i <= 7 && j <= 7; i++, j++) //아래 오른쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockDR_X = i;
					blockDR_Y = j;
					break;
				}
			for (i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--) //위 왼쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockUL_X = i;
					blockUL_Y = j;
					break;
				}
			for (i = x + 1, j = y - 1; i <= 7 && j >= 0; i++, j--) //아래 왼쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockDL_X = i;
					blockDL_Y = j;
					break;
				}
			for (i = x - 1, j = y + 1; i >= 0 && j <= 7; i--, j++) //위 오른쪽 장애물
				if (p[i][j]->isempty == 0) {
					blockUR_X = i;
					blockUR_Y = j;
					break;
				}

			for (i = x + 1; i <= 7; i++) // 아래쪽 장애물
				if (p[i][y]->isempty == 0) {
					blockD = i;
					break;
				}
			for (i = x - 1; i >= 0; i--) // 위쪽 장애물
				if (p[i][y]->isempty == 0) {
					blockU = i;
					break;
				}
			for (i = y + 1; i <= 7; i++) // 오른쪽 장애물
				if (p[x][i]->isempty == 0) {
					blockR = i;
					break;
				}
			for (i = y - 1; i >= 0; i--) // 왼쪽 장애물
				if (p[x][i]->isempty == 0) {
					blockL = i;
					break;
				}

			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (j == y && ((i < blockU) || (i > blockD))) // 위와 아래 장애물이 있을 때
						p[i][j]->isMove = 0;
					else if (i == x && ((j < blockL) || (j > blockR))) // 왼쪽과 오른쪽 장애물이 있을 때
						p[i][j]->isMove = 0;
					else if (i < blockUL_X && j < blockUL_Y) // 위 왼쪽 장애물
						p[i][j]->isMove = 0;
					else if (i<blockUR_X && j>blockUR_Y) // 위 오른쪽 장애물
						p[i][j]->isMove = 0;
					else if (i > blockDL_X && j < blockDL_Y) // 아래 왼쪽 장애물
						p[i][j]->isMove = 0;
					else if (i > blockDR_X && j > blockDR_Y) // 아래 오른쪽 장애물
						p[i][j]->isMove = 0;
					else if (p[i][j]->color == p[x][y]->color)
						p[i][j]->isMove = 0;
					else if (i == x || j == y)
						p[i][j]->isMove = 1;
					else if (abs(i - x) == abs(j - y))
						p[i][j]->isMove = 1;
					else
						p[i][j]->isMove = 0;
				}
			}
			break;
		case 5: // KING
			break;
		case 6: // empty
			break;
		}
	}
}

void  Initialize_map(Piece* (*p)[8]) {//맵 초기화

	for (int i = 0; i < 8; i++) {//폰 초기화
			//흑색 폰 초기화
		p[1][i]->n = PAWN;
		p[1][i]->isempty = 0;
		p[1][i]->color = 1;
		p[1][i]->count = 0;
		p[1][i]->alive = 1;

		//백색 폰 초기화
		p[6][i]->n = PAWN;
		p[6][i]->isempty = 0;
		p[6][i]->color = 0;
		p[6][i]->count = 0;
		p[6][i]->alive = 1;

		//흑색 첫줄 초기화
		p[0][i]->isempty = 0;
		p[0][i]->color = 1;
		p[0][i]->count = 0;
		p[0][i]->alive = 1;

		//백색 두번째줄 초기화
		p[7][i]->isempty = 0;
		p[7][i]->color = 0;
		p[7][i]->count = 0;
		p[7][i]->alive = 1;
	}
	//빈 공간 초기화
	for (int i = 2; i < 6; i++)
		for (int j = 0; j < 8; j++) {
			p[i][j]->isempty = 1;
			p[i][j]->color = 3;
			p[i][j]->count = 0;
			p[i][j]->alive = 0;
			p[i][j]->n = NONE;
		}

	//룩 초기화
	p[0][0]->n = ROOK;
	p[0][7]->n = ROOK;
	p[7][0]->n = ROOK;
	p[7][7]->n = ROOK;
	//나이트 초기화
	p[0][1]->n = KNIGHT;
	p[0][6]->n = KNIGHT;
	p[7][1]->n = KNIGHT;
	p[7][6]->n = KNIGHT;
	//비숍 초기화
	p[0][2]->n = BISHOP;
	p[0][5]->n = BISHOP;
	p[7][2]->n = BISHOP;
	p[7][5]->n = BISHOP;
	//퀸 초기화
	p[0][3]->n = QUEEN;
	p[7][3]->n = QUEEN;
	//킹 초기화
	p[0][4]->n = KING;
	p[7][4]->n = KING;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
			p[i][j]->isMove = 0;
		}
}

void print_map(Piece* (*p)[8], int x, int y) {

	initialize_Move(p, x, y);
	printf("　　a   b   c   d   e   f   g   h  \n");
	for (int i = 0; i < 8; i++) {
		printf(" %d ", i + 1);
		for (int j = 0; j < 8; j++) {
			if (p[i][j]->isempty)
			{
				if (x == -1 && y == -1) // 기물을 선택하지 않았을 때
					printf(" -- "); //빈자리
				else
				{
					switch (p[x][y]->n) {
					case 0: // 폰
						if (p[x][y]->color == 1) // 흑이면
						{
							if ((i == (x + 1)) && j == y)
								printf(" @@ ");
							else if (i == (x + 2) && j == y)
							{
								if (p[x][y]->count == 0)
									printf(" @@ "); // 한 번도 안 움직였으면 2칸 움직일 수 있음
								else
									printf(" -- ");
							}
							else if (isEnpassantTurn[1][j] == turn - 1 && x == 4 && turn > 1 && i == x + 1)  // 앙파상 가능한 턴이면
							{
								printf(" @@ ");
							}

							else
								printf(" -- ");
							break;
						}
						else
						{
							if ((i == (x - 1)) && j == y)
								printf(" @@ ");
							else if (i == (x - 2) && j == y)
							{
								if (p[x][y]->count == 0)
									printf(" @@ "); // 한 번도 안 움직였으면 2칸 움직일 수 있음
								else
									printf(" -- ");
							}
							else if (isEnpassantTurn[0][j] == turn - 1 && x == 3 && turn > 1 && i == x - 1) // 앙파상 가능한 턴이면
							{
								printf(" @@ ");
							}
							else
								printf(" -- ");
							break;
						}

					case 1: // 나이트
						if (((abs(x - i) == 2) && abs(y - j) == 1) || ((abs(x - i) == 1) && abs(y - j) == 2))
							printf(" @@ ");
						else
							printf(" -- ");
						break;
					case 2: // 비숍
						if ((abs(x - i) == abs(y - j)) && p[i][j]->isMove == 1)
							printf(" @@ ");
						else
							printf(" -- ");
						break;
					case 3: // 룩
						if ((x == i || y == j) && p[i][j]->isMove == 1)
							printf(" @@ ");
						else
							printf(" -- ");
						break;
					case 4: // 퀸
						if (((abs(x - i) == abs(y - j)) || x == i || y == j) && p[i][j]->isMove == 1)
							printf(" @@ ");
						else
							printf(" -- ");
						break;
					case 5: // 킹
						if (abs(x - i) <= 1 && abs(y - j) <= 1)
							printf(" @@ ");
						else
							printf(" -- ");
						break;
					}
				}
			}

			else {
				if (p[i][j]->color) {//흑색
					switch (p[i][j]->n) {
					case 0:
						printf(" ● ");
						break;
					case 1:
						printf(" ◆ ");
						break;
					case 2:
						printf(" ▲ ");
						break;
					case 3:
						printf(" ■ ");
						break;
					case 4:
						printf(" ▼ ");
						break;
					case 5:
						printf(" ★ ");
						break;
					}
				}

				else {//백색
					switch (p[i][j]->n) {
					case 0:
						printf(" ○ ");
						break;
					case 1:
						printf(" ◇ ");
						break;
					case 2:
						printf(" △ ");
						break;
					case 3:
						printf(" □ ");
						break;
					case 4:
						printf(" ▽ ");
						break;
					case 5:
						printf(" ☆ ");
						break;

					}
				}

			}
		}
		printf("\n");
	}
}

//폰 승급 함수
void Pawnlomotion(Piece* (*p)[8], int x, int y)
{
	enum name nombre;

	printf("폰 승급 가능! 1:나이트 2:비숍 3:룩 4:퀸\n");
	while (1)
	{
		scanf("%d", &nombre);

		if (nombre < 1 || nombre > 4)
			printf("잘못된 값을 입력했습니다.\n");
		else
			break;
	}

	p[x][y]->n = nombre;
}

//이동할 때 상태 변경 함수
void statuemove(Piece* (*p)[8], int x, int y, int toX, int toY) {
	p[toX - 1][toY - 1]->isempty = 0;
	p[toX - 1][toY - 1]->n = p[x][y]->n;
	p[toX - 1][toY - 1]->color = p[x][y]->color;
	p[toX - 1][toY - 1]->alive = p[x][y]->alive;
	p[toX - 1][toY - 1]->count = p[x][y]->count + 1;
	p[x][y]->isempty = 1;
	p[x][y]->n = NONE;
	p[x][y]->color = 2;
	p[x][y]->alive = 0;
	p[x][y]->count = 0;
	turn++;
}


//폰 이동 함수
void pawn_move(Piece* (*p)[8], int x, int y) {
	Piece temp;
	int toX, toY;
	char tmp;

	printf("이동할 칸 좌표:");
	scanf(" %c %d", &tmp, &toX);

	toY = (int)(tmp - 'a' + 1);

	if (p[x][y]->color == p[toX - 1][toY - 1]->color) { //같은 편 먹기 불가
		printf("불가능한 이동입니다.\n");
		return;
	}

	temp = *p[toX - 1][toY - 1]; //이동시 킹이 체크 될 경우 이동 불가
	statuemove(p, x, y, toX, toY);
	p[toX - 1][toY - 1]->count--;
	turn--;
	if (check(p) == 0) {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
	}
	else {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
		printf("불가능한 이동입니다.\n");
		return;
	}


	if (p[x][y]->color == 0) { //선택한 말이 백일때
		if (p[toX - 1][toY - 1]->isempty == 1) { //가려는 곳에 말이 없을 때
				//앙파상
			if (isEnpassantTurn[0][toY - 1] == turn - 1) // 바로 전에 상대방이 가려는 열의 폰을 2칸 움직였을 때
			{
				if (x == 3 && p[toX][toY - 1]->n == 0) // 서로 폰일 때
				{
					{
						p[toX][toY - 1]->isempty = 1;
						p[toX][toY - 1]->n = NONE;
						p[toX][toY - 1]->color = 2;
						p[toX][toY - 1]->alive = 0;
						p[toX][toY - 1]->count = 0;
						statuemove(p, x, y, toX, toY);
						return;
					}

				}
			}

			if (p[x][y]->count == 0)         //폰이 한번도 안 움직였을 때
				if ((x - (toX - 1) == 2 && (y - (toY - 1)) == 0) || (x - (toX - 1) == 1 && (y - (toY - 1)) == 0)) {         //세로로 2칸까지 이동 가능
					if ((x - (toX - 1) == 2 && (y - (toY - 1)) == 0))         // 폰이 2칸 움직였으면 바로 다음 턴에 앙파상이 가능
						isEnpassantTurn[1][y] = turn;         // 흑이 y좌표로 앙파상이 가능

					statuemove(p, x, y, toX, toY);
				}
				else printf("불가능한 이동입니다.\n");
			else {         //폰이 1번 이상 움직였을 때
				if ((x - (toX - 1) == 1 && (y - (toY - 1)) == 0)) {         //1칸까지 이동 가능
					statuemove(p, x, y, toX, toY);
				}
				else printf("불가능한 이동입니다.\n");
			}
		}
		else { //가려는 곳에 상대가 있을 때
			if ((x - (toX - 1)) == 1 && abs(y - (toY - 1)) == 1) { //대각선으로 한 칸씩만 이동 가능
				statuemove(p, x, y, toX, toY);
			}
			else printf("불가능한 이동입니다.\n");
		}
	}
	else { //선택한 말이 흑일때
		if (p[toX - 1][toY - 1]->isempty == 1) { //가려는 곳에 말이 없을 때
				//앙파상
			if (isEnpassantTurn[1][toY - 1] == turn - 1) // 바로 전에 상대방이 가려는 열의 폰을 2칸 움직였을 때
			{
				if (x == 4 && p[toX - 2][toY - 1]->n == 0) // 서로 폰일 때
				{
					{
						p[toX - 2][toY - 1]->isempty = 1;
						p[toX - 2][toY - 1]->n = NONE;
						p[toX - 2][toY - 1]->color = 2;
						p[toX - 2][toY - 1]->alive = 0;
						p[toX - 2][toY - 1]->count = 0;
						statuemove(p, x, y, toX, toY);
						return;
					}
				}
			}
			if (p[x][y]->count == 0) //폰이 한번도 안 움직였을 때
				if ((x - (toX - 1) == -2 && (y - (toY - 1)) == 0) || (x - (toX - 1) == -1 && (y - (toY - 1)) == 0)) { //세로로 2칸까지 이동 가능
					if ((x - (toX - 1) == -2 && (y - (toY - 1)) == 0)) // 폰이 2칸 움직였으면 바로 다음 턴에 앙파상이 가능
						isEnpassantTurn[0][y] = turn; // 백이 y좌표로 앙파상이 가능

					statuemove(p, x, y, toX, toY);
				}
				else printf("불가능한 이동입니다.\n");
			else { //폰이 1번 이상 움직였을 때
				if ((x - (toX - 1) == -1 && (y - (toY - 1)) == 0)) { //1칸까지 이동 가능
					statuemove(p, x, y, toX, toY);
				}
				else printf("불가능한 이동입니다.\n");
			}
		}
		else { // 가려는 곳에 상대가 있을 때
			if ((x - (toX - 1)) == -1 && abs(y - (toY - 1)) == 1) { //대각선으로 한 칸씩만 이동 가능
				statuemove(p, x, y, toX, toY);
			}
			else printf("불가능한 이동입니다.\n");
		}
	}

	// 폰이 끝까지 가면 승급
	if (toX == 1 || toX == 8)
		if (p[toX - 1][toY - 1]->n == PAWN)
			Pawnlomotion(p, toX - 1, toY - 1);
}


void knight_move(Piece* (*p)[8], int x, int y) {
	int toX, toY;
	char tmp;
	Piece temp;
	printf("이동할 칸 좌표:");
	scanf(" %c %d", &tmp, &toX);

	toY = (int)(tmp - 'a' + 1);

	temp = *p[toX - 1][toY - 1]; //이동시 킹이 체크 될 경우 이동 불가
	statuemove(p, x, y, toX, toY);
	p[toX - 1][toY - 1]->count--;
	turn--;
	if (check(p) == 0) {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
	}
	else {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
		printf("불가능한 이동입니다.\n");
		return;
	}

	if ((toX - 1) == x && (toY - 1) == y) printf("현재 위치입니다.\n"); //현재 위치 불가능
	else if (p[x][y]->color == p[toX - 1][toY - 1]->color) printf("불가능한 이동입니다.\n"); //같은 편 먹기 불가
	else if (((abs(x - (toX - 1)) == 1) && (abs(y - (toY - 1)) == 2)) || ((abs(x - (toX - 1)) == 2) && (abs(y - (toY - 1)) == 1))) {
		statuemove(p, x, y, toX, toY);
	}
	else
		printf("불가능한 이동입니다.\n");
}



void bishop_move(Piece* (*p)[8], int x, int y) {
	int toX, toY;
	char tmp;
	Piece temp;
	printf("이동할 칸 좌표:");
	scanf(" %c %d", &tmp, &toX);

	toY = (int)(tmp - 'a' + 1);

	temp = *p[toX - 1][toY - 1]; //이동시 킹이 체크 될 경우 이동 불가
	statuemove(p, x, y, toX, toY);
	p[toX - 1][toY - 1]->count--;
	turn--;
	if (check(p) == 0) {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
	}
	else {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
		printf("불가능한 이동입니다.\n");
		return;
	}

	if ((toX - 1) == x && (toY - 1) == y) printf("현재 위치입니다.\n");
	else if (p[x][y]->color == p[toX - 1][toY - 1]->color) printf("불가능한 이동입니다.\n");
	else if (abs(x - (toX - 1)) != abs(y - (toY - 1))) printf("불가능한 이동입니다.\n");
	else if (p[toX - 1][toY - 1]->isMove == 0) printf("불가능한 이동입니다.\n");
	else
		statuemove(p, x, y, toX, toY);
}


void rook_move(Piece* (*p)[8], int x, int y) {
	int toX, toY;
	char tmp;
	Piece temp;

	printf("이동할 칸 좌표:");
	scanf(" %c %d", &tmp, &toX);

	toY = (int)(tmp - 'a' + 1);

	temp = *p[toX - 1][toY - 1]; //이동시 킹이 체크 될 경우 이동 불가
	statuemove(p, x, y, toX, toY);
	p[toX - 1][toY - 1]->count--;
	turn--;
	if (check(p) == 0) {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
	}
	else {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
		printf("불가능한 이동입니다.\n");
		return;
	}

	if ((toX - 1) == x && (toY - 1) == y) printf("현재 위치입니다.\n");
	else if (p[x][y]->color == p[toX - 1][toY - 1]->color) printf("불가능한 이동입니다.\n");
	else if ((abs(x - (toX - 1)) > 0 && abs(y - (toY - 1)) > 0)) printf("불가능한 이동입니다.\n");
	else if (p[toX - 1][toY - 1]->isMove == 0) printf("불가능한 이동입니다.\n");
	else
		statuemove(p, x, y, toX, toY);
}


void queen_move(Piece* (*p)[8], int x, int y) {
	int toX, toY;
	char tmp;
	Piece temp;

	printf("이동할 칸 좌표:");
	scanf(" %c %d", &tmp, &toX);

	toY = (int)(tmp - 'a' + 1);

	temp = *p[toX - 1][toY - 1]; //이동시 킹이 체크 될 경우 이동 불가
	statuemove(p, x, y, toX, toY);
	p[toX - 1][toY - 1]->count--;
	turn--;
	if (check(p) == 0) {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
	}
	else {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
		printf("불가능한 이동입니다.\n");
		return;
	}

	if ((toX - 1) == x && (toY - 1) == y) printf("현재 위치입니다.\n");
	else if (p[x][y]->color == p[toX - 1][toY - 1]->color) printf("불가능한 이동입니다.\n");
	else if (p[toX - 1][toY - 1]->isMove == 0) printf("불가능한 이동입니다.");
	else if ((abs(x - (toX - 1)) == abs(y - (toY - 1))) || (abs(x - (toX - 1)) > 0 && abs(y - (toY - 1)) == 0) || (abs(x - (toX - 1)) == 0 && abs(y - (toY - 1)) > 0))
		statuemove(p, x, y, toX, toY);
	else
		printf("불가능한 이동입니다.\n");
}


void king_move(Piece* (*p)[8], int x, int y) {
	int toX, toY;
	char tmp;
	Piece temp;

	printf("이동할 칸 좌표:");
	scanf(" %c %d", &tmp, &toX);

	toY = (int)(tmp - 'a' + 1);

	temp = *p[toX - 1][toY - 1];
	statuemove(p, x, y, toX, toY);
	p[toX - 1][toY - 1]->count--;
	turn--;
	if (check(p) == 0) {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
	}
	else {
		statuemove(p, toX - 1, toY - 1, x + 1, y + 1);
		p[x][y]->count--;
		turn--;
		*p[toX - 1][toY - 1] = temp;
		printf("불가능한 이동입니다.\n");
		return;
	}

	if ((toX - 1) == x && (toY - 1) == y) printf("현재 위치입니다.\n");
	else if (p[x][y]->color == p[toX - 1][toY - 1]->color) printf("불가능한 이동입니다.\n");
	else if (abs(x - (toX - 1)) > 1 || abs(y - (toY - 1)) > 1) printf("불가능한 이동입니다.\n");
	else
		statuemove(p, x, y, toX, toY);
}

//체크인지 확인하는 함수
int check(Piece* (*p)[8]) {
	int King_x = 7, King_y = 4, Knight_num = 0, Knight1_x, Knight1_y, Knight2_x = 0, Knight2_y = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (p[i][j]->n == KING && p[i][j]->color == turn % 2) { //킹의 위치 확인
				King_x = i;
				King_y = j;
			}

			else if (p[i][j]->n == KNIGHT && p[i][j]->color == (turn + 1) % 2) { //나이트의 위치 확인
				Knight_num++;
				Knight1_x = i;
				Knight1_y = j;
			}

	if (Knight_num == 2) //나이트가 2개일 경우 두번째 나이트 위치 확인
		for (int i = Knight1_x - 1; i >= 0; i--)
			for (int j = Knight1_y - 1; j >= 0; j--)
				if (p[i][j]->n == KNIGHT && p[i][j]->color == (turn + 1) % 2) {
					Knight2_x = i;
					Knight2_y = j;
				}

	for (int i = King_x + 1, j = King_y + 1; i <= 7 && j <= 7; i++, j++)
		if (p[i][j]->isempty == 0) { //아래 오른쪽 장애물에 상대 퀸 혹은 비숏 확인
			if ((p[i][j]->color == (turn + 1) % 2) && ((p[i][j]->n == QUEEN) || (p[i][j]->n == BISHOP))) {
				enemy = *p[i][j];
				enemy_x = i;
				enemy_y = j;
				return 1;
			}
			else
				break;
		}
	for (int i = King_x - 1, j = King_y - 1; i >= 0 && j >= 0; i--, j--) //위 왼쪽 장애물에 상대 퀸 혹은 비숏 확인
		if (p[i][j]->isempty == 0) {
			if ((p[i][j]->color == (turn + 1) % 2) && ((p[i][j]->n == QUEEN) || (p[i][j]->n == BISHOP))) {
				enemy = *p[i][j];
				enemy_x = i;
				enemy_y = j;
				return 1;
			}
			else
				break;
		}
	for (int i = King_x + 1, j = King_y - 1; i <= 7 && j >= 0; i++, j--) //아래 왼쪽 장애물에 상대 퀸 혹은 비숏 확인
		if (p[i][j]->isempty == 0) {
			if ((p[i][j]->color == (turn + 1) % 2) && ((p[i][j]->n == QUEEN) || (p[i][j]->n == BISHOP))) {
				enemy = *p[i][j];
				enemy_x = i;
				enemy_y = j;
				return 1;
			}
			else
				break;
		}
	for (int i = King_x - 1, j = King_y + 1; i >= 0 && j <= 7; i--, j++) //위 오른쪽 장애물에 상대 퀸 혹은 비숏 확인
		if (p[i][j]->isempty == 0) {
			if ((p[i][j]->color == (turn + 1) % 2) && ((p[i][j]->n == QUEEN) || (p[i][j]->n == BISHOP))) {
				enemy = *p[i][j];
				enemy_x = i;
				enemy_y = j;
				return 1;
			}
			else
				break;
		}
	for (int i = King_x + 1; i <= 7; i++) // 아래쪽 장애물에 상대 퀸 혹은 룩 확인
		if (p[i][King_y]->isempty == 0) {
			if ((p[i][King_y]->color == (turn + 1) % 2) && ((p[i][King_y]->n == QUEEN) || (p[i][King_y]->n == ROOK))) {
				enemy = *p[i][King_y];
				enemy_x = i;
				enemy_y = King_y;
				return 1;
			}
			else
				break;
		}
	for (int i = King_x - 1; i >= 0; i--) // 위쪽 장애물에 상대 퀸 혹은 룩 확인
		if (p[i][King_y]->isempty == 0) {
			if ((p[i][King_y]->color == (turn + 1) % 2) && ((p[i][King_y]->n == QUEEN) || (p[i][King_y]->n == ROOK))) {
				enemy = *p[i][King_y];
				enemy_x = i;
				enemy_y = King_y;
				return 1;
			}
			else
				break;
		}
	for (int i = King_y + 1; i <= 7; i++) // 오른쪽 장애물에 상대 퀸 혹은 룩 확인
		if (p[King_x][i]->isempty == 0) {
			if ((p[King_x][i]->color == (turn + 1) % 2) && ((p[King_x][i]->n == QUEEN) || (p[King_x][i]->n == ROOK))) {
				enemy = *p[King_x][i];
				enemy_x = i;
				enemy_y = King_y;
				return 1;
			}
			else
				break;
		}
	for (int i = King_y - 1; i >= 0; i--) // 왼쪽 장애물에 상대 퀸 혹은 룩 확인
		if (p[King_x][i]->isempty == 0) {
			if ((p[King_x][i]->color == (turn + 1) % 2) && ((p[King_x][i]->n == QUEEN) || (p[King_x][i]->n == ROOK))) {
				enemy = *p[King_x][i];
				enemy_x = King_x;
				enemy_y = i;
				return 1;
			}
			else
				break;
		}
	if (Knight_num == 2) { //상대 나이트가 2개일때 위치 확인 후 체크
		if (((abs(King_x - Knight1_x) == 1) && (abs(King_y - Knight1_y) == 2)) || ((abs(King_x - Knight1_x) == 2) && (abs(King_y - Knight1_y) == 1))) {
			enemy = *p[Knight1_x][Knight1_y];
			enemy_x = Knight1_x;
			enemy_y = Knight1_y;
			return 1;
		}
		else if (((abs(King_x - Knight2_x) == 1) && (abs(King_y - Knight2_y) == 2)) || ((abs(King_x - Knight2_x) == 2) && (abs(King_y - Knight2_y) == 1))) {
			enemy = *p[Knight2_x][Knight2_y];
			enemy_x = Knight2_x;
			enemy_y = Knight2_y;
			return 1;
		}
	}
	else if (Knight_num == 1) { //상대 나이트가 1개일때 위치 확인 후 체크
		if (((abs(King_x - Knight1_x) == 1) && (abs(King_y - Knight1_y) == 2)) || ((abs(King_x - Knight1_x) == 2) && (abs(King_y - Knight1_y) == 1))) {
			enemy = *p[Knight1_x][Knight1_y];
			enemy_x = Knight1_x;
			enemy_y = Knight1_y;
			return 1;
		}
	}

	if (turn % 2 == 0) { //백색의 턴일때 킹과 폰 위치 비교 후 체크
		if ((King_x > 0) && (King_y > 0) && (King_y < 7)) {
			if ((p[King_x - 1][King_y - 1]->n == PAWN) && (p[King_x - 1][King_y - 1]->color == 1)) {
				enemy = *p[King_x - 1][King_y - 1];
				enemy_x = King_x - 1;
				enemy_y = King_y - 1;
				return 1;
			}
			else if ((p[King_x - 1][King_y + 1]->n == PAWN) && (p[King_x - 1][King_y + 1]->color == 1)) {
				enemy = *p[King_x - 1][King_y + 1];
				enemy_x = King_x - 1;
				enemy_y = King_y + 1;
				return 1;
			}
		}
		else if (King_x > 0 && King_y == 0) {
			if ((p[King_x - 1][King_y + 1]->n == PAWN) && (p[King_x - 1][King_y + 1]->color == 1)) {
				enemy = *p[King_x - 1][King_y + 1];
				enemy_x = King_x - 1;
				enemy_y = King_y + 1;
				return 1;
			}
		}
		else if (King_x > 0 && King_y == 7)
			if ((p[King_x - 1][King_y - 1]->n == PAWN) && (p[King_x - 1][King_y - 1]->color == 1)) {
				enemy = *p[King_x - 1][King_y - 1];
				enemy_x = King_x - 1;
				enemy_y = King_y - 1;
				return 1;
			}
	}
	else if ((turn + 1) % 2 == 0) { ////흑색의 턴일때 킹과 폰 위치 비교 후 체크
		if ((King_x < 7) && (King_y > 0) && (King_y < 7)) {
			if ((p[King_x + 1][King_y - 1]->n == PAWN) && (p[King_x + 1][King_y - 1]->color == 0)) {
				enemy = *p[King_x + 1][King_y - 1];
				enemy_x = King_x + 1;
				enemy_y = King_y - 1;
				return 1;
			}
			else if ((p[King_x + 1][King_y + 1]->n == PAWN) && (p[King_x + 1][King_y + 1]->color == 0)) {
				enemy = *p[King_x + 1][King_y + 1];
				enemy_x = King_x + 1;
				enemy_y = King_y + 1;
				return 1;
			}
		}
		else if (King_x < 7 && King_y == 0) {
			if ((p[King_x + 1][King_y + 1]->n == PAWN) && (p[King_x + 1][King_y + 1]->color == 0)) {
				enemy = *p[King_x + 1][King_y + 1];
				enemy_x = King_x + 1;
				enemy_y = King_y + 1;
				return 1;
			}
		}
		else if (King_x < 7 && King_y == 7)
			if ((p[King_x + 1][King_y - 1]->n == PAWN) && (p[King_x + 1][King_y - 1]->color == 0)) {
				enemy = *p[King_x + 1][King_y - 1];
				enemy_x = King_x + 1;
				enemy_y = King_y - 1;
				return 1;
			}
	}

	return 0;
}

int checkmate(Piece* (*p)[8]) {
	int King_x, King_y, R_enemy_x, R_enemy_y;
	Piece temp, R_enemy;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (p[i][j]->n == KING && p[i][j]->color == turn % 2) { //킹의 위치 확인
				King_x = i;
				King_y = j;
			}
	R_enemy_x = enemy_x;
	R_enemy_y = enemy_y;
	R_enemy = enemy;
	if (King_x > 0 && King_x < 7 && King_y > 0 && King_y < 7) {
		for (int i = King_x - 1; i <= King_x + 1; i++)
			for (int j = King_y - 1; j <= King_y + 1; j++) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x == 0 && King_y > 0 && King_y < 7) {
		for (int i = King_x; i <= King_x + 1; i++)
			for (int j = King_y - 1; j <= King_y + 1; j++) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x == 7 && King_y > 0 && King_y < 7) {
		for (int i = King_x; i >= King_x - 1; i--)
			for (int j = King_y - 1; j <= King_y + 1; j++) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x > 0 && King_x < 7 && King_y == 0) {
		for (int i = King_x - 1; i <= King_x + 1; i++)
			for (int j = King_y; j <= King_y + 1; j++) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x > 0 && King_x < 7 && King_y == 7) {
		for (int i = King_x - 1; i <= King_x + 1; i++)
			for (int j = King_y; j >= King_y - 1; j--) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x == 0 && King_y == 0) {
		for (int i = King_x; i <= King_x + 1; i++)
			for (int j = King_y; j <= King_y + 1; j++) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x == 7 && King_y == 7) {
		for (int i = King_x; i >= King_x - 1; i--)
			for (int j = King_y; j >= King_y - 1; j--) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x == 0 && King_y == 7) {
		for (int i = King_x; i <= King_x + 1; i++)
			for (int j = King_y; j >= King_y - 1; j--) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}

	else if (King_x == 7 && King_y == 0) {
		for (int i = King_x; i >= King_x - 1; i--)
			for (int j = King_y; j <= King_y + 1; j++) {
				if ((p[King_x][King_y]->color != p[i][j]->color) && (i != King_x || j != King_y)) {
					temp = *p[i][j];
					statuemove(p, King_x, King_y, i + 1, j + 1);
					turn--;
          p[i][j]->count--;
					if (check(p) == 0) {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
						return 0;
					}
					else {
						statuemove(p, i, j, King_x + 1, King_y + 1);
						turn--;
            p[King_x][King_y]->count--;
						*p[i][j] = temp;
					}
				}
			}
	}
	check(p);

	switch (enemy.n) {
	case 0:

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				if (p[i][j]->color == turn % 2 && p[i][j]->n != 5) {
					initialize_Move(p, i, j);
					if (p[enemy_x][enemy_y]->isMove == 1) {
						temp = *p[enemy_x][enemy_y];
						statuemove(p, i, j, enemy_x + 1, enemy_y + 1);
						turn--;
            p[enemy_x][enemy_y]->count--;
						if (check(p) == 0) {
							enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
							statuemove(p, enemy_x, enemy_y, i + 1, j + 1);
							turn--;
							p[i][j]->count--;
							*p[enemy_x][enemy_y] = temp;
							return 0;
						}
						else {
							enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
							statuemove(p, enemy_x, enemy_y, i + 1, j + 1);
							turn--;
							p[i][j]->count--;
							*p[enemy_x][enemy_y] = temp;
						}
					}
				}
			}
		return 1;
	case 1:
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				if (p[i][j]->color == turn % 2 && p[i][j]->n != 5) {
					initialize_Move(p, i, j);
					if (p[enemy_x][enemy_y]->isMove == 1) {
						temp = *p[enemy_x][enemy_y];
						statuemove(p, i, j, enemy_x + 1, enemy_y + 1);
						turn--;
						p[enemy_x][enemy_y]->count--;
						if (check(p) == 0) {
							enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
							statuemove(p, enemy_x, enemy_y, i + 1, j + 1);
							turn--;
							p[i][j]->count--;
							*p[enemy_x][enemy_y] = temp;
							return 0;
						}
						else {
							enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
							statuemove(p, enemy_x, enemy_y, i + 1, j + 1);
							turn--;
							p[i][j]->count--;
							*p[enemy_x][enemy_y] = temp;
						}
					}
				}
			}
		return 1;
	case 2:
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				if (p[i][j]->color == turn % 2 && p[i][j]->n != 5) {
					initialize_Move(p, i, j);
					if (King_x > enemy_x && King_y > enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x + k][enemy_y + k]->isMove == 1) {
								temp = *p[enemy_x + k][enemy_y + k];
								statuemove(p, i, j, enemy_x + k + 1, enemy_y + k + 1);
								turn--;
								p[enemy_x+k][enemy_y+k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y + k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y + k] = temp;
								}
							}
						}
					}
					else if (King_x > enemy_x && King_y < enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x + k][enemy_y - k]->isMove == 1) {
								temp = *p[enemy_x + k][enemy_y - k];
								statuemove(p, i, j, enemy_x + k + 1, enemy_y - k + 1);
								turn--;
								p[enemy_x+k][enemy_y-k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y - k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y - k] = temp;
								}
							}
						}
					}
					else if (King_x < enemy_x && King_y > enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x - k][enemy_y + k]->isMove == 1) {
								temp = *p[enemy_x - k][enemy_y + k];
								statuemove(p, i, j, enemy_x - k + 1, enemy_y + k + 1);
								turn--;
								p[enemy_x-k][enemy_y+k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y + k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y + k] = temp;
								}
							}
						}
					}
					else if (King_x < enemy_x && King_y < enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x - k][enemy_y - k]->isMove == 1) {
								temp = *p[enemy_x - k][enemy_y - k];
								statuemove(p, i, j, enemy_x - k + 1, enemy_y - k + 1);
								turn--;
								p[enemy_x-k][enemy_y-k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y - k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y - k] = temp;
								}
							}
						}
					}
				}
		return 1;
	case 3:
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				if (p[i][j]->color == turn % 2 && p[i][j]->n != 5) {
					initialize_Move(p, i, j);
					if (King_x > enemy_x && King_y == enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x + k][enemy_y]->isMove == 1) {
								temp = *p[enemy_x + k][enemy_y];
								statuemove(p, i, j, enemy_x + k + 1, enemy_y + 1);
								turn--;
								p[enemy_x+k][enemy_y]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y] = temp;
								}
							}
						}
					}
					else if (King_x < enemy_x && King_y == enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x - k][enemy_y]->isMove == 1) {
								temp = *p[enemy_x - k][enemy_y];
								statuemove(p, i, j, enemy_x - k + 1, enemy_y + 1);
								turn--;
								p[enemy_x-k][enemy_y]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y] = temp;
								}
							}
						}
					}
					else if (King_x == enemy_x && King_y > enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x][enemy_y + k]->isMove == 1) {
								temp = *p[enemy_x][enemy_y + k];
								statuemove(p, i, j, enemy_x + 1, enemy_y + k + 1);
								turn--;
								p[enemy_x][enemy_y+k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x][enemy_y + k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x][enemy_y + k] = temp;
								}
							}
						}
					}
					else if (King_x == enemy_x && King_y < enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x][enemy_y - k]->isMove == 1) {
								temp = *p[enemy_x][enemy_y - k];
								statuemove(p, i, j, enemy_x + 1, enemy_y - k + 1);
								turn--;
								p[enemy_x][enemy_y-k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x][enemy_y - k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x][enemy_y - k] = temp;
								}
							}
						}
					}
				}
		return 1;
	case 4:
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				if (p[i][j]->color == turn % 2 && p[i][j]->n != 5) {
					initialize_Move(p, i, j);
					if (King_x > enemy_x && King_y > enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x + k][enemy_y + k]->isMove == 1) {
								temp = *p[enemy_x + k][enemy_y + k];
								statuemove(p, i, j, enemy_x + k + 1, enemy_y + k + 1);
								turn--;
								p[enemy_x+k][enemy_y+k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y + k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y + k] = temp;
								}
							}
						}
					}
					else if (King_x > enemy_x && King_y < enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x + k][enemy_y - k]->isMove == 1) {
								temp = *p[enemy_x + k][enemy_y - k];
								statuemove(p, i, j, enemy_x + k + 1, enemy_y - k + 1);
								turn--;
								p[enemy_x+k][enemy_y-k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y - k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x + k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x + k][enemy_y - k] = temp;
								}
							}
						}
					}
					else if (King_x < enemy_x && King_y > enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x - k][enemy_y + k]->isMove == 1) {
								temp = *p[enemy_x - k][enemy_y + k];
								statuemove(p, i, j, enemy_x - k + 1, enemy_y + k + 1);
								turn--;
								p[enemy_x-k][enemy_y+k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y + k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y + k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y + k] = temp;
								}
							}
						}
					}
					else if (King_x < enemy_x && King_y < enemy_y) {
						for (int k = 0; k < abs(King_x - enemy_x); k++) {
							if (p[enemy_x - k][enemy_y - k]->isMove == 1) {
								temp = *p[enemy_x - k][enemy_y - k];
								statuemove(p, i, j, enemy_x - k + 1, enemy_y - k + 1);
								turn--;
								p[enemy_x-k][enemy_y-k]->count--;
								if (check(p) == 0) {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y - k] = temp;
									return 0;
								}
								else {
									enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
									statuemove(p, enemy_x - k, enemy_y - k, i + 1, j + 1);
									turn--;
									p[i][j]->count--;
									*p[enemy_x - k][enemy_y - k] = temp;
								}
							}
							else if (King_x > enemy_x && King_y == enemy_y) {
								for (int k = 0; k < abs(King_x - enemy_x); k++) {
									if (p[enemy_x + k][enemy_y]->isMove == 1) {
										temp = *p[enemy_x + k][enemy_y];
										statuemove(p, i, j, enemy_x + k + 1, enemy_y + 1);
										turn--;
										p[enemy_x+k][enemy_y]->count--;
										if (check(p) == 0) {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x + k, enemy_y, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x + k][enemy_y] = temp;
											return 0;
										}
										else {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x + k, enemy_y, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x + k][enemy_y] = temp;
										}
									}
								}
							}
							else if (King_x < enemy_x && King_y == enemy_y) {
								for (int k = 0; k < abs(King_x - enemy_x); k++) {
									if (p[enemy_x - k][enemy_y]->isMove == 1) {
										temp = *p[enemy_x - k][enemy_y];
										statuemove(p, i, j, enemy_x - k + 1, enemy_y + 1);
										turn--;
										p[enemy_x-k][enemy_y]->count--;
										if (check(p) == 0) {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x - k, enemy_y, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x - k][enemy_y] = temp;
											return 0;
										}
										else {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x - k, enemy_y, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x - k][enemy_y] = temp;
										}
									}
								}
							}
							else if (King_x == enemy_x && King_y > enemy_y) {
								for (int k = 0; k < abs(King_x - enemy_x); k++) {
									if (p[enemy_x][enemy_y + k]->isMove == 1) {
										temp = *p[enemy_x][enemy_y + k];
										statuemove(p, i, j, enemy_x + 1, enemy_y + k + 1);
										turn--;
										p[enemy_x][enemy_y+k]->count--;
										if (check(p) == 0) {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x, enemy_y + k, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x][enemy_y + k] = temp;
											return 0;
										}
										else {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x, enemy_y + k, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x][enemy_y + k] = temp;
										}
									}
								}
							}
							else if (King_x == enemy_x && King_y < enemy_y) {
								for (int k = 0; k < abs(King_x - enemy_x); k++) {
									if (p[enemy_x][enemy_y - k]->isMove == 1) {
										temp = *p[enemy_x][enemy_y - k];
										statuemove(p, i, j, enemy_x + 1, enemy_y - k + 1);
										turn--;
										p[enemy_x][enemy_y-k]->count--;
										if (check(p) == 0) {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x, enemy_y - k, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x][enemy_y - k] = temp;
											return 0;
										}
										else {
											enemy_x = R_enemy_x; enemy_y = R_enemy_y; enemy = R_enemy;
											statuemove(p, enemy_x, enemy_y - k, i + 1, j + 1);
											turn--;
											p[i][j]->count--;
											*p[enemy_x][enemy_y - k] = temp;
										}
									}
								}
							}
						}
					}
				}
		return 1;
	}
}

void saveGame(Piece* (*p)[8]) {//게임 저장

	FILE* fp;
	char path[20];

	printf("이름을 입력하세요: ");
	scanf("%s", path);
	strcat(path, ".bin");

	fp = fopen(path, "wb");
	if (fp == NULL) {
		printf("파일을 열 수 없습니다.\n");
		return;
	}

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			fwrite(p[i][j], sizeof(*p[i][j]), 1, fp);
	fclose(fp);

	printf("저장이 완료되었습니다.\n");

}

void LoadGame(Piece* (*p)[8]) {//게임 불러오기

	FILE* fp;
	int lineNum = 0, bytes;
	char path[20], buffer[35];

	printf("이름을 입력하세요: ");
	scanf("%s", path);
	strcat(path, ".bin");

	fp = fopen(path, "rb");
	if (fp == NULL) {
		printf("저장된 게임이 없습니다.새로운 게임을 시작합니다.\n");
		Sleep(3000);
		return;
	}

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) fread(p[i][j], sizeof(*p[i][j]), 1, fp);
}

void clearBuffer(void) {//입력 버퍼 비우기
	while (getchar() != '\n');
}

void castling(Piece* (*p)[8]) {
	int King_x, King_y, Rook1_x, Rook1_y, Rook2_x, Rook2_y, Rooknum = 0, num;
	Piece temp;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (p[i][j]->n == KING && p[i][j]->color == turn % 2) { //킹의 위치 확인
				King_x = i;
				King_y = j;
			}

			if (p[i][j]->n == ROOK && p[i][j]->color == turn % 2) { //킹의 위치 확인
				if (Rooknum == 0) {
					Rook1_x = i;
					Rook1_y = j;
					Rooknum++;
				}
				else {
					Rook2_x = i;
					Rook2_y = j;
				}
			}
		}
	}

	if (p[King_x][King_y]->count != 0) {
		printf("캐슬링 불가\n");
		return;
	}

	else {

		printf("1.킹 사이드 캐슬링 2. 퀸 사이드 캐슬링");
		scanf("%d", &num);


		if (num == 1) {

			if (p[Rook2_x][Rook2_y]->count != 0) {
				printf("룩 이동함. 캐슬링 불가\n");
				return;
			}

			else {
				for (int i = King_y + 1; i < Rook2_y; i++) {
					if (p[King_x][i]->isempty == 0) {
						printf("장애물. 캐슬링 불가\n");
						return;
					}
				}

				temp = *p[King_x][King_y + 2]; //이동시 킹이 체크 될 경우 이동 불가
				statuemove(p, King_x, King_y, King_x + 1, King_y + 3);
				p[King_x][King_y + 2]->count--;
				turn--;
				if (check(p) == 0) {
					statuemove(p, Rook2_x, Rook2_y, King_x + 1, King_y + 2);
					p[King_x][King_y + 2]->count++;
					*p[King_x][King_y] = temp;
				}
				else {
					statuemove(p, King_x, King_y + 2, King_x + 1, King_y + 1);
					p[King_x][King_y]->count--;
					turn--;
					*p[King_x][King_y + 2] = temp;
					printf("이동시 체크. 캐슬링 불가\n");
					return;
				}


			}
		}

		if (num == 2) {
			if (p[Rook1_x][Rook1_y]->count != 0) {
				printf("캐슬링 불가\n");
				return;
			}
			else {

				for (int i = Rook1_y + 1; i < King_y; i++) {
					if (p[King_x][i]->isempty == 0) {
						printf("장애물. 캐슬링 불가\n");
						return;
					}
				}

				temp = *p[King_x][King_y - 2]; //이동시 킹이 체크 될 경우 이동 불가
				statuemove(p, King_x, King_y, King_x + 1, King_y - 1);
				p[King_x][King_y - 2]->count--;
				turn--;
				if (check(p) == 0) {
					statuemove(p, Rook1_x, Rook1_y, King_x + 1, King_y);
					p[King_x][King_y - 2]->count++;
					*p[King_x][King_y] = temp;
				}
				else {
					statuemove(p, King_x, King_y - 2, King_x + 1, King_y + 1);
					p[King_x][King_y]->count--;
					turn--;
					*p[King_x][King_y - 2] = temp;
					printf("이동시 체크. 캐슬링 불가\n");
					return;
				}

			}
		}

	}

	system("cls");
	print_map(p, -1, -1);

}


void move(Piece* (*p)[8]) {
	int fromX, fromY, save;
	char tmp;

	while (1) {
		printf("1.이동  2.게임 저장 3.캐슬링 4.종료 ");
		if (scanf("%d", &save) != 1) clearBuffer();
		if (save == 1) break;
		if (save == 2) saveGame(p);
		if (save == 3) castling(p);
		if (save == 4) exit(0);
	}

	while (1)
	{
		printf("이동할 기물 좌표(입력 방식 : a 1) : ");
		scanf(" %c %d", &tmp, &fromX);

		fromY = (int)(tmp - 'a' + 1);

		if (fromX < 1 || fromX>8 || fromY < 1 || fromY>8) printf("맵을 벗어난 좌표입니다.\n"); // 맵을 벗어나면
		else
		{
			if (p[fromX - 1][fromY - 1]->isempty == 1) printf("이동할 수 있는 기물이 없습니다..\n"); // 그 좌표에 말이 없으면 다시 입력받음
			else if (turn % 2 == 0 && p[fromX - 1][fromY - 1]->color == 1) printf("백색의 턴입니다.\n");
			else if (turn % 2 == 1 && p[fromX - 1][fromY - 1]->color == 0) printf("흑색의 턴입니다.\n");
			else
				break;
		}
	}

	system("cls");
	print_map(p, fromX - 1, fromY - 1);

	switch (p[fromX - 1][fromY - 1]->n) {
	case 0:
		pawn_move(p, fromX - 1, fromY - 1);
		break;
	case 1:
		knight_move(p, fromX - 1, fromY - 1);
		break;
	case 2:
		bishop_move(p, fromX - 1, fromY - 1);
		break;
	case 3:
		rook_move(p, fromX - 1, fromY - 1);
		break;
	case 4:
		queen_move(p, fromX - 1, fromY - 1);
		break;
	case 5:
		king_move(p, fromX - 1, fromY - 1);
		break;
	}


}


void startGame(Piece* (*p)[8]) {//게임 시작 메뉴
	int menuNum;
	while (1) {
		printf("1. 새 게임  2. 이어하기 3. 종료\n");
		if (!scanf("%d", &menuNum)) clearBuffer();
		if (menuNum == 1) break;
		else if (menuNum == 2) {
			LoadGame(p);
			break;
		}
		else if (menuNum == 3) exit(0);
	}
}



int main() {

	Piece* map[8][8];

	for (int i = 0; i < 8; i++) { //맵 배열에 메모리 할당{
		for (int j = 0; j < 8; j++) {
			map[i][j] = (Piece*)malloc(sizeof(Piece));
			map[i][j]->isempty = 1;
		}
	}

	Initialize_map(map);
	startGame(map);
	system("cls");

	while (1)
	{
		print_map(map, -1, -1); // 움직일 기물을 선택 안했을 때의 맵 출력
		if (check(map) == 1) {
			if (checkmate(map) == 1) {
				printf("체크메이트\n");
				exit(0);
			}
			else
				printf("체크\n");
		}

		move(map);
		printf("\n");
		system("cls");
	}

	for (int i = 0; i < 8; i++) { //맵 배열 메모리 할당 해제
		for (int j = 0; j < 8; j++) {
			free(map[i][j]);
		}
	}

	return 0;
}
