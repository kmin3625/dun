#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };                // �ڿ� ���� ��� ��ġ
const POSITION map_pos = { 3, 2 };                      // �� ��� ���� ��ġ
const POSITION system_message_pos = { MAP_HEIGHT + 3, 2 }; // ���� �ϴ� �ý��� �޽��� ��ġ
const POSITION object_info_pos = { 3, MAP_WIDTH + 12 };  // ������ ��� ��ü ���� ��ġ
const POSITION commands_pos = { MAP_HEIGHT + 3, MAP_WIDTH + 12 }; // ������ �ϴ� ��ɾ� ��ġ

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
    display_resource(resource);        // �ڿ� ���� ���
    display_map(map);                  // �� ���
    display_cursor(cursor);            // Ŀ�� ���
    display_system_message(map);       // �ý��� �޽��� ���
    display_object_info(map);          // ��ü ���� ���
    display_commands(map);             // ��ɾ� ���
}

void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max);
}

// ���� �ٸ� ���۷� �����ϴ� �Լ�
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

    // �� ��踦 #�� ���
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                // �� ���� #���� ǥ��
                printc(padd(map_pos, (POSITION) { i, j }), '#', COLOR_DEFAULT);
            }
            else if (frontbuf[i][j] != backbuf[i][j]) {
                // ��踦 ������ ���� ���� ���
                POSITION pos = { i, j };
                printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
            }
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// Ŀ�� ��ġ�� ���� ���� �� �ٽ� ���
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
    // �ý��� �޽��� ���
}

void display_object_info(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_OBJECT_INFO);
    gotoxy(object_info_pos);
    printf("Object Info: Selected object details go here\n");
    // ��ü ���� ���
}

void display_commands(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    set_color(COLOR_COMMANDS);
    gotoxy(commands_pos);
    printf("Commands: 'q' to quit, 'm' for map, etc.\n");
    // ��ɾ� ���
}