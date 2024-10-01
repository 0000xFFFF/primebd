#ifndef SHELL_H
#define SHELL_H

typedef enum {
	SHELL_MODE_SERVICE,
    SHELL_MODE_USER,
	SHELL_MODE_KEYLOG
} SHELL_MODES;

extern int shell_main(SHELL_MODES);

#endif