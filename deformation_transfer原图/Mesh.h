#pragma once
#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;

class Mesh
{
public:
	Mesh(const std::string &fileName);
	~Mesh();

	std::vector<TriMesh::Point> m_vertices;
	std::vector<unsigned int> m_face_indices;
	std::vector<TriMesh::Normal> m_vert_normals;

private:
	TriMesh m_mesh;


};

