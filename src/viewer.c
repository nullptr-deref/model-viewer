#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "wavefront/wavefront.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

const char *vshadersrc =
"#version 330\n" \
"layout(location = 0) in vec4 position;\n" \
"void main() {\n" \
"gl_Position = position; }";

const char *fshadersrc =
"#version 330\n" \
"out vec4 color;\n" \
"void main() {\n" \
"color = vec4(1, 0, 0, 1); }";

const char *NAME = "Model Preview";
const size_t WIDTH = 640;
const size_t HEIGHT = 480;

const char *model_filepath = "./resources/male.obj";

typedef struct {
    GLuint buffers[2]; // VBO + indices
    GLuint vao;
} WavefrontGL;

size_t collect_total_indices_count(WavefrontGeometry *g) {
    size_t result = 0;
    for (size_t i = 0; i < g->faces_count; i++) {
        result += g->faces[i].vertices_count;
    }

    return result;
}

GLuint *extract_geometry_indices(WavefrontGeometry *g, GLuint *restart_index) {
    const size_t indices_count = collect_total_indices_count(g) + g->faces_count;
    *restart_index = indices_count;
    GLuint *indices = (GLuint *)malloc(indices_count * sizeof(GLuint));
    size_t offset = 0;
    for (size_t i = 0; i < g->faces_count; i++) {
        for (size_t j = 0; j < g->faces[i].vertices_count; j++) {
            indices[offset + j] = g->faces[i].vertices_data[j*3] - 1;
        }
        indices[offset + g->faces[i].vertices_count] = *restart_index;
        offset += g->faces[i].vertices_count + 1;
    }

    return indices;
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) {
        puts("Could not initialize GLFW.");
        return -1;
    }

    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(WIDTH, HEIGHT, NAME, NULL, NULL);
    if (!window) {
        puts("Could not initialize window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        puts("Could not load glew.");
        glfwTerminate();
        return -1;
    }

    FILE *objfile = fopen(model_filepath, "r");
    if (objfile == NULL) {
        printf("Cannot open %s: no such file or directory.\n", model_filepath);
        return 1;
    }

    WavefrontGeometry *g = wavefront_fread(objfile);
    WavefrontGL gl_obj;
    glGenVertexArrays(1, &gl_obj.vao);
    glBindVertexArray(gl_obj.vao);
    glGenBuffers(2, gl_obj.buffers);
    glBindBuffer(GL_ARRAY_BUFFER, gl_obj.buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, g->vertices_count * sizeof(GLfloat) * 4, g->vertices, GL_STATIC_DRAW);
    const GLsizei stride = sizeof(GLfloat) * 4;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_obj.buffers[1]);
    GLuint restart_index;
    GLuint *indices = extract_geometry_indices(g, &restart_index);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(restart_index);
    const GLsizei indices_count = collect_total_indices_count(g) + g->faces_count;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(GLuint), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vshadersrc, NULL);
    glCompileShader(vshader);

    GLint vcompiled;
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &vcompiled);
    if (vcompiled != GL_TRUE) {
        puts("Vertex shader sucks due to the following:");
        GLsizei loglen = 0;
        GLchar msg[1024];
        glGetShaderInfoLog(vshader, 1024, &loglen, msg);
        puts(msg);
    }
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fshadersrc, NULL);
    glCompileShader(fshader);

    GLint fcompiled;
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &fcompiled);
    if (fcompiled != GL_TRUE) {
        puts("Fragment shader sucks due to the following:");
        GLsizei loglen = 0;
        GLchar msg[1024];
        glGetShaderInfoLog(fshader, 1024, &loglen, msg);
        puts(msg);
    }
    GLuint p = glCreateProgram();
    glAttachShader(p, vshader);
    glAttachShader(p, fshader);
    glLinkProgram(p);
    GLint linked;
    glGetProgramiv(p, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        puts("Program was not linked:");
        GLsizei loglen = 0;
        GLchar msg[1024];
        glGetProgramInfoLog(p, 1024, &loglen, msg);
        puts(msg);
    }

    glUseProgram(p);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("E: %d\n", err);
    }
    glBindVertexArray(gl_obj.vao);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        // glDrawArrays(GL_POINTS, 0, g->vertices_count);
        glDrawElements(GL_TRIANGLE_FAN, indices_count, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(indices);
    wavefront_geometry_free(g);

    glfwTerminate();
    return 0;
}
