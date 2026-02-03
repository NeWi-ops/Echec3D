#include "Scene3D.hpp"
#include <fstream> // <--- Pour lire les fichiers
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream> // <--- Pour lire le texte
#include <vector>

std::string loadShaderFile(const char *filename) {
  std::ifstream file;
  std::stringstream buf;
  std::string ret = "";

  file.open(filename);
  if (file.is_open()) {
    buf << file.rdbuf();
    ret = buf.str();
    file.close();
  } else {
    std::cerr << "ERREUR : Impossible d'ouvrir le shader : " << filename
              << std::endl;
  }
  return ret;
}
float cubeVertices[] = {
    // positions          // normales
    // Face Arrière
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
    -1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,
    0.0f, -1.0f,

    // Face Avant
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    // Face Gauche
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,
    0.0f, -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f,
    0.0f,

    // Face Droite
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

    // Face Bas
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,
    0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f,
    0.0f, -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f,
    -1.0f, 0.0f,

    // Face Haut
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

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

  const char *vsCode = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        out vec3 FragPos;
        out vec3 Normal;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

  const char *fsCode = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 FragPos;
        in vec3 Normal;
        uniform vec3 uColor;
        uniform vec3 uLightDir;
        uniform vec3 uViewPos;
        void main() {
            vec3 ambient = 0.3 * vec3(1.0);
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(-uLightDir);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(1.0);
            vec3 viewDir = normalize(uViewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = 0.5 * spec * vec3(1.0);  
            vec3 result = (ambient + diffuse + specular) * uColor;
            FragColor = vec4(result, 1.0);
        }
    )";

  unsigned int vs = compileShader(GL_VERTEX_SHADER, vsCode);
  unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fsCode);

  // Sécurité : si le fichier est vide ou pas trouvé, on évite le crash moche

  // 2. COMPILATION
  // On doit convertir std::string en const char* pour OpenGL

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

  // 1. POSITION (Location 0)
  // 6 * sizeof(float) car chaque sommet a maintenant 6 valeurs
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // 2. NORMALES (Location 1)
  // On commence à lire après les 3 premiers floats (les positions)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Scene3D::draw(const Game &game, float deltaTime) {

  // 1. SETUP DE BASE
  glEnable(GL_DEPTH_TEST);

  // Si tu as un Skybox, dessine-le ICI (avant le reste)
  // ...

  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);

  // 2. GESTION ANIMATION
  if (isAnimating) {
    animProgress += animSpeed * deltaTime;
    if (animProgress >= 1.0f) {
      isAnimating = false;
      animProgress = 1.0f;
    }
  }

  // 3. CAMÉRA & INPUTS
  updateCameraInput();

  glm::vec3 camPos;
  float yawRad = glm::radians(camYaw);
  float pitchRad = glm::radians(camPitch);

  camPos.x = camTarget.x + camDistance * cos(pitchRad) * cos(yawRad);
  camPos.y = camTarget.y + camDistance * sin(pitchRad);
  camPos.z = camTarget.z + camDistance * cos(pitchRad) * sin(yawRad);

  // 4. PROJECTION (Ratio écran)
  ImGuiIO &io = ImGui::GetIO();
  float width = io.DisplaySize.x;
  float height = io.DisplaySize.y;
  if (height <= 0)
    height = 1.0f;
  float aspectRatio = width / height;

  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));

  // 5. RÉCUPÉRATION DES UNIFORMS (Nouveaux noms !)
  int modelLoc = glGetUniformLocation(shaderProgram, "model");
  int viewLoc = glGetUniformLocation(shaderProgram, "view");
  int projLoc = glGetUniformLocation(shaderProgram, "projection");

  int uColorLoc = glGetUniformLocation(shaderProgram, "uColor");
  int uLightDirLoc = glGetUniformLocation(shaderProgram, "uLightDir");
  int uViewPosLoc = glGetUniformLocation(shaderProgram, "uViewPos");

  // 6. ENVOI DES DONNÉES GLOBALES (Lumière & Caméra)
  // Direction du soleil (vient d'en haut à droite)
  glUniform3f(uLightDirLoc, -0.5f, -1.0f, -0.5f);
  // Position de la caméra (pour le brillant spéculaire)
  glUniform3f(uViewPosLoc, camPos.x, camPos.y, camPos.z);

  // On envoie View et Projection UNE SEULE FOIS (optimisation)
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // 7. BOUCLE D'AFFICHAGE (Plateau + Pièces)
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {

      // --- A. LE SOL (CASE) ---
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(x, 0.0f, y));
      model = glm::scale(model, glm::vec3(0.95f, 0.1f, 0.95f));

      // On envoie juste la matrice Model de l'objet
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      if ((x + y) % 2 != 0)
        glUniform3f(uColorLoc, 0.3f, 0.3f, 0.3f); // Noir
      else
        glUniform3f(uColorLoc, 0.9f, 0.9f, 0.9f); // Blanc

      glDrawArrays(GL_TRIANGLES, 0, 36);

      // --- B. LA PIÈCE (STATIQUE) ---
      const Piece *p = game.getBoard().getPiece(x, y);

      if (p) {
        // Si c'est la pièce qui bouge, on ne la dessine pas ici
        if (isAnimating && x == animTargetSquare.x && y == animTargetSquare.y) {
          continue;
        }

        glm::mat4 pieceModel = glm::mat4(1.0f);
        pieceModel = glm::translate(pieceModel, glm::vec3(x, 0.5f, y));

        float height = (p->getType() == PieceType::King) ? 1.5f : 0.8f;
        pieceModel = glm::scale(pieceModel, glm::vec3(0.4f, height, 0.4f));

        // Envoi Model Pièce
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pieceModel));

        // Couleurs
        if (p->getColor() == PieceColor::White)
          glUniform3f(
              uColorLoc, 1.0f, 0.9f,
              0.8f); // Blanc un peu "ivoire" pour mieux réagir à la lumière
        else
          glUniform3f(uColorLoc, 0.1f, 0.1f, 0.1f); // Noir pas totalement noir

        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
  }

  // 8. DESSIN DE L'ANIMATION
  if (isAnimating) {
    glm::vec3 currentPos = glm::mix(animStartPos, animEndPos, animProgress);
    float jumpHeight = 1.0f;
    currentPos.y += jumpHeight * 4.0f * animProgress * (1.0f - animProgress);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, currentPos);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

    float h = (animPieceType == PieceType::King) ? 1.5f : 0.8f;
    model = glm::scale(model, glm::vec3(0.4f, h, 0.4f));

    // Envoi Model Animation
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    if (animPieceColor == PieceColor::White)
      glUniform3f(uColorLoc, 1.0f, 0.9f, 0.8f);
    else
      glUniform3f(uColorLoc, 0.1f, 0.1f, 0.1f);

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