#ifndef Living_cells_hpp
#define Living_cells_hpp

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdio.h>
#include "Gamerules.hpp"

class Living_cells
{
    public:
        // Constructors
        Living_cells();
        Living_cells(int worksheet);
        Living_cells(std::vector<std::pair<int,int>> init);
        Living_cells(std::vector<std::pair<int,int>> init,
            bool Automatic,
            bool Borderless,
            bool diag_count,
            int neighbor_to_die_l,
            int neighbor_to_die_h,
            int neighbor_to_born
            );

        // Destructor
        ~Living_cells();

        // Methods
        int count_neighbors(std::pair<int,int> pos);
        std::vector<std::pair<int,int>> potential_new_cells();
        void evolve();
        std::vector<std::pair<int,int>> get_cells_to_frontend();
        int get_generation();
        void set_list_living(std::vector<std::pair<int,int>> pos);
        void set_living_from_keyboard(char letter);

    protected:
        int m_generation;
        std::vector<std::pair<int,int>> m_living;
        std::vector<std::pair<int,int>> m_next_living;
        Gamerules *m_rules;

};


bool belongs(std::pair<int,int> pos, std::vector<std::pair<int,int>> list);
std::vector<std::pair<int,int>> read_excel(int worksheet);
// std::vector<std::pair<int,int>> read_excel(const char* filename);


#endif