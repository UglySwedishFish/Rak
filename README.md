![screenshot](https://github.com/UglySwedishFish/Rak/blob/master/GUI/Welcome.png?raw=true)

Rak is a real-time hybrid path tracing engine written in C++. It currently uses OpenGL for rendering. Currently uses a SAH BVH as the acceleration structure.

Rak uses SFML for window setup and texture loading, assimp for model loading, glm for general maths and a modified version of sfml imgui. 

The repo for sfml imgui can be found [here](https://github.com/eliasdaler/imgui-sfml)

# Rak current version - 0.11a
Rak is currently **not** ready for any production (or even hobby) use. Rak is **seriously** lacking features and is in a **very** early stage of development. Thus, there will be no releases for this and many upcomming versions. There is no date for the first Rak release promised, as it is hard to estimate how long some of these features will take to implement. 

# New with Rak 0.11a
  - Fixed bug where material selection wouldn't always select the correct material
  - Fixed bug where emissive objects would be completely white if emission was set too high 
  - Fixed bug with premade materials not being properly handled in lighting
  - **[Indev]** Added preview mode to all the different materials, making material selection far better
 
# New with Rak 0.1a
  - Completely new Material Creator, allowing for *3* material types. These are, *premade material*, *custom lambert material* and *Custom emissive material
  - Fixed bug where traversal wouldn't work on certain gpus
  - Fixed **error** with templates in core.cpp which would fail to compile in certain compiler
  - Other minor bug fixes

  
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
  - (Potentially) Faster Ray/Triangle intersection. 
  - Proper importing of models 
  - Tesselation
  - Compositing & Post processing effects (this includes bloom/glow, bokeh dof, proper color correction) 
  - Way, way more premade materials 
  - 3 new material types (*Refractive material*, *Shadow catcher*, *Translucent material*)
  - Custom textured materials (right now only solid values are supported) 
  - Properly physically acurate bsdfs (right now its mostly just guesswork) 
  - Proper documentation (both via github but also in the program itself
  - Improved temporal stability in both checkerboard system and temporal rendering 
  - Improved spatial filtering that uses physically accurate weights instead of kernels. 
  - Ability to actually render images, and save these rendered images to files. (png + jpeg)
  - Basic key-frame animation (position and scale, not rotation nor skeletal) 
  - Ability to have both rendered, solid and wireframe view. (currently only rendered is supported) 
  - And way, way more!
