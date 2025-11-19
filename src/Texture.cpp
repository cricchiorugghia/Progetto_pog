#include "Texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//costruttore
Texture :: Texture(const std:: string& path):
ID_texture(0)
{
    //variabili locali
    int tex_width, tex_height, tex_channels;
    std::string typeName;

    std::cout << "DEBUG: Test caricamento file: " << path << std::endl;

    unsigned char* data = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, 0);

    if(data)
    {
        GLenum internalFormat; // Formato in cui la GPU memorizzerà i dati
        GLenum dataFormat;     // Formato in cui i dati sono nella RAM (da stb_image)

        if (tex_channels == 1)
        {
            internalFormat = GL_RED;
            dataFormat = GL_RED;
        }
        else if (tex_channels == 3)
        {
            internalFormat = GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (tex_channels == 4)
        {
            internalFormat = GL_RGBA;
            dataFormat = GL_RGBA;
        }
        else
        {
            // Questo è un caso di errore, o un formato non gestito.
            // Potresti stampare un errore o usare un default.
            std::cout << "ATTENZIONE: Numero di canali texture non supportato: " << tex_channels << std::endl;
            internalFormat = GL_RGB; // Fallback
            dataFormat = GL_RGB;    // Fallback
        }

    

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disabilita l'allineamento a 4 byte
    // Genera e collega la texture
        glGenTextures(1, &ID_texture); // Usa la tua variabile
        glBindTexture(GL_TEXTURE_2D, ID_texture); // Usa la tua variabile

        // Imposta i parametri della texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Carica i dati dell'immagine sulla GPU
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tex_width, tex_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // **** RIPRISTINA L'ALLINEAMENTO DI DEFAULT (molto importante!) ****
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);


        // Libera la memoria della CPU
        stbi_image_free(data);
    }
    else
    {
        std::cout << "ERRORE: Impossibile caricare la texture: " << path << std::endl;
        std::cout << "RAGIONE DEL FALLIMENTO STB: " << stbi_failure_reason() << std::endl;

        // Questo è fondamentale: se fallisce, l'ID deve essere invalido.
        ID_texture = 0;
    }
}

// --- Distruttore ---
Texture::~Texture()
{
    // Il distruttore pulisce la risorsa OpenGL
    //glDeleteTextures(1, &ID_texture); // Usa la tua variabile
}

// --- Bind ---
void Texture::Bind(GLuint slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID_texture); // Usa la tua variabile
}

// --- Unbind ---s
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}


