#include "game.hpp"
#include <ncurses.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#define printdbg(A) printw("%s = %u\n", #A, A)

template<typename T>
T abs(T a) { return a >= 0 ? a : -a; }

void init_getch(){
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

game::game(uint16_t n)
    : board(new Cell*[n]), bsize(n), score(0), score_to_add(0), headx(n / 2), heady(n / 2), direction(DOWN) {
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

void game::start(uint16_t time){
    std::srand(std::time(NULL));
    place_apple();
    for(;;){
        move(getdir());
        regress();
        //printdbg(headx);
        //printdbg(heady);
        //printdbg(direction);
        //printdbg(appxdbg);
        //printdbg(appydbg);
        //printdbg(board[appydbg][appxdbg].has_apple);
        printdbg(score);
        draw();
        //if(headx == appxdbg && heady == appydbg) pause = true;
        //printdbg(pause);
        //while(pause);
        if(gameover){
            return;
        }
        napms(time);
    }
}

uint16_t game::get_score() const{
    return score;
}

void game::draw() const{
    //printw("\033[2J\033[1;1H");
    for(int16_t i = -1; i <= bsize; i++){
        for(int16_t j = -1; j <= bsize; j++){
            char c = ' ';
            if(i == -1 || i == bsize || j == -1 || j == bsize){ c = 'W'; }
            else if(j == headx && i == heady){ c = 'H'; }
            else if(board[i][j].has_apple) c = 'A';
            else switch(board[i][j].val){
                case 0:
                    c = ' ';
                    break;
                default:
                    if(board[i][j].is_vertical)
                        c = '|';
                    else c = '=';
            }
            printw("%c%c", c, c);
        }
        printw("\n");
    }
    refresh();
    clear();
}

void game::regress(){
    if(score_to_add > 0){
        score++;
        score_to_add--;
        return;
    }
    for(uint8_t i = 0; i < bsize; i++){
        for(uint8_t j = 0; j < bsize; j++){
            if(board[i][j].val <= 0) continue;
            if(i == heady && j == headx) continue;
            board[i][j].val--;
        }
    }
}

void game::move(enum DIRECTION newdir){
    if(abs((int)direction - (int)newdir) != 2) direction = newdir;
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
    if(board[heady][headx].has_apple) {
        score_to_add++;
        board[heady][headx].val = score + 2;
        board[heady][headx].is_vertical = will_be_vertical;
        board[heady][headx].has_apple = false;
        place_apple();
    }
    else {
        board[heady][headx].val = score + 1;
        board[heady][headx].is_vertical = will_be_vertical;
    }
}

enum game::DIRECTION game::getdir(){
    auto c = getch();
    flushinp();
    switch(c){
        case KEY_UP:
            return UP;
        case KEY_DOWN:
            return DOWN;
        case KEY_LEFT:
            return LEFT;
        case KEY_RIGHT:
            return RIGHT;
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
                return;
            }
            if(board[i][j].val == 0) pos--;
        }
    }
}
