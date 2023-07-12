#include "mbrush_manager.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include "height_brushes/mraise.h"



MBrushManager::MBrushManager(){
    add_height_brush(memnew(MRaise));
}

void MBrushManager::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_height_brush_list"), &MBrushManager::get_height_brush_list);
    ClassDB::bind_method(D_METHOD("get_height_brush_id", "brush_name"), &MBrushManager::get_height_brush_id);
}

MBrushManager::~MBrushManager(){
    for(int i=0;i<height_brushes.size();i++){
        memdelete(height_brushes[i]);
    }
}

void MBrushManager::add_height_brush(MHeightBrush* brush){\
    ERR_FAIL_COND_MSG(height_brush_map.has(brush->_get_name()), "Duplicate brush name: "+brush->_get_name());
    height_brushes.append(brush);
    height_brush_map.insert(brush->_get_name(), height_brushes.size() -1);
}

MHeightBrush* MBrushManager::get_height_brush(int brush_id){
    ERR_FAIL_COND_V(brush_id >= height_brushes.size(), nullptr);
    return height_brushes[brush_id];
}

PackedStringArray MBrushManager::get_height_brush_list(){
    PackedStringArray out;
    for(int i=0;i<height_brushes.size();i++){
        out.append(height_brushes[i]->_get_name());
    }
    return out;
}

int MBrushManager::get_height_brush_id(String brush_name){
    if(!height_brush_map.has(brush_name)) return -1;
    return height_brush_map[brush_name];
}



