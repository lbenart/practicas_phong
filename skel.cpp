#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>

// GLM library to deal with matrix operations
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "textfile_ALT.h"

unsigned int loadTexture(const char *path);
int gl_width = 640;
int gl_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void render(double);

GLuint shader_program = 0; // shader program to set render pipeline

GLuint shader_lamp = 0; 

GLuint cube_vao = 0; // Vertext Array Object to set input data

GLuint light_vao = 0;

GLint model_location, view_location, proj_location; // Uniforms for transformation matrices
GLint model_lamp, view_lamp, proj_lamp; // Uniforms for transformation matrices

GLint normal_location, camera_location;
GLint l_ambient, l_diffuse, l_specular, l_position;

GLint m_ambient, m_diffuse, m_specular, m_shininess;

unsigned int diffuseMap;

// Shader filenames for cube
const char *vertexFileName = "skel_vs.glsl";
const char *fragmentFileName = "skel_fs.glsl";

// Shader filenames for lamp
const char *vertexFileNameLamp = "lamp_vs.glsl";
const char *fragmentFileNameLamp = "lamp_fs.glsl";

// Camera
glm::vec3 camera_pos(0.0f, 0.0f, 3.0f);

// Lighting
//glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

glm::vec3 light_pos(0.5f, 0.5f, 0.5f);
glm::vec3 light_ambient(0.2f, 0.2f, 0.2f);
glm::vec3 light_diffuse(0.5f, 0.5f, 0.5f);
glm::vec3 light_specular(1.0f, 1.0f, 1.0f);

// Material
glm::vec3 material_ambient(1.0f, 0.5f, 0.31f);
glm::vec3 material_diffuse(1.0f, 0.5f, 0.31f);
glm::vec3 material_specular(0.5f, 0.5f, 0.5f);
const GLfloat material_shininess = 32.0f;

