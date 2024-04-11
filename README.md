# The Independent
Faculty project for course Computer Graphics. Project consists of a scene rendered by program written in C++ using OpenGL, GLFW, GLM and Glad. 

The project contains:
* **Models**: Islands, Lighthouse, Eyeball, House, Picnic Table, Round Table, Candle, Tree
* **Lighting**: directional, spot, and point light implemented using Blinn-Phong lighting model
* **Shaders**: shader for rendering and and lightning models, skybox shader, hdr shader and blur shader.
* **Skybox cubemap**: 6 images of nebula combined to create a skybox and illusion of space;
* **Face culling**: Implemented in a way that inside of a lighthouse model is not rendered;
* **Blending**: Implemented with discarding parts of textures that have low alpha level. Specifically implemented with textures of flowers on the tree model. 
* **HDR and Bloom**

---------------------------
## **Commands**

| ***Key***                | ***Action***                  |
|--------------------------|-------------------------------|
| ***Camera Movement***    |
| W                        | Forward                       |
| S                        | Back                          |
| A                        | Left                          |
| D                        | Right                         |
| Scroll                   | Zoom                          |
| ***Advanced lightning*** |
| P                        | Blinn/Phong Toggle            |
| H                        | HDR Toggle                    |
| B                        | Bloom Toggle                  |
| ***General***            |
| F                        | Flashlight (Camera spotlight) |
| Esc                      | Exit                          |

---------------------------
# Recources
All models are free and downloaded from sketchfab.com from creators listed below.
* **Island model**: annaeregina69
* **Lighthouse model**: dylanheyes
* **Eyeball model**: Johana-PS
* **House model**: Helindu
* **Picnic table model**: Anserkon
* **Round table model**: CheBe
* **Tree model**: TatianaDevos 
* **Candle model**: Incg5764
