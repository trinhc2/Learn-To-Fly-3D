#include "mathLib3D.h"
#include "coin.h"
#include <iostream>
#include "main.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <string>

Coin::Coin(float x, float y, float z) {
    position = Point3D(x, y, z);
}

CoinSystem::CoinSystem() {
  // generate random coins acorss the map
  for(int i=0; i<50; i++) {
	v.push_back(Coin(0.5, randomFloat(1000), randomFloat(9) - 4.5));
  //randomFloat(9) - 4.5 generates number between -4.5 and 4.5;
  }
}

void CoinSystem::update(Rocket& r) {
  //Continuous rotation
  if (rotation > 360) {
	rotation = 0;
  } else {
	rotation += 1;
  }
  //If coin is collected then remove it from the game (the vector)
  for (std::size_t i = 0; i < v.size(); i++) {
	  //Obstacles are 1 unit wide (0.5) and rocket is 0.3 units wide (0.15)
    if (inRangeY(v.at(i).position.mY + 0.65, v.at(i).position.mY - 0.65, r)
		  && inRangeZ(v.at(i).position.mZ + 0.65, v.at(i).position.mZ - 0.65, r)) {
	    //if collision: remove coin from game and increment coins total
	    v.erase(v.begin() + i);
	    r.coins += 100;
	  }
    //If coin is behind rocket, then we don't need to keep track of it anymore
    else if (v.at(i).position.mY + 2< r.position.mY + r.forwardDistance){
      v.erase(v.begin() + i);
    }
  }
}


bool CoinSystem::loadOBJ(const char* path) {
    FILE* file = fopen(path, "r");

    std::vector<Point3D> temp_vertices; 
    std::vector<Point2D> temp_uvs;
    std::vector<Vec3D> temp_normals;

    if (file == NULL) {
        std::cout << "Error opening obj file" << std::endl;
        // Exit if we encounter an error; could potentially just return false instead but the program won't look right anyway
        exit(1);
    }

    while(1) {
        char lineHeader[128];
        // Read what the line starts with (v, vt, vn, or f)
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;
        // Vertex
        if (strcmp( lineHeader, "v" ) == 0 ){
            // Create point for holding values
            Point3D vertex;
            // Get the values from the file
            fscanf(file, "%f %f %f\n", &vertex.mX, &vertex.mY, &vertex.mZ );
            // Push the vertex into our vertices array
            temp_vertices.push_back(vertex);
        // UV
        } else if (strcmp( lineHeader, "vt" ) == 0 ) {
            Point2D uv;
            fscanf(file, "%f, %f\n", &uv.mX, &uv.mY);
            temp_uvs.push_back(uv);
        // Normals
        } else if ( strcmp( lineHeader, "vn" ) == 0 ) {
            Vec3D normal;
            fscanf(file, "%f %f %f\n", &normal.mX, &normal.mY, &normal.mZ );
            temp_normals.push_back(normal);
        } else if ( strcmp( lineHeader, "f" ) == 0 ) {
        // Faces
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9) {
                printf("Error reading file\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        } else {
            // A comment or something that's not v, vt, vn, or f - just eat the rest of the line
            char buf[1000];
            fgets(buf, 1000, file);
        }
    }

    for(unsigned int i = 0; i < vertexIndices.size(); i++ ){
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // obj indices start at 1, C++ indices start at 0, so -1
        Point3D vertex = temp_vertices[vertexIndex-1];
        Point2D uv = temp_uvs[uvIndex-1];
        Vec3D normal = temp_normals[normalIndex-1];

        out_vertices.push_back(vertex);
        out_uvs     .push_back(uv);
        out_normals .push_back(normal);
    }

    // Close file after everything is done
    fclose(file);

    // Return true to signify that obj file was read successfully
    return true;

    // NOTE! We use these resulting vertex/uv/normal/face arrays in the main program, not inside the Rocket class
}
