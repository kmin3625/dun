#include "display.h"
#include "io.h"

/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };                // 자원 상태 출력 위치
const POSITION map_pos = { 3, 2 };                     // 맵 출력 시작 위치
const POSITION system_message_pos = { MAP_HEIGHT + 3, 2 }; // 왼쪽 하단 시스템 메시지 위치
const POSITION object_info_pos = { 3, MAP_WIDTH + 12 };  // 오른쪽 상단 객체 정보 위치
const POSITION commands_pos = { MAP_HEIGHT + 3, MAP_WIDTH + 12 }; // 오른쪽 하단 명령어 위치

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// 자원 상태 출력
void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population = %d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max);
}

// 맵을 다른 버퍼로 전사하는 함수
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            for (int k = 0; k < N_LAYER; k++) {
                if (src[k][i][j] >= 0) {
                    dest[i][j] = src[k][i][j];
                }
            }
        }
    }
}

// 구조물마다 색상 설정
int get_object_color(char obj, int row, int col) {
    switch (obj) {
    case 'B':
        // 아트레이디스 본진 위치 (하단)
        if (row >= MAP_HEIGHT - 4 && col < MAP_WIDTH / 2) return 9; // 파란색
        // 하코넨 본진 위치 (상단)
        else return 12; // 빨간색
    case 'P': return 8;  // 장판 (연한 회색 예시)
    case 'H':
        // 하베스터 (진형에 따라 색상 결정)
        if (row >= MAP_HEIGHT - 4 && col < MAP_WIDTH / 2) return 9; // 아트레이디스: 파란색
        else return 12; // 하코넨: 빨간색
    case '5': return 6;  // 스파이스 (주황색 예시)
    case 'W': return 14; // 샌드웜 (보라색 예시)
    case 'R': return 7;  // Rock (회색 예시)
    default: return COLOR_DEFAULT;
    }
}

// 맵에 구조물 배치 및 출력
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    // 경계 # 출력
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                printc(padd(map_pos, (POSITION) { i, j }), '#', COLOR_DEFAULT);  // 경계 표시
            }
            else {
                frontbuf[i][j] = backbuf[i][j];  // 내부 맵 출력
            }
        }
    }

    // 진형 및 구조물 배치 (map[0] 레이어)
    map[0][MAP_HEIGHT - 2][1] = 'B';   // 아트레이디스 본진
    map[0][MAP_HEIGHT - 2][2] = 'B';
    map[0][MAP_HEIGHT - 3][1] = 'B';
    map[0][MAP_HEIGHT - 3][2] = 'B';
    map[0][MAP_HEIGHT - 2][3] = 'P';
    map[0][MAP_HEIGHT - 2][4] = 'P';
    map[0][MAP_HEIGHT - 3][3] = 'P';
    map[0][MAP_HEIGHT - 3][4] = 'P';
    map[0][MAP_HEIGHT - 4][1] = 'H';
    map[0][MAP_HEIGHT - 6][1] = '5';
    map[0][3][MAP_WIDTH / 8] = 'W';
    map[0][MAP_HEIGHT - 6][7] = 'R';

    map[0][1][MAP_WIDTH - 2] = 'B';   // 하코넨 본진
    map[0][1][MAP_WIDTH - 3] = 'B';
    map[0][2][MAP_WIDTH - 2] = 'B';
    map[0][2][MAP_WIDTH - 3] = 'B';
    map[0][1][MAP_WIDTH - 4] = 'P';
    map[0][1][MAP_WIDTH - 5] = 'P';
    map[0][2][MAP_WIDTH - 4] = 'P';
    map[0][2][MAP_WIDTH - 5] = 'P';
    map[0][3][MAP_WIDTH - 2] = 'H';
    map[0][5][MAP_WIDTH - 2] = '5';
    map[0][MAP_HEIGHT - 8][MAP_WIDTH - MAP_WIDTH / 3] = 'W';
    map[0][4][MAP_WIDTH - 7] = 'R';

    map[0][MAP_HEIGHT - 4][MAP_WIDTH - 9] = 'R';
    map[0][4][MAP_WIDTH / 4 - 1] = 'R';
    map[0][4][MAP_WIDTH / 4] = 'R';
    map[0][5][MAP_WIDTH / 4 - 1] = 'R';
    map[0][5][MAP_WIDTH / 4] = 'R';
    map[0][MAP_HEIGHT - 4][MAP_WIDTH - MAP_WIDTH / 2 - 1] = 'R';
    map[0][MAP_HEIGHT - 4][MAP_WIDTH - MAP_WIDTH / 2] = 'R';
    map[0][MAP_HEIGHT - 5][MAP_WIDTH - MAP_WIDTH / 2 - 1] = 'R';
    map[0][MAP_HEIGHT - 5][MAP_WIDTH - MAP_WIDTH / 2] = 'R';

    project(map, backbuf);  // 맵을 backbuf로 복사

    // 경계를 제외한 내부 영역 출력
    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            if (frontbuf[i][j] != backbuf[i][j]) {
                POSITION pos = { i, j };
                printc(padd(map_pos, pos), backbuf[i][j], get_object_color(backbuf[i][j], i, j));
            }
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// 커서 위치만 색상 변경 후 다시 출력
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    char ch = frontbuf[prev.row][prev.column];
    printc(padd(map_pos, prev), ch, get_object_color(ch, prev.row, prev.column));

    ch = frontbuf[curr.row][curr.column];
    printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_system_message(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_DEFAULT);
    gotoxy(system_message_pos);
    printf("System Message: No new messages\n");
}

void display_object_info(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_OBJECT_INFO);
    gotoxy(object_info_pos);
    printf("Object Info: Selected object details go here\n");
}

void display_commands(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_COMMANDS);
    gotoxy(commands_pos);
    printf("Commands: 'q' to quit, 'm' for map, etc.\n");
}

void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor
) {
    display_resource(resource);        // 자원 상태 출력
    display_map(map);                  // 맵 출력
    display_cursor(cursor);            // 커서 출력
    display_system_message(map);       // 시스템 메시지 출력
    display_object_info(map);          // 객체 정보 출력
    display_commands(map);             // 명령어 출력
}
