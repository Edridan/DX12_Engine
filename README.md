# DX12_Engine
A DX12 mini game engine made for learning DirectX12 and rendering/game engine in general.

This is a WIP engine, and the code is not clean or final. I'm currently working on this project.

Currently the engine is featuring :
 * A little basic editor (using ImGui)
 * Game Object component architecture
 * Deferred Shading for lighting
 * Ability to load .obj meshes
 

# WIP
- Editor is actually working. Currently featuring :
 * Scene Editor
 * Actor Editor
Working on features :
 * Node editor for materials
 * Ability to save/load a scene
- Add a physic engine to the game engine (ReactPhysics) 
- Material management, now and because of the new implementation I've made, textures are currently not supported.

# Future
- Implementing Multi Threaded Rendering
- Ability to save scene with game component
- Make the engine more "game friendly"

# Libs
I use some open source libraries with the engine :

TinyobjLoader : https://github.com/syoyo/tinyobjloader

ReactPhysics : http://www.reactphysics3d.com/index.html

Imgui : https://github.com/ocornut/imgui

# Resources
DX12 Tutorial : https://www.braynzarsoft.net/viewtutorial/q16390-04-directx-12-braynzar-soft-tutorials

Meshes : https://free3d.com/3d-models/mesh
  - Low poly tree : https://free3d.com/3d-model/low-poly-tree-73217.html
