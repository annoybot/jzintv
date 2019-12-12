#ifndef _CHEAT_H

typedef struct cheat_code_cmd_t {
    int      cmd;
    long     arg1;
    long     arg2;
} cheat_code_cmd_t;

typedef  cheat_code_cmd_t** cheat_code_t;

typedef struct cheat_code_list_t {
    int           count;
    int           capacity;
    cheat_code_t* codes;
} cheat_code_list_t;

cheat_code_list_t* init_cheat_list();
int                parse_cheat_str(cheat_code_list_t *dst, char *str);

#define CHEAT_LIST_ALLOC_SIZE 16

#define _CHEAT_H 1
#endif