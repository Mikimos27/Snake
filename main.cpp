#include "game.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>


int main(int argc, char** argv){
    std::string filename = "options.cfg";
    if(argc > 1){
        filename = argv[1];
    }
    uint16_t score = 0;
    uint16_t width = 32;
    uint16_t timems = 60;
    bool wrap = false;
    bool hidewalls = false;
    bool loopondeath = false;
    bool startpaused = false;
    std::ifstream file(filename);
    if(file){
        uint16_t out = 0;
        if(file >> out) width = out;
        if(file >> out) timems = out;
        if(file >> out) wrap = out;
        if(file >> out) hidewalls = out;
        if(file >> out) loopondeath = out;
        if(file >> out) startpaused = out;
    } else if(argc > 1) {
        file.close();
        std::ofstream makefile(filename);
        makefile << width << '\n';
        makefile << timems << '\n';
        makefile << wrap << '\n';
        makefile << hidewalls << '\n';
        makefile << loopondeath << '\n';
        makefile << startpaused << '\n';
    }

    {
        game g(width, wrap, hidewalls, startpaused, loopondeath);
        do{
            g.start(timems);
            int nscore = g.get_score();
            score = nscore > score ? nscore : score;
            if(loopondeath) g.reset();
            else break;
        }while(1);
    }
    std::cout << "Score: " << score << '\n';
    if(score >= width * width - 1) std::cout << "WIN\n";
    return 0;
}
