#include "stdafx.h"
#include "Mesh.h"





Mesh::Mesh(const std::string & fileName)
{
	if (!OpenMesh::IO::read_mesh(m_mesh, fileName))
	{
		std::cerr << "Failed to load mesh " << fileName << std::endl;
		exit(-1);
	}
	
	if (!m_mesh.has_vertex_normals())
		m_mesh.request_vertex_normals();
	if (!m_mesh.has_face_normals())
		m_mesh.request_face_normals();
	m_mesh.update_normals();

	TriMesh::VertexIter v_it, v_end(m_mesh.vertices_end());
	for (v_it = m_mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		TriMesh::Point thisVert = m_mesh.point(*v_it);
		m_vertices.push_back(thisVert);
		TriMesh::Normal thisNormal = m_mesh.normal(*v_it);
		m_vert_normals.push_back(thisNormal);
	}

	TriMesh::FaceIter f_it, f_end(m_mesh.faces_end());
	for (f_it = m_mesh.faces_begin(); f_it != f_end; ++f_it)
	{
		for (TriMesh::FaceVertexCCWIter fv_it = m_mesh.fv_ccwiter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			unsigned int thisIdx = fv_it->idx();
			m_face_indices.push_back(thisIdx);
		}
	}
	std::cout << fileName << " has been loaded." << std::endl;
}

Mesh::~Mesh()
{
}
