#ifndef SUBMISSION_H
#define SUBMISSION_H
#include <string>
struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryDomain;
    int secondaryDomain;
    Submission(int id, const std::string &title,
               const std::string &authors,
               const std::string &email,
               int primaryDomain, int secondaryDomain = 0)
            : id(id), title(title), authors(authors), email(email),
              primaryDomain(primaryDomain), secondaryDomain(secondaryDomain) {}
};
#endif