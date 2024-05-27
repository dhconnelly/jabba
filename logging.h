#ifndef LOGGING_H_
#define LOGGING_H_

/* logs a message (with metadata) to stdout */
void trace(const char *fmt, ...);

/* logs a message (with metadata) to stderr and exists with status 1 */
void fatal(const char *fmt, ...);

/* prints a message to stderr and exits with status 1 */
void die(const char *fmt, ...);

#endif
