# Gabriel

![FullLogo_Transparent](https://user-images.githubusercontent.com/37471793/236649204-6807e8b6-7fc0-478b-9d2c-68d40dc3e595.png)

## Introduction

This project has a more or less educational goal on programming 3D real time graphics desktop application, from scratch. I am making this project on my own in order to have more practice in Computer Graphics.
It doesn't mean that I am inventing comletely all ideas from scratch. I am still referencing to some other resources that have a quick and deep guide on certain features and systems. 
Project is written in C++ with OpenGL API and other libraries. You will find a list of libraries below.

## References & Links
Channel or Tutorial   | Links
----------------------|-------
The Cherno            | https://www.youtube.com/@TheCherno
OGLDEV                | https://www.youtube.com/@OGLDEV
Learn OpenGL          | https://learnopengl.com/
QianMo                | https://gitea.yiem.net/QianMo/Real-Time-Rendering-4th-Bibliography-Collection
Victor Gordan         | https://www.youtube.com/@VictorGordan

## Libraries
Library | Links
--------|------
GLFW    | https://github.com/glfw/glfw
OpenGL Mathematics (GLM) | https://github.com/g-truc/glm
stb     | https://github.com/nothings/stb
Open Asset Import Library (assimp) | https://github.com/assimp/assimp
FreeType | https://freetype.org/
Dear ImGui | https://github.com/ocornut/imgui
spdlog | https://github.com/gabime/spdlog

## Features & Systems

#### Event Handling
  - [x] Monitor 
  - [x] Keyboard
  - [x] Mouse

#### Textures
  - [x] 2D
  - [x] Cubemap
  - [x] Image load/store operator
  - [x] Seamless cubemap
  - [x] Mipmapping
  - [x] Anisotropic Filtering
    
#### Camera
  - [x] Movement
  - [x] Look around
  - [x] Zoom in/out
  - [x] Select object
  - [x] Drag object
  - [x] Hover object  

#### ECS
  - [x] Entity
  - [x] Components
  - [x] Scene  

#### Lighting
  - [x] Phong
  - [x] Directional
  - [x] Point 
  - [x] Spot
  - [x] HDR environment
  - [ ] Emissive

#### Shadows
  - [x] Directional Shadow Mapping
  - [x] Point Shadow Mapping 

#### Rendering
  - [x] PBR (Physically Based Rendering)
  - [x] Deferred Shading
  - [x] Basic Raytracer using Compute Shader

#### IO
  - [x] Loading 3D models
  - [x] Loading materials
  - [x] Loading 3D skeletal animations
  - [x] Loading any image, including HDR image

#### Materials
  - [x] Albedo, Base Color
  - [x] Bumping
  - [x] Parallax Occlusion
  - [x] Emission
  - [x] Metallic
  - [x] Roughness
  - [x] Ambient Occlusion

#### Post-Processing effects
  - [x] Bloom
  - [x] Blur
  - [x] HDR (high-dynamic range color), Tone mapping, Exposure
  - [x] Gamma correction
  - [x] FXAA (Fast Approximate Anti-Aliasing)    
  - [x] SSAO (Screen Space Ambient Occlusion) 

## Gallery

### PBR and IBL
![Знімок екрана (39)](https://github.com/CheerWizard/Gabriel/assets/37471793/d294cd7a-2551-439e-98b7-b0b7b96f2b79)

### Blur and Bloom
![Знімок екрана (40)](https://github.com/CheerWizard/Gabriel/assets/37471793/cdb912c5-c2b9-4a67-abda-c163b7d519b1)

### HDR, Exposure, Tone mapping
![Знімок екрана (42)](https://github.com/CheerWizard/Gabriel/assets/37471793/861cc4e1-2a8d-4dc5-ba24-2aabe94f5c6a)
![Знімок екрана (41)](https://github.com/CheerWizard/Gabriel/assets/37471793/74e2bcff-1145-4ee3-ad12-4b799ba1a54a)

### Debug Positions 
![Знімок екрана (43)](https://github.com/CheerWizard/Gabriel/assets/37471793/0a73c768-06a4-4184-bae4-f26fb1af32cf)

### Debug Normals
![Знімок екрана (44)](https://github.com/CheerWizard/Gabriel/assets/37471793/a203f427-3941-4833-937d-c070a27faf9e)

### Debug Albedos
![Знімок екрана (45)](https://github.com/CheerWizard/Gabriel/assets/37471793/ca3cd0bd-9711-40b7-a884-cb5c14ecd83b)

### Debug PBR params (metalness, roughness, ambient occlusion)
![Знімок екрана (46)](https://github.com/CheerWizard/Gabriel/assets/37471793/88d468e1-efae-48f3-8501-84fd909877e3)

### Debug SSAO
![Знімок екрана (48)](https://github.com/CheerWizard/Gabriel/assets/37471793/a5694e06-c03f-4140-8761-04363355edf5)

### Debug Directional PCF Shadow Mapping
![Знімок екрана (47)](https://github.com/CheerWizard/Gabriel/assets/37471793/2c5fecfb-bbf5-4649-968b-db944b95ea21)

### UI viewports, docking, properties panel
![Знімок екрана (51)](https://github.com/CheerWizard/Gabriel/assets/37471793/69f9dfef-68ee-4851-b23f-634b5e87417a)
![Знімок екрана (49)](https://github.com/CheerWizard/Gabriel/assets/37471793/90daf308-e289-449a-970c-57df02114757)
![Знімок екрана (50)](https://github.com/CheerWizard/Gabriel/assets/37471793/30ddf023-fa3a-488b-ae46-37aa8dcc162b)

### UI Entity, Components panels
![Знімок екрана (52)](https://github.com/CheerWizard/Gabriel/assets/37471793/03a30862-8ce9-40c4-9b1f-857fb45e66ad)

### Emission Material
![Знімок екрана (65)](https://github.com/CheerWizard/Gabriel/assets/37471793/1c1b2039-36d3-4d4b-b74c-378015cb16c5)
![Знімок екрана (66)](https://github.com/CheerWizard/Gabriel/assets/37471793/01c5f52f-97a7-40e9-9d30-8766b73281e2)
![Знімок екрана (58)](https://github.com/CheerWizard/Gabriel/assets/37471793/67667e1a-edbd-4b2a-8e44-dbfe9837e812)
![Знімок екрана (60)](https://github.com/CheerWizard/Gabriel/assets/37471793/091902bc-7ec4-4128-99f0-50b933a2247b)

### FXAA
![Знімок екрана (75)](https://github.com/CheerWizard/Gabriel/assets/37471793/8a61648e-f4e3-4fd8-b190-d3164af27dda)
![Знімок екрана (76)](https://github.com/CheerWizard/Gabriel/assets/37471793/0b83ce7e-f42d-4460-9639-7fa221cdcc32)
