#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SOIL2/SOIL2.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;

GLint textureFromFile(const char* path, string directory);

class Model {
public:
    
    Model(GLchar *path) {
        this->loadModel(path);
    }
    
    void draw(Shader shader) {
        for (GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].draw(shader);
        }
    }
    
private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;
    
    void loadModel(string path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        
        this->directory = path.substr(0, path.find_last_of('/'));
        this->processNode(scene->mRootNode, scene);
    }
    
    void processNode(aiNode *node, const aiScene *scene) {
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        
        // Children
        for (GLuint i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene);
        }
    }
    
    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        // Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        
        // Walk through each of the mesh's vertices
        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector(1); // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            
            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
            
            // Texture Coordinates
            // Does the mesh contain texture coordinates?
            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            } else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }
            
            vertices.push_back(vertex);
        }
        
        // Mesh faces
        for (GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for (GLuint j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        
        // Process materials
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // 1. Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            // 2. Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        return Mesh(vertices, indices, textures);
    }
    
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            
            GLboolean skip = false;
            
            for (GLuint j = 0; j < this->textures_loaded.size(); j++) {
                if (this->textures_loaded[j].path == str) {
                    textures.push_back(this->textures_loaded[j]);
                    skip = true;
                    
                    break;
                }
            }
            
            if (!skip) {
                Texture texture;
                texture.id = textureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                
                this->textures_loaded.push_back(texture);
            }
        }
        
        return textures;
    }
};

GLint textureFromFile(const char* path, string directory) {
    string filename = string(path);
    filename = directory + '/' + filename;
    GLuint textureId;
    glGenTextures(1, &textureId);
    
    int width, height;
    
    unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SOIL_free_image_data(image);
    
    return textureId;
}











