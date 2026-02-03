const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // On reçoit la normale

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;      // Matrice de l'objet
uniform mat4 view;       // Matrice caméra
uniform mat4 projection; // Matrice projection

void main() {
    // Calcul de la position dans le monde
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Calcul de la normale (la matrice "Normal Matrix" gère les rotations/échelles)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";