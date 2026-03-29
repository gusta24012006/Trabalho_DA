#ifndef SUBMISSION_H
#define SUBMISSION_H
#include <string>
 
/// @brief Representa uma submissao de artigo cientifico.
struct Submission {
    int id;                  ///< Identificador unico da submissao
    std::string title;       ///< Titulo do artigo
    std::string authors;     ///< Nome(s) do(s) autor(es)
    std::string email;       ///< Email de contacto
    int primaryDomain;       ///< Dominio topico primario (obrigatorio)
    int secondaryDomain;     ///< Dominio topico secundario (0 se nao definido)
 
    /// @brief Constroi uma submissao com os dados fornecidos.
    Submission(int id, const std::string &title,
               const std::string &authors,
               const std::string &email,
               int primaryDomain, int secondaryDomain = 0)
            : id(id), title(title), authors(authors), email(email),
              primaryDomain(primaryDomain), secondaryDomain(secondaryDomain) {}
};
#endif
 