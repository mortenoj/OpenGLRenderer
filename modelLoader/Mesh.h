//
//  Mesh.h
//  GameForFuns
//
//  Created by Morten Omholt-Jensen on 20/04/2019.
//  Copyright © 2019 Morten Omholt-Jensen. All rights reserved.
//
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        this->setupMesh();
    }
    
    void draw(Shader shader) {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        
        for (GLuint i = 0; i < this->textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            
            stringstream ss;
            string number;
            string name = this->textures[i].type;
            
            if (name == "texture_diffuse") {
                ss << diffuseNr++;
            } else if (name == "texture_specular") {
                ss << specularNr++;
            }
            
            number = ss.str();
            glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }
        
        // Default shininess values
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        for (GLuint i = 0; i < this->textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0); // unbind
        }
    }
    
private:
    GLuint VAO, VBO, EBO;
    
    void setupMesh() {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);
        
        glBindVertexArray(this->VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
        
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) 0);
        
        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, normal));
        
        // Vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, texCoords));
        
        glBindVertexArray(0);
        
    }
    
};













