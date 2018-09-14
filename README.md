![screenshot](https://github.com/UglySwedishFish/Rak/blob/master/GUI/Welcome.png?raw=true)

Rak is a real-time hybrid path tracing engine written in C++. It currently uses OpenGL for rendering. Currently uses a SAH BVH as the acceleration structure.

Rak uses SFML for window setup and texture loading, assimp for model loading, glm for general maths and a modified version of sfml imgui. 

The repo for sfml imgui can be found [here](https://github.com/eliasdaler/imgui-sfml)

# Rak current version - 0.1a
Rak is currently **not** ready for any production (or even hobby) use. Rak is **seriously** lacking features and is in a **very** early stage of development. Thus, there will be no releases for this and many upcomming versions. There is no date for the first Rak release promised, as it is hard to estimate how long some of these features will take to implement. 

# New with Rak 0.1-0.19a
  - Completely new Material Creator, allowing for *3* material types. These are, *premade material*, *custom lambert material* and *Custom emissive material* 
  - Minor bug fixes
  **These bug fixes include** 
  - Fixed bug where traversal wouldn't work on certain gpus
  - Fixed **error** with templates in core.cpp which would fail to compile in certain compilers
  
# Planned for Rak 0.2a 
  - Saving and loading rak scenes. 
  - Proper camera 
  - More settings from gui 
# Planned until Rak 1.0
  - Point lights and spot lights 
  - Better directional shadows 
  - Ability to translate, rotate and scale models. 
  - Better BVH generation 
  - More Cache-friendly traversal of said BVH
  - Proper importing of models 
  - Tesselation
  - Way, way more premade materials 
  - 2 new material types (*Refractive material* and *Translucent material*) 
  - Proper documentation (both via github but also in the program itself
  - Better temporal + spatial filtering
  - Ability to actually render images, and save these rendered images to files. (png + jpeg)
  - Basic key-frame animation (position and scale, or rotation nor skeletal) 
  - Ability to have both rendered, solid and wireframe view. (currently only rendered is supported) 
  - And way, way more!
