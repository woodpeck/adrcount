#include <limits>

#define OSMIUM_WITH_PBF_INPUT
#define OSMIUM_WITH_XML_INPUT

#include <osmium.hpp>

#include <google/sparse_hash_set>
#include <google/sparse_hash_map>

using google::sparse_hash_set;
using google::sparse_hash_map;

struct eqint
{
    bool operator()(uint64_t a, uint64_t b) const
    {
        return (a==b);
    }
    bool operator()(uint32_t a, uint32_t b) const
    {
        return (a==b);
    }
};

class AddressHandler : public Osmium::Handler::Base 
{
    sparse_hash_set<uint64_t, std::tr1::hash<uint64_t>, eqint > mnodes;
    sparse_hash_set<uint64_t, std::tr1::hash<uint64_t>, eqint > mways; 
    sparse_hash_set<uint64_t, std::tr1::hash<uint64_t>, eqint > mrelations; 
    sparse_hash_map<uint32_t, int, std::tr1::hash<uint32_t>, eqint > mcounters; 
    char *mdir;

public:

    AddressHandler(char *directory)
    {
        mdir = directory;
        mnodes.set_deleted_key(0);
        mways.set_deleted_key(0);
        mrelations.set_deleted_key(0);
    }

    ~AddressHandler() 
    {
    }

    void init(__attribute__((unused))Osmium::OSM::Meta& meta) 
    {
        if (mdir) deserialize();
    }

    void node(const shared_ptr<Osmium::OSM::Node const>& node) 
    {
        object(node, mnodes);
    }
    void way(const shared_ptr<Osmium::OSM::Way const>& way) 
    {
        object(way, mways);
    }
    void relation(const shared_ptr<Osmium::OSM::Relation const>& rel) 
    {
        object(rel, mrelations);
    }

    void object(const shared_ptr<Osmium::OSM::Object const>& obj, sparse_hash_set<uint64_t, std::tr1::hash<uint64_t>, eqint >& hash)
    {
        // not interested in anonymous users
        if (obj->uid() == -1) return;

        if (!obj->visible() || !obj->tags().get_value_by_key("addr:housenumber"))
        {
            // object deleted or housenumber tag removed
            sparse_hash_set<uint64_t, std::tr1::hash<uint64_t>, eqint >::iterator i = hash.find(obj->id());
            if (i != hash.end())
            {
                hash.erase(i);
                mcounters[obj->uid()]--;
            }
        }
        else
        {
            // new version has housenumber; only interesting if
            // previous version hadn't (or didn't exist)
            // we don't automatically assume object creation for
            // version==1 as we might accidentally be re-applying
            // an .osc file
            if (hash.find(obj->id()) == hash.end())
            {
                hash.insert(obj->id());
                mcounters[obj->uid()]++;
            }
        }
    }

    void final()
    {
        if (mdir) serialize();
    }

    void serialize() 
    {
        char filename[PATH_MAX];
        sprintf(filename, "%s/objects.dat", mdir);
        FILE *f = fopen(filename, "w");
        if (!f)
        {
            std::cerr << "cannot open " << filename << " for writing" << std::endl;
            exit(1);
        }

        mnodes.write_metadata(f);
        mnodes.write_nopointer_data(f);

        mways.write_metadata(f);
        mways.write_nopointer_data(f);

        mrelations.write_metadata(f);
        mrelations.write_nopointer_data(f);
        fclose(f);

        sprintf(filename, "%s/users.dat", mdir);
        f = fopen(filename, "w");
        if (!f)
        {
            std::cerr << "cannot open " << filename << " for writing" << std::endl;
            exit(1);
        }

        mcounters.write_metadata(f);
        mcounters.write_nopointer_data(f);
        fclose(f);
    };

    void deserialize() 
    {
        char filename[PATH_MAX];
        sprintf(filename, "%s/objects.dat", mdir);
        FILE *f = fopen(filename, "r");
        if (!f)
        {
            std::cerr << "cannot open " << filename << " for reading - starting with empty counters" << std::endl;
            return; 
        }
        mnodes.read_metadata(f);
        mnodes.read_nopointer_data(f);

        mways.read_metadata(f);
        mways.read_nopointer_data(f);

        mrelations.read_metadata(f);
        mrelations.read_nopointer_data(f);
        fclose(f);

        sprintf(filename, "%s/users.dat", mdir);
        f = fopen(filename, "r");
        if (!f)
        {
            std::cerr << "cannot open " << filename << " for reading" << std::endl;
            exit(1);
        }
        mcounters.read_metadata(f);
        mcounters.read_nopointer_data(f);
        fclose(f);
    }
};

/* ================================================== */

int main(int argc, char* argv[]) 
{
    if (argc == 1 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " OSMFILE [DIR]\n\n";
        std::cerr << "  OSMFILE - OSM full history file, or OSC file.\n";
        std::cerr << "  DIR     - directory to read/write status info from/to.\n";
        exit(1);
    }

    char *dir = NULL;
    if (argc == 3) dir = argv[2];

    Osmium::OSMFile infile(argv[1]);
    AddressHandler handler(dir);
    Osmium::Input::read(infile, handler);

    google::protobuf::ShutdownProtobufLibrary();
}

