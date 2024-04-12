// ldd: -lncurses
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#define ROWS 4
#define COLS 4
int board[ROWS][COLS];
int score = 0;

typedef enum {
    up = KEY_UP,
    down = KEY_DOWN,
    left = KEY_LEFT,
    right = KEY_RIGHT,
    quit = KEY_BACKSPACE,
} key;

void INIT_2048() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    srand(time(NULL));
}

void DONE_2048() {
    endwin();
    exit(0);
}


void init_board() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = 0;
        }
    }
}

void display_board() {
    move(0, 0);
    printw("== 2048 =============================\n========================= score:%5i\n", score);
    for (int i = 0; i < ROWS; i++) {
        printw("+---------+---------+---------+---------+\n");
        for (int l = 0; l < 3; l++) {
            if (l == 1) {
                for (int j = 0; j < COLS; j++) {
                    if (board[i][j] > 0) printw("|%5i    ", board[i][j]);
                    else printw("|         ");
                }
                printw("|\n");
            } else printw("|         |         |         |         |\n");
        }
    }
    printw("+---------+---------+---------+---------+\n");
    refresh();
}

int count_empty() {
    int count = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == 0) count++;
        }
    }
    return count;
}

void add_two(int empty) {
    int n = rand() % empty;
    int k = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == 0) {
                if (k == n) {
                    board[i][j] = 2;
                    return;
                }
                k++;
            }
        }
    }
}

int game_over(int add) {
    int n = count_empty();
    if (n == 0) {
        printw("============= GAME OVER =============\n");
        printw("=========== (press a key) ===========");
        getch();
        return 1;
    } else if (add) {
        add_two(n);
        display_board();
    }
    return 0;
}

int shift_board() {
    int hasChanged = 0;
    int pos;
    for (int i = 0; i < ROWS; i++) {
        pos = 0;
        for (int j = 0; j < COLS; j++) {
            if (board[i][j]) {
                if (j > pos) {
                    board[i][pos] = board[i][j];
                    board[i][j] = 0;
                    hasChanged = 1;
                }
                pos++;
            }
        }
    }
    return hasChanged;
}

int add_adjacents() {
    int hasChanged = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS - 1; j++) {
            if (board[i][j] && board[i][j] == board[i][j + 1]) {
                board[i][j + 1] *= 2;
                score += board[i][j + 1];
                board[i][j] = 0;
                j++; // inutile de regarder la prochaine case
                hasChanged = 1;
            }
        }
    }
    return hasChanged;
}

int update_board() {
    int hasChanged = 0;
    hasChanged += shift_board();
    hasChanged += add_adjacents();
    hasChanged += shift_board();
    return hasChanged > 0;
}

key get_key(){
    int key;
    do {
        key = getch();
    }
    while(key != KEY_UP && key != KEY_DOWN && key != KEY_LEFT && key != KEY_RIGHT && key != KEY_BACKSPACE);
    return key;
}

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void mirror_board() {
    for (int i = 0; i < ROWS; i++) {
        int left = 0;
        int right = 3;
        while (left < right) {
            swap(&board[i][left], &board[i][right]);
            left++;
            right--;
        }
    }
}

void pivot_board() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = i + 1; j < COLS; j++) {
            swap(&board[i][j], &board[j][i]);
        }
    }
}

int play(key dir) {
    int hasChanged = 0;
    switch (dir) {
        case up:
            mirror_board();
            pivot_board();
            hasChanged += update_board();
            pivot_board();
            mirror_board();
            break;
        case down:
            pivot_board();
            mirror_board();
            hasChanged += update_board();
            mirror_board();
            pivot_board();
            break;
        case left:
            // Pas besoin de rotation pour la gauche
            hasChanged += update_board();
            break;
        case right:
            mirror_board();
            hasChanged += update_board();
            mirror_board();
            break;
    }
    return hasChanged;
}

int main() {
    INIT_2048();
    init_board();
    int add = 1;
    while (!game_over(add)) {
        key k = get_key();
        if(k == quit) break;
        if(play(k)){
            display_board();
        }
    }
    DONE_2048();
}