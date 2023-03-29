#ifndef Gamerules_hpp
#define Gamerules_hpp

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <tuple>

class Gamerules
{
    public :
    
    // Constructor
    
    Gamerules();
    Gamerules(bool Automatic,
              bool Borderless,
              bool diag_count,
              int neighbor_to_die_l,
              int neighbor_to_die_h,
              int neighbor_to_born);
    
    // Destructor
    
    ~Gamerules();
    
    // Methods
    
    bool get_automatic();
    bool get_borderless();
    bool get_diag_count();
    int get_neighbor_to_die_l();
    int get_neighbor_to_die_h();
    int get_neighbor_to_born();

    protected :
    
    // Variables
    bool m_Automatic;
    bool m_Borderless;
    bool m_diag_count;
    int m_neighbor_to_die_l;
    int m_neighbor_to_die_h;
    int m_neighbor_to_born;

};


#endif /* Gamerules_hpp */