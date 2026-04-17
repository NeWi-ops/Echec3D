#pragma once
#include "../model/game.hpp"
#include "imgui.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "AssetManager.hpp"
#include "../utils/RandomGenerator.hpp"

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifeTime;
    float maxLifeTime;
    glm::vec4 color;
    float scale;
};


class Scene3D {
public:
  Scene3D();
  ~Scene3D();

  void init();
  void draw(const Game &game, float deltaTime,
            std::optional<Coords> selectedCase,
            const std::vector<Coords> &possibleMoves);
  void triggerMoveAnimation(Coords from, Coords to, PieceType type,
                            PieceColor color);
  void triggerPathAnimation(const std::vector<Coords>& path, PieceType type, PieceColor color);
  bool isAnimationPlaying() const;

  std::optional<Coords> getClickedSquare();
  
  void spawnExplosion(glm::vec3 origin, glm::vec4 baseColor);
  void updateParticles(float deltaTime);

private:
  unsigned int skyboxShader;
  unsigned int skyboxVAO, skyboxVBO;
  unsigned int cubemapTexture;

  unsigned int whiteTileTex;
  unsigned int blackTileTex;

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

  struct AnimState {
      PieceType type;
      PieceColor color;
      std::vector<Coords> path;
      float progress;
      bool active;
  };
  std::vector<AnimState> activeAnimations;

  float camYaw = -90.0f;
  float camPitch = 45.0f;
  float camDistance = 12.0f;
  glm::vec3 camTarget = glm::vec3(3.5f, 0.0f, 3.5f);

  AssetManager assetManager;
    
  std::shared_ptr<Model3D> modelPawn;
  std::shared_ptr<Model3D> modelRook;
  std::shared_ptr<Model3D> modelKnight;
  std::shared_ptr<Model3D> modelBishop;
  std::shared_ptr<Model3D> modelQueen;
  std::shared_ptr<Model3D> modelKing;

  void updateCameraInput();

  glm::mat4 lastProjection;
  glm::mat4 lastView;

  bool isClicking = false;
  std::vector<Particle> m_particles;
};
