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
    sparse_hash_map<uint32_t, int, std::tr1::hash<uint32_t>, eqint > mcount1; 
    sparse_hash_map<uint32_t, int, std::tr1::hash<uint32_t>, eqint > mcount2; 

    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " user1.dat user2.dat\n";
        std::cerr << "Dumps differences in address counters to stdout (two columns, userid and diff)\n";
        std::cerr << "No output for unchanged userids.\n";
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        std::cerr << "cannot open " << argv[1] << " for reading" << std::endl;
        exit(1);
    }
    mcount1.read_metadata(f);
    mcount1.read_nopointer_data(f);
    fclose(f);

    f = fopen(argv[2], "r");
    if (!f)
    {
        std::cerr << "cannot open " << argv[2] << " for reading" << std::endl;
        exit(1);
    }
    mcount2.read_metadata(f);
    mcount2.read_nopointer_data(f);
    fclose(f);

    for (sparse_hash_map<uint32_t, int, std::tr1::hash<uint32_t>, eqint >::iterator i = 
        mcount2.begin(); i != mcount2.end(); i++)
    {
        int d = i->second - mcount1[i->first];
        if (d)
        {
            std::cout << i->first << " " << d << std::endl;
        }
    }
}

