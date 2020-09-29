#ifndef TABULEIRO_HPP
#define TABULEIRO_HPP

#include <list>

#include "knights.hpp"
#include "queens.hpp"

class Tabuleiro{
    public:
        Tabuleiro(unsigned short int n);
    private:
        unsigned short int tamanho;
        int* array_mapa;
        std::list<Knights*> cavaleiros;
        std::list<Queens*> rainhas;
};

#endif