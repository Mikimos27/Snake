#include "game.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>


int main(void){
    uint16_t score = 0;
    uint16_t width = 32;
    uint16_t timems = 180;
    bool wrap = false;
    bool hidewalls = false;
    std::ifstream file("options.ini");
    if(file){
        uint16_t out = 0;
        bool outb = false;
        if(file >> out) width = out;
        if(file >> out) timems = out;
        if(file >> outb) wrap = outb;
        if(file >> outb) hidewalls = outb;
    }

    {
        game g(width, wrap, hidewalls);
        g.start(timems);
        score = g.get_score();
    }
    std::cout << "Score: " << score << '\n';
    if(score == width * width - 1) std::cout << "WIN\n";
    return 0;
}
