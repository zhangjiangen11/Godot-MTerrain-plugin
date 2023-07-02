#include "mterrain.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/reg_ex.hpp>
#include <godot_cpp/classes/reg_ex_match.hpp>



void MTerrain::_bind_methods() {
    ADD_SIGNAL(MethodInfo("finish_updating_chunks"));
    ADD_SIGNAL(MethodInfo("finish_updating_physics"));

    ClassDB::bind_method(D_METHOD("finish_terrain"), &MTerrain::finish_terrain);
    ClassDB::bind_method(D_METHOD("start"), &MTerrain::start);
    ClassDB::bind_method(D_METHOD("create_grid"), &MTerrain::create_grid);
    ClassDB::bind_method(D_METHOD("remove_grid"), &MTerrain::remove_grid);
    ClassDB::bind_method(D_METHOD("restart_grid"), &MTerrain::restart_grid);
    ClassDB::bind_method(D_METHOD("update"), &MTerrain::update);
    ClassDB::bind_method(D_METHOD("finish_update"), &MTerrain::finish_update);
    ClassDB::bind_method(D_METHOD("finish_update_physics"), &MTerrain::finish_update_physics);
    ClassDB::bind_method(D_METHOD("is_finishing_update_chunks"), &MTerrain::is_finish_updating_chunks);
    ClassDB::bind_method(D_METHOD("is_finishing_update_physics"), &MTerrain::is_finish_updating_physics);
    ClassDB::bind_method(D_METHOD("get_closest_height", "world_position"), &MTerrain::get_closest_height);

    ClassDB::bind_method(D_METHOD("set_dataDir","dir"), &MTerrain::set_dataDir);
    ClassDB::bind_method(D_METHOD("get_dataDir"), &MTerrain::get_dataDir);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "dataDir"), "set_dataDir", "get_dataDir");
    ClassDB::bind_method(D_METHOD("set_grid_create","val"), &MTerrain::set_create_grid);
    ClassDB::bind_method(D_METHOD("get_create_grid"), &MTerrain::get_create_grid);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "create_grid"), "set_grid_create", "get_create_grid");
    ClassDB::bind_method(D_METHOD("get_material"), &MTerrain::get_material);
    ClassDB::bind_method(D_METHOD("set_material", "terrain_material"), &MTerrain::set_material);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "ShaderMaterial"), "set_material", "get_material");
    
    ClassDB::bind_method(D_METHOD("set_update_chunks_interval","interval"), &MTerrain::set_update_chunks_interval);
    ClassDB::bind_method(D_METHOD("get_update_chunks_interval"), &MTerrain::get_update_chunks_interval);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "update_chunks_interval"), "set_update_chunks_interval", "get_update_chunks_interval");
    
    ClassDB::bind_method(D_METHOD("set_update_chunks_loop", "val"), &MTerrain::set_update_chunks_loop);
    ClassDB::bind_method(D_METHOD("get_update_chunks_loop"), &MTerrain::get_update_chunks_loop);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "update_chunks_loop"), "set_update_chunks_loop", "get_update_chunks_loop");

    ClassDB::bind_method(D_METHOD("set_update_physics_interval", "val"), &MTerrain::set_update_physics_interval);
    ClassDB::bind_method(D_METHOD("get_update_physics_interval"), &MTerrain::get_update_physics_interval);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "update_physics_interval"), "set_update_physics_interval", "get_update_physics_interval");

    ClassDB::bind_method(D_METHOD("set_update_physics_loop", "val"), &MTerrain::set_update_physics_loop);
    ClassDB::bind_method(D_METHOD("get_update_physics_loop"), &MTerrain::get_update_physics_loop);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "update_physics_loop"), "set_update_physics_loop", "get_update_physics_loop");

    ClassDB::bind_method(D_METHOD("set_physics_update_limit", "val"), &MTerrain::set_physics_update_limit);
    ClassDB::bind_method(D_METHOD("get_physics_update_limit"), &MTerrain::get_physics_update_limit);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_update_limit"), "set_physics_update_limit", "get_physics_update_limit");

    ClassDB::bind_method(D_METHOD("get_terrain_size"), &MTerrain::get_terrain_size);
    ClassDB::bind_method(D_METHOD("set_terrain_size", "size"), &MTerrain::set_terrain_size);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I,"terrain_size"), "set_terrain_size", "get_terrain_size");
    ClassDB::bind_method(D_METHOD("set_offset", "offset"), &MTerrain::set_offset);
    ClassDB::bind_method(D_METHOD("get_offset"), &MTerrain::get_offset);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "offset"), "set_offset", "get_offset");

    ClassDB::bind_method(D_METHOD("set_region_size", "region_size"), &MTerrain::set_region_size);
    ClassDB::bind_method(D_METHOD("get_region_size"), &MTerrain::get_region_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"region_size"), "set_region_size", "get_region_size");
    
    
    ClassDB::bind_method(D_METHOD("set_max_range", "max_range"), &MTerrain::set_max_range);
    ClassDB::bind_method(D_METHOD("get_max_range"), &MTerrain::get_max_range);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_range"), "set_max_range", "get_max_range");
    ClassDB::bind_method(D_METHOD("set_custom_camera", "camera"), &MTerrain::set_custom_camera);
    ClassDB::bind_method(D_METHOD("set_editor_camera", "camera"), &MTerrain::set_editor_camera);

    
    ClassDB::bind_method(D_METHOD("set_min_size","index"), &MTerrain::set_min_size);
    ClassDB::bind_method(D_METHOD("get_min_size"), &MTerrain::get_min_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"min_size",PROPERTY_HINT_ENUM, M_SIZE_LIST_STRING), "set_min_size", "get_min_size");

    ClassDB::bind_method(D_METHOD("set_max_size","index"), &MTerrain::set_max_size);
    ClassDB::bind_method(D_METHOD("get_max_size"), &MTerrain::get_max_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"max_size",PROPERTY_HINT_ENUM, M_SIZE_LIST_STRING), "set_max_size", "get_max_size");
    
    ClassDB::bind_method(D_METHOD("set_min_h_scale","index"), &MTerrain::set_min_h_scale);
    ClassDB::bind_method(D_METHOD("get_min_h_scale"), &MTerrain::get_min_h_scale);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"min_h_scale",PROPERTY_HINT_ENUM, M_H_SCALE_LIST_STRING), "set_min_h_scale", "get_min_h_scale");

    ClassDB::bind_method(D_METHOD("set_max_h_scale","index"), &MTerrain::set_max_h_scale);
    ClassDB::bind_method(D_METHOD("get_max_h_scale"), &MTerrain::get_max_h_scale);
    ADD_PROPERTY(PropertyInfo(Variant::INT,"max_h_scale",PROPERTY_HINT_ENUM, M_H_SCALE_LIST_STRING), "set_max_h_scale", "get_max_h_scale");

    ClassDB::bind_method(D_METHOD("set_size_info", "size_info"), &MTerrain::set_size_info);
    ClassDB::bind_method(D_METHOD("get_size_info"), &MTerrain::get_size_info);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "size_info",PROPERTY_HINT_NONE,"",PROPERTY_USAGE_STORAGE), "set_size_info", "get_size_info");

    ClassDB::bind_method(D_METHOD("set_lod_distance", "lod_distance"), &MTerrain::set_lod_distance);
    ClassDB::bind_method(D_METHOD("get_lod_distance"), &MTerrain::get_lod_distance);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "lod_distance",PROPERTY_HINT_NONE,"", PROPERTY_USAGE_STORAGE),"set_lod_distance","get_lod_distance");
}

