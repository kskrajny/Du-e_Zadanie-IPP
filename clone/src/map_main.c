/** @file
 * executable file for the second part of the Task
 * @author  Jakub Skrajny
 * @date 15.05.2019
 */
#include "map.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#undef NDEBUG
#include <assert.h>

/**
 * List of strings
 */
typedef struct List List;

/** @brief converts char into unsigned
 * @param[in] str - pointer to a string
 * @return unsigned which is represented by string given in parameters
 * returns 0 if the string contains not only digits or first digit is 0
 */
unsigned Char_to_Unsigned(const char* str)
{
    size_t len = strlen(str);
    unsigned result = 0;
    if(str[0] == 48)
    {
        return 0;
    }
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] < 48 && str[i] > 57)
        {
            return 0;
        }
        result = 10 * result + str[i] - '0';
    }
    return result;
}

/** @brief converts char into int
 * @param[in] str - pointer to a string
 * @return int which is represented by string given in parameters
 * or 0 if the string contains not only digits
 */
int Char_to_Int(const char* str)
{
    size_t len = strlen(str);
    int result = 0, sign = 1;
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] < 48 && str[i] > 57)
        {
            if (i == 0 && str[i] == '-')
            {
                sign = -1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            result = 10 * result + str[i] - 48;
        }
    }
    return sign * result;
}

/**
 * List of strings
 */
struct List
{
    List *nast; ///< pointer to a List
    char *string; ///< pointer to the string
};

/** @brief creates new List
 * @return pointer to new List or NULL if malloc failed
 */
List *newList()
{
    List *l = malloc(sizeof(List));
    l->nast = NULL;
    l->string = NULL;
    return l;
}

/** @brief add elem at the end of List
 * @param[in] s - pointer to char
 * @param[in] l - pointer to elem of the List
 * @return true if it added new elem to the List
 * else returns false
 */
bool add_to_List(char *s, List *l)
{
    while(l->nast != NULL)
    {
        l = l->nast;
    }
    l->string = s;
    l->nast = malloc(sizeof(List));
    if(l->nast == NULL)
    {
        return false;
    }
    l = l->nast;
    l->nast = NULL;
    l->string = NULL;
    return true;
}

/** @brief executes given orders
 * reads from stdin orders and executes them
 * as in description of task
 * !! It is about order: numer drogi krajowej;nazwa miasta;...
 * if route that may be created is somehow bad written
 * it doesnt do anything,
 */
