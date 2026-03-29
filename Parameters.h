#ifndef PARAMETERS_H
#define PARAMETERS_H
 
/// @brief Parametros do algoritmo de atribuicao de revisoes.
struct Parameters {
    int minReviewsPerSubmission  = 1; ///< Numero minimo de revisoes por submissao
    int maxReviewsPerReviewer    = 3; ///< Numero maximo de revisoes por revisor
    int primaryReviewerExpertise   = 1; ///< Peso do dominio primario do revisor
    int secondaryReviewerExpertise = 0; ///< Peso do dominio secundario do revisor
    int primarySubmissionDomain    = 1; ///< Peso do dominio primario da submissao
    int secondarySubmissionDomain  = 1; ///< Peso do dominio secundario da submissao
};
#endif
 