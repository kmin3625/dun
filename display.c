#include "display.h"
#include "io.h"

/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };                // �ڿ� ���� ��� ��ġ
const POSITION map_pos = { 3, 2 };                     // �� ��� ���� ��ġ
const POSITION system_message_pos = { MAP_HEIGHT + 3, 2 }; // ���� �ϴ� �ý��� �޽��� ��ġ
const POSITION object_info_pos = { 3, MAP_WIDTH + 12 };  // ������ ��� ��ü ���� ��ġ
const POSITION commands_pos = { MAP_HEIGHT + 3, MAP_WIDTH + 12 }; // ������ �ϴ� ��ɾ� ��ġ

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// �ڿ� ���� ���
void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population = %d/%d\n",
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

// ���������� ���� ����
int get_object_color(char obj, int row, int col) {
    switch (obj) {
    case 'B':
        // ��Ʈ���̵� ���� ��ġ (�ϴ�)
        if (row >= MAP_HEIGHT - 4 && col < MAP_WIDTH / 2) return 9; // �Ķ���
        // ���ڳ� ���� ��ġ (���)
        else return 12; // ������
    case 'P': return 8;  // ���� (���� ȸ�� ����)
    case 'H':
        // �Ϻ����� (������ ���� ���� ����)
        if (row >= MAP_HEIGHT - 4 && col < MAP_WIDTH / 2) return 9; // ��Ʈ���̵�: �Ķ���
        else return 12; // ���ڳ�: ������
    case '5': return 6;  // �����̽� (��Ȳ�� ����)
    case 'W': return 14; // ����� (����� ����)
    case 'R': return 7;  // Rock (ȸ�� ����)
    default: return COLOR_DEFAULT;
    }
}

// �ʿ� ������ ��ġ �� ���
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    // ��� # ���
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                printc(padd(map_pos, (POSITION) { i, j }), '#', COLOR_DEFAULT);  // ��� ǥ��
            }
            else {
                frontbuf[i][j] = backbuf[i][j];  // ���� �� ���
            }
        }
    }

    // ���� �� ������ ��ġ (map[0] ���̾�)
    map[0][MAP_HEIGHT - 2][1] = 'B';   // ��Ʈ���̵� ����
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

    map[0][1][MAP_WIDTH - 2] = 'B';   // ���ڳ� ����
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

    project(map, backbuf);  // ���� backbuf�� ����

    // ��踦 ������ ���� ���� ���
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

// Ŀ�� ��ġ�� ���� ���� �� �ٽ� ���
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
    display_resource(resource);        // �ڿ� ���� ���
    display_map(map);                  // �� ���
    display_cursor(cursor);            // Ŀ�� ���
    display_system_message(map);       // �ý��� �޽��� ���
    display_object_info(map);          // ��ü ���� ���
    display_commands(map);             // ��ɾ� ���
}
