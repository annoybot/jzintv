// Created by Patrick Nadeau on 2019-12-11.

#define PCRE2_CODE_UNIT_WIDTH 8

#include <stdio.h>
#include <string.h>
#include <pcre2.h>
#include <ctype.h>

#include "cheat.h"

static const char *cheat_regexp = "(?:(?:\\s|\\|)*([PpEe])\\s+\\$([0-9A-Fa-f]+)\\s+([0-9A-Fa-f]+)(?:\\s|\\|)*)";

cheat_code_cmd_t* create_struct(const PCRE2_UCHAR8 *subject, const size_t *ovector);

cheat_code_list_t* init_cheat_list()
{
    cheat_code_list_t* result = (cheat_code_list_t*) calloc(CHEAT_LIST_ALLOC_SIZE, sizeof(cheat_code_list_t));

    result->codes =(cheat_code_t*) calloc(CHEAT_LIST_ALLOC_SIZE, sizeof(cheat_code_t *));
    result->capacity = CHEAT_LIST_ALLOC_SIZE;
    result->count = 0;

    return result;
}

int parse_cheat_str(cheat_code_list_t *list, char *str) {

    pcre2_code *re;
    PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
    PCRE2_SPTR subject;     /* the appropriate width (in this case, 8 bits). */
    PCRE2_SPTR name_table;

    int crlf_is_newline;
    int errornumber;
    int find_all =1 ;
    int i;
    int rc;
    int utf8;

    uint32_t option_bits;
    uint32_t namecount;
    uint32_t name_entry_size;
    uint32_t newline;

    PCRE2_SIZE erroroffset;
    PCRE2_SIZE *ovector;
    PCRE2_SIZE subject_length;

    pcre2_match_data *match_data;

    pattern = (PCRE2_SPTR)cheat_regexp;
    subject = (PCRE2_SPTR)str;
    subject_length = (PCRE2_SIZE)strlen((char *)subject);

    int result_list_size= CHEAT_LIST_ALLOC_SIZE;

    cheat_code_cmd_t**  cheat_code = (cheat_code_cmd_t**) calloc(result_list_size, sizeof(cheat_code_cmd_t*) );
    int result_count = 0;

    //printf("parse_cheat_str: %p\tcount: %d\tsize: %d\n", cheat_code, result_count, result_list_size);

    re = pcre2_compile(
            pattern,               /* the pattern */
            PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
            0,                     /* default options */
            &errornumber,          /* for error number */
            &erroroffset,          /* for error offset */
            NULL);                 /* use default compile context */

    /* Compilation failed: print the error message and exit. */
    if (re == NULL)
    {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        printf("parse_cheat_str: PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
               buffer);
        exit(1);
    }

    match_data = pcre2_match_data_create_from_pattern(re, NULL);

    rc = pcre2_match(
            re,                   /* the compiled pattern */
            subject,              /* the subject string */
            subject_length,       /* the length of the subject */
            0,                    /* start at offset 0 in the subject */
            0,                    /* default options */
            match_data,           /* block for storing the result */
            NULL);                /* use default match context */

    /* Matching failed: handle error cases */

    if (rc < 0)
    {
        switch(rc)
        {
            case PCRE2_ERROR_NOMATCH:
                printf("parse_cheat_str: Invalid cheat code string: %s\n", str);
                exit(1);

                break;
                /*
                Handle other special cases if you like
                */
            default:
                printf("parse_cheat_str: Matching error %d while parsing cheat code %s\n", rc, str);
                exit(1);
                break;
        }
        pcre2_match_data_free(match_data);   /* Release memory used for the match */
        pcre2_code_free(re);                 /*   data and the compiled pattern. */
        exit(1);
    }

    /* Match succeded. Get a pointer to the output vector, where string offsets are stored. */

    ovector = pcre2_get_ovector_pointer(match_data);

    if (rc == 0) {
        printf("parse_cheat_str: ovector was not big enough for all the captured substrings\n");
        exit(1);
    }

    cheat_code[result_count] = create_struct(subject, ovector);

    ++result_count;

    /* Before running the loop, check for UTF-8 and whether CRLF is a valid newline
       sequence. First, find the options with which the regex was compiled and extract
       the UTF state. */

    (void)pcre2_pattern_info(re, PCRE2_INFO_ALLOPTIONS, &option_bits);
    utf8 = (option_bits & PCRE2_UTF) != 0;

    /* Now find the newline convention and see whether CRLF is a valid newline sequence. */

    (void)pcre2_pattern_info(re, PCRE2_INFO_NEWLINE, &newline);
    crlf_is_newline = newline == PCRE2_NEWLINE_ANY ||
                      newline == PCRE2_NEWLINE_CRLF ||
                      newline == PCRE2_NEWLINE_ANYCRLF;

    /* Loop for second and subsequent matches */

    for (;;)
    {
        uint32_t options = 0;                   /*  No options */
        PCRE2_SIZE start_offset = ovector[1];   /* Start at end of previous match */

        /* If the previous match was for an empty string, we are finished if we are
        at the end of the subject. Otherwise, arrange to run another match at the
        same point to see if a non-empty match can be found. */

        if (ovector[0] == ovector[1])
        {
            if (ovector[0] == subject_length)
                break;

            options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
        }

            /* If the previous match was not an empty string, there is one tricky case to
            consider. If a pattern contains \K within a lookbehind assertion at the
            start, the end of the matched string can be at the offset where the match
            started. Without special action, this leads to a loop that keeps on matching
            the same substring. We must detect this case and arrange to move the start on
            by one character. The pcre2_get_startchar() function returns the starting
            offset that was passed to pcre2_match(). */

        else
        {
            PCRE2_SIZE startchar = pcre2_get_startchar(match_data);
            if (start_offset <= startchar)
            {
                if (startchar >= subject_length) break;   /* Reached end of subject.   */
                start_offset = startchar + 1;             /* Advance by one character. */
                if (utf8)                                 /* If UTF-8, it may be more  */
                {                                       /*   than one code unit.     */
                    for (; start_offset < subject_length; start_offset++)
                        if ((subject[start_offset] & 0xc0) != 0x80) break;
                }
            }
        }

        /* Look for the next match */

        rc = pcre2_match(
                re,                   /* the compiled pattern */
                subject,              /* the subject string */
                subject_length,       /* the length of the subject */
                start_offset,         /* starting offset in the subject */
                options,              /* options */
                match_data,           /* block for storing the result */
                NULL);                /* use default match context */

        /* This time, a result of NOMATCH isn't an error. If the value in "options"
        is zero, it just means we have found all possible matches, so the loop ends.
        Otherwise, it means we have failed to find a non-empty-string match at a
        point where there was a previous empty-string match. In this case, we do what
        Perl does: advance the matching position by one character, and continue. We
        do this by setting the "end of previous match" offset, because that is picked
        up at the top of the loop as the point at which to start again.

        There are two complications: (a) When CRLF is a valid newline sequence, and
        the current position is just before it, advance by an extra byte. (b)
        Otherwise we must ensure that we skip an entire UTF character if we are in
        UTF mode. */

        if (rc == PCRE2_ERROR_NOMATCH)
        {
            if (options == 0) break;                    /* All matches found */
            ovector[1] = start_offset + 1;              /* Advance one code unit */
            if (crlf_is_newline &&                      /* If CRLF is a newline & */
                start_offset < subject_length - 1 &&    /* we are at CRLF, */
                subject[start_offset] == '\r' &&
                subject[start_offset + 1] == '\n')
                ovector[1] += 1;                          /* Advance by one more. */
            else if (utf8)                              /* Otherwise, ensure we */
            {                                         /* advance a whole UTF-8 */
                while (ovector[1] < subject_length)       /* character. */
                {
                    if ((subject[ovector[1]] & 0xc0) != 0x80) break;
                    ovector[1] += 1;
                }
            }
            continue;    /* Go round the loop again */
        }

        /* Other matching errors are not recoverable. */

        if (rc < 0)
        {
            printf("parse_cheat_str: Matching error %d\n", rc);
            pcre2_match_data_free(match_data);
            pcre2_code_free(re);
            exit(1);
        }


        /* The match succeeded, but the output vector wasn't big enough. This should not happen. */

        if (rc == 0) {
            printf("parse_cheat_str: ovector was not big enough for all the captured substrings\n");
            exit(1);
        }

        /* Reallocate the return_list buffer if there are more than result_list_alloc_size matches found. */
        if( result_count > result_list_size-1) {
            cheat_code = realloc(cheat_code, (result_list_size + CHEAT_LIST_ALLOC_SIZE) * sizeof(cheat_code_cmd_t *) );
            result_list_size += CHEAT_LIST_ALLOC_SIZE;

            memset(&cheat_code[result_count], 0, CHEAT_LIST_ALLOC_SIZE * sizeof(cheat_code_cmd_t *));
        }

        //printf("parse_cheat_str: %p\tcount: %d\tsize: %d\n", cheat_code, result_count, result_list_size);

        cheat_code[result_count] = create_struct(subject, ovector);

        ++result_count;

    }      /* End of loop to find second and subsequent matches */

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    /* Reallocate the list of cheat codes if we need more space. */
    if( list->count > list->capacity -1) {
        list->codes = realloc(list->codes, (list->capacity + CHEAT_LIST_ALLOC_SIZE) *sizeof(cheat_code_t*) );
        list->capacity += CHEAT_LIST_ALLOC_SIZE;
        memset( &list->codes[list->count], 0, CHEAT_LIST_ALLOC_SIZE * sizeof(cheat_code_t *));
    }

    //Was crashing here
    list->codes[list->count] = cheat_code;
    ++list->count;

    printf("parse_cheat_str: %p\tcount: %d\tcapacity: %d\n", list->codes, list->count, list->capacity);
    printf("parsed cheat string CHEAT%d: \"%s\"\n", list->count, str);

    return 0;
}

