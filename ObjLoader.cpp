
#pragma once

#define _USE_MATH_DEFINES
#include "ObjLoader.h"
#include <cmath>
#include <iostream>
#include <fstream>




static glm::vec3 colorForAllVertices = glm::vec3(1.f, 0.f, 0.f);
static void ScaleVertexs(std::vector<VtxPosClr>& vertexs);



struct VtxPosClr {
	glm::vec3 pos; // vertex position coordinates (x, y, z)
	glm::vec3 clr; // vertex color coordinates (r, g, b)
};

struct Normal
{
	glm::vec3 startPoint;
	glm::vec3 EndPoint;
};

struct model
{
	std::string m_ObjectName;
	std::vector<VtxPosClr> vertexIndicies; 
	std::vector<glm::vec3>normalIndicies;
	std::vector<Normal> m_debugdraw_normals;
	std::vector<VtxPosClr>temp_verts;
	std::vector<unsigned int> indexs;
	bool active = 0;
	
};



/*
*	When you call the object loader it populates a vector with every object file
* contained within the assets folder and any subdirectories you might have in
* assets.
*
*/
ObjLoader::ObjLoader()
{
	
	loadedObject = std::unique_ptr<model>(new model);

	if (std::filesystem::exists(location))
	{
		
		std::cout << "Files Read from the Directory:\n " <<
			" if your file is not found please re-add it and hit refresh" << std::endl;
		for (const auto& object : std::filesystem::recursive_directory_iterator(location))
		{
			
			std::cout << object << std::endl;
			++objectMapCount;
			auto filePath = object.path().filename();
			auto fileFull = filePath.string();
			auto lastIndex = fileFull.find_last_of(".");
			auto file = fileFull.substr(0, lastIndex);
			filepaths.push_back(file);
		}
		Objects.reserve(objectMapCount);
	}

}

ObjLoader::~ObjLoader()
{
	if (objectMapCount)
	{
		Objects.clear();
	}
}

void ObjLoader::Refresh() noexcept
{
	objectMapCount = 0;
	Objects.clear();
	std::cout << "Files Read from the Directory " <<
		" if your file is not found please re-add it and hit refresh" << std::endl;
	for (const auto& object : std::filesystem::recursive_directory_iterator(location))
	{

		
		std::cout << object << std::endl;
		++objectMapCount;
		auto filePath = object.path().filename();
		auto fileFull = filePath.string();
		auto lastIndex = fileFull.find_last_of(".");
		auto file = fileFull.substr(0, lastIndex);
		filepaths.push_back(file);
		std::cout << object << std::endl;
	}
	Objects.reserve(objectMapCount);
}

/*
* @func: LoadModel(const std::string& path)
* 
* @params: const std::string& path - name of the file to load.
* 
* 
*/