MTerrain::MTerrain() {
    //connect("ready", Callable(this, "start"));
    connect("tree_exiting", Callable(this, "finish_terrain"));
    recalculate_terrain_config(true);
    grid = memnew(MGrid);
    update_chunks_timer = memnew(Timer);
    update_chunks_timer->set_wait_time(update_chunks_interval);
    update_chunks_timer->set_one_shot(true);
    add_child(update_chunks_timer);
    update_chunks_timer->connect("timeout", Callable(this, "finish_update"));

    update_physics_timer = memnew(Timer);
    update_physics_timer->set_wait_time(update_physics_interval);
    update_physics_timer->set_one_shot(true);
    add_child(update_physics_timer);
    update_physics_timer->connect("timeout", Callable(this, "finish_update_physics"));
}

MTerrain::~MTerrain() {
    memdelete(grid);
}


void MTerrain::finish_terrain() {
    if(update_thread_chunks.valid()){
        update_thread_chunks.wait();
    }
}

void MTerrain::start() {
    create_grid();
}

void MTerrain::create_grid(){
    _chunks = memnew(MChunks);
    _chunks->create_chunks(size_list[min_size_index],size_list[max_size_index],h_scale_list[min_h_scale_index],h_scale_list[max_h_scale_index],size_info);
    grid->set_scenario(get_world_3d()->get_scenario());
    grid->space = get_world_3d()->get_space();
    grid->offset = offset;
    grid->dataDir = dataDir;
    grid->region_size = region_size;
    if(material.is_valid()){
        grid->set_material(material);
    }
    grid->lod_distance = lod_distance;
    grid->create(terrain_size.x,terrain_size.y,_chunks);
    get_cam_pos();
    update_uniforms();
    grid->update_regions_uniforms(uniforms);
    grid->update_chunks(cam_pos);
    grid->apply_update_chunks();
    grid->update_physics(cam_pos);
    if(update_physics_loop){
        update_physics();
    }
    if(update_chunks_loop){
        update();
    }
    UtilityFunctions::print("Chunks has been created ");
}

