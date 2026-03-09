#include "AssetManager.hpp"
#include "Model3D.hpp"
// IMPORTANT : N'oublie pas d'inclure ici ton vrai fichier qui définit la classe Model3D
// #include "Model3D.hpp" 

std::shared_ptr<Model3D> AssetManager::getModel(const std::string& filepath) {
    // 1. On cherche si ce fichier a DÉJÀ été chargé auparavant
    auto it = models.find(filepath);

    if (it != models.end()) {
        // TROUVÉ ! On renvoie simplement le pointeur vers la mémoire existante.
        // Cela prend 0 milliseconde et ne consomme pas de VRAM supplémentaire.
        return it->second; 
    }

    // 2. NON TROUVÉ. C'est la première fois qu'on demande ce fichier.
    std::cout << "[AssetManager] Chargement du nouveau modèle : " << filepath << std::endl;

    // Tu dois remplacer cette ligne par la vraie façon dont tu instancies ton modèle.
    // Exemple : std::shared_ptr<Model3D> newModel = std::make_shared<Model3D>(filepath);
    std::shared_ptr<Model3D> newModel = std::make_shared<Model3D>(filepath); 

    // 3. On l'enregistre dans notre dictionnaire pour la prochaine fois
    if (newModel) {
        models[filepath] = newModel;
    } else {
        std::cerr << "[AssetManager] ERREUR : Impossible de charger " << filepath << std::endl;
    }

    return newModel;
}

void AssetManager::clear() {
    // std::shared_ptr va automatiquement détruire les modèles si plus personne ne s'en sert
    models.clear();
    textures.clear();
    std::cout << "[AssetManager] Memoire nettoyee." << std::endl;
}