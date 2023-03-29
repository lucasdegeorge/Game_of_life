#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <tuple>
#include "Gamerules.hpp"

#define NEIGHBOR_TO_DIE_L 2 
#define NEIGHBOR_TO_DIE_H 4
#define NEIGHBOR_TO_BORN 3
#define BORDERLESS true 
#define DIAG_COUNT true 
#define AUTOMATIC false
using namespace std;

// Constructor

Gamerules::Gamerules() :
    m_Automatic(AUTOMATIC),
    m_Borderless(BORDERLESS),
    m_diag_count(DIAG_COUNT),
    m_neighbor_to_die_l(NEIGHBOR_TO_DIE_L),
    m_neighbor_to_die_h(NEIGHBOR_TO_DIE_H),
    m_neighbor_to_born(NEIGHBOR_TO_BORN)
{

};

Gamerules::Gamerules(bool Automatic,
    bool Borderless,
    bool diag_count,
    int neighbor_to_die_l,
    int neighbor_to_die_h,
    int neighbor_to_born) :
    m_Automatic(Automatic),
    m_Borderless(Borderless),
    m_diag_count(diag_count),
    m_neighbor_to_die_l(neighbor_to_die_l),
    m_neighbor_to_die_h(neighbor_to_die_h),
    m_neighbor_to_born(neighbor_to_born)
{

};


// Destructor

Gamerules::~Gamerules()
{
}

// Methods

bool Gamerules::get_automatic()
{
    return m_Automatic;
}

bool Gamerules::get_borderless()
{
    return m_Borderless;
}

bool Gamerules::get_diag_count()
{
    return m_diag_count;
}

int Gamerules::get_neighbor_to_die_l()
{
    return m_neighbor_to_die_l;
}

int Gamerules::get_neighbor_to_die_h()
{
    return m_neighbor_to_die_h;
}

int Gamerules::get_neighbor_to_born()
{
    return m_neighbor_to_born;
}
