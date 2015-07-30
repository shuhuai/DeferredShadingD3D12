#Simple deferred shading with DirectX 12

A DirectX 12 implementation of basic deferred shading. 

<img src="https://github.com/shuhuai/D3D12_DeferredShading/blob/master/Demo.png" width="600x" height="450px"/>


The project consists of the following stages: 

1. Initializing D3D12 components.

2. A G-buffer creation pass to render the scene to G-Buffers.

3. A screen-space lighting pass to lighting the scene.  



##Initializing components :  
In the initialization stage, all D3D12 components need to be initialized. The D3D12 components are initialized as follows: 

####1.D3D Device initialization.

a.Create a d3d12 device and a swap chain.

b.Create a command allocator and command queues.

c.Create a back-buffer RTV heap.

d.Create RTVs for back-buffers.

####2.Load shader compiled code.

####3.Create deferred shading components.

a.Create heaps for RTV, SRV, DSV and CBV.

b.Create constant buffers resources(light and camera data).

c.Create a quad model( vertex buffer resources and view).

d.Create SRVs, RTVs and DSV.

e.Create a descriptor table for the deferred shading.

f.Create G-buffer creation PSO.

g.Create screen-space lighting pass PSO.

####4.Load models. 

a.Create vertex buffer and index buffer for the model.

b.Create vertex buffer view and index buffer view.



##Rendering the scene: 

In the rendering stage, a G-buffer creation pass is to render the scene to G-Buffers. 
The scene is quite simple, it includes a point light and a sphere model to show the shading result. 
The lighting pass uses the point light as a direct lighting with BRDF shading model. 
The specular shading model is microfacet specular BRDF with Ggx NDF, and the diffuse shading model is Lambert. 

1.Update constant buffers.

2.Set G-buffers creation PSO.

3.Render models.

4.Set screen-space lighting pass PSO.

5.Render the quad  to do screen-space lighting.



###A few management classes are used in this project: 

1.Device manager: 
  Some basic functions to do rendering, and the mannager helps to initialize and get the D3D12 global components. 


2.Shader manager: 
  Loading shaders to memory and accessing shaders for PSO. 


3.Camera Manager: 
A free-lock camera. 
