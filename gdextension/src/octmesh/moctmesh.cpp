#include "moctmesh.h"

#define RS RenderingServer::get_singleton()
#include <godot_cpp/classes/world3d.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

void MOctMesh::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_active_mesh"), &MOctMesh::get_active_mesh);

    ClassDB::bind_method(D_METHOD("set_mesh_lod","input"), &MOctMesh::set_mesh_lod);
    ClassDB::bind_method(D_METHOD("get_mesh_lod"), &MOctMesh::get_mesh_lod);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT,"mesh_lod",PROPERTY_HINT_RESOURCE_TYPE,"MMeshLod"),"set_mesh_lod","get_mesh_lod");

    ClassDB::bind_method(D_METHOD("set_material_override","input"), &MOctMesh::set_material_override);
    ClassDB::bind_method(D_METHOD("get_material_override"), &MOctMesh::get_material_override);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT,"material_override",PROPERTY_HINT_RESOURCE_TYPE,"BaseMaterial3D,ShaderMaterial"),"set_material_override","get_material_override");

    ClassDB::bind_method(D_METHOD("set_shadow_setting","input"), &MOctMesh::set_shadow_setting);
    ClassDB::bind_method(D_METHOD("get_shadow_setting"), &MOctMesh::get_shadow_setting);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"shadow_setting",PROPERTY_HINT_ENUM,"Off,On,Double-Sided,Shadows Only"),"set_shadow_setting","get_shadow_setting");

    ClassDB::bind_method(D_METHOD("set_ignore_occlusion_culling","input"), &MOctMesh::set_ignore_occlusion_culling);
    ClassDB::bind_method(D_METHOD("get_ignore_occlusion_culling"), &MOctMesh::get_ignore_occlusion_culling);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,"ignore_occlusion_culling"),"set_ignore_occlusion_culling","get_ignore_occlusion_culling");

    ClassDB::bind_method(D_METHOD("set_transparency","input"), &MOctMesh::set_transparency);
    ClassDB::bind_method(D_METHOD("get_transparency"), &MOctMesh::get_transparency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,"transparency",PROPERTY_HINT_RANGE,"0,1"),"set_transparency","get_transparency");

    ClassDB::bind_method(D_METHOD("set_lod_bias","input"), &MOctMesh::set_lod_bias);
    ClassDB::bind_method(D_METHOD("get_lod_bias"), &MOctMesh::get_lod_bias);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,"lod_bias",PROPERTY_HINT_RANGE,"0.001,128"),"set_lod_bias","get_lod_bias");

    ClassDB::bind_method(D_METHOD("set_extra_cull_margin","input"), &MOctMesh::set_extra_cull_margin);
    ClassDB::bind_method(D_METHOD("get_extra_cull_margin"), &MOctMesh::get_extra_cull_margin);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,"extra_cull_margin",PROPERTY_HINT_RANGE,"0,16384"),"set_extra_cull_margin","get_extra_cull_margin");

    ClassDB::bind_method(D_METHOD("set_custom_aabb","input"), &MOctMesh::set_custom_aabb);
    ClassDB::bind_method(D_METHOD("get_custom_aabb"), &MOctMesh::get_custom_aabb);
    ADD_PROPERTY(PropertyInfo(Variant::AABB,"custom_aabb"),"set_custom_aabb","get_custom_aabb");

    ClassDB::bind_method(D_METHOD("set_enable_global_illumination","input"), &MOctMesh::set_enable_global_illumination);
    ClassDB::bind_method(D_METHOD("get_enable_global_illumination"), &MOctMesh::get_enable_global_illumination);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,"enable_global_illumination"),"set_enable_global_illumination","get_enable_global_illumination");

    ClassDB::bind_method(D_METHOD("_lod_mesh_changed"), &MOctMesh::_lod_mesh_changed);
}


///STATIC PART
WorkerThreadPool::TaskID MOctMesh::thread_task_id;
std::mutex MOctMesh::update_mutex;
bool MOctMesh::is_octtree_inserted = false;
bool MOctMesh::is_updating = false;
uint16_t MOctMesh::oct_id = 0;
int32_t MOctMesh::last_oct_point_id = OCT_POINT_ID_START;
HashMap<int32_t,MOctMesh*> MOctMesh::octpoint_to_octmesh;
MOctTree* MOctMesh::octtree = nullptr;


