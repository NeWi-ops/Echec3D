#include "Scene3D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uMVP; 

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 uColor; 

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

float cubeVertices[] = {

    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
    0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

    -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
    0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
    0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f};

Scene3D::Scene3D() {

  camYaw = 90.0f;
  camPitch = 50.0f;
  camDistance = 12.0f;
}

Scene3D::~Scene3D() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
}

void Scene3D::updateCameraInput() {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !io.WantCaptureMouse) {

    float sensitivity = 0.5f;

    camYaw += io.MouseDelta.x * sensitivity;
    camPitch += io.MouseDelta.y * sensitivity;

    if (camPitch > 89.0f)
      camPitch = 89.0f;
    if (camPitch < 10.0f)
      camPitch = 10.0f;
  }

  if (!io.WantCaptureMouse) {
    if (io.MouseWheel != 0.0f) {
      camDistance -= io.MouseWheel * 1.0f;
      if (camDistance < 5.0f)
        camDistance = 5.0f;
      if (camDistance > 20.0f)
        camDistance = 20.0f;
    }
  }
}

unsigned int Scene3D::compileShader(unsigned int type, const char *source) {
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &source, nullptr);
  glCompileShader(id);

  return id;
}

void Scene3D::init() {

  unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);

  glDeleteShader(vs);
  glDeleteShader(fs);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Scene3D::draw(const Game &game, float deltaTime) {

  glEnable(GL_DEPTH_TEST);
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);

  if (isAnimating) {
    animProgress += animSpeed * deltaTime;
    if (animProgress >= 1.0f) {
      isAnimating = false;
      animProgress = 1.0f;
    }
  }

  updateCameraInput();

  glm::vec3 camPos;
  float yawRad = glm::radians(camYaw);
  float pitchRad = glm::radians(camPitch);

  camPos.x = camTarget.x + camDistance * cos(pitchRad) * cos(yawRad);
  camPos.y = camTarget.y + camDistance * sin(pitchRad);
  camPos.z = camTarget.z + camDistance * cos(pitchRad) * sin(yawRad);

  ImGuiIO &io = ImGui::GetIO();
  float width = io.DisplaySize.x;
  float height = io.DisplaySize.y;

  if (height <= 0)
    height = 1.0f;

  float aspectRatio = width / height;
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));

  int uMVPLoc = glGetUniformLocation(shaderProgram, "uMVP");
  int uColorLoc = glGetUniformLocation(shaderProgram, "uColor");

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(x, 0.0f, y));
      model = glm::scale(model, glm::vec3(0.95f, 0.1f, 0.95f));

      glm::mat4 MVP = projection * view * model;
      glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));

      if ((x + y) % 2 != 0)
        glUniform3f(uColorLoc, 0.3f, 0.3f, 0.3f);
      else
        glUniform3f(uColorLoc, 0.9f, 0.9f, 0.9f);

      glDrawArrays(GL_TRIANGLES, 0, 36);

      const Piece *p = game.getBoard().getPiece(x, y);

      if (p) {
        if (isAnimating && x == animTargetSquare.x && y == animTargetSquare.y) {
          continue;
        }

        glm::mat4 pieceModel = glm::mat4(1.0f);
        pieceModel = glm::translate(pieceModel, glm::vec3(x, 0.5f, y));

        float height = (p->getType() == PieceType::King) ? 1.5f : 0.8f;
        pieceModel = glm::scale(pieceModel, glm::vec3(0.4f, height, 0.4f));

        glm::mat4 pieceMVP = projection * view * pieceModel;
        glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(pieceMVP));

        if (p->getColor() == PieceColor::White)
          glUniform3f(uColorLoc, 1.0f, 0.0f, 0.0f);
        else
          glUniform3f(uColorLoc, 0.0f, 0.0f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
  }

  if (isAnimating) {
    glm::vec3 currentPos = glm::mix(animStartPos, animEndPos, animProgress);
    float jumpHeight = 1.0f;
    currentPos.y += jumpHeight * 4.0f * animProgress * (1.0f - animProgress);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, currentPos);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

    float h = (animPieceType == PieceType::King) ? 1.5f : 0.8f;
    model = glm::scale(model, glm::vec3(0.4f, h, 0.4f));

    glm::mat4 MVP = projection * view * model;
    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));

    if (animPieceColor == PieceColor::White)
      glUniform3f(uColorLoc, 1.0f, 0.0f, 0.0f);
    else
      glUniform3f(uColorLoc, 0.0f, 0.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  glBindVertexArray(0);
}

void Scene3D::triggerMoveAnimation(Coords from, Coords to, PieceType type,
                                   PieceColor color) {
  isAnimating = true;
  animProgress = 0.0f;

  animStartPos = glm::vec3(from.x, 0.0f, from.y);
  animEndPos = glm::vec3(to.x, 0.0f, to.y);

  animPieceType = type;
  animPieceColor = color;
  animTargetSquare = to;
}