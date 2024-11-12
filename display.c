#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };                // 자원 상태 출력 위치
const POSITION map_pos = { 3, 2 };                      // 맵 출력 시작 위치
const POSITION system_message_pos = { MAP_HEIGHT + 3, 2 }; // 왼쪽 하단 시스템 메시지 위치
const POSITION object_info_pos = { 3, MAP_WIDTH + 12 };  // 오른쪽 상단 객체 정보 위치
const POSITION commands_pos = { MAP_HEIGHT + 3, MAP_WIDTH + 12 }; // 오른쪽 하단 명령어 위치

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_system_message(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_object_info(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_commands(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);

void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor)
{
    display_resource(resource);        // 자원 상태 출력
    display_map(map);                  // 맵 출력
    display_cursor(cursor);            // 커서 출력
    display_system_message(map);       // 시스템 메시지 출력
    display_object_info(map);          // 객체 정보 출력
    display_commands(map);             // 명령어 출력
}

void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
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

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);

    // 맵 경계를 #로 출력
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                // 맵 경계는 #으로 표시
                printc(padd(map_pos, (POSITION) { i, j }), '#', COLOR_DEFAULT);
            }
            else if (frontbuf[i][j] != backbuf[i][j]) {
                // 경계를 제외한 내부 영역 출력
                POSITION pos = { i, j };
                printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
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
    printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

    ch = frontbuf[curr.row][curr.column];
    printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_system_message(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_DEFAULT);
    gotoxy(system_message_pos);
    printf("System Message: No new messages\n");
    // 시스템 메시지 출력
}

void display_object_info(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_OBJECT_INFO);
    gotoxy(object_info_pos);
    printf("Object Info: Selected object details go here\n");
    // 객체 정보 출력
}

void display_commands(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_COMMANDS);
    gotoxy(commands_pos);
    printf("Commands: 'q' to quit, 'm' for map, etc.\n");
    // 명령어 출력
}