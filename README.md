# The Independent
Faculty project for course Computer Graphics. Project consists of a scene rendered by program written in C++ using OpenGL, GLFW, GLM and Glad. 

The project contains:
* **Models**: Islands, Lighthouse, Eyeball, House, Picnic Table, Round Table, Lamp, Tree
* **Lighting**: directional, spot, and point light implemented using Blinn-Phong lighting model
* **Shaders**: shader for rendering and and lightning models, skybox shader, hdr shader and blur shader.
* **Skybox cubemap**: 6 images of nebula combined to create a skybox and illusion of space;
* **Face culling**: Implemented in a way that inside of a lighthouse model is not rendered;
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
| B                        | Blinn/Phong Toggle            |
| H                        | HDR Toggle                    |
| J                        | Bloom Toggle                  |
| ***General***            |
| F                        | Flashlight (Camera spotlight) |
| Esc                      | Exit                          |