bool ObjLoader::LoadModel(const std::string& modelName) noexcept
{
	
	//check isvalidpath;
	std::ifstream validpath(location+modelName+".obj", std::ios::in);
	if (!validpath)
		return false;
	
	
	m_previous_loaded_model = loadedObject->m_ObjectName;
	Objects[m_previous_loaded_model] = std::move(loadedObject);
	//check if alreadycreated;
	auto it = Objects.find(modelName);
	if (it != Objects.end())
	{
		loadedObject.reset();
		loadedObject = Objects[modelName];
		loadedObject->m_ObjectName = modelName;
		return true;
	}


	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_normals;
	Objects[modelName] = std::shared_ptr<model>(new model);
	temp_vertices.clear();
	std::string _line;
	while (std::getline(validpath, _line))
	{
		std::istringstream lineHeader(_line);
		lineHeader >> m_lineTag;
		

			


		//vertices
		if (m_lineTag == "v")
		{
			VtxPosClr readVerticies = { glm::vec3(0), glm::vec3(0) };
		
			lineHeader >> readVerticies.pos.x >> readVerticies.pos.y >> readVerticies.pos.z;
		
			Objects[modelName]->temp_verts.push_back(readVerticies);
			
		}

		if (m_lineTag == "vn")
		{
			glm::vec3 vertexNormal;
			lineHeader >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;
			temp_normals.push_back(vertexNormal);

		}

		if (m_lineTag == "f")
		{
			unsigned int index[3],uvIndices[3],  normalIndex[3];

			

			//if there are only vertices with no normals or texture coords
			if (sscanf(_line.c_str(), "f %d %d %d\n", &index[0], &index[1], &index[2]) == 3)
			{
				vertexIndices.push_back(index[0]);
				vertexIndices.push_back(index[1]);
				vertexIndices.push_back(index[2]);
			}

			//if there are verticies and normals but no texcoords
			if(sscanf(_line.c_str(), "f %d//%d %d//%d %d//%d\n",&index[0], &normalIndex[0], &index[1], &normalIndex[1], &index[2], &normalIndex[2]) == 6)
			{
				vertexIndices.push_back(index[0]);
				vertexIndices.push_back(index[1]);
				vertexIndices.push_back(index[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}

			//if all 3 are defined however i do not do anything with the UV coords
		  if(sscanf(_line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n",&index[0], &uvIndices[0], &normalIndex[0], &index[1], &uvIndices[1], &normalIndex[1], &index[2], &uvIndices[2], &normalIndex[2]) == 9)
			{
			  vertexIndices.push_back(index[0]);
        vertexIndices.push_back(index[1]);
        vertexIndices.push_back(index[2]);
        normalIndices.push_back(normalIndex[0]);
        normalIndices.push_back(normalIndex[1]);
        normalIndices.push_back(normalIndex[2]);
      }		

			
		}

		
	}
	loadedObject.reset();
	loadedObject = std::move(Objects[modelName]);
	loadedObject->m_ObjectName = modelName;
	ScaleVertexs(loadedObject->temp_verts);
	

	for (unsigned int i = 0; i < vertexIndices.size(); ++i)
	{
		std::size_t vIndex = vertexIndices[i];
		loadedObject->indexs.push_back((vIndex-1));
		VtxPosClr vertex = loadedObject->temp_verts[vIndex-1];
		vertex.clr = colorForAllVertices;
		loadedObject->vertexIndicies.push_back(vertex);
		

	}
	GetVertexNormals();

	for (unsigned int i = 0; i < normalIndices.size(); ++i)
	{
		std::size_t nIndices = normalIndices[i];
		glm::vec3 normalIndex = temp_normals[nIndices-1];
		loadedObject->normalIndicies.push_back(normalIndex);
	}

	//setting color for lambertian shading. 
	for(unsigned int i = 0; i < loadedObject->vertexIndicies.size(); ++i)
	{
		static glm::vec3 light_pos = glm::normalize( glm::vec3(1/sqrt(3),1/sqrt(3), 1/sqrt(3)));
	  std::size_t nIndices = loadedObject->indexs[i];
		glm::vec3 weighedNormal = loadedObject->m_debugdraw_normals[nIndices].EndPoint;
		float lambertian = std::max(0.0f, glm::dot(weighedNormal, light_pos));
		loadedObject->vertexIndicies[i].clr = colorForAllVertices * lambertian;
	}
	

  if (loadedObject != NULL)
  {
		
		return true;
  }
	return false;
}

std::string ObjLoader::GetModelName()
{
	return loadedObject->m_ObjectName;
}

std::vector<VtxPosClr>& ObjLoader::GetVertexs()
{
	return loadedObject->vertexIndicies;
}

std::vector<Normal>& ObjLoader::GetDebugDrawNormals()
{
  return loadedObject->m_debugdraw_normals;
}

std::vector<glm::vec3>& ObjLoader::getNormalIndicies()
{
	return loadedObject->normalIndicies;
}

std::vector<unsigned int>& ObjLoader::GetIndexs()
{
   return loadedObject->indexs;
}

std::vector<VtxPosClr>& ObjLoader::GetrawVertexs()
{
  return loadedObject->temp_verts;
}

std::vector<std::string>& ObjLoader::GetFilepaths()
{
	return filepaths;
}


//WORKING SOLUTION  FOR MWA
void  ObjLoader::GetVertexNormals() {
	//if we failed to load the object dont load anything.
	if (loadedObject->vertexIndicies.empty()) {
		return;
	}

	loadedObject->m_debugdraw_normals.clear();
	//get the size of the buffer. we need the same amount of Raw verticies as the OBJ file.
	loadedObject->m_debugdraw_normals.resize(loadedObject->temp_verts.size());

	// get the face
	for (size_t i = 0; i < loadedObject->indexs.size(); i += 3) {
		// Get the vertex indices for this face
		uint32_t v1Index = loadedObject->indexs[i];
		uint32_t v2Index = loadedObject->indexs[i + 1];
	  uint32_t v3Index = loadedObject->indexs[i + 2];

		//Calculate the face normal
		glm::vec3 v1 = loadedObject->temp_verts[v1Index].pos;
		glm::vec3 v2 = loadedObject->temp_verts[v2Index].pos;
		glm::vec3 v3 = loadedObject->temp_verts[v3Index].pos;

		//get the edges of vertex V1V2 and V1V3
		glm::vec3 edge1 = v2 - v1;
		glm::vec3 edge2 = v3 - v1;

		//sin(theta) = |a x b| / (|a| |b|)
		// get length of cross product of edges
		auto face_normal = glm::cross(edge1, edge2);
		auto  faceNormalLength = glm::length(face_normal);

		//getthe edges of vertex V2V3 and V2V1
		glm::vec3 edge3 = v3 - v2;
		glm::vec3 edge4 = v1 - v2;
	

		//getthe edges of vertex V3V1 and V3V2
		glm::vec3 edge5 = v1 - v3;
		glm::vec3 edge6 = v2 - v3;



		// Calculate angles and weights for each vertex
		//gets the angle from inverse sin
		
		auto  angle1 = glm::asin(faceNormalLength/(glm::dot(glm::length(edge1),glm::length( edge2))));
		float faceweight1 = (float)(angle1 / M_PI);

		float angle2 = glm::asin(faceNormalLength / (glm::dot(glm::length(edge3), glm::length(edge4))));
		float faceweight2 = (float)(angle2 / M_PI);

		float angle3 = glm::asin(faceNormalLength / (glm::dot(glm::length(edge5), glm::length(edge6))));
		float faceweight3 = (float)(angle3 / M_PI);

		// Update the vertex normals with weighted contributions
		loadedObject->m_debugdraw_normals[v1Index].startPoint = v1;
		loadedObject->m_debugdraw_normals[v1Index].EndPoint +=(faceweight1 * face_normal);
		loadedObject->m_debugdraw_normals[v2Index].startPoint = v2;
		loadedObject->m_debugdraw_normals[v2Index].EndPoint += (faceweight2 * face_normal);
		loadedObject->m_debugdraw_normals[v3Index].startPoint = v3;
		loadedObject->m_debugdraw_normals[v3Index].EndPoint += (faceweight3 * face_normal);
	}

	// Normalize vertex normals
	for (Normal& normal : loadedObject->m_debugdraw_normals) {
		normal.EndPoint = normal.startPoint+ glm::normalize(normal.EndPoint)*.2f;

	}
	
}


static void ScaleVertexs(std::vector<VtxPosClr>& vertexs)
{
	// Calculating Center and unit scale

	glm::vec3 modelCenter(0.0f);
	for (const auto& vertex : vertexs) {
		modelCenter += vertex.pos;
	}
	modelCenter /= static_cast<float>(vertexs.size());

	// Offset the object to the center
	for (auto& vertex : vertexs) {
		vertex.pos -= modelCenter;
	}



	// Offset object to center

	float maxExtent = 0.0f;
	for (const auto& vertex : vertexs) {
		float extent = glm::length(vertex.pos);
		maxExtent = std::max(maxExtent, extent);
	}
	// Calculate the scaling factor to fit within a unit cube
	float scaleFactor = 1.0f / (maxExtent); // Assuming maxExtent represents half of the object's size


	for (auto& vertex : vertexs) {
		vertex.pos *= scaleFactor;
	}

}