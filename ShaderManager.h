//a list to save all shader
//using name get shader

//To do :

//2.Multi-thread loading
//3.A Map table to speed up accessing shaders
#pragma once
#include "DirectxHelper.h"
#include <vector>



class ShaderManager
{
public:
	BOOL loadCompiledShader(std::string filename);
	BOOL LoadFolder(std::string foldername);
	ShaderObject* getShaderObj(std::string filename);

private:
	std::vector<ShaderObject> mShaderList;
	std::string mFolderName="Shaders\\";
	std::string mExtensionName = ".cso";
};

extern ShaderManager g_ShaderManager;