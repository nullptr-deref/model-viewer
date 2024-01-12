#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "wavefront/wavefront.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

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

GLuint *extract_geometry_indices(WavefrontGeometry *g) {
    const size_t indices_count = collect_total_indices_count(g);
    GLuint *indices = (GLuint *)malloc(indices_count * sizeof(GLsizei));
    size_t offset = 0;
    for (size_t i = 0; i < g->faces_count; i++) {
        memcpy(&indices[offset], g->faces[i].vertices_data, g->faces[i].vertices_count);
        offset += g->faces[i].vertices_count;
    }

    return indices;
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) {
        puts("Could not initialize GLFW.");
        return -1;
    }

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
    glGenBuffers(2, &gl_obj.buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, gl_obj.buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, g->vertices_count * sizeof(GLfloat), g->vertices, GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &gl_obj.vao);
    glBindVertexArray(gl_obj.vao);
    const GLsizei stride = sizeof(float) * 4;
    glVertexAttribPointer(0, 4, GL_FLOAT, 0, stride, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_obj.buffers[1]);
    GLuint *indices = extract_geometry_indices(g);
    const GLsizei indices_count = collect_total_indices_count(g);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(GLsizei), indices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        glBindBuffer(GL_ARRAY_BUFFER, gl_obj.buffers[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_obj.buffers[1]);
        glBindVertexArray(gl_obj.vao);
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, (void *)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(indices);

    glfwTerminate();
    return 0;
}
