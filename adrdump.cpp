#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <limits>

#include <google/sparse_hash_set>
#include <google/sparse_hash_map>

using google::sparse_hash_set;
using google::sparse_hash_map;

struct eqint
{
    bool operator()(uint32_t a, uint32_t b) const
    {
        return (a==b);
    }
};

int main(int argc, char* argv[]) 
{
    sparse_hash_map<uint32_t, int, std::tr1::hash<uint32_t>, eqint > mcounters; 

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " user.dat\n";
        std::cerr << "Dumps address counters from user.dat to stdout (two columns, userid and count)\n";
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        std::cerr << "cannot open " << argv[1] << " for reading" << std::endl;
        exit(1);
    }
    mcounters.read_metadata(f);
    mcounters.read_nopointer_data(f);
    fclose(f);

    for (sparse_hash_map<uint32_t, int, std::tr1::hash<uint32_t>, eqint >::iterator i = 
        mcounters.begin(); i != mcounters.end(); i++)
    {
        std::cout << i->first << " " << i->second << std::endl;
    }

}

