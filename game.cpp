#include "game.hpp"
#include <ncurses.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#define printdbg(A) printw("%s = %u\n", #A, A)
#define COLOR_WALL COLOR_PAIR(1)
#define COLOR_APPLE COLOR_PAIR(2)
#define COLOR_SNAKEHEAD COLOR_PAIR(3)
#define COLOR_SNAKETAIL COLOR_PAIR(4)
#define COLOR_EMPTY COLOR_PAIR(5)
#define EMPTY_CHAR ' '

template<typename T>
T abs(T a) { return a >= 0 ? a : -a; }

void init_getch(){
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_CYAN, COLOR_CYAN);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
}

game::game(uint16_t n, bool wrap, bool hidewalls, bool paused, bool& doloop)
    : board(new Cell*[n]), bsize(n), score(0), score_to_add(0), headx(n / 2), heady(n / 2), prevheadx(headx), prevheady(heady), direction(DOWN), wrap(wrap), hidewalls(hidewalls), paused(paused), doloop(doloop) {
    std::srand(std::time(NULL));
    if(!board) exit(1);
    for(uint8_t i = 0; i < n; i++){
        board[i] = new Cell[n];
        if(!board[i]) exit(1);
    }
    init_getch();
}
game::~game(){
    for(uint8_t i = 0; i < bsize; i++){
        delete[] board[i];
    }
    delete[] board;
    endwin();
}

void game::println(const char* str, uint16_t val){
    mvaddch(bsize + 2, 0, ' ');
    printw(str, val);
}

void game::start(uint16_t time){
    drawstart();
    this->doloop = doloop;
    place_apple();
    for(;;){
        if(!wrap) move(getdir());
        else move_wrapped(getdir());
        regress();
        //printdbg(score);
        println("Score: %u\n", score);
        //println("Headx: %u\n", headx);
        draw();
        if(gameover) return;
        napms(time);
        while(paused){
            char c = getch();
            if(c == 'p' || c == 'P') paused = false;
        }
    }
}

void game::reset(){
    score = 0;
    score_to_add = 0;
    gameover = false;
    headx = bsize / 2;
    heady = bsize / 2;
    for(uint16_t i = 0; i < bsize; i++)
        for(uint16_t j = 0; j < bsize; j++)
            board[i][j].reset();
}

uint16_t game::get_score() const{
    return score;
}
void game::drawstart() const{
    clear();
    if(!hidewalls)
        for(int16_t i = -1; i <= bsize; i++){
            for(int16_t j = -1; j <= bsize; j++){
                if(j == -1 || j == bsize || i == -1 || i == bsize) {
                    attron(COLOR_WALL);
                    printw("  ");
                    attroff(COLOR_WALL);
                }
                else {
                    attron(COLOR_EMPTY);
                    printw("%c%c", EMPTY_CHAR, EMPTY_CHAR);
                    attroff(COLOR_EMPTY);
                }
            }
            printw("\n");
        }
    //draw_xy(headx, heady, 'H');
    printw("\n");
    refresh();
}

void game::draw_xy(int x, int y, unsigned int c) const {
    mvaddch(y + 1, 2 * x + 2, c);
    mvaddch(y + 1, 2 * x + 3, c);
}

void game::draw() const{
    draw_xy(headx, heady, ' ' | COLOR_SNAKEHEAD);
    auto& ccell = board[prevheady][prevheadx];
    char c = EMPTY_CHAR;
    auto color = COLOR_SNAKETAIL;
    if(ccell.val > 0){
        c = '=';
        if(ccell.is_corner) c = ':';
        else if (ccell.is_vertical) c = '|';
    }
    else color = COLOR_EMPTY;
    draw_xy(prevheadx, prevheady, c | color);
    refresh();
}

void game::regress(){
    if(score_to_add > 0){
        score++;
        score_to_add--;
        return;
    }
    for(uint8_t i = 0; i < bsize; i++){
        for(uint8_t j = 0; j < bsize; j++){
            if(board[i][j].has_apple) continue;
            if(i == heady && j == headx) continue;
            if(board[i][j].val == 1) {
                draw_xy(j, i, EMPTY_CHAR | COLOR_EMPTY);
                board[i][j].val = 0;
                continue;
            }
            if(board[i][j].val > 0) board[i][j].val--;
        }
    }
}