void MTerrain::remove_grid(){
    update_chunks_timer->stop();
    update_physics_timer->stop();
    if(update_thread_chunks.valid()){
        update_thread_chunks.wait();
        finish_updating_chunks = true;
    }
    if(update_thread_physics.valid()){
        update_thread_physics.wait();
        finish_updating_physics = true;
    }
    grid->clear();
}

void MTerrain::restart_grid(){
    remove_grid();
    create_grid();
}

void MTerrain::update() {
    ERR_FAIL_COND(!finish_updating_chunks);
    ERR_FAIL_COND(!grid->is_created());
    get_cam_pos();
    finish_updating_chunks = false;
    update_thread_chunks = std::async(std::launch::async, &MGrid::update_chunks, grid, cam_pos);
    update_chunks_timer->start();
}

void MTerrain::finish_update() {
    std::future_status status = update_thread_chunks.wait_for(std::chrono::microseconds(1));
    if(status == std::future_status::ready){
        finish_updating_chunks = true;
        grid->apply_update_chunks();
        emit_signal("finish_updating_chunks");
        if(update_chunks_loop){
            update();
        }
    } else {
        update_chunks_timer->start();
    }
}

void MTerrain::update_physics(){
    ERR_FAIL_COND(!finish_updating_physics);
    ERR_FAIL_COND(!grid->is_created());
    get_cam_pos();
    finish_updating_physics = false;
    update_thread_physics = std::async(std::launch::async, &MGrid::update_physics, grid, cam_pos);
    update_physics_timer->start();
}

void MTerrain::finish_update_physics(){
    std::future_status status = update_thread_physics.wait_for(std::chrono::microseconds(1));
    if(status == std::future_status::ready){
        finish_updating_physics = true;
        emit_signal("finish_updating_physics");
        if(update_physics_loop){
            update_physics();
        }
    } else {
        update_physics_timer->start();
    }
}

bool MTerrain::is_finish_updating_chunks(){
    return finish_updating_chunks;
}
bool MTerrain::is_finish_updating_physics(){
    return finish_updating_physics;
}

void MTerrain::get_cam_pos() {
    if(custom_camera != nullptr){
        cam_pos = custom_camera->get_position();
        return;
    }
    if(editor_camera !=nullptr){
        cam_pos = editor_camera->get_position();
        return;
    }
    Viewport* v = get_viewport();
    Camera3D* c = v->get_camera_3d();
    cam_pos = c->get_position();
}

void MTerrain::set_dataDir(String input) {
    dataDir = input;
}

String MTerrain::get_dataDir() {
    return dataDir;
}

void MTerrain::set_create_grid(bool input){
    if(!is_inside_tree()){
        return;
    }
    if(grid->is_created() && !input){
        remove_grid();
        return;
    }
    if(!grid->is_created() && input){
        create_grid();
        return;
    }
}

bool MTerrain::get_create_grid(){
    return grid->is_created();
}


Ref<ShaderMaterial> MTerrain::get_material(){
    return material;
}

void MTerrain::set_material(Ref<ShaderMaterial> m){
    material = m;
}

float MTerrain::get_update_chunks_interval(){
    return update_chunks_interval;
}
void MTerrain::set_update_chunks_interval(float input){
    update_chunks_interval = input;
    if(input < 0.001){
        update_chunks_interval = 0.001;
    }
    update_chunks_timer->set_wait_time(update_chunks_interval);
}