bool MOctMesh::is_my_octtree(MOctTree* input){
    return input == MOctMesh::octtree;
}

uint16_t MOctMesh::get_oct_id(){
    return oct_id;
}


bool MOctMesh::set_octtree(MOctTree* input){
    ERR_FAIL_COND_V(input==nullptr,false);
    if(octtree){
        WARN_PRINT("OctTree "+octtree->get_name()+" is already assigned! Only one OctTree can be assing to update MOctMesh!");
        return false;
    }
    octtree = input;
    if(octtree!=nullptr){
        oct_id = octtree->get_oct_id();
        // Here we insert all points
    }
    return true;
}

void MOctMesh::remove_octtree(MOctTree* input){
    if(input == octtree){
        octtree->remove_oct_id(oct_id);
        octtree = nullptr;
    }
}

void MOctMesh::insert_points(){
    ERR_FAIL_COND(octtree==nullptr);
    is_octtree_inserted = true;
    PackedVector3Array points_pos;
    PackedInt32Array points_ids;
    for(HashMap<int32_t,MOctMesh*>::Iterator it=octpoint_to_octmesh.begin();it!=octpoint_to_octmesh.end();++it){
        points_ids.push_back(it->key);
        Vector3 oct_pos;
        if(it->value->is_inside_tree()){
            oct_pos = it->value->get_global_position();
        }
        it->value->oct_position = oct_pos;
        points_pos.push_back(oct_pos);
    }
    octtree->insert_points(points_pos,points_ids,oct_id);
}

int32_t MOctMesh::add_octmesh(MOctMesh* input){
    last_oct_point_id++;
    if(octtree!=nullptr && is_octtree_inserted){
        bool res = octtree->insert_point(input->get_global_position(),last_oct_point_id,oct_id);
        ERR_FAIL_COND_V_MSG(!res,INVALID_OCT_POINT_ID,"Single point can't be inserted!");
        input->oct_position = input->get_global_position();
    }

    octpoint_to_octmesh.insert(last_oct_point_id,input);
    return last_oct_point_id;
}

void MOctMesh::remove_octmesh(int32_t id){
    ERR_FAIL_COND(!octpoint_to_octmesh.has(id));
    MOctMesh* m = octpoint_to_octmesh[id];
    m->oct_point_id = INVALID_OCT_POINT_ID;
    m->lod = INVALID_LOD;
    octpoint_to_octmesh.erase(id);
    if(octtree!=nullptr && is_octtree_inserted){
        octtree->remove_point(id,m->oct_position,oct_id);
    }
}

void MOctMesh::move_octmesh(MOctMesh* input){
    ERR_FAIL_COND(input->oct_point_id == INVALID_OCT_POINT_ID);
    ERR_FAIL_COND(!octpoint_to_octmesh.has(input->oct_point_id));
    Vector3 old_pos = input->oct_position;
    Vector3 new_pos = input->get_global_position();
    if(octtree && is_octtree_inserted && old_pos!=new_pos){
        octtree->add_move_req(MOctTree::PointMoveReq(input->oct_point_id,oct_id,old_pos,new_pos));
        input->oct_position = new_pos;
    }
}

void MOctMesh::octtree_update(const Vector<MOctTree::PointUpdate>* update_info){
    if(update_info->size() > 0) {
        is_updating = true;
        thread_task_id = WorkerThreadPool::get_singleton()->add_native_task(&MOctMesh::octtree_thread_update,(void*)update_info,true);
    } else {
        octtree->point_process_finished(oct_id);
    }
}

void MOctMesh::octtree_thread_update(void* input){
    const Vector<MOctTree::PointUpdate>* update_info = (const Vector<MOctTree::PointUpdate>*)input;
    for(int i=0; i < update_info->size(); i++){
        std::lock_guard<std::mutex> lock(MOctMesh::update_mutex);
        MOctTree::PointUpdate p = update_info->get(i);
        if(!octpoint_to_octmesh.has(p.id)){
            continue;
        }
        MOctMesh* oct_mesh = octpoint_to_octmesh.get(p.id);
        ERR_CONTINUE(!UtilityFunctions::is_instance_valid(oct_mesh));
        oct_mesh->update_lod_mesh(p.lod);
    }
}

