#ifndef GLUTILS_H
#define GLUTILS_H

#include <exception>

class GLRuntimeException: public std::exception {
public:
    GLRuntimeException(std::string msg)
    : msg(msg)
    {}
private:
    virtual const char * what() const throw(){
        return msg.c_str();
    }
    std::string msg;
};

// print buffer status errors
GLuint glBufferStatus(const std::string msg = ""){
    GLuint e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(e){
        case GL_FRAMEBUFFER_UNDEFINED:
        throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_UNDEFINED\n") );
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n") );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n") );
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_UNSUPPORTED\n") );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            throw( GLRuntimeException(msg+" GLERROR: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n") );
            break;
    }
    return e;
}

// print gl error codes
GLuint glError(const std::string msg = ""){
    GLuint e = glGetError();
    switch(e){
        case GL_NO_ERROR:
         break;
        case GL_INVALID_ENUM:
            throw( GLRuntimeException(msg+" GL_INVALID_ENUM\n") );
            break;
        case GL_INVALID_VALUE:
            throw( GLRuntimeException(msg+" GLERROR: GL_INVALID_VALUE\n") );
            break;
        case GL_INVALID_OPERATION:
            throw( GLRuntimeException(msg+" GLERROR: GL_INVALID_OPERATION\n") );
            break;
        case GL_OUT_OF_MEMORY:
            throw( GLRuntimeException(msg+" GLERROR: GL_OUT_OF_MEMORY\n") );
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            throw( GLRuntimeException(msg+" GLERROR: GL_INVALID_FRAMEBUFFER_OPERATION\n") );
            break;
    }
    return e;
}

// compile a gl shader given a program and source code as const char *
void compileShader(GLuint & shaderProgram, const char * vert, const char * frag){

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vert,NULL);
    glCompileShader(vertexShader);

    // check it worked!
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw( GLRuntimeException( std::string("GLSL (VERTEX) ERROR: \n") + infoLog + "\n") );
    }

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&frag,NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        throw( GLRuntimeException( std::string("GLSL (FRAGMENT) ERROR: \n") + infoLog +"\n") );
    }

    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);

    // check it linked
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        throw( GLRuntimeException( std::string("GLSL (LINK) ERROR: \n") + infoLog + "\n") );
    }
}


#endif
