#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdio.h>
#include "Living_cells.hpp"
#include "Gamerules.hpp"
#include "Excel_reader/BasicExcel.hpp"
#include "Excel_reader/BasicExcel.cpp"

using namespace std;
using namespace YExcel;

// Function belongs 
// returns true if the pair p belongs to list
bool belongs(std::pair<int,int> p, std::vector<std::pair<int,int>> list)
{
    for (auto& pair : list)
    {
        if (pair.first == p.first && pair.second == p.second)
        {
            return true;
        }
    }
    return false;
}


// Function read_excel
// Function based on the classes of BasicExcel
// It reads the worksheet number worksheet in the excel file example1.xls 
// and returns a vector of pairs containing the initial config read.

vector<pair<int,int>> read_excel(int worksheet)
{
    BasicExcel e;
	vector<pair<int, int>> init;

    // Loading of the sheet
	e.Load("example1.xls");
	BasicExcelWorksheet* sheet1 = e.GetWorksheet(worksheet);

	if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

        // Reading of the cells
		for (size_t r=0; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = sheet1->Cell(r,c);
				switch (cell->Type())
				{
                    // If a 1 is written in the cell, we add this cell as a living cell in the initial config 
					case BasicExcelCell::INT:
						if (cell->GetInteger() == 1)
						{
							pair<int, int> paire(r, c);
							init.push_back(paire);
						}
						break;

					case BasicExcelCell::DOUBLE:
						if (int (cell->GetDouble()) == 1)
						{
							pair<int, int> paire(r, c);
							init.push_back(paire);
						}
						break;
				}
			}
		}
	}
	else
	{
		cout << "loading failed " << endl;
	}

    return init;
}



// Constructors
Living_cells::Living_cells() : m_generation(0)
{
    vector<pair<int, int>> init; 
    pair<int, int> paire(9, 10);
    init.push_back(paire);
    paire = make_pair(10,10);
    init.push_back(paire);
    paire = make_pair(11,10);
    init.push_back(paire);
    // This configuration is the blinker
    m_living = init;
    m_rules = new Gamerules(true, true, true, 2, 4, 3);
}

// Constructor allowing to set the initial config from the excel file
Living_cells::Living_cells(int worksheet) : m_generation(0)
{
    vector<pair<int,int>> init = read_excel(worksheet);
    m_living = init;
    m_rules = new Gamerules(true, true, true, 2, 4, 3);
}

Living_cells::Living_cells(std::vector<std::pair<int,int>> init) : m_living(init), m_generation(0)
{
    m_rules = new Gamerules(true, true, true, 2, 4, 3);
}

Living_cells::Living_cells(std::vector<std::pair<int,int>> init,
    bool Automatic,
    bool Borderless,
    bool diag_count,
    int neighbor_to_die_l,
    int neighbor_to_die_h,
    int neighbor_to_born
    ) : m_living(init), m_generation(0)
{
    m_rules = new Gamerules(Automatic, Borderless, diag_count, neighbor_to_die_l,  neighbor_to_die_h, neighbor_to_born);
}


// Destructor
Living_cells::~Living_cells()
{
    delete m_rules;
}


// Methods

// Counts the living neighbors of the cell denoted by pos
int Living_cells::count_neighbors(std::pair<int,int> pos)
{
    int count(0);
    int i = pos.first;
    int j = pos.second;

    // We determine the potential living neighbors of the cell denoted by pos
    vector<pair<int,int>> potential_neighbors;
    if (m_rules->get_diag_count())
    {
        potential_neighbors = {make_pair(i-1,j-1),
                               make_pair(i-1,j),
                               make_pair(i-1,j+1),
                               make_pair(i,j-1),
                               make_pair(i,j+1),
                               make_pair(i+1,j-1),
                               make_pair(i+1,j),
                               make_pair(i+1,j+1)
                               };
    }
    else 
    {
        potential_neighbors = {make_pair(i-1,j),
                               make_pair(i,j-1),
                               make_pair(i,j+1),
                               make_pair(i+1,j),
                               };
    }
    
    // We check if one of the living cells is a potential neighbor : 
    for (auto& posi : potential_neighbors)
    {
        if (belongs(posi, m_living))
        {
            count++;
        }
    }
    return count;
}

// Consider all the cells that could possibly be a living cell at the next generation
std::vector<std::pair<int,int>> Living_cells::potential_new_cells()
{
    vector<pair<int,int>> res;
    for (auto& pos : m_living)
    {
        int i(pos.first);
        int j(pos.second);

        vector<pair<int,int>> potential_neighbors;
        if (m_rules->get_diag_count())
        {
            potential_neighbors = {make_pair(i-1,j-1),
                                make_pair(i-1,j),
                                make_pair(i-1,j+1),
                                make_pair(i,j-1),
                                make_pair(i,j+1),
                                make_pair(i+1,j-1),
                                make_pair(i+1,j),
                                make_pair(i+1,j+1)
                                };
        }
        else 
        {
            potential_neighbors = {make_pair(i-1,j),
                                make_pair(i,j-1),
                                make_pair(i,j+1),
                                make_pair(i+1,j),
                                };
        }
        for (auto& p : potential_neighbors)
        {
            if (!(belongs(p, res)))
            {
                res.push_back(p);
            }
        }
        
    }
    return res;
}

// main function 
void Living_cells::evolve()
{
    int neighbor_to_die_l = m_rules->get_neighbor_to_die_l();
    int neighbor_to_die_h = m_rules->get_neighbor_to_die_h();
    int neighbor_to_born = m_rules->get_neighbor_to_born();

    // We determine if each already living case will stay alive
    for (auto& pos : m_living)
    {
        if ((count_neighbors(pos) >= neighbor_to_die_l) && (count_neighbors(pos) < neighbor_to_die_h))
        {
            m_next_living.push_back(pos);
        }
    }

    // We determine if each potential neighbor will live
    for (auto& pos : potential_new_cells())
    {
        if (count_neighbors(pos) == neighbor_to_born)
        {
            m_next_living.push_back(pos);
        }
    }

    //  Then, we switch the values
    m_living = m_next_living;
    m_next_living = {};

    m_generation++;
}


vector<std::pair<int,int>> Living_cells::get_cells_to_frontend()
{
    return m_living;
}

int Living_cells::get_generation()
{
    return m_generation;
}

void Living_cells::set_list_living(vector<pair<int,int>> pos)
{
    m_generation = 0;
    m_living = pos;
}

void Living_cells::set_living_from_keyboard(char letter)
{   
    int worksheet = int (letter) - 97;
    vector<pair<int,int>> new_config = read_excel(worksheet);
    set_list_living(new_config);
}