void MTerrain::set_update_chunks_loop(bool input) {
    update_chunks_loop = input;
    if(update_chunks_interval && finish_updating_chunks){
        update();
    }
}

bool MTerrain::get_update_chunks_loop() {
    return update_chunks_loop;
}

float MTerrain::get_update_physics_interval(){
    return update_physics_interval;
}
void MTerrain::set_update_physics_interval(float input){
    update_physics_interval = input;
    if(input < 0.001){
        update_physics_interval = 0.001;
    }
    update_physics_timer->set_wait_time(update_physics_interval);
}
bool MTerrain::get_update_physics_loop(){
    return update_physics_loop;
}
void MTerrain::set_update_physics_loop(float input){
    update_physics_loop = input;
    if(update_physics_loop && finish_updating_physics){
        update_physics();
    }
}

void MTerrain::set_physics_update_limit(int32_t input){
    if(input<0){
        grid->physics_update_limit = 0;
    } else {
        grid->physics_update_limit = input;
    }
}
int32_t MTerrain::get_physics_update_limit(){
    return grid->physics_update_limit;
}

Vector2i MTerrain::get_terrain_size(){
    return terrain_size;
}

void MTerrain::set_terrain_size(Vector2i size){
    if(size.x < 1 || size.y < 1){
        return;
    }
    if(size == terrain_size){
        return;
    }
    terrain_size = size;
}


void MTerrain::set_max_range(int32_t input) {
    max_range = input;
    grid->max_range = input;
}

int32_t MTerrain::get_max_range() {
    return max_range;
}

void MTerrain::set_editor_camera(Node3D* camera_node){
    editor_camera = camera_node;
}
void MTerrain::set_custom_camera(Node3D* camera_node){
    custom_camera = camera_node;
}

void MTerrain::set_offset(Vector3 input){
    offset = input;
}

Vector3 MTerrain::get_offset(){
    return offset;
}


void MTerrain::set_region_size(int32_t input) {
    region_size = input;
}

int32_t MTerrain::get_region_size() {
    return region_size;
}

void MTerrain::update_uniforms() {
    ERR_FAIL_COND(!material.is_valid());
    ERR_FAIL_COND(!material->get_shader().is_valid());
    Array uniform_list = material->get_shader()->get_shader_uniform_list();
    Ref<RegEx> reg_compression;
    reg_compression.instantiate();
    reg_compression->compile("mterrain_(\\d+)");
    Ref<RegEx> reg_name;
    reg_name.instantiate();
    reg_name->compile("mterrain_\\d*_?(.*)");
    for(int i=0; i<uniform_list.size();i++){
        Dictionary u = uniform_list[i];
        String u_name = u["name"];
        int u_type = u["type"];
        if(u_type==24 && u_name.begins_with("mterrain_")){
            int compression = -1;
            String name;
            Ref<RegExMatch> res = reg_compression->search(u_name);
            if(res.is_valid()){
                PackedStringArray finds = res->get_strings();
                compression = finds[1].to_int();
            }
            res = reg_name->search(u_name);
            if(res.is_valid()){
                PackedStringArray finds = res->get_strings();
                name = finds[1];
            }
            Dictionary uniform_info;
            uniform_info["uniform_name"] = u_name;
            uniform_info["name"] = name;
            uniform_info["compression"] = compression;
            uniforms.append(uniform_info);
        }
    }
}

void MTerrain::recalculate_terrain_config(const bool& force_calculate) {
    if(!is_inside_tree() && !force_calculate){
        return;
    }
    // Calculating max size
    max_size = (int8_t)(max_size_index - min_size_index);
    // Calculating max lod
    max_lod = (int8_t)(max_h_scale_index - min_h_scale_index);
    if(h_scale_list[max_h_scale_index] > size_list[min_size_index]){
        size_info.clear();
        notify_property_list_changed();
        ERR_FAIL_COND("min size is smaller than max h scale");
    }
    size_info.clear();
    size_info.resize(max_size+1);
    for(int i=0;i<size_info.size();i++){
        Array lod;
        lod.resize(max_lod+1);
        for(int j=0;j<lod.size();j++){
            if(j==lod.size()-1){
                lod[j] = true;
                continue;
            }
            lod[j] = i <=j;
        }
        size_info[i] = lod;
    }

    /// Calculating LOD distance
    lod_distance.resize(max_lod);
    int32_t ll = lod_distance[0];
    for(int i=1;i<lod_distance.size();i++){
        if(lod_distance[i] <= ll){
            lod_distance[i] = ll + 1; 
        }
        ll = lod_distance[i];
    }
    notify_property_list_changed();
}





