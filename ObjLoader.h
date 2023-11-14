#pragma once 

#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <glm/glm.hpp>
#include <sstream>
#include <vector>



typedef struct  VtxPosClr VtxPosClr;
typedef struct model model;
typedef struct Normal Normal;


class ObjLoader
{
private:
	std::vector<std::string> filepaths;
	std::unordered_map<std::string, std::shared_ptr<model>> Objects;
	std::size_t objectMapCount = 0;
	std::shared_ptr<model> loadedObject;
	std::string m_previous_loaded_model;
	std::stringstream m_lineread;
	std::string m_lineTag;
	std::vector< VtxPosClr > temp_vertices;
	std::string location = "./assets/";
public:
	ObjLoader();
	~ObjLoader();
	void Refresh() noexcept;
	bool LoadModel(const std::string& path) noexcept;
	std::string GetModelName();
	std::vector<VtxPosClr>& GetVertexs();
	std::vector<Normal>& GetDebugDrawNormals();
	std::vector<glm::vec3>& getNormalIndicies();
  std::vector<unsigned>& GetIndexs();
	std::vector<VtxPosClr>& GetrawVertexs();
	std::vector<std::string>& GetFilepaths();
	void GetVertexNormals();
};
