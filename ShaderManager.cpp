#include "stdafx.h"
#include "ShaderManager.h"
#include <Direct.h>
ShaderManager g_ShaderManager;

BOOL ShaderManager::loadCompiledShader(std::string filename)
{
	ShaderObject shaderObj;
	FILE* fpVS = nullptr;
	fopen_s(&fpVS, (filename).c_str(), "rb");
	if (!fpVS) { return FALSE; }
	fseek(fpVS, 0, SEEK_END);
	shaderObj.size = ftell(fpVS); 
	rewind(fpVS);
	shaderObj.binaryPtr = malloc(shaderObj.size);
	fread(shaderObj.binaryPtr, 1, shaderObj.size, fpVS);
	fclose(fpVS); 
	fpVS = nullptr;
	std::size_t strIdx = filename.find_last_of("/\\");
	std::size_t endIdx = filename.find_last_of(".");
	filename=filename.substr(strIdx +1,  (endIdx- strIdx)-1);
	
	shaderObj.name = filename;
	mShaderList.push_back(shaderObj);
	return TRUE;
}

BOOL ShaderManager::LoadFolder(std::string foldername)
{
	using namespace std;
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((foldername + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return 0; /* No files found */

	do {
		const string file_name = file_data.cFileName;
		std::size_t idx= file_name.find_last_of(".");
		string extension = file_name.substr(idx);
		if (extension.compare(mExtensionName) == 0)
		{
			const string full_file_name = foldername + "/" + file_name;


			loadCompiledShader(full_file_name);
			const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

			if (file_name[0] == '.')
				continue;

			if (is_directory)
				continue;
		}

	
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);

	return 0;
}

ShaderObject* ShaderManager::getShaderObj(std::string filename)
{
	for (unsigned int i = 0;i < mShaderList.size();i++)
	{
		if (mShaderList[i].name.compare(filename) == 0)
		{
			return &mShaderList[i];
		}
	}
	return nullptr;
}
