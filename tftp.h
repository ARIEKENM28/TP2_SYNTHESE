#ifndef TFTP_H
#define TFTP_H

void tftp_get(const char *host, const char *file);
void tftp_put(const char *host, const char *file);

#endif // TFTP_H
