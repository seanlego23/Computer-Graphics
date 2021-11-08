#include <fstream>

#include "ObjModel.h"
#include "textures.h"

#pragma warning(push)
#pragma warning(disable : 26451)

ModelImporter::ModelImporter() { }

int ModelImporter::getNumOfVertices() { return vertices.size(); }

std::vector<glm::vec3> ModelImporter::getVertices() { return vertices; }

std::vector<glm::vec2> ModelImporter::getTextureCoordinates() { return texCoords; }

std::vector<glm::vec3> ModelImporter::getNormals() { return normals; }

std::vector<objMesh> ModelImporter::getMeshes() { return meshes; }

ObjModel::ObjModel(Material* m, glm::mat4 xForm, const char* filePath) {
    material = m;
    modelMatrix = xForm;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, VBO);
    glGenBuffers(1, &EBO);

    ModelImporter modelImporter = ModelImporter();
    modelImporter.parseObj(filePath);

    const std::vector<glm::vec3>& vertices = modelImporter.getVertices();
    const std::vector<glm::vec2>& texureCoords = modelImporter.getTextureCoordinates();
    const std::vector<glm::vec3>& normals = modelImporter.getNormals();

    std::vector<float> vboValues;
    std::vector<unsigned int> indices;

    for (int i = 0; i < modelImporter.getNumOfVertices(); i++) {
        vboValues.push_back(vertices[i].x);
        vboValues.push_back(vertices[i].y);
        vboValues.push_back(vertices[i].z);

        vboValues.push_back(normals[i].x);
        vboValues.push_back(normals[i].y);
        vboValues.push_back(normals[i].z);

        vboValues.push_back(texureCoords[i].x);
        vboValues.push_back(texureCoords[i].y);

        indices.push_back(i);
    }

    for (objMesh temp : modelImporter.getMeshes())
        meshes.push_back(temp);

    options.count = indices.size();

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vboValues.size() * sizeof(float), &vboValues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
};

void ObjModel::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) {
    if (meshes.size() == 0) {
        renderer::render(vMat, pMat, deltaTime, sg);
    } else {
        for (int i = 0; i < meshes.size(); i++) {

            glm::mat4 mvp;

            Material *m = Material::materials[meshes[i].name];
            unsigned int shaderID = m->use();

            mvp = pMat * vMat * modelMatrix;

            glUniformMatrix4fv(glGetUniformLocation(shaderID, "m"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "v"), 1, GL_FALSE, glm::value_ptr(vMat));
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "p"), 1, GL_FALSE, glm::value_ptr(pMat));

            glUniformMatrix4fv(glGetUniformLocation(shaderID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

            glUniform4f(glGetUniformLocation(shaderID, "material.ambient"), m->ambient.r, m->ambient.g, m->ambient.b, m->ambient.a);
            glUniform4f(glGetUniformLocation(shaderID, "material.diffuse"), m->diffuse.r, m->diffuse.g, m->diffuse.b, m->diffuse.a);
            glUniform4f(glGetUniformLocation(shaderID, "material.specular"), m->specular.r, m->specular.g, m->specular.b, m->specular.a);
            glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), m->shininess);
            if (m->getTextureID()) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m->getTextureID());
                glUniform1i(glGetUniformLocation(shaderID, "material.texture"), 0);
                glUniform1i(glGetUniformLocation(shaderID, "material.useTexture"), 1);
            } else
                glUniform1i(glGetUniformLocation(shaderID, "material.useTexture"), 0);
            glUniform1d(glGetUniformLocation(shaderID, "elapsedTime"), elapsedTime += deltaTime);

            glBindVertexArray(VAO);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);

            unsigned int endingVert = options.count;

            if ((i + 1) < meshes.size())
                endingVert = meshes[i + 1].startingVertex;

            glDrawElementsInstanced(options.renderType, endingVert - meshes[i].startingVertex, options.indexType, (void*)(meshes[i].startingVertex * sizeof(unsigned int)), options.instance_count);
        }
    }
}

static std::string getPathName(const std::string& s) {

    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(0, i) + "/");
    }

    sep = '\\';

    i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(0, i) + "/");
    }

    return("");
}

