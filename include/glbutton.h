#ifndef GLBUTTON_H
#define GLBUTTON_H

#include <glm/glm.h>

class Button {
public:
    glm::vec2 position;   // screen coordinates (e.g. pixels)
    glm::vec2 size;
    std::string label;

    GLuint VAO, VBO;
    bool isHovered = false;

    Button(glm::vec2 pos, glm::vec2 sz, const std::string& text)
        : position(pos), size(sz), label(text) {
        createGeometry();
    }

    void createGeometry() {
        // Quad vertices: position (x,y) + UV (optional)
        float vertices[] = {
            // x, y
            position.x,          position.y,           // top-left
            position.x + size.x, position.y,           // top-right
            position.x + size.x, position.y + size.y,  // bottom-right

            position.x,          position.y,           // top-left
            position.x + size.x, position.y + size.y,  // bottom-right
            position.x,          position.y + size.y   // bottom-left
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void update(glm::vec2 mousePos) {
        isHovered = (mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
            mousePos.y >= position.y && mousePos.y <= position.y + size.y);
    }

    void draw(Shader& shader) {
        shader.use();
        shader.setVec3("color", isHovered ? glm::vec3(0.8f, 0.8f, 1.0f) : glm::vec3(0.3f, 0.3f, 0.8f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    ~Button() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

#endif
