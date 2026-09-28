#ifndef SNAKEGAME_HPP
#define SNAKEGAME_HPP

#include <cstdint>

class game{
public:
    game(uint16_t, bool, bool);
    ~game();

    void start(uint16_t time);
    void reset();
    uint16_t get_score() const;
private:
    enum DIRECTION{
        UP = 0,
        LEFT = 1,
        DOWN = 2,
        RIGHT = 3,
        INVALID = 20
    };
    struct Cell{
        uint16_t val = 0;
        bool is_vertical = true;
        bool is_corner = false;
        bool has_apple = false;
        void reset(){
            val = 0;
            is_vertical = true;
            is_corner = false;
            has_apple = false;
        }
    };
    void drawstart() const;
    void draw() const;
    void draw_xy(int x, int y, char c) const;
    void println(const char*, uint16_t);
    void regress();
    void move(enum DIRECTION newdir);
    void move_wrapped(enum DIRECTION newdir);
    enum DIRECTION getdir();
    void place_apple();
    

private:
    Cell** board;
    uint16_t bsize;
    uint16_t score;
    uint16_t score_to_add;
    uint16_t headx;
    uint16_t heady;
    uint16_t prevheadx;
    uint16_t prevheady;
    enum DIRECTION direction;
    bool gameover = false;
    bool wrap;
    bool hidewalls;

    uint16_t appxdbg = 0;
    uint16_t appydbg = 0;
};

#endif
