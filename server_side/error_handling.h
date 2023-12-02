#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);

#endif  // ERROR_HANDLING_H
