#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "mathLib2D.h"
#include "mathLib3D.h"
#include <vector>

bool loadOBJ(const char* path, 
        std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices, 
        std::vector<Point3D>& out_vertices, std::vector<Point2D>& out_uvs, std::vector<Vec3D>& out_normals);

#endif
