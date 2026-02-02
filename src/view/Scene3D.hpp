#pragma once
#include "../model/game.hpp"
#include "imgui.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Scene3D {
public:
  Scene3D();
  ~Scene3D();

  void init();
  void draw(const Game &game, float deltaTime);
  void triggerMoveAnimation(Coords from, Coords to, PieceType type,
                            PieceColor color);

private:
  unsigned int shaderProgram;
  unsigned int VAO, VBO;
  unsigned int compileShader(unsigned int type, const char *source);

  bool isAnimating = false;
  float animProgress = 0.0f;
  float animSpeed = 2.0f;
  glm::vec3 animStartPos, animEndPos;
  PieceType animPieceType;
  PieceColor animPieceColor;
  Coords animTargetSquare;

  float camYaw = -90.0f;
  float camPitch = 45.0f;
  float camDistance = 12.0f;
  glm::vec3 camTarget = glm::vec3(3.5f, 0.0f, 3.5f);

  void updateCameraInput();
};