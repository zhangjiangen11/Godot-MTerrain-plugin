# godot-mterrain-plugin
## how to start
Watch first this youtube video:
https://www.youtube.com/watch?v=PcAkWClET4U
And then this video shows how to use use height brushes to modifying the terrain:
https://www.youtube.com/watch?v=e7nplXnemGo
## download
To downalod the latest release use this link:
https://github.com/mohsenph69/Godot-MTerrain-plugin/releases
## build by yourself
First clone this repo on your local machine, so you need godot-cpp to exist in GDExtension folder so you can build that, godot-cpp is added as a submodule in this project so to put that inside GDExtension folder only thing you need to do after cloning this repo is runing this code
```
git submodule update --init --recursive
```
This will automaticly pull godot-cpp into GDextension folder, After that go inside GDExtension folder and use scons to build this project
