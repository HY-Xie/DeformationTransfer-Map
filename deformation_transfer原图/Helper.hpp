#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cassert>
#include "Mesh.h"
namespace Help
{


void readCorresIdx(const std::string &fileName, std::vector<int> &left, std::vector<int> &right);
void readCorresVert(const std::string & corrFile, Mesh *leftMesh, Mesh * rightMesh, std::vector<TriMesh::Point> &left, std::vector<TriMesh::Point> &right);
void combine(std::vector<TriMesh::Point> &left, std::vector<TriMesh::Point> &right, std::vector<TriMesh::Point> &together);

void readCorresIdx(const std::string &fileName, std::vector<int> &left, std::vector<int> &right)
{
	std::ifstream file(fileName);
	if (file.is_open())
	{
		int numOfCorres;
		file >> numOfCorres;
		std::cout << numOfCorres << std::endl;

		int leftCorr, rightCorr;
		while (file >> leftCorr >> rightCorr)
		{
			left.push_back(leftCorr-1);
			right.push_back(rightCorr-1);
		}
	}
	
}

void readCorresVert(const std::string & corrFile, Mesh *leftMesh, Mesh * rightMesh, std::vector<TriMesh::Point> &left, std::vector<TriMesh::Point> &right)
{
	assert(leftMesh->m_vertices.size() != 0 && rightMesh->m_vertices.size() != 0 );

	std::vector<int> leftIdx, rightIdx;
	readCorresIdx(corrFile, leftIdx, rightIdx);
	assert(leftIdx.size() == rightIdx.size());

	for (int i = 0; i < leftIdx.size(); ++i)
	{
		left.push_back(leftMesh->m_vertices[leftIdx[i]]);
		right.push_back(rightMesh->m_vertices[rightIdx[i]]);
	}

}

void combine(std::vector<TriMesh::Point> &left, std::vector<TriMesh::Point> &right, std::vector<TriMesh::Point> &together)
{
	together.clear();
	assert(left.size() == right.size());
	for (int i = 0; i < left.size(); ++i)
	{
		together.push_back(left[i]);
		together.push_back(right[i]);
	}

}
}