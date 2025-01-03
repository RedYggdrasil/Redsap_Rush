# Redsap Rush
Redsap Rush is a **3D Space shooter** game in developpement.
It is being developped for **Windows** as a Win32 app in **C++** and **HLSL** using the **DirectX12** graphic library and Microsoft **Game/Direct Input**.

<div align="center"> 

  [![Link to a youtube video showcasing the project](https://quentin-noguier.fr/Curriculum_Vitae/images/RedsapRush/Redsap_Rush_Showcase_Github.png)](https://www.youtube.com/watch?v=maSV1JOmacY)
</div>


## Redsap Rush Branches
Redsap Rush contains two main branches.  
```redsap_rush__the_trench``` build the normal game scene using the ```#define START_SCENE EStartScene::TheTrench``` in the file .\App\CMake_Source\Public\Gen_App\Config.h.in.

```physic_demo_01``` build the physic demo scene using the ```#define START_SCENE EStartScene::PhysicDemo1``` in the file .\App\CMake_Source\Public\Gen_App\Config.h.in.

```main``` simply correspond to the project code up to date, and may point to either scenes, but currently point to ```redsap_rush__the_trench```.


## Build Redsap Rush from sources
To build Redsap Rush from these sources, you need to : 

0. Git clone the complete solution.
1. Generate the visual studio solution.
2. Build the project.
3. Copy the _Assets_ folder in the build directory.

### 0. Git clone the complete solution
**_In addition to DirectX12 and Game/Direct input, Redsap rush depend on [Assimp](https://github.com/assimp/assimp) for mesh loading and [Tracy](https://github.com/wolfpld/tracy) for profiling._**\
 Both are included into the visual studio project and built with Redsap Rush. **_To ensure your project is valid. please git clone recursively, as theses two projects are linked via git submodule._**

### 1. Generate the visual studio solution
Generating the visual studio solution can be by either :
#### 1.a. Using the .bat files
Simply run **RegenerateAll.bat** at the project root. this will automatically run _CleanBuild.bat_ and then _Generate_CMake.bat_.

#### 1.b. Running Cmake in command line
Write these lines in your CLI with the current context being the project's root directory.
```
mkdir "Build"
cd Build
cmake ..
```

### 2. Build the project
Run the newly created ```./Build/RedSap_Rush.sln``` and build the **App** project in **Debug** or **Release** for the **x64** platform.

### 3. Copy the _Assets_ folder in the build directory
Generating the visual studio solution can be by either :

#### 3.a. Using the .bat files
Simply run **CopyAssets.bat** at the project root. This will copy the ```./App/Assets``` folder to both the ```./Build/App/Debug``` and  and ```./Build/App/Release``` folders.

#### 3.b. By hands
Copy the ```./App/Assets``` folder to the newly generated ```./Build/App/Debug``` or ```./Build/App/Release``` folder, alonside the generated **App.exe** application (depending on if you build debug or release).  

This should result in a newly present **Assets** folder in the same directory as the **App.exe** application.


## Redsap Rush Build options
Redsap Rush can behave in various manners depending on a few CMake build options, these can be changed in the file ```.\App\CMake_Source\Public\Gen_App\Config.h.in```. Any change to this file will trigger a CMake visual studio solution regeneration during the next build.

### Scene selection
Redsap Rush currently has two scenes, but only run one or the other, this is because there is currently no menu to change scenes.
The starting scene can be the to the Redsap Rush game scene with this line :
```
#define START_SCENE EStartScene::TheTrench
```
Alternatively you can make the project run the physic demo scene with the line :
```
#define START_SCENE EStartScene::PhysicDemo1
```

### Use Mouse vs Xbox Controller
Redsap Rush currently has two control schemes, but only use one or the other, this is because there is currently no menu to change controls.

You can set the input method to keyboard and mouse using the line :
```
#define USE_START_INPUT EMainInputDevice::MouseKeyboard
```

Alternatively you can set the input method to Xbox controller with the line :
```
#define USE_START_INPUT EMainInputDevice::GamePad
```

### Use project _Assets_ folder vs build _Assets_ folder
The Redsap Rush app will use assets from either the project folder via using hard link generated by CMake, or locally, where the **App.exe** is located on the disk.

You can set **App.exe** to use _project_ Assets folder with the line :
```
#define USE_${APP_NAME}_PROJECT_ASSETS true
```

Alternatively you can set **App.exe** to use the local Assets folder with the line :
```
#define USE_${APP_NAME}_PROJECT_ASSETS false
```

### Draw physic colliders
The Redsap Rush app can draw the physic colliders in the 3D scene depending on their physic type, this option needs to be enabled globally, and then each physic type can be debug drawn or ignored.

To enable physic debugging, set _DEBUG_PHYSIC_ to 1 (was 0) :
```
#define DEBUG_PHYSIC 1
```
To enable or disable a physic type, set to 1, or 0 to disable said type. For exemple this will draw the collision primitives of _WORLD_DYNAMIC_ objects in the scene : 

```
#define DEBUG_PHYSIC_WORLD_DYNAMIC 1
```
