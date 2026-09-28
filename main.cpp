#include "game.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>


int main(int argc, char** argv){
    std::string filename = "options.ini";
    if(argc > 1){
        filename = argv[1];
    }
    uint16_t score = 0;
    uint16_t width = 16;
    uint16_t timems = 180;
    bool wrap = false;
    bool hidewalls = false;
    bool loopondeath = false;
    std::ifstream file(filename);
    if(file){
        uint16_t out = 0;
        if(file >> out) width = out;
        if(file >> out) timems = out;
        if(file >> out) wrap = out;
        if(file >> out) hidewalls = out;
        if(file >> out) loopondeath = out;
    } else{
        file.close();
        std::ofstream makefile(filename);
        makefile << width << '\n';
        makefile << timems << '\n';
        makefile << wrap << '\n';
        makefile << hidewalls << '\n';
        makefile << loopondeath << '\n';
    }

    {
        game g(width, wrap, hidewalls);
        do{
            g.start(timems);
            if(loopondeath){
                score = g.get_score();
                if(score == width * width - 1){
                    std::cout << "WIN\n";
                    return 0;
                }
                g.reset();
            }else break;
        }while(1);
    }
    std::cout << "Score: " << score << '\n';
    if(score == width * width - 1) std::cout << "WIN\n";
    return 0;
}