int main() {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  //  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(gl_width, gl_height, "My spinning cube", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  // glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte* vendor = glGetString(GL_VENDOR); // get vendor string
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* glversion = glGetString(GL_VERSION); // version as a string
  const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string
  printf("Vendor: %s\n", vendor);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", glversion);
  printf("GLSL version supported %s\n", glslversion);
  printf("Starting viewport: (width: %d, height: %d)\n", gl_width, gl_height);

  // Enable Depth test: only draw onto a pixel if fragment closer to viewer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // set a smaller value as "closer"

  // SHADERS FOR CUBE 
  // Vertex Shader
  char* vertex_shader = textFileRead(vertexFileName);

  // Fragment Shader
  char* fragment_shader = textFileRead(fragmentFileName);

  // Shaders compilation
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  free(vertex_shader);
  glCompileShader(vs);

  int  success;
  char infoLog[512];
  glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vs, 512, NULL, infoLog);
    printf("ERROR: Vertex Shader for cube compilation failed!\n%s\n", infoLog);

    return(1);
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  free(fragment_shader);
  glCompileShader(fs);

  glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fs, 512, NULL, infoLog);
    printf("ERROR: Fragment Shader for cube compilation failed!\n%s\n", infoLog);

    return(1);
  }

  // Create program, attach shaders to it and link it
  shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  glValidateProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("ERROR: Shader Program linking failed!\n%s\n", infoLog);

    return(1);
  }

  // Release shader objects
  glDeleteShader(vs);
  glDeleteShader(fs);


  // SHADERS FOR LAMP 
  // Vertex Shader
  char* vertex_shader_lamp = textFileRead(vertexFileNameLamp);

  // Fragment Shader
  char* fragment_shader_lamp = textFileRead(fragmentFileNameLamp);

  // Shaders compilation
  GLuint vs_lamp = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs_lamp, 1, &vertex_shader_lamp, NULL);
  free(vertex_shader_lamp);
  glCompileShader(vs_lamp);

  //int  success;
  //char infoLog[512];
  glGetShaderiv(vs_lamp, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vs, 512, NULL, infoLog);
    printf("ERROR: Vertex Shader for lamp compilation failed!\n%s\n", infoLog);

    return(1);
  }

  GLuint fs_lamp = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs_lamp, 1, &fragment_shader_lamp, NULL);
  free(fragment_shader_lamp);
  glCompileShader(fs_lamp);

  glGetShaderiv(fs_lamp, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fs, 512, NULL, infoLog);
    printf("ERROR: Fragment Shader for lamp compilation failed!\n%s\n", infoLog);

    return(1);
  }

  // Create program, attach shaders to it and link it
  shader_lamp = glCreateProgram();
  glAttachShader(shader_lamp, fs_lamp);
  glAttachShader(shader_lamp, vs_lamp);
  glLinkProgram(shader_lamp);

  glValidateProgram(shader_lamp);
  glGetProgramiv(shader_lamp, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shader_lamp, 512, NULL, infoLog);
    printf("ERROR: Shader Program linking failed!\n%s\n", infoLog);

    return(1);
  }

  // Release shader objects
  glDeleteShader(vs_lamp);
  glDeleteShader(fs_lamp);

  // Cube to be rendered
  //
  //          0        3
  //       7        4 <-- top-right-near
  // bottom
  // left
  // far ---> 1        2
  //       6        5
  //
  float vertex_positions[] = {
     // positions          // normals           // texture coords
    -0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,// 1
    -0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,// 0
     0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,// 2

     0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,// 3
     0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,// 2
    -0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,// 0

     0.25f, -0.25f, -0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,// 2
     0.25f,  0.25f, -0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,// 3
     0.25f, -0.25f,  0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,// 5

     0.25f,  0.25f,  0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,// 4
     0.25f, -0.25f,  0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,// 5
     0.25f,  0.25f, -0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,// 3

     0.25f, -0.25f,  0.25f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,// 5
     0.25f,  0.25f,  0.25f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,// 4
    -0.25f, -0.25f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,// 6

    -0.25f,  0.25f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,// 7
    -0.25f, -0.25f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,// 6
     0.25f,  0.25f,  0.25f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,// 4

    -0.25f, -0.25f,  0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,// 6
    -0.25f,  0.25f,  0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,// 7
    -0.25f, -0.25f, -0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,// 1

    -0.25f,  0.25f, -0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,// 0
    -0.25f, -0.25f, -0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,// 1
    -0.25f,  0.25f,  0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,// 7

     0.25f, -0.25f, -0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,// 2
     0.25f, -0.25f,  0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,// 5
    -0.25f, -0.25f, -0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,// 1

    -0.25f, -0.25f,  0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,// 6
    -0.25f, -0.25f, -0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,// 1
     0.25f, -0.25f,  0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,// 5

     0.25f,  0.25f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,// 4
     0.25f,  0.25f, -0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,// 3
    -0.25f,  0.25f,  0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,// 7

    -0.25f,  0.25f, -0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, // 0
    -0.25f,  0.25f,  0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, // 7
     0.25f,  0.25f, -0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f  // 3
  };