cheat_code_cmd_t * create_struct(const PCRE2_UCHAR8 *subject, const size_t *ovector) {

    cheat_code_cmd_t* result = (cheat_code_cmd_t*) calloc(1, sizeof(cheat_code_cmd_t) );

    int index = 1;

    //The first group is the command ('P' or 'E')
    PCRE2_SPTR substring_start = subject + ovector[2 * index];
    PCRE2_SIZE substring_length = ovector[2 * index + 1] - ovector[2 * index];
    result->cmd = toupper((unsigned char) *substring_start);

    ++index;

    //The second group is the address. Ex: 173, we put hsi in arg1 in the struct
    substring_start = subject + ovector[2*index];
    substring_length = ovector[2*index+1] - ovector[2*index];

    char* arg = calloc(substring_length+2, sizeof(char) );
    strlcpy(arg, substring_start, substring_length+1);

    result->arg1 = strtol(arg, NULL, 16);
    free(arg);

    ++index;

    //The third group is the address. Ex: 173, we put hsi in arg1 in the struct
    substring_start = subject + ovector[2*index];
    substring_length = ovector[2*index+1] - ovector[2*index];

    arg = calloc(substring_length+1, sizeof(char) );
    strlcpy(arg, substring_start, substring_length+1);

    result->arg2 = strtol(arg, NULL, 16);
    free(arg);

    return result;
}
