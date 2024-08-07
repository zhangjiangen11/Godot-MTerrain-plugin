@tool
extends Control

signal layer_visibility_changed
signal layer_renamed
signal layer_index_changed
signal layer_removed
signal layer_selected
signal color_layer_selected
signal layer_merged_with_background


@onready var visibility_button = $hbox/visible
@onready var name_button = $hbox/name
@onready var merge_down_button = $hbox/merge_down
@onready var remove_button = $hbox/remove
@onready var move_up_button = $hbox/move_up
@onready var move_down_button = $hbox/move_down
@onready var rename_button = $hbox/rename
@onready var rename_input = $hbox/rename_input
var icon_visible = preload("res://addons/m_terrain/icons/eye.svg")
var icon_hidden = preload("res://addons/m_terrain/icons/eye-close.svg")

var selected = false

var confirmation_popup_scene = preload("res://addons/m_terrain/gui/mtools_layer_warning_popup.tscn")

func disconnect_signals():			
	for connection in name_button.get_signal_connection_list("pressed"):
		connection.signal.disconnect(connection.callable)	 
	for connection in remove_button.get_signal_connection_list("pressed"):
		connection.signal.disconnect(connection.callable)	 
	for connection in rename_button.get_signal_connection_list("pressed"):
		connection.signal.disconnect(connection.callable)	 
	for connection in rename_input.get_signal_connection_list("text_submitted"):
		connection.signal.disconnect(connection.callable)	 
	for connection in rename_input.get_signal_connection_list("focus_exited"):
		connection.signal.disconnect(connection.callable)	 
		
func init_for_heightmap():
	disconnect_signals()
	if name == "background": 
		rename_button.queue_free()
		remove_button.queue_free()
		move_up_button.queue_free()
		move_down_button.queue_free()
		visibility_button.queue_free()			
	name_button.text = name
	visibility_button.toggled.connect(change_visibility)	
	name_button.pressed.connect(select_heightmap_layer)
	remove_button.pressed.connect(remove_layer)
	merge_down_button.pressed.connect(merge_layer_down)
	rename_button.pressed.connect(begin_rename)
	rename_input.text_submitted.connect(end_rename)
	rename_input.focus_exited.connect(end_rename.bind(""))

func select_heightmap_layer():
	layer_selected.emit(name_button.text)

func change_visibility(toggle_on):
	visibility_button.icon = icon_hidden if toggle_on else icon_visible
	layer_visibility_changed.emit(name)

func remove_layer():
	var popup = confirmation_popup_scene.instantiate()
	add_child(popup)
	popup.confirmed.connect( func():
		layer_removed.emit(name)		
		queue_free()
	)

func merge_layer_down():
	var popup = confirmation_popup_scene.instantiate()
	add_child(popup)
	popup.confirmed.connect( func():
		layer_merged_with_background.emit(name)
		queue_free()
	)

func begin_rename():
	rename_input.text = name_button.text
	rename_input.visible = true
	name_button.visible = false
	rename_button.visible = false
	rename_input.grab_focus()
	
func end_rename(_new_name=""):
	if rename_input.text == "": return
	rename_input.visible = false
	name_button.visible = true
	rename_button.visible = true
	layer_renamed.emit(name_button, rename_input.text)
	

func init_for_colors():	
	disconnect_signals()
	rename_button.queue_free()
	rename_input.queue_free()
	remove_button.queue_free()
	move_up_button.queue_free()
	move_down_button.queue_free()
	merge_down_button.queue_free()
	visibility_button.queue_free()		

	name_button.text = name
	name_button.pressed.connect(select_color_layer)
	#remove_button.pressed.connect(remove_layer)
	#rename_button.pressed.connect(begin_rename)
	#rename_input.text_submitted.connect(end_rename)
	#rename_input.focus_exited.connect(end_rename.bind(""))

func select_color_layer():
	layer_selected.emit(get_index())
	