// Vertex Buffer Object (for vertex coordinates)
  GLuint vbo = 0;
  // Vertex Array Object
  glGenVertexArrays(1, &cube_vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

  glBindVertexArray(cube_vao);
  
  // Vertex attributes
  // 0: vertex position (x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Normal attributes
  // 1: vertex normals (x, y, z)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Texture coord
  // 2: texture (x, y)
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // second, configure the light's VAO (VBO stays the same; 
  // the vertices are the same for the light object which is also a 3D cube)
  glGenVertexArrays(1, &light_vao);
  glBindVertexArray(light_vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // note that we update the lamp's position attribute's stride to reflect the updated buffer data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Unbind vbo (it was conveniently registered by VertexAttribPointer)
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Unbind vao
  glBindVertexArray(0);


  // Uniforms
  // - Model matrix
  // - View matrix
  // - Projection matrix
  // - Normal matrix: normal vectors from local to world coordinates
  // - Camera position
  // - Light data
  // - Material data
  
  model_location = glGetUniformLocation(shader_program, "model");
  view_location = glGetUniformLocation(shader_program, "view");
  proj_location = glGetUniformLocation(shader_program, "projection");
  // [...]
  normal_location = glGetUniformLocation(shader_program, "normal_to_world");

  camera_location = glGetUniformLocation(shader_program, "view_pos");

  l_position  = glGetUniformLocation(shader_program,"light.position");  
  l_ambient = glGetUniformLocation(shader_program, "light.ambient");
  l_diffuse  = glGetUniformLocation(shader_program,"light.diffuse");
  l_specular = glGetUniformLocation(shader_program,"light.specular");

  //m_ambient = glGetUniformLocation(shader_program, "material.ambient");
  m_diffuse  = glGetUniformLocation(shader_program,"material.diffuse");
  m_specular  = glGetUniformLocation(shader_program,"material.specular");  
  m_shininess = glGetUniformLocation(shader_program,"material.shininess");

  model_lamp = glGetUniformLocation(shader_lamp, "model");
  view_lamp = glGetUniformLocation(shader_lamp, "view");
  proj_lamp = glGetUniformLocation(shader_lamp, "projection");

  glUseProgram(shader_program);
  glUniform1i(m_diffuse, 0); //LBA

  //diffuseMap = loadTexture(fs::path("resources/texture.jpg").c_str());
  diffuseMap = loadTexture(fs::path("resources/container.png").c_str());

// Render loop
  while(!glfwWindowShouldClose(window)) {

    processInput(window);

    render(glfwGetTime());

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

void render(double currentTime) {
  float f = (float)currentTime * 0.3f;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, gl_width, gl_height);

  glUseProgram(shader_program);
  
  //camera position
  glUniform3fv(camera_location, 1, &camera_pos[0]);

  // light properties
  glUniform3fv(l_position, 1, &light_pos[0]);
  glUniform3fv(l_ambient, 1, &light_ambient[0]);
  glUniform3fv(l_diffuse, 1, &light_diffuse[0]);
  glUniform3fv(l_specular, 1, &light_specular[0]);

  // material properties
  //glUniform3fv(m_ambient, 1, &material_ambient[0]);
  //glUniform3fv(m_diffuse, 1, &material_diffuse[0]); //LBA
  glUniform3fv(m_specular, 1, &material_specular[0]);
  glUniform1f(m_shininess, material_shininess);

  glm::mat4 model_matrix, view_matrix, proj_matrix;
  glm::mat3 normal_matrix;

  model_matrix = glm::mat4(1.f);
  view_matrix = glm::lookAt(                 camera_pos,  // pos
                            glm::vec3(0.0f, 0.0f, 0.0f),  // target
                            glm::vec3(0.0f, 1.0f, 0.0f)); // up

  
  // Moving the cube
  //model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -4.0f));
  model_matrix = glm::translate(model_matrix,
                             glm::vec3(sinf(2.1f * f) * 0.5f,
                                       cosf(1.7f * f) * 0.5f,
                                       sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));

  model_matrix = glm::rotate(model_matrix,
                              glm::radians((float)currentTime * 45.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));
  
  model_matrix = glm::rotate(model_matrix,
                          glm::radians((float)currentTime * 81.0f),
                          glm::vec3(1.0f, 0.0f, 0.0f));
  
  glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));
  glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));  

  //Normal matrix: normal vectors to world coordinates
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
  glUniformMatrix3fv(normal_location, 1, GL_FALSE, glm::value_ptr(normal_matrix));

  // Projection
  proj_matrix = glm::perspective(glm::radians(50.0f),
                                 (float) gl_width / (float) gl_height,
                                 0.1f, 1000.0f);
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  // bind diffuse map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, diffuseMap);
  
  glBindVertexArray(cube_vao);
  glDrawArrays(GL_TRIANGLES, 0, 36);



  // Draw the lamp object
  glUseProgram(shader_lamp);
  glUniformMatrix4fv(proj_lamp, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix4fv(view_lamp, 1, GL_FALSE, glm::value_ptr(view_matrix));  
  model_matrix = glm::mat4(1.0f);
  model_matrix = glm::translate(model_matrix, light_pos);
  model_matrix = glm::scale(model_matrix, glm::vec3(0.2f));
  glUniformMatrix4fv(model_lamp, 1, GL_FALSE, glm::value_ptr(model_matrix));

  glBindVertexArray(light_vao);
  glDrawArrays(GL_TRIANGLES, 0, 36);

}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}

// Callback function to track window size and update viewport
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
  gl_width = width;
  gl_height = height;
  printf("New viewport: (width: %d, height: %d)\n", width, height);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
