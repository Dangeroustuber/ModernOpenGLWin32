#ifndef SHADER_H
#define SHADER_H


#include <gl/glew.h>
#include <stdbool.h>

typedef struct {
    unsigned int* buffer;
    size_t bufferLength;
} SpirvData;

// we do not keep a const char* to a vertex/fragment filepath so we can keep the function which reads the binary simpler.
// if we kept them seperate, we would effectively have to double all the code in the function instead of just calling it twice
// with a different name.
typedef struct {
	GLuint program;
} Shader;

bool readBinaryFileIntoSpirvData(const char* filename, SpirvData* data) {
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

static inline compileShaders(Shader* shader, SpirvData* vertexShaderData, SpirvData* fragmentShaderData) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexShaderData->buffer, vertexShaderData->bufferLength);
    glSpecializeShader(vertexShader, "main", NULL, NULL, NULL);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentShaderData->buffer, fragmentShaderData->bufferLength);
    glSpecializeShader(fragmentShader, "main", NULL, NULL, NULL);

    shader->program = glCreateProgram();

    glAttachShader(shader->program, vertexShader);
    glAttachShader(shader->program, fragmentShader);

    glLinkProgram(shader->program);

    glDetachShader(shader->program, vertexShader);
    glDetachShader(shader->program, fragmentShader);

    free(vertexShaderData->buffer);
    free(fragmentShaderData->buffer);
}
#endif // SHADER_H