void MOctMesh::update_tick(){
    if(is_updating){
        if(WorkerThreadPool::get_singleton()->is_task_completed(thread_task_id)){
            is_updating = false;
            WorkerThreadPool::get_singleton()->wait_for_task_completion(thread_task_id);
            ERR_FAIL_COND(octtree==nullptr);
            octtree->point_process_finished(oct_id);
        }
    }
    
}

/////////////////////////////////////////////////////////////////
//FINISH STATIC PART
////////////////////////////////////////////////////////////////

void MOctMesh::_update_visibilty(){
    if(!is_inside_tree()){
        return;
    }
    if(instance.is_valid()){
        RS->instance_set_visible(instance,is_visible_in_tree());
    }
}

MOctMesh::MOctMesh(){
    set_notify_transform(true);
}

MOctMesh::~MOctMesh(){
    update_mutex.lock();
    if(has_valid_oct_point_id()){
        MOctMesh::remove_octmesh(oct_point_id);
    }
    lod = -3;
    if(instance.is_valid()){
        RS->free_rid(instance);
        instance = RID();
    }
    update_mutex.unlock();
}

// CURRENT_LOD means update current mesh without changing LOD
// INVALID_LOD is invalide object, or it will removed
void MOctMesh::update_lod_mesh(int8_t new_lod){
    if(new_lod==CURRENT_LOD){
        new_lod = lod;
    }
    if(new_lod==INVALID_LOD){
        return;
    }
    lod.store(new_lod,std::memory_order_relaxed);
    // if is not in tree return but just update lod
    // then call this function when enter the tree to update the mesh in case re enter the tree
    if(!is_inside_tree()){
        return;
    }
    RID new_mesh_rid ;
    if(mesh_lod.is_valid()){
        new_mesh_rid = mesh_lod->get_mesh_rid(new_lod);
    } else {
        new_mesh_rid = RID();
    }
    if(new_mesh_rid == current_mesh){
        return;
    }
    current_mesh = new_mesh_rid;
    if(current_mesh.is_valid()){
        if(!instance.is_valid()){
            RID created_instance = RS->instance_create();
            RS->instance_attach_object_instance_id(created_instance,get_instance_id());
            instance = created_instance;
            RID scenario = MOctMesh::octtree->get_scenario();
            RS->instance_set_scenario(instance,scenario);
            RS->instance_set_transform(instance, get_global_transform());
            if(material_override.is_valid()){
                RS->instance_geometry_set_material_override(instance,material_override->get_rid());
            }
            RS->instance_geometry_set_cast_shadows_setting(instance,shadow_setting);
            RS->instance_geometry_set_flag(instance, RenderingServer::INSTANCE_FLAG_USE_BAKED_LIGHT, enable_global_illumination);
            RS->instance_geometry_set_flag(instance, RenderingServer::INSTANCE_FLAG_IGNORE_OCCLUSION_CULLING, ignore_occlusion_culling);
            RS->instance_geometry_set_lod_bias(instance, lod_bias);
            RS->instance_geometry_set_transparency(instance, transparency);
            RS->instance_set_extra_visibility_margin(instance, extra_cull_margin);
        }
        RS->instance_set_base(instance,current_mesh);
        RS->instance_set_custom_aabb(instance, custom_aabb);
    } else {
        if(instance.is_valid()){
            RS->free_rid(instance);
            instance = RID();
        }
    }
    call_deferred("update_gizmos");
}

Ref<Mesh> MOctMesh::get_active_mesh(){
    if(mesh_lod.is_valid()){
        int8_t clod = lod.load(std::memory_order_relaxed);
        return mesh_lod->get_mesh(clod);
    }
    Ref<Mesh> out;
    return out;
}


void MOctMesh::set_mesh_lod(Ref<MMeshLod> input){
    std::lock_guard<std::mutex> lock(MOctMesh::update_mutex);
    if(mesh_lod.is_valid()){
        mesh_lod->disconnect("meshes_changed", Callable(this,"_lod_mesh_changed"));
    }
    if(input.is_valid()){
        input->connect("meshes_changed", Callable(this,"_lod_mesh_changed"));
    }
    mesh_lod = input;
    if(has_valid_oct_point_id()){
        update_lod_mesh();
    }
}

Ref<MMeshLod> MOctMesh::get_mesh_lod(){
    return mesh_lod;
}

