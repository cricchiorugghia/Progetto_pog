#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <algorithm> // std::max
#include <cmath>     // sinf, cosf
#include <fstream>
#include <sstream>
#include <Vector2.h>
#include <Vector3.h>
#include <Matrix4.h>
#include "Camera.h"
#include "Texture.h"
#include <vector>
#include "Model.h"



Camera camera(10.0f, 0.0f, 0.0f);

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

        camera.ProcessInput(window);

}

//funzione per legegre file di testo
    std::string loadTextFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
    }

     // Funzione per controllare errori di compilazione/linking degli shader. Nel terminale, se scrivo ./app, mi compilerà il programma e mi dira quali errori sono presenti
    void checkShaderErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERRORE::SHADER_COMPILATION_ERROR di tipo: " << type << "\n" << infoLog << "\n -- -------------------------- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERRORE::PROGRAM_LINKING_ERROR di tipo: " << type << "\n" << infoLog << "\n -- ------------------ " << std::endl;
        }
    }
}



int main() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Progetto", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
    }

    int fbW, fbH;

    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);     // su Mac (Retina) è fondamentale

    Matrix4 Projection = Matrix4::perspective(60.0f, (float)fbW/(float)fbH, 0.01f, 1000.0f);


    glEnable(GL_DEPTH_TEST);

    // --- Compilazione shader Oggetto ---
    std::string objectVSsrc = loadTextFile("Shaders/default.vert"); // Assicurati che il percorso sia corretto!
    std::string objectFSsrc = loadTextFile("Shaders/default.frag"); // Assicurati che il percorso sia corretto!

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* ovs = objectVSsrc.c_str();
    glShaderSource(vertexShader, 1, &ovs, nullptr);
    glCompileShader(vertexShader);
    checkShaderErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* ofs = objectFSsrc.c_str();
    glShaderSource(fragmentShader, 1, &ofs, nullptr);
    glCompileShader(fragmentShader);
    checkShaderErrors(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkShaderErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // --- shader Luce ---
    std::string lightVSsrc = loadTextFile("shaders/light.vert");
    std::string lightFSsrc = loadTextFile("shaders/light.frag");
    
    GLuint lightVS = glCreateShader(GL_VERTEX_SHADER);
    const char* lvs = lightVSsrc.c_str();
    glShaderSource(lightVS, 1, &lvs, nullptr);
    glCompileShader(lightVS);
    checkShaderErrors(lightVS, "VERTEX (Luce)");
    
    GLuint lightFS = glCreateShader(GL_FRAGMENT_SHADER);
    const char* lfs = lightFSsrc.c_str();
    glShaderSource(lightFS, 1, &lfs, nullptr);
    glCompileShader(lightFS);
    checkShaderErrors(lightFS, "FRAGMENT (Luce)");
    
    GLuint LightProgram = glCreateProgram();
    glAttachShader(LightProgram, lightVS);
    glAttachShader(LightProgram, lightFS);
    glLinkProgram(LightProgram);
    checkShaderErrors(LightProgram, "PROGRAM (Luce)");
    
    glDeleteShader(lightVS);
    glDeleteShader(lightFS);



    
	//stbi_set_flip_vertically_on_load(true); // Inverti l'immagine ma Assimp: Il flag aiProcess_FlipUVs fa già questo lavoro



    Model Modello3D("modelli/scene.obj");
       

    GLfloat VerticiLuci[] =                         
    {       
        -0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,
         0.1f,  0.1f, -0.1f,
        -0.1f,  0.1f, -0.1f,
        -0.1f, -0.1f,  0.1f,
         0.1f, -0.1f,  0.1f,
         0.1f,  0.1f,  0.1f,
        -0.1f,  0.1f,  0.1f
 
    };

    GLuint IndiciLuci[] = 
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 7,
        0, 7, 3,
        0, 1, 5,
        0, 5, 4,
        1, 5, 6,
        1, 6, 2, 
        3, 2, 6,
        3, 6, 7,
        4, 5, 6,
        4, 6, 7,

    };

        //Crea il Mesh del cubo grande. TUTTO il setup (VAO, VBO, Attrib)
        //è ora gestito dal costruttore della classe Mesh.
        
       // Mesh cubo(vertici, indici);

        // --- Setup (lascia EBO bound al VAO, opzionale togliere Unbind) ---
        /*VAO vao;
        vao.Bind();

        VBO vbo(vertici, sizeof(vertici));
        EBO ebo(indici, sizeof(indici));*/

        // ===== CUBO LUCE =====
        VAO lightVAO;
        lightVAO.Bind();

        VBO lightVBO(VerticiLuci, sizeof(VerticiLuci));
        EBO lightEBO(IndiciLuci, sizeof(IndiciLuci));

        // solo posizione
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        lightVAO.Unbind();

        //Texture textureMuro("texture/muro.png");

        // Collega la texture al sampler tex0
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "tex0"), 0);

        Vector3 lightPos = Vector3(0.0f, 1.2f, 1.8f);       //posizione luce

   
        // Loop principale
        while (!glfwWindowShouldClose(window))
        {
            processInput(window);

      

            glClearColor(0.31f, 0.28f, 0.48f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       

            glUseProgram(shaderProgram);
      

            // Oggetto fermo
            Matrix4 Model = Matrix4::Identity();     //Creazione della matrice identità       

            // Posizione camera in orbita
            Matrix4 View = camera.GetViewMatrix();
            Vector3 eye = camera.GetPosition();
   

            // === DISEGNA CUBO GRANDE (OGGETTO) ===
            glUseProgram(shaderProgram);

           //Model = Matrix4::scale(Vector3(0.01f, 0.01f, 0.01f));


            // invio matrici
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Model"), 1, GL_TRUE,Model.data());
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "View"), 1, GL_TRUE, View.data());
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Projection"), 1, GL_TRUE, Projection.data());      //Projection.data() dice a OpenGL dove inizia la mia matrice in memoria (a)

            // invio luce + camera
            glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),  lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(shaderProgram, "camPos"),     eye.x, eye.y, eye.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "ambientColor"), 1.0f, 1.0f, 1.0f); // luce ambiente bianca


            // texture
            //textureMuro.Bind(0);

            //cubo.Draw();


            Modello3D.Draw();


            glUseProgram(LightProgram);

            // 1) colore bianco per la luce
            GLint lightColorLoc = glGetUniformLocation(LightProgram, "lightColor");
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);  // bianco puro

            // 2) MVP per il cubo luce (posizionato in lightPos)
            Matrix4 lightModel = Matrix4::Identity();
            lightModel = Matrix4::traslate(lightPos);       //traslazione cubo piccolo 
            Matrix4 lightMVP = (Projection.prod_mat_mat(View)).prod_mat_mat(lightModel);


            GLint mvpLocL = glGetUniformLocation(LightProgram, "MVP");
            glUniformMatrix4fv(mvpLocL, 1, GL_TRUE, lightMVP.data());

            // 3) disegno cubo luce
            lightVAO.Bind();
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        //cubo.Delete();
        Modello3D.Delete();
        glDeleteProgram(shaderProgram);
        glfwTerminate();
        return 0;
}
