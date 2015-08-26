#Simple deferred shading with DirectX 12

A DirectX 12 implementation of basic deferred shading. 

<img src="https://github.com/shuhuai/D3D12_DeferredShading/blob/master/Demo.png" width="600x" height="450px"/>


The project consists of the following stages: 

1. Initialize D3D12 components.

2. Render the scene to G-Buffers.

3. A lighting pass to light the scene.  



##Initializing components:  
During the initialization stage, the D3D12 components are initialized as follows:

####1.D3D Device initialization.

a.	Create a d3d12 device and a swap chain.

b.	Create a command allocator and a command queues.

c.	Create a back-buffer RTV heap.

d.	Create RTVs for back-buffers.

####2.Load shader compiled code.

####3.Create deferred shading components.

a.	Create heaps for RTV, SRV, DSV and CBV.

b.	Create constant buffers resources (light and camera data).

c.	Create a quad model( vertex buffer resources and view).

d.	Create SRVs, RTVs and DSV.

e.	Create a descriptor table for the deferred shading.

f.	Create a PSO for G-buffers creation.

g.	Create a PSO for the screen-space lighting pass.

####4.Load models. 

a. Create a vertex buffer and an index buffer for the model.

b. Create a vertex buffer view and an index buffer view.



##Rendering the scene: 

The rendering stage is to render the scene to G-Buffers. There are a point light and a sphere model in the scene. The lighting pass uses the point light to calculate lighting with BRDF shading model. The specular shading model is microfacet specular BRDF with GGX NDF, and the diffuse shading model is Lambert. 

1. Update constant buffers.

2. Set the PSO for G-buffers creation.

3. Render models.

4. Set the PSO for screen-space lighting pass.

5. Render a quad to do screen-space lighting.



###A few management components in the project: 

1. Device manager: 
D3D12 global components management. 

2. Shader manager: 
Load shaders to memory. 

3. Camera Manager: 
A free-lock camera.