void MOctMesh::set_material_override(Ref<Material> input){
    std::lock_guard<std::mutex> lock(update_mutex);
    material_override = input;
    if(instance.is_valid()){
        if(material_override.is_valid()){
            RS->instance_geometry_set_material_override(instance,material_override->get_rid());
        } else {
            RS->instance_geometry_set_material_override(instance,RID());
        }
    }
}

Ref<Material> MOctMesh::get_material_override(){
    return material_override;
}

void MOctMesh::set_shadow_setting(RenderingServer::ShadowCastingSetting input){
    std::lock_guard<std::mutex> lock(update_mutex);
    shadow_setting = input;
    if(instance.is_valid()){
        RS->instance_geometry_set_cast_shadows_setting(instance, shadow_setting);
    }
}

RenderingServer::ShadowCastingSetting MOctMesh::get_shadow_setting(){
    return shadow_setting;
}

void MOctMesh::set_ignore_occlusion_culling(bool input){
    std::lock_guard<std::mutex> lock(update_mutex);
    ignore_occlusion_culling = input;
    if(instance.is_valid()){
        RS->instance_geometry_set_flag(instance, RenderingServer::INSTANCE_FLAG_IGNORE_OCCLUSION_CULLING, input);
    }
}
bool MOctMesh::get_ignore_occlusion_culling(){
    return ignore_occlusion_culling;
}

void MOctMesh::set_enable_global_illumination(bool input){
    std::lock_guard<std::mutex> lock(update_mutex);
    enable_global_illumination = input;
    if(instance.is_valid()){
        RS->instance_geometry_set_flag(instance, RenderingServer::INSTANCE_FLAG_USE_BAKED_LIGHT, input);
    }
}

bool MOctMesh::get_enable_global_illumination(){
    return enable_global_illumination;
}

void MOctMesh::set_transparency(float input){
    std::lock_guard<std::mutex> lock(update_mutex);
    transparency = input;
    if(instance.is_valid()){
        RS->instance_geometry_set_transparency(instance, transparency);
    }
}
float MOctMesh::get_transparency(){
    return transparency;
}

void MOctMesh::set_lod_bias(float input){
    std::lock_guard<std::mutex> lock(update_mutex);
    lod_bias = input;
    if(instance.is_valid()){
        RS->instance_geometry_set_lod_bias(instance,lod_bias);
    }
}

float MOctMesh::get_lod_bias(){
    return lod_bias;
}

void MOctMesh::set_extra_cull_margin(float input){
    std::lock_guard<std::mutex> lock(update_mutex);
    extra_cull_margin = input;
    if(instance.is_valid()){
        RS->instance_set_extra_visibility_margin(instance,extra_cull_margin);
    }
}

float MOctMesh::get_extra_cull_margin(){
    return extra_cull_margin;
}

void MOctMesh::set_custom_aabb(AABB input){
    std::lock_guard<std::mutex> lock(update_mutex);
    custom_aabb = input;
    if(instance.is_valid()){
        RS->instance_set_custom_aabb(instance,custom_aabb);
    }
}
AABB MOctMesh::get_custom_aabb(){
    return custom_aabb;
}



bool MOctMesh::has_valid_oct_point_id(){
    return oct_point_id != INVALID_OCT_POINT_ID;
}

void MOctMesh::_notification(int p_what){
    switch (p_what)
    {
    case NOTIFICATION_TRANSFORM_CHANGED:
        update_mutex.lock();
        MOctMesh::move_octmesh(this);
        if(instance.is_valid()){
            RS->instance_set_transform(instance, get_global_transform());
        }
        update_mutex.unlock();
        break;
    case NOTIFICATION_VISIBILITY_CHANGED:
        update_mutex.lock();
        _update_visibilty();
        update_mutex.unlock();
        break;
    case NOTIFICATION_ENTER_TREE:
        update_mutex.lock();
        if(!has_valid_oct_point_id() && is_inside_tree()){
            oct_point_id = MOctMesh::add_octmesh(this);
        }
        update_lod_mesh();
        _update_visibilty();
        update_mutex.unlock();
        break;
    case NOTIFICATION_EXIT_TREE:
        update_mutex.lock();
        if(instance.is_valid()){
            RS->instance_set_visible(instance,false);
        }
        update_mutex.unlock();
        break;
    default:
        break;
    }
}

void MOctMesh::_lod_mesh_changed(){
    std::lock_guard<std::mutex> lock(MOctMesh::update_mutex);
    update_lod_mesh();
}