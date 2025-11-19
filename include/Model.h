#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include "Mesh.h"
#include "Texture.h"
//includo le intestazione di Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model
{
    public:

        //dati
        std :: vector<Mesh> meshes;  //un modello è fatto di una o più mesh

        /*std::string typeName;*/

        //costruttore
        Model(std :: string path);

        void Draw();

        void Delete();

    public:

        std :: string directory;
        std :: vector<Texture> texture_loaded;     

        //funzione privata per caricare il modello
        void loadModel(std :: string path);

        //processa ricorsivamente i 'nodi' della scena di Assimp
        void processNode(aiNode *node, const aiScene *scene);

        //'Traduce' un aiMesh di Assimp in una mia Mesh
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> loadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif