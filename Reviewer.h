#ifndef REVIEWER_H
#define REVIEWER_H

#include <string>

struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise;

    Reviewer(int id, const std::string &name,
             const std::string &email,
             int primaryExpertise, int secondaryExpertise = 0)
            : id(id), name(name), email(email),
              primaryExpertise(primaryExpertise), secondaryExpertise(secondaryExpertise) {}
};

#endif
