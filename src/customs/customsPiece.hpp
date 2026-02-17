#pragma once

// 1. On inclut le Moteur (Model)
// (On remonte d'un dossier avec ".." pour aller dans model)
#include "./../model/Pieces/piece.hpp"
#include "./../model/Types/PieceEnum.hpp"
// 2. On inclut les types nécessaires pour l'affichage
#include <glm/glm.hpp> // Pour glm::vec3 (couleurs)
#include <string>

/**
 * CustomPiece
 * * Cette classe abstraite sert de "Pont" entre le moteur de jeu et les
 * extensions. Elle hérite de Piece (pour la logique) mais ajoute l'obligation
 * de fournir des métadonnées visuelles pour la Vue.
 */
class CustomPiece : public Piece {
public:
  // Constructeur : Force le type à "Custom" automatiquement
  explicit CustomPiece(PieceColor color) : Piece(color, PieceType::Custom) {}

  virtual ~CustomPiece() = default;

  // =========================================================
  // LE CONTRAT VISUEL (Utilisé par Scene3D / AssetManager)
  // =========================================================

  /**
   * @return L'identifiant du modèle 3D (ex: "paladin", "dragon_mesh")
   * Doit correspondre à une clé chargée dans l'AssetManager.
   */
  virtual std::string getModelId() const = 0;

  /**
   * @return L'identifiant de la texture (ex: "gold_metal", "scales")
   * Si tu utilises des couleurs unies, tu peux renvoyer une string vide.
   */
  virtual std::string getTextureId() const = 0;

  /**
   * @return Un facteur d'échelle (défaut 1.0).
   * Utile si ton modèle 3D importé est trop gros ou trop petit par rapport au
   * plateau.
   */
  virtual float getScale() const { return 1.0f; }

  /**
   * @return Un vecteur de couleur pour teinter la pièce (RGB).
   * Par défaut : Blanc (1,1,1) pour ne pas altérer la texture.
   * Tu peux l'utiliser pour rendre les pièces noires un peu plus sombres.
   */
  virtual glm::vec3 getColorModifier() const {
    if (this->getColor() == PieceColor::White) {
      return glm::vec3(1.0f, 1.0f, 1.0f); // Blanc pur
    } else {
      return glm::vec3(0.3f, 0.3f, 0.3f); // Gris foncé pour les pièces noires
    }
  }

  virtual std::string getIcon() const = 0;
};