void ModelImporter::parseMTL(const char* filePath) {
    std::ifstream fileStream(filePath, std::ios_base::in);
    std::istringstream mtlStream("");
    std::string line = "";

    std::string mtlName = "";
    glm::vec4 ambient(0.0f);
    glm::vec4 diffuse(0.0f);
    glm::vec4 specular(0.0f);
    float specularExponent = 64.0f;
    unsigned int diffuseTexture = 0;

    std::string key;

    if (fileStream.good()) {
        while (!fileStream.eof()) {
            std::getline(fileStream, line);
            mtlStream.clear();
            mtlStream.str(line);

            mtlStream >> key;

            if (mtlStream.fail())
                continue;

            if (key[0] == '#')
                continue;

            if (key == "newmtl") {
                if (mtlName.size()) {
                    new Material(ambient, diffuse, specular, specularExponent, diffuseTexture, Shader::shaders["material"], mtlName);
                    mtlName.clear();
                    ambient = diffuse = specular = glm::vec4(0.0f);
                    specularExponent = 64.0f;
                    diffuseTexture = 0;
                }
                mtlStream >> mtlName;
            } else if (key == "Ns") {
                mtlStream >> specularExponent;
            } else if (key == "Ka" || key == "Kd" || key == "Ks") {
                float r, g, b;
                mtlStream >> r >> g >> b;
                
                glm::vec4&& colorVec = glm::vec4(r, g, b, 1.0f);
                if (key[1] == 'a')
                    ambient = colorVec;
                else if (key[1] == 'd')
                    diffuse = colorVec;
                else if (key[1] == 's')
                    specular = colorVec;
            } else if (key == "map_Kd") {
                std::string file;
                mtlStream >> file;
                diffuseTexture = loadTexture((getPathName(filePath) + file).c_str());
            }
        }
        new Material(ambient, diffuse, specular, specularExponent, diffuseTexture, Shader::shaders["material"], mtlName);
    }
}

void ModelImporter::parseObj(const char* filePath) {
    std::ifstream fileStream(filePath, std::ios_base::in);
    std::istringstream objStream("");
    std::string line;

    float x, y, z;

    std::string key;

    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        objStream.clear();
        objStream.str(line);

        objStream >> key;

        if (objStream.fail())
            continue;

        if (key[0] == '#')
            continue;

        if (key == "mtllib") {
            std::string fileName = objStream.str().substr(7); //Using substr allows for filenames with spaces
            fileName = getPathName(filePath) + fileName;
            parseMTL(fileName.c_str());
        } else if (key == "usemtl") {
            objMesh mesh;
            objStream >> mesh.name;
            if (meshes.empty() || meshes.back().name != mesh.name) {
                mesh.startingVertex = getNumOfVertices();
                meshes.push_back(mesh);
            }
        } else if (key == "v") {
            objStream >> x >> y >> z;
            vertexValues.push_back(glm::vec3(x, y, z));
        } else if (key == "vt") {
            objStream >> x >> y;
            texCoordValues.push_back(glm::vec2(x, y));
        } else if (key == "vn") {
            objStream >> x >> y >> z;
            normalValues.push_back(glm::vec3(x, y, z));
        } else if (key == "f") {
            std::string corner;
            std::vector<vertIndices> values;

            for (int i = 0; ; i++) {
                objStream >> corner;
                if (objStream.fail())
                    break;

                vertIndices value;
                value.ti = value.ni = value.vi = 0;

                // parse each face's indices
                if (sscanf(corner.c_str(), "%i/%i/%i", &value.vi, &value.ti, &value.ni) != 3) {
                    value.ti = value.ni = value.vi = 0;
                    if (sscanf(corner.c_str(), "%i//%i", &value.vi, &value.ni) != 2) {
                        value.ti = value.ni = value.vi = 0;
                        if (sscanf(corner.c_str(), "%i/%i", &value.vi, &value.ti) != 2) {
                            value.ti = value.ni = value.vi = 0;
                            if (sscanf(corner.c_str(), "%i", &value.vi) != 1)
                                std::cout << "unimplemented face format\n";
                        }
                    }
                }

                // account for relative indexing, adjust by number of preceding attributes
                if (value.vi < 0) 
                    value.vi += vertexValues.size() + 1;
                if (value.ni < 0)
                    value.ni += normalValues.size() + 1;
                if (value.ti < 0) 
                    value.ti += texCoordValues.size() + 1;

                values.push_back(value);

                if (i > 2) {
                    // if more than 3 verts in face, it's a quad, so re-emit 2 verts to cause a tri
                    values.push_back(values[0]);
                    values.push_back(values[values.size() - 1]); //Makes a triangle fan
                }
            }

            glm::vec3 cNormal(0.0f);
            if (values[0].ni == 0) {
                glm::vec3 ab, bc;

                ab = vertexValues[values[2].vi - 1] - vertexValues[values[1].vi - 1];
                bc = vertexValues[values[0].vi - 1] - vertexValues[values[1].vi - 1];

                cNormal = cross(ab, bc);
            }

            for (vertIndices value : values) {
                int vertRef = value.vi;
                int tcRef = value.ti;
                int normRef = value.ni;

                // references start at 1, not 0, so adjust
                vertRef--;
                tcRef--;
                normRef--;

                if (vertRef > -1) {
                    vertices.push_back(vertexValues[vertRef]);
                } else {
                    vertices.push_back(glm::vec3(0.0f));
                }

                if (tcRef > -1) {
                    texCoords.push_back(texCoordValues[tcRef]);
                } else {
                    texCoords.push_back(glm::vec2(0.0f));
                }

                if (normRef > -1) {
                    normals.push_back(normalValues[normRef]);
                } else {
                    normals.push_back(cNormal);
                }
            }
        }
    }
}

#pragma warning(pop)
