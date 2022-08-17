#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

using namespace std;

unsigned int createShader(unsigned int shadertype, const char* shaderSource) {
  unsigned int shader;
  shader = glCreateShader(shadertype);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  int status;
  char info[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    glGetShaderInfoLog(shader, 512, NULL, info);
    cout << "FAILED TO COMPILE!\n" << info << '\n';
    glDeleteShader(shader);
  }

  return shader;
}

unsigned int createShaderProgram(const char* VshaderSrc, const char* FshaderSrc) {
  unsigned int vertexShader = createShader(GL_VERTEX_SHADER, VshaderSrc);
  unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, FshaderSrc);

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int status;
  char info[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, info);
    cout << "FAILED TO LINK PROGRAM\n" << info << '\n';
  }

  return shaderProgram;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void closeWindow(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void cobwebDiagmramForLogistics(float x0, float miu, float* ptr) {
  float y = miu * x0 * (1 - x0);
  *(ptr) = x0 * 2 - 1;
  *(ptr + 1) = -1.0f;
  *(ptr + 2) = 0.0f;

  *(ptr + 3) = x0 * 2 - 1;
  *(ptr + 4) = y * 2 - 1;
  *(ptr + 5) = 0.0f;

  for (int i = 1; i < 196; i++) {

    *(ptr + i * 6) = y * 2 - 1;
    *(ptr + i * 6 + 1) = y * 2 - 1;
    *(ptr + i * 6 + 2) = 0.0f;
    *(ptr + i * 6 + 3) = y * 2 - 1;

    y *= (1 - y) * miu;
    *(ptr + i * 6 + 4) = y * 2 - 1;
    *(ptr + i * 6 + 5) = 0.0f;
  }
}

int main() {
  float x0 = -1.0f, miu = 4.1f;
  while (x0 < 0.0f || x0 > 1.0f || miu < 0.0f || miu > 4.0f) {
      cout << "Please enter values of x0 and miu, range of x0 is from 0 to 1, range of miu is from 0 to 4\n";
      cin >> x0 >> miu;
  }
  float CobwebPlot[1188];
  cobwebDiagmramForLogistics(x0, miu, &CobwebPlot[0]);
  vector<unsigned int> v;
  v.push_back(0);
  for (int i = 1; i < 196; i += 2) {
      v.push_back(i);
      v.push_back(i);
      v.push_back(i + 1);
      v.push_back(i + 1);
  }
  v.push_back(197);
  int size = v.size();
  unsigned int positions[394];
  for (int i = 0; i < v.size(); i++) {
      positions[i] = v[i];
  }

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Cobweb_Diagram", NULL, NULL);

  if (window == NULL) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      cout << "Failed to initialize GLAD" << endl;
      return -1;
  }

  unsigned int cobwebBuffer;
  unsigned int bufferArray;
  unsigned int elementArray;
  glGenVertexArrays(1, &bufferArray);
  glGenBuffers(1, &cobwebBuffer);
  glGenBuffers(1, &elementArray);

  glBindVertexArray(bufferArray);

  glBindBuffer(GL_ARRAY_BUFFER, cobwebBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CobwebPlot), CobwebPlot, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArray);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(positions), positions,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  const char* vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

  const char* fragmentShaderSource = R"(
      #version 330 core
      out vec4 lineColor;
      void main()
      {
          lineColor = vec4(1.0f, 0.1f, 0.2f, 1.0f);
      }
)";

  unsigned int shaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  while (!glfwWindowShouldClose(window)) {
    closeWindow(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(bufferArray);
    glUseProgram(shaderProgram);
    glDrawElements(GL_LINES, 394, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}