int MTerrain::get_min_size() {
    return min_size_index;
}

void MTerrain::set_min_size(int index) {
    if(index >= max_size_index){
        return;
    }
    min_size_index = index;
    recalculate_terrain_config(false);
}

int MTerrain::get_max_size() {
    return max_size_index;
}

void MTerrain::set_max_size(int index) {
    if(index <= min_size_index){
        return;
    }
    max_size_index = index;
    recalculate_terrain_config(false);
}

void MTerrain::set_min_h_scale(int index) {
    if(index >= max_h_scale_index){
        return;
    }
    min_h_scale_index = index;
    recalculate_terrain_config(false);
}

int MTerrain::get_min_h_scale() {
    return min_h_scale_index;
}

void MTerrain::set_max_h_scale(int index) {
    if(index <= min_h_scale_index){
        return;
    }
    max_h_scale_index = index;
    recalculate_terrain_config(false);
}

int MTerrain::get_max_h_scale(){
    return max_h_scale_index;
}


void MTerrain::set_size_info(const Array& arr) {
    size_info = arr;
}
Array MTerrain::get_size_info() {
    return size_info;
}

void MTerrain::set_lod_distance(const PackedInt32Array& input){
    lod_distance = input;
}

PackedInt32Array MTerrain::get_lod_distance() {
    return lod_distance;
}

void MTerrain::_get_property_list(List<PropertyInfo> *p_list) const {
    //Adding lod distance property
    PropertyInfo sub_lod(Variant::INT, "LOD distance", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SUBGROUP);
    p_list->push_back(sub_lod);
    for(int i=0; i<lod_distance.size();i++){
        PropertyInfo p(Variant::INT,"M_LOD_"+itos(i),PROPERTY_HINT_NONE,"",PROPERTY_USAGE_EDITOR);
        p_list->push_back(p);
    }
    //Adding size info property
    for(int size=0;size<size_info.size();size++){
        Array lod_info = size_info[size];
        PropertyInfo sub(Variant::INT, "Size "+itos(size_list[size+min_size_index]), PROPERTY_HINT_NONE, "", PROPERTY_USAGE_SUBGROUP);
        p_list->push_back(sub);
        for(int lod=0;lod<lod_info.size();lod++){
            PropertyInfo p(Variant::BOOL,"SIZE_"+itos(size)+"_LOD_"+itos(lod)+"_HSCALE_"+itos(h_scale_list[lod+min_h_scale_index]),PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR);
            p_list->push_back(p);
        }
    }
}

real_t MTerrain::get_closest_height(const Vector3& pos) {
    return grid->get_closest_height(pos);
}

bool MTerrain::_get(const StringName &p_name, Variant &r_ret) const {
    if(p_name.begins_with("SIZE_")){
        PackedStringArray parts = p_name.split("_");
        if(parts.size() != 6){
            return false;
        }
        int64_t size = parts[1].to_int();
        int64_t lod = parts[3].to_int();
        Array lod_info = size_info[size];
        r_ret = lod_info[lod];
        return true;
    }
    if(p_name.begins_with("M_LOD_")){
        int64_t index = p_name.get_slicec('_',2).to_int();
        r_ret = (float)lod_distance[index];
        return true;
    }
    return false;
}


bool MTerrain::_set(const StringName &p_name, const Variant &p_value) {
    if(p_name.begins_with("SIZE_")){
        PackedStringArray parts = p_name.split("_");
        if(parts.size() != 6){
            return false;
        }
        int64_t size = parts[1].to_int();
        if(size==0){
            return false;
        }
        int64_t lod = parts[3].to_int();
        Array lod_info = size_info[size];
        lod_info[lod] = p_value;
        size_info[size] = lod_info;
        return true;
    }
    if(p_name.begins_with("M_LOD_")){
        int64_t index = p_name.get_slicec('_',2).to_int();
        int32_t val = p_value;
        if(val<0){
            return false;
        }
        lod_distance[index] = val;
        return true;
    }
    return false;
}