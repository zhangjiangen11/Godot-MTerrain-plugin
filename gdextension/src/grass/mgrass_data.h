#ifndef MGRASS_DATA
#define MGRASS_DATA

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "../mconfig.h"

using namespace godot;

struct MGrassUndoData {
    uint8_t* data;

    void free(){
        memdelete_arr(data);
    }
};

class MGrassData : public Resource {
    GDCLASS(MGrassData,Resource);

    protected:
    static void _bind_methods();

    public:
    MGrassData();
    ~MGrassData();
    PackedByteArray data;
    int current_undo_id=0;
    int lowest_undo_id=0;
    HashMap<int,MGrassUndoData> undo_data;
    int density_index=2;
    float density=1;

    void set_data(const PackedByteArray& d);
    const PackedByteArray& get_data();
    void set_density(int input);
    int get_density();

    void add(int d);

    void check_undo(); // register a stage for undo
    void undo();

};
#endif