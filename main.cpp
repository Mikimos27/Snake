#include "game.hpp"
#include <iostream>
#include <cstdint>


int main(void){
    uint16_t score = 0;
    {
        game g(30);
        g.start(100);
        score = g.get_score();
    }
    std::cout << "Score: " << score << '\n';
    return 0;
}
