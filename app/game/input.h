#pragma once

#include <stddef.h>
#include <stdint.h>

#include "game.h"

typedef struct key_binding {
    uint8_t key;
    const char* key_name;
    game_cmd_t cmd;
    const char* cmd_name;
} key_binding_t;

extern const key_binding_t key_bindings[];
extern const size_t num_key_bindings;

void input_start(void);