void game::move(enum DIRECTION newdir){
    bool changeddir = false;
    if(abs((int)direction - (int)newdir) != 2 && direction != newdir){
        direction = newdir;
        changeddir = true;
    }
    switch(direction){
        case UP:
            if(heady == 0 || board[heady - 1][headx].val > 0) {
                gameover = 1; return;
            }
            break;
        case LEFT:
            if(headx == 0 || board[heady][headx - 1].val > 0) {
                gameover = 1; return;
            }
            break;
        case DOWN:
            if(heady == bsize - 1 || board[heady + 1][headx].val > 0){
                gameover = 1;
                return;
            }
            break;
        case RIGHT:
            if(headx == bsize - 1 || board[heady][headx + 1].val > 0){
                gameover = 1;
                return;
            }
            break;
        default:
            break;
    }

    prevheadx = headx;
    prevheady = heady;

    board[heady][headx].is_corner = changeddir;
    bool will_be_vertical = true;
    switch(direction){
        case UP:
            heady--;
            break;
        case LEFT:
            headx--;
            will_be_vertical = false;
            break;
        case DOWN:
            heady++;
            break;
        case RIGHT:
            headx++;
            will_be_vertical = false;
            break;
        default:
            break;
    }
    board[heady][headx].is_vertical = will_be_vertical;
    
    if(board[heady][headx].has_apple) {
        score_to_add++;
        board[heady][headx].val = score + 2;
        board[heady][headx].has_apple = false;
        place_apple();
    }
    else {
        board[heady][headx].val = score + 1;
    }
}

void game::move_wrapped(enum DIRECTION newdir){
    bool changeddir = false;
    if(abs((int)direction - (int)newdir) != 2 && direction != newdir){
        direction = newdir;
        changeddir = true;
    }
    switch(direction){
        case UP:
            if(board[(heady - 1 + bsize) % bsize][headx].val > 0) {
                gameover = 1; return;
            }
            break;
        case LEFT:
            if(board[heady][(headx - 1 + bsize) % bsize].val > 0) {
                gameover = 1; return;
            }
            break;
        case DOWN:
            if(board[(heady + 1) % bsize][headx].val > 0){
                gameover = 1;
                return;
            }
            break;
        case RIGHT:
            if(board[heady][(headx + 1) % bsize].val > 0){
                gameover = 1;
                return;
            }
            break;
        default:
            break;
    }

    prevheadx = headx;
    prevheady = heady;

    board[heady][headx].is_corner = changeddir;
    bool will_be_vertical = true;
    switch(direction){
        case UP:
            if(heady == 0) heady += bsize;
            heady--;
            break;
        case LEFT:
            if(headx == 0) headx += bsize;
            headx--;
            will_be_vertical = false;
            break;
        case DOWN:
            heady++;
            if(heady == bsize) heady = 0;
            break;
        case RIGHT:
            headx++;
            if(headx == bsize) headx = 0;
            will_be_vertical = false;
            break;
        default:
            break;
    }
    board[heady][headx].is_vertical = will_be_vertical;
    if(board[heady][headx].has_apple) {
        score_to_add++;
        board[heady][headx].val = score + 2;
        board[heady][headx].has_apple = false;
        place_apple();
    }
    else {
        board[heady][headx].val = score + 1;
    }
}

enum game::DIRECTION game::getdir(){
    auto c = getch();
    if(c == ERR) return direction;
    switch(c){
        case KEY_UP:
        case 'w':
        case 'W':
            return UP;
        case KEY_DOWN:
        case 's':
        case 'S':
            return DOWN;
        case KEY_LEFT:
        case 'a':
        case 'A':
            return LEFT;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            return RIGHT;
        case '[':
            return (enum DIRECTION)((direction + 1) % 4);
        case ']':
            return (enum DIRECTION)((direction + 3) % 4);
        case 'p':
        case 'P':
            paused = true;
            break;
        case 'q':
        case 'Q':
            doloop = false;
            gameover = true;
            break;

    }
    return direction;
}


void game::place_apple(){
    uint16_t available = bsize * bsize - score - 2;
    if(available == 0){
        gameover = 1;
        return;
    }
    uint16_t pos = std::rand() % available;
    for(uint16_t i = 0; i < bsize; i++){
        for(uint16_t j = 0; j < bsize; j++){
            if(pos == 0 && board[i][j].val == 0) {
                if(headx == j && heady == i) continue;
                board[i][j].has_apple = true;
                appxdbg = j;
                appydbg = i;
                draw_xy(appxdbg, appydbg, ' ' | COLOR_APPLE);
                return;
            }
            if(board[i][j].val == 0) pos--;
        }
    }
}
