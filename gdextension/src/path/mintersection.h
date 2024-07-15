#ifndef _MSEGMENTMESH
#define _MSEGMENTMESH


#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/vmap.hpp>
#include <godot_cpp/templates/vset.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/classes/material.hpp>

#include <godot_cpp/classes/array_mesh.hpp>

#include "../octmesh/mmesh_lod.h"


using namespace godot;

class MIntersectionInfo : public RefCounted
{
    public:
    int num_sockts;
    float lenght;
    RID mesh_rid = RID();
    Ref<Material> material;
    PackedVector3Array vertex;
    PackedVector3Array normal;
    PackedFloat32Array tangent;
    PackedColorArray color;
    PackedVector2Array uv;
    PackedVector2Array uv2;
    PackedInt32Array index;
    PackedFloat32Array weights;
};


class MIntersection : public Resource {
    GDCLASS(MIntersection,Resource);

    protected:
    static void _bind_methods();

    private:
    bool _is_init = false;
    Ref<MMeshLod> mesh;
    TypedArray<Transform3D> sockets;
    Vector<Ref<MIntersectionInfo>> mesh_info;

    Ref<ArrayMesh> debug_mesh;

    public:
    bool is_init();
    Ref<MIntersectionInfo> get_mesh_info(int lod);
    void generate_mesh_info();
    
    private:
    void _generate_mesh_info(Ref<Mesh> m, Ref<MIntersectionInfo> info);

    public:
    Ref<ArrayMesh> get_debug_mesh();

    void set_mesh(Ref<MMeshLod> input);
    Ref<MMeshLod> get_mesh();
    int get_mesh_count();

    void set_sockets(TypedArray<Transform3D> input);
    TypedArray<Transform3D> get_sockets();
    int get_socket_count();
    Vector<Transform3D> _get_sockets();
};
#endif