#ifndef REVIEWER_H
#define REVIEWER_H
#include <string>
 
/// @brief Representa um revisor de artigos cientificos.
struct Reviewer {
    int id;                  ///< Identificador unico do revisor
    std::string name;        ///< Nome do revisor
    std::string email;       ///< Email do revisor
    int primaryExpertise;    ///< Dominio de expertise primario (obrigatorio)
    int secondaryExpertise;  ///< Dominio de expertise secundario (0 se nao definido)
 
    /// @brief Constroi um revisor com os dados fornecidos.
    Reviewer(int id, const std::string &name,
             const std::string &email,
             int primaryExpertise, int secondaryExpertise = 0)
            : id(id), name(name), email(email),
              primaryExpertise(primaryExpertise), secondaryExpertise(secondaryExpertise) {}
};
#endif
 