#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>

struct Parameters {
    int minReviewsPerSubmission = 1;
    int maxReviewsPerReviewer = 3;
    int primaryReviewerExpertise = 1;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain = 1;
    int secondarySubmissionDomain = 1;
};

#endif
