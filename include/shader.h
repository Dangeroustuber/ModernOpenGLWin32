#ifndef SHADER_H
#define SHADER_H

#include <gl/glew.h>
#include <stdbool.h>

typedef struct {
    unsigned int* buffer;
    size_t bufferLength;
} SpirvData;

// we do not keep a const char* to a vertex/fragment filepath so we can keep the function which reads the binary simpler.
// if we kept them seperate, we would effectively have to duplicate all the code in the function instead of just calling it twice
// with a different name.
typedef struct {
	GLuint program;
} Shader;

static inline bool readBinaryFileIntoSpirvData(const char* filename, SpirvData* data) {
    FILE* handle;
    int result;
    fopen_s(&handle, filename, "rb");
    if (handle == NULL) return false;

    fseek(handle, 0, SEEK_END);
    data->bufferLength = ftell(handle);
    rewind(handle);

    data->buffer = (unsigned int*)malloc(sizeof(unsigned int) * data->bufferLength);
    if (data->buffer == NULL) return false;

    result = fread(data->buffer, 1, data->bufferLength, handle);
    if (result != data->bufferLength) return false;
    fclose(handle);

    return true;
}

static inline bool compileShaders(Shader* shader, SpirvData* vertexShaderData, SpirvData* fragmentShaderData) {
    GLint compile = 0;
    GLint link = 0;
    char infoLog[1024];
    GLint maxLength = 0;
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexShaderData->buffer, vertexShaderData->bufferLength);
    glSpecializeShader(vertexShader, "main", NULL, NULL, NULL);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compile);
    if (compile == GL_FALSE) {
        FILE* outfile;

        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        glDeleteShader(vertexShader);

        outfile = fopen("vertex_compilation_error.txt", "w");

        int results = fputs(infoLog, outfile);
        if (results == EOF) {
            MessageBoxA(0, L"Unable to write to file", L"ERROR", 0);
            return false;
        }
        fclose(outfile);
        return false;
    }


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentShaderData->buffer, fragmentShaderData->bufferLength);
    glSpecializeShader(fragmentShader, "main", NULL, NULL, NULL);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compile);
    if (compile == GL_FALSE) {
        FILE* outfile;

        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        outfile = fopen("fragment_compilation_error.txt", "w");

        int results = fputs(infoLog, outfile);
        if (results == EOF) {
            MessageBoxA(0, L"Unable to write to file", L"ERROR", 0);
            return false;
        }
        fclose(outfile);

        return false;
    }

    shader->program = glCreateProgram();

    glAttachShader(shader->program, vertexShader);
    glAttachShader(shader->program, fragmentShader);

    glLinkProgram(shader->program);

    glGetProgramiv(shader->program, GL_LINK_STATUS, (int*)&link);
    if (link == GL_FALSE) {
        FILE* outfile;

        glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &maxLength);
        glGetProgramInfoLog(shader->program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(shader->program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        outfile = fopen("shader_link_error.txt", "w");

        int results = fputs(infoLog, outfile);
        if (results == EOF) {
            MessageBoxA(0, L"Unable to write to file", L"ERROR", 0);
            return false;
        }
        fclose(outfile);

        return false;
    }

    glDetachShader(shader->program, vertexShader);
    glDetachShader(shader->program, fragmentShader);

    free(vertexShaderData->buffer);
    free(fragmentShaderData->buffer);
    return true;
}
#endif // SHADER_H