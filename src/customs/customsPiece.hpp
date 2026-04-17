#pragma once

#include "./../model/Pieces/piece.hpp"
#include "./../model/Types/PieceEnum.hpp"
#include <glm/glm.hpp> 
#include <string>

class CustomPiece : public Piece {
public:
  explicit CustomPiece(PieceColor color) : Piece(color, PieceType::Custom) {}

  virtual ~CustomPiece() = default;




  virtual std::string getModelId() const = 0;

  virtual std::string getTextureId() const = 0;


  virtual float getScale() const { return 1.0f; }

  virtual glm::vec3 getColorModifier() const {
    if (this->getColor() == PieceColor::White) {
      return glm::vec3(1.0f, 1.0f, 1.0f); 
    } else {
      return glm::vec3(0.3f, 0.3f, 0.3f); 
    }
  }

  virtual std::string getIcon() const = 0;
};