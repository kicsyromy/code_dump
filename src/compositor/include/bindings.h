#ifndef ROOTSTON_BINDINGS_H
#define ROOTSTON_BINDINGS_H

#include "input.h"
#include "seat.h"

void execute_binding_command(struct roots_seat *seat,
                             struct roots_input *input,
                             const char *command);

#endif //ROOTSTON_BINDINGS_H
