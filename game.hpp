#ifndef SNAKEGAME_HPP
#define SNAKEGAME_HPP

#include <cstdint>

class game{
public:
    game(uint16_t);
    ~game();

    void start(uint16_t time);
    uint16_t get_score() const;
private:
    enum DIRECTION{
        UP = 0,
        LEFT = 1,
        DOWN = 2,
        RIGHT = 3,
        INVALID = 20
    };
    void draw() const;
    void regress();
    void move(enum DIRECTION newdir);
    enum DIRECTION getdir();
    void place_apple();
    

private:
    int16_t** board;
    uint16_t bsize;
    uint16_t score;
    uint16_t score_to_add;
    uint16_t headx;
    uint16_t heady;
    enum DIRECTION direction;
    bool gameover = 0;

    uint16_t appxdbg = 0;
    uint16_t appydbg = 0;
};

#endif