int main()
{
    Map *m = newMap();
    char* line = NULL;
    if(m == NULL)
    {
        fprintf(stderr, "ERROR 1\n");
        return 1;
    }
    size_t n = 0;
    for (unsigned lineNr = 1;; lineNr++)
    {
        List *l = newList();
        List *q = l;
        ssize_t size = getline(&line, &n, stdin);
        if (size == -1)
        {
            free(l);
            break;
        }
        int s = 0;
        char* help;
        size_t last = 0;
        if(size > 1 && line[size-2] == ';')
        {
            free(l);
            fprintf(stderr, "ERROR %u\n", lineNr);
            continue;
        }
        if(size > 0 && line[0] == '#')
        {
            free(l);
            continue;
        }
        for (int i = 0; i < size; i++)
        {
            if (line[i] == ';' || line[i] == '\n' || line[i] == EOF)
            {
                if (i == 0 && line[i] == '\n')
                {
                    free(l);
                    continue;
                }
                help = malloc(i - last + 1);
                if (help == NULL)
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
                memcpy(help, line + last, i - last);
                if(!add_to_List(help, l))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
                q->string[i - last] = '\0';
                q = q->nast;
                last = i + 1;
                s++;
            }
        }
        // ^ reads current line and divides it into separete strings

        if (s == 0) continue;
        if (strcmp(l->string, "addRoad") == 0)
        {
            if (s != 5)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                if(!addRoad(m, l->nast->string, l->nast->nast->string, Char_to_Unsigned(l->nast->nast->nast->string), Char_to_Int(l->nast->nast->nast->nast->string)))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
            }
        }
        else if (strcmp(l->string, "repairRoad") == 0)
        {
            if (s != 4)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                if(!repairRoad(m, l->nast->string, l->nast->nast->string, Char_to_Int(l->nast->nast->nast->string)))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
            }
        }
        else if (strcmp(l->string, "getRouteDescription") == 0)
        {
            if (s != 2)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                const char* str = getRouteDescription(m, Char_to_Unsigned(l->nast->string));
                if (str == NULL)
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
                else
                {
                    printf("%s\n", str);
                }
                free((void*) str);
            }
        }
        else if (strcmp(l->string, "newRoute") == 0)
        {
            if (s != 4)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                if (!newRoute(m, Char_to_Unsigned(l->nast->string), l->nast->nast->string, l->nast->nast->nast->string))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
            }
        }
        else if (strcmp(l->string, "extendRoute") == 0)
        {
            if (s != 3)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                if (!extendRoute(m, Char_to_Unsigned(l->nast->string), l->nast->nast->string))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
            }
        }
        else if (strcmp(l->string, "removeRoad") == 0)
        {
            if (s != 3)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                if (!removeRoad(m, l->nast->string, l->nast->nast->string))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
            }
        }
        else if (strcmp(l->string, "removeRoute") == 0)
        {
            if (s != 2)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            else
            {
                if(!removeRoute(m, Char_to_Unsigned(l->nast->string)))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
            }
        }
        else
        {
            if(s < 5)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            q = l;
            if(q->nast == NULL)
            {
                fprintf(stderr, "ERROR %u\n", lineNr);
                goto skip;
            }
            q = q->nast;

            List *h;
            while(q->nast != NULL && q->nast->nast != NULL && q->nast->nast->nast != NULL)
            {
                h = q;
                while(h->nast != NULL && h->nast->nast != NULL && h->nast->nast->nast != NULL && h->nast->nast->nast->nast != NULL)
                {
                    h = h->nast->nast->nast;
                    if(strcmp(q->string, h->string) == 0)
                    {
                        fprintf(stderr, "ERROR %u\n", lineNr);
                        goto skip;
                    }
                }
                q = q->nast->nast->nast;
            }
            // ^ checks if names of given cities are different

            q = l;
            q = q->nast;
            while(q->nast != NULL && q->nast->nast != NULL && q->nast->nast->nast != NULL && q->nast->nast->nast->nast != NULL)
            {
                if(!checkRoad(m, q->string, q->nast->nast->nast->string, Char_to_Unsigned(q->nast->string), Char_to_Int(q->nast->nast->string)))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
                q = q->nast->nast->nast;
            }

            q = l;
            q = q->nast;
            while(q->nast != NULL && q->nast->nast != NULL && q->nast->nast->nast != NULL && q->nast->nast->nast->nast != NULL)
            {
                if(!addRoad(m, q->string, q->nast->nast->nast->string, Char_to_Unsigned(q->nast->string), Char_to_Int(q->nast->nast->string))
                        && !repairRoad(m, q->string, q->nast->nast->nast->string, Char_to_Int(q->nast->nast->string)))
                {
                    fprintf(stderr, "ERROR %u\n", lineNr);
                    goto skip;
                }
                q = q->nast->nast->nast;
            }

            q = l;
            q = q->nast;
            bool start = true;
            while(q->nast != NULL && q->nast->nast != NULL && q->nast->nast->nast != NULL && q->nast->nast->nast->nast != NULL)
            {
                if(start)
                {
                    if(!makeRoute(m, Char_to_Unsigned(l->string), q->string, q->nast->nast->nast->string))
                    {
                        fprintf(stderr, "ERROR %u\n", lineNr);
                        goto skip;
                    }
                    start = false;
                }
                else
                {
                    add_to_Route(m, Char_to_Unsigned(l->string), q->nast->nast->nast->string);
                }
                q = q->nast->nast->nast;
            }
        }
skip:
        while(l->nast != NULL)
        {
            q = l;
            l = l->nast;
            free(q->string);
            free(q);
        }
        free(l);
    }
    free(line);
    deleteMap(m);
    return 0;
}
