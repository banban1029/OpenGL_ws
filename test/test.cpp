// test.cpp
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ウィンドウサイズ
const int WIDTH = 800;
const int HEIGHT = 600;

// 回転角
float angle = 0.0f;

// GLM + OpenGL データ
GLuint shaderProgram;
GLuint VAO;

// ESC キーで終了
void key_callback(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC
}

// シェーダー簡易コンパイル
GLuint compileShader(const char* src, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error: " << info << "\n";
    }
    return shader;
}

// シェーダープログラム作成
GLuint createShaderProgram() {
    const char* vs = R"(
        #version 460 core
        layout(location=0) in vec3 aPos;
        uniform mat4 transform;
        void main(){ gl_Position = transform * vec4(aPos,1.0); }
    )";
    const char* fs = R"(
        #version 460 core
        out vec4 FragColor;
        void main(){ FragColor=vec4(0.2,0.6,0.8,1.0); }
    )";

    GLuint vertex = compileShader(vs, GL_VERTEX_SHADER);
    GLuint fragment = compileShader(fs, GL_FRAGMENT_SHADER);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vertex);
    glAttachShader(prog, fragment);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(prog, 512, nullptr, info);
        std::cerr << "Program link error: " << info << "\n";
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return prog;
}

// 描画関数
void display() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 transform = glm::rotate(glm::mat4(1.0f),
                                      glm::radians(angle),
                                      glm::vec3(0, 0, 1));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // FreeGLUT で文字描画
    glColor3f(1, 1, 0);
    glRasterPos2f(-0.9f, 0.9f);
    const char* msg = "OpenGL 4.6 + FreeGLUT + GLM Demo";
    for (const char* c = msg; *c != '\0'; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    glutSwapBuffers();
}

// アイドル関数（回転更新）
void idle() {
    angle += 0.5f;
    if (angle > 360.0f) angle -= 360.0f;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Perfect: OpenGL 4.6 + FreeGLUT + GLM Demo");

    glutKeyboardFunc(key_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    glm::vec3 v1(1, 0, 0), v2(0, 1, 0);
    glm::vec3 cross = glm::cross(v1, v2);
    std::cout << "GLM cross product: " << cross.x << "," << cross.y << "," << cross.z << "\n";

    // 三角形データ
    float triangleVertices[] = { 0.0f,0.5f,0.0f, -0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f };

    glGenVertexArrays(1, &VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shaderProgram = createShaderProgram();

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
