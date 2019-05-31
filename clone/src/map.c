/** @file
 * Implementation of class that keeps map of roads
 *
 * @author  Jakub Skrajny
 * @date 27.04.2019
 */
#ifndef MAP_C
#include <stdbool.h>
#include "map.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define MAP_C
/** @brief struct represents city
 * struct contains:
 * name of city,
 * pointers to other cities (BST with ascii order, list of all),
 * pointer to road, bool table of affiliation to route,
 * some variables that are used while searching for route
 */
typedef struct City City;
/** @brief struct represents road
 * struct contains:
 * pointers to cities which this road connetcs
 * pointers to roads (BST with ascii order, list of all roads)
 * year of building or ast renovation, length of road,
 * bool table of affiliation to route,
 */
typedef struct Road Road;
/** @brief struct represents route
 * struct contains:
 * pointers to routes, prev, next (Route is a list),
 * pointer to current city,
 * pointer to road that connects curr city with previous one
 */
typedef struct Route Route;
/** @brief Queue of cities
 * queue of cities,
 * it is used while searching for route
 */
typedef struct CityQ CityQ;
/** @brief list of cities
 * list of cities,
 * it is used to implement queue
 */
typedef struct CityL CityL;
/** @brief struct represents road
 * struct contains:
 * pointers to cities which this road connetcs
 * pointers to roads (BST with ascii order, list of all roads)
 * year of building or ast renovation, length of road,
 * bool table of affiliation to route,
 */
struct Road
{
    Road *next_road; ///< pointer to next road in list of all roads
    City *city1; ///< pointer to city, which are connected by this road
    City *city2; ///< pointer to city, which are connected by this road
    int age; ///< year of built or last repair
    unsigned length; ///< length of road
    bool routes[1000]; ///< bool tab, says if certin road is a part of route
// index - id of the route, true - part of route, false - not part of route
// routes[0] is useless, want to have routes[999]
    Road *left_road_city1; ///< pointer to left son in BST of roads of city1
    Road *right_road_city1; ///< pointer to right son in BST of roads of city1
    Road *left_road_city2; ///< pointer to left son in BST of roads of city2
    Road *right_road_city2; ///< pointer to rigth son in BST of roads of city2
// BST of roads in an ascii order, city2 is fixed in this tree
    Road *next_road_city1; ///< pointer next road in listof roads that connect city1
    Road *next_road_city2; ///< pointer to next road in list of roads that connect city1
};
/** @brief struct represents city
 * struct contains:
 * name of city,
 * pointers to other cities (BST with ascii order, list of all),
 * pointer to road, bool table of affiliation to route,
 * some variables that are used while searching for route
 */
struct City
{
    char *name; ///< pointer to string that represents name of the city
    City *next_city; ///< pointer to next city in list of all cities
    City *left_city; ///< pointer to left son in BST of all cities
    City *right_city; ///< pointer to right son in BST of all cities
// BST of cities in an ascii order
    Road *first_road_BST; ///< pointer to first road in BST of roads that conects this city
    Road *first_road_L; ///< pointer to first road in lst of roads that connecnts this city
    bool routes[1000];///< tab of affiliation to routes
//routes[0] is useless, want to have routes[999]
    int eldest; ///< eldest part of possible route from certain city to this one
//3000 means undefined
    int eldest_0equi; ///< eldest part of possible routes if equivalence_l is false
    int dist; ///< distance to this city from certain city, -1 means undefined yet
    bool equivalence; ///< says if route between two cities can be determined equivalently
    bool equivalence_l; ///< says if route between two cities can be determined equivalently by length
    Road *prev_road; ///< pointer to road that leads to this city and can be a part of route
};

/** @brief list of cities
 * list of cities,
 * it is used to implement queue
 */
struct CityL
{
    City *city; ///< pointer to city
    CityL *next; ///< pointer to next elem of list
};
/** @brief Queue of cities
 * queue of cities,
 * it is used while searching for route
 */
struct CityQ
{
    CityL *first; ///< pointer to first elem of list
    CityL *last; ///< pointer to last elem of list
};
/**
 * creates new empty queue
 * @returns pointer to struct or
 * NULL if allocation of memory failed
  */
CityQ *newCityQ()
{
    CityQ *new = malloc(sizeof(CityQ));
    new->first = NULL;
    new->last = NULL;
    return new;
}
/** @brief check if queue is empty
* @param[in] *q - pointer to struct queue
* @return true if q points to empty struct
* false if it doesn't
*/
bool emptyCityQ(CityQ *q)
{
    return (q->first == NULL);
}
/** @brief adds city to queue
 * creates new CityL that contains pointer to city1,
 * adds CityL to the end of the list that the queue consists of,
 * if queue is empty it sets pointers of queue to new CityL,
 * @param[in] c - pointer to city,
 * @param[in] q - pointer to queue
 */
void addCityQ(City *c, CityQ *q)
{
    if((*q).first == NULL)
    {
        (*q).first = malloc(sizeof(CityL));
        (*q).first->city = c;
        (*q).last = (*q).first;
    }
    else
    {
        (*q).last->next = malloc(sizeof(CityL));
        (*q).last = (*q).last->next;
        (*q).last->city = c;
    }
}
/** @brief take city from queue
 * changes *c to pointer to the city that first CityL
 * in the queue has pointer to,
 * remove from the list first CityL in the queue,
 * changes pointer first of the queue,
 * @param[in] c - pointer to pointer to the city
 * @param[in] q - pointer to the queue
 */
void takeCityQ(City **c,CityQ *q)
{
    if((*q).first == NULL)
    {
        return;
    }
    else
    {
        *c = (*q).first->city;
        CityL *pom = (*q).first;
        if((*q).first == (*q).last)
        {
            (*q).first = NULL;
        }
        else
        {
            (*q).first = (*q).first->next;
        }
        free(pom);
    }
}
/** @brief struct represents route
 * struct contains:
 * pointers to routes, prev, next (Route is a list),
 * pointer to current city,
 * pointer to road that connects curr city with previous one
 */
struct Route
{
    Road *prev_road; ///< pointer to road that leads to this->city
    City *city; ///< pointer to current city
    Route* prev; ///< pointer to previous elem of the route
    Route *next; ///< pointer to next elem of the route
};
/**
 * Struktura przechowująca mapę dróg krajowych.
 */
struct Map
{
    City *first_city; ///< pointer to first city in BST and list of all cities
    Road *first_road; ///< pointer to first road in list of all roads
    Route* routes[1000]; ///< table of pointers to last elems of routes,
// routes[0] is useless but I want to have routes[999]
};

/** @brief Tworzy nową strukturę.
 * Tworzy nową, pustą strukturę niezawierającą żadnych miast, odcinków dróg ani
 * dróg krajowych.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci.
 */
Map* newMap()
{
    Map *map = malloc(sizeof(Map));
    if(map == NULL)
    {
        return NULL;
    }
    map->first_city = NULL;
    map->first_road = NULL;
    for(int i=0; i<1000; i++)
    {
        map->routes[i] = NULL;
    }
    return map;
}
// setting pointers on NULL
/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p map.
 * Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] map        – wskaźnik na usuwaną strukturę.
 */
void deleteMap(Map *map)
{
    if(map != NULL)
    {
        City *help_city = map->first_city;
        while(help_city != NULL)
        {
            City *pom = help_city;
            free(pom->name);
            help_city = help_city->next_city;
            free(pom);
        }

        Road *help_road = map->first_road;
        while(help_road != NULL)
        {
            Road *pom = help_road;
            help_road = help_road->next_road;
            free(pom);
        }
        for(int i=0; i<1000; i++)
        {
            while(map->routes[i] != NULL)
            {
                Route *pom = map->routes[i];
                map->routes[i] = map->routes[i]->prev;
                free(pom);
            }
        }

        free(map);
    }
}
// deleting, cities, roads, routes

/** @brief check if string can be a name of a city
 * check if sting meets requirements given in the task
 * @param a - pointer to a string
 * @return true if the string can be a name of a city1
 * false if it can't
 */
bool check_name(const char *a)
{
    int i=0;
    int A = *a;
    while(*(a+i) != '\0' && !(A>-1 && A<32) && *(a+i) != ';')
    {
        i++;
        A = *(a+i);
    }
    return *(a+i) == '\0';
}
/** @brief compare two strings
 * compare ascii codes of two strings,
 * first in ascii order is this string which char
 * in the first place that strings are different
 * has smaller ascii code,
 * @param[in] a - pointer to string
 * @param[in] b - pointer to string
 * @return 0 if these are identical strings,
 * 1 if string a is before string b in ascii order,
 * -1 if string b is before string a in ascii order
 */
int compare_names(const char *a, const char *b)
{
    int i=0;

    while(*(a+i) != '\0' && *(b+i) != '\0' && *(a+i) == *(b+i))
    {
        i++;
    }

    if(*(a+i) == *(b+i) && *(b+i) == '\0')
    {
        return 0;
    }

    int A = *(a+i);
    int B = *(b+i);

    if(A < B)
    {
        return 1;
    }

    return -1;
}
// 0 - same strings, 1 - a before b in ascii order, -1 - b before a


/** @brief counts number of digits
 * counts number of digits w,
 * if w < 0 it adds also 1 for "-"
 * @param[in] w - int
 * @return number of digits in w
 */
int Int_length(int w)
{
    int h = w;
    int i = 0;
    if(w<0)
    {
        i++;
    }
    while(h != 0)
    {
        h = h/10;
        i++;
    }
    return i;
}
/** @brief counts number of chars in string
 * counts number of chars in string that a
 * points to,
 * it assumes that string can be a name of a city
 * @param[in] a - pointer to string
 * @return number of chars in string
 */
int Char_length(const char *a)
{
    int i = 0;
    int A = *a;
    while(*(a+i) != '0' && !(A>-1 && A<32) && *(a+i) != ';')
    {
        i++;
        A = *(a+i);
    }
    return i;
}
/** @brief converts int to char
 * write to help next digits of w
 * @param[in] help - pointer to pointer to a part of a string that
 * is going to be overwritten
 * @param[in] w -int
 */
void Int_to_Char(char **help, int w)
{
    int i = Int_length(w);
    int s = i;
    int g = w;
    if(w < 0)
    {
        g = -w;
    }
    while((i != 0 && w >= 0) || (i != 1 && w < 0))
    {
        i--;
        *(*help+i) = (g % 10)+48;
        g = g/10;
    }
    if(w < 0)
    {
        i--;
        *(*help+i) = 45;
    }
    *help = (*help)+s;
}
/** @brief overwrites string
 * write to help next chars
 * @param[in] help - pointer to pointer to a part of a string that
 * is going to be overwritten
 * @param[in] a - pointer to a string
 */
void Char_to_Char(char **help, const char *a)
{
    int i = Char_length(a);
    int s = 0;
    while(s != i)
    {
        **help = *(a+s);
        *help = ((*help)+1);
        s++;
    }
}


/** @brief creates new road and sets its attributes
 * creates new struct Road that connects
 * cities that names are identical like these ones that
 * city1 and city2 point to,
 * sets its built year as builtYear,
 * sets its length as length,
 * sets most of the pointers as NULL
 * adds this struct to list of all roads in the map
 * @param[in] map - pointer to the map
 * @param[in] city1 - pointer to a string that represents name of a city
 * @param[in] city2 - pointer to a string that represents name of a city
 * @param[in] length - length of new road(km)
 * @param[in] builtYear = built year of new road
 * @return pointer to new road or NULL if alloation
 * of a memory failed
  */
Road* create_road(Map *map, City *city1, City *city2,
                  unsigned length, int builtYear)
{
    Road *road = malloc(sizeof(Road));
    if(road == NULL)
    {
        return NULL;
    }
    road->city1 = city1;
    road->city2 = city2;
    road->age = builtYear;
    road->length = length;
    road->left_road_city1 = NULL;
    road->right_road_city1 = NULL;
    road->left_road_city2 = NULL;
    road->right_road_city2 = NULL;
    road->next_road_city1 = NULL;
    road->next_road_city2 = NULL;
    road->next_road = NULL;
    for(int i=0; i<1000; i++)
    {
        road->routes[i] = false;
    }
    if(map->first_road == NULL)
    {
        map->first_road = road;
    }
    Road *help_road = map->first_road->next_road;
    map->first_road->next_road = road;
    road->next_road = help_road;
    return road;
}

/** @brief creates new city and sets its attributes
 * creates new city, sets its name,
 * sets most of the pointers as NULL
 * adds city to a list of all cities in the map
 * @param[in] map - pointer to the map
 * @param[in] city_name  pointer to a string that represents name of a city
 * @return pointer to new city or NULL if allocation
 * of a memory failed
 */
City* create_city(Map *map, const char *city_name)
{
    City *city = malloc(sizeof(City));
    if(city == NULL)
    {
        return NULL;
    }
    int l = Char_length(city_name);
    char *h = calloc(sizeof(char), l+1);
    if(h == NULL)
    {
        return NULL;
    }
    *(h+l) = '\0';
    char *name = h;
    Char_to_Char(&h, city_name);
    city->right_city = NULL;
    city->left_city = NULL;
    city->next_city = NULL;
    for(int i=0; i<1000; i++)
    {
        city->routes[i] = false;
    }
    city->first_road_BST = NULL;
    city->first_road_L = NULL;
    city->name = name;
    if(map->first_city == NULL)
    {
        map->first_city = city;
    }
    else
    {
        City *help_city = map->first_city->next_city;
        map->first_city->next_city = city;
        city->next_city = help_city;
    }
// inserting into the list
    return city;
}
/** @brief looks for a city in mapy
 * looks for the city in BST with ascii order,
 * action depends on param create, it says if new city
 * should be created in case that the city doesn't exists,
 * if new city is created with create_city()
 * it is inserted into BST of all cities in the map,
 * @param[in] map - pointer to the mapy
 * @param[in] city - pointer to a string that represents name of a city
 * @param[in] create - says if new city can be created
 * @return if create == false it returns pointer to the city
 * or NULL if it doesn't r1_exists,
 * if create == true it returns pointer to the city
 */
City* find_or_create_City(Map *map, const char *city, bool create)
{

    City *tree = map->first_city;
    City *previous_city = NULL;
    if(tree == NULL)
    {
        if(create == true)
        {
            return create_city(map, city);
        }
        else
        {
            return NULL;
        }
    }
    int alpha_order;
    // if a_o = 1 the go to left son
    // if a_o = -1, rigth son
    // 0 - this is place where road from param can be
    while(tree != NULL)
    {
        alpha_order = compare_names(city, tree->name);
        if(alpha_order == 0)
        {
            break;
        }
        previous_city = tree;
        if(alpha_order == 1)
        {
            tree = tree->left_city;
        }
        if(alpha_order == -1)
        {
            tree = tree->right_city;
        }
    }

    if(alpha_order == 0)
    {
        return tree;
    }
    if(!create)
    {
        return NULL;
    }
    if(alpha_order == 1)
    {
        previous_city->left_city = create_city(map, city);
        tree = previous_city->left_city;
    }
    else
    {
        previous_city->right_city = create_city(map, city);
        tree = previous_city->right_city;
    }
    return tree;
}
// looks for the city
// if create == true
//      if it doesnt exist it is going to be created
//      return this
// else
//      if exists return this
//      else NULL


/** @brief looks for the road
 * looks for the road that connects cities named
 * by strings that city and fixed_city points to
 * in BST of roads in start,
 * each city has its own BST of roads which connects this city
 * with other, fixed_city should point to name of start,
 * if the road doesn't exists it can be instrted
 * in a proper place in BST if insert == true,
 * @param[in] start - pointer to the city
 * @param[in] city - pointer to a string that represents a name of city
 * @param[in] fixed_city - pointer to a string tat represents
 * a name of a city,
 * @param[in] insert - says if new road should be inserted into BST
 * @param[in] new_road - pointer to road that optionally can be inserted
 * into BST of roads in the city1
 * @return
 * if insert == true:
 * 	looks for a place to insert new_road
 *	returns *new_road if it can be inserted
 *	returns NULL if it already exists
 * if insert == false;
 * 	looks for a road
 *	returns this road if it exists
 *	returns NULL if it doesnt exists
 */
Road *find_or_insert_Road(City *start, const char *city, const char *fixed_city, bool insert, Road *new_road)
{
    Road *tree = start->first_road_BST;
    if(tree == NULL)
    {
        if(insert)
        {
            start->first_road_BST = new_road;
            start->first_road_L = new_road;
            return new_road;
        }
        else
        {
            return NULL;
        }
    }
    int one;
    //if one = 0 it means that certain road has fixed city as city1 in his struct
    int two;
    //if two = 0 it means that certain road has fixed city as city1 in his struct
    int alpha_order;
    // if a_o = 1 the go to left son
    // if a_o = -1, rigth son
    // 0 - this is place where road from param can be
    Road *previous_road = NULL;
    while(tree != NULL)
    {
        one = compare_names(fixed_city, tree->city1->name);
        two = compare_names(fixed_city, tree->city2->name);
        if(one == 0)
        {
            alpha_order = compare_names(city, tree->city2->name);
        }
        if(two == 0)
        {
            alpha_order = compare_names(city, tree->city1->name);
        }
        if(alpha_order == 0)
        {
            break;
        }
        previous_road = tree;
        if(alpha_order == 1)
        {
            if(one == 0)
            {
                tree = tree->left_road_city1;
            }
            if(two == 0)
            {
                tree = tree->left_road_city2;
            }
        }
        if(alpha_order == -1)
        {
            if(one == 0)
            {
                tree = tree->right_road_city1;
            }
            if(two == 0)
            {
                tree = tree->right_road_city2;
            }
        }
    }
    if(alpha_order == 0 && insert == true)
    {
        return NULL;
    }
    if(insert == false)
    {
        return tree;
    }
    if(insert == true)
    {
        if(alpha_order == 1)
        {
            if(one == 0)
            {
                previous_road->left_road_city1 = new_road;
            }
            if(two == 0)
            {
                previous_road->left_road_city2 = new_road;
            }
        }
        if(alpha_order == -1)
        {
            if(one == 0)
            {
                previous_road->right_road_city1 = new_road;
            }
            if(two == 0)
            {
                previous_road->right_road_city2 = new_road;
            }
        }
// inserting at the end list of roads
        Road *h = start->first_road_L;
        bool stop = false;
        while(!stop)
        {
            if(compare_names(fixed_city, h->city1->name) == 0)
            {
                if(h->next_road_city1 == NULL)
                {
                    h->next_road_city1 = new_road;
                    stop = true;
                }
                h = h->next_road_city1;
            }
            else
            {
                if(h->next_road_city2 == NULL)
                {
                    h->next_road_city2 = new_road;
                    stop = true;
                }
                h = h->next_road_city2;
            }
        }
    }
    return new_road;
}
/** @brief inserts subtree of roads to main tree of roads with fixed city
 * algorithm is almost the same as in find_or_insert_Road(),
 * but there is no adding to queue of al roads because there is already
 * new_road in this,
 * it is used only in deleteRoad()
 *
 */
void insert_Road(City *start, const char *city, const char *fixed_city, Road* road)
{
    if(road == NULL)
    {
        return;
    }
    Road *tree = start->first_road_BST;
    if(tree == NULL)
    {
        start->first_road_BST = road;
        return;
    }
    int one;
    //if one = 0 it means that certain road has fixed city as city1 in his struct
    int two;
    //if two = 0 it means that certain road has fixed city as city1 in his struct
    int alpha_order;
// if a_o = 1 the go to left son
// if a_o = -1, rigth son
// 0 - this is place where road from param can be
    Road *previous_road = NULL;
    while(tree != NULL)
    {
        one = compare_names(fixed_city, tree->city1->name);
        two = compare_names(fixed_city, tree->city2->name);
        if(one == 0)
        {
            alpha_order = compare_names(city, tree->city2->name);
        }
        if(two == 0)
        {
            alpha_order = compare_names(city, tree->city1->name);
        }
        if(alpha_order == 0)
        {
            break;
        }
        previous_road = tree;
        if(alpha_order == 1)
        {
            if(one == 0)
            {
                tree = tree->left_road_city1;
            }
            if(two == 0)
            {
                tree = tree->left_road_city2;
            }
        }
        if(alpha_order == -1)
        {
            if(one == 0)
            {
                tree = tree->right_road_city1;
            }
            if(two == 0)
            {
                tree = tree->right_road_city2;
            }
        }
    }
    if(alpha_order == 1)
    {
        if(one == 0)
        {
            previous_road->left_road_city1 = road;
        }
        if(two == 0)
        {
            previous_road->left_road_city2 = road;
        }
    }
    if(alpha_order == -1)
    {
        if(one == 0)
        {
            previous_road->right_road_city1 = road;
        }
        if(two == 0)
        {
            previous_road->right_road_city2 = road;
        }
    }
}

/** @brief Dodaje do mapy odcinek drogi między dwoma różnymi miastami.
 * Jeśli któreś z podanych miast nie istnieje, to dodaje go do mapy, a następnie
 * dodaje do mapy odcinek drogi między tymi miastami.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] length     – długość w km odcinka drogi;
 * @param[in] builtYear  – rok budowy odcinka drogi.
 * @return Wartość @p true, jeśli odcinek drogi został dodany.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, obie podane nazwy miast są identyczne, odcinek drogi między tymi
 * miastami już istnieje lub nie udało się zaalokować pamięci.
 */
bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear)
{
    if(map == NULL ||
            !check_name(city1) ||
            !check_name(city2) ||
            compare_names(city1, city2) == 0 ||
            builtYear == 0 ||
            length == 0)
    {
        return false;
    }
    City *Fcity = find_or_create_City(map, city1, true);
    City *Scity = find_or_create_City(map, city2, true);
    if(Fcity == NULL || Scity == NULL)
    {
        return false;
    }
    Road *new_road = create_road(map, Fcity, Scity, length, builtYear);
    if(new_road == NULL)
    {
        return false;
    }
    if(find_or_insert_Road(Fcity, Scity->name, Fcity->name, true, new_road) == NULL)
    {
        return false;
    }
    if(find_or_insert_Road(Scity, Fcity->name, Scity->name, true, new_road) == NULL)
    {
        return false;
    }
    return true;
}
/** @brief checks if road cen be added or overwritten
 * checks if road with parameters given in params
 * can come to existance
 * @param[in] map - pointer to struct that represents map
 * @param[in] city1 - pointer to a string that represents a name of city
 * @param[in] city2 - pointer to a string that represents a name of city
 * @param[in] length - unsigned that represents length of the road2
 * @param[in] Year - int that represents date of last improvement of the road
 * @return true if there is a possibility for the road
 * with parameters given in parameters to exists
 * else false
 */
bool checkRoad(Map *map, const char *city1, const char *city2, unsigned length, int Year)
{
    if(map == NULL ||
            !check_name(city1) ||
            !check_name(city2) ||
            compare_names(city1, city2) == 0 ||
            Year == 0 ||
            length == 0)
    {
        return false;
    }
    City *Fcity = find_or_create_City(map, city1, true);
    City *Scity = find_or_create_City(map, city2, true);
    if(Fcity == NULL || Scity == NULL)
    {
        return false;
    }
    Road *r = find_or_insert_Road(Fcity, Scity->name, Fcity->name, false, NULL);
    if(r == NULL)
    {
      return true;
    }
    else
    {
      if(length != r->length)
      {
        return false;
      }
      else
      {
        if(Year < r->age)
        {
          return false;
        }
      }
    }
    return true;
}


/** @brief Modyfikuje rok ostatniego remontu odcinka drogi.
 * Dla odcinka drogi między dwoma miastami zmienia rok jego ostatniego remontu
 * lub ustawia ten rok, jeśli odcinek nie był jeszcze remontowany.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] repairYear – rok ostatniego remontu odcinka drogi.
 * @return Wartość @p true, jeśli modyfikacja się powiodła.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, któreś z podanych miast nie istnieje, nie ma odcinka drogi między
 * podanymi miastami, podany rok jest wcześniejszy niż zapisany dla tego odcinka
 * drogi rok budowy lub ostatniego remontu.
 */
bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear)
{
    if(map == NULL ||
            !check_name(city1) ||
            !check_name(city2) ||
            compare_names(city1, city2) == 0 ||
            repairYear == 0)
    {
        return false;
    }
    City *Fcity = find_or_create_City(map, city1, true);
    City *Scity = find_or_create_City(map, city2, true);
    if(Fcity == NULL || Scity == NULL)
    {
        return false;
    }
    Road *road = find_or_insert_Road(Fcity, Scity->name, Fcity->name, false, NULL);
    if(road == NULL)
    {
        return false;
    }
    if(road->age > repairYear)
    {
        return false;
    }
    road->age = repairYear;
    return true;
}

/** sets some attributes of cities
 * sets these attributes that are used only
 * while searching for new route
 */
void set_attr(Map *map)
{
    if(map != NULL)
    {
        City *g = map->first_city;
        while(g != NULL)
        {
            g->equivalence = false;
            g->equivalence_l = true;
            // it is checked inly if equivalence is false
            g->eldest = 99999;
            g->eldest_0equi = 99999;
            // 0 means undefined
            g->dist = -1;
            g->prev_road = NULL;
            g = g->next_city;
        }
    }
}
/**@brief Dijkstra's algorithm
 * changes attributes of cities:
 * eldest, distm equivalence, prev_road
 * @param[in] start - pointer to city from wich algorithm starts
 */
void Dkstra(City* start)
{
    CityQ *q = newCityQ();
    // queue of cities to visit
    addCityQ(start, q);
    start->prev_road = NULL;
    start->dist = 0;
    start->equivalence = true;
    City *g;
    // length of route, -1 means idnefined yet
    int dist;
    while(!emptyCityQ(q))
    {
        takeCityQ(&g, q);
        Road *r = g->first_road_L;
        while(r != NULL)
        {
            dist = r->length + g->dist;
            if(compare_names(g->name, r->city1->name) == 0)
            {
                if(r->city2->dist == -1 || r->city2->dist >= dist)
                {
                    if(r->city2->dist >= dist &&
                    (r->city2->prev_road != r || r->city2->dist > dist))
                    {

                        if(r->city2->dist == dist)
                        {
                            r->city2->equivalence_l = false;
                        }
                        else
                        {
                            r->city2->equivalence_l = true;
                        }
                        if(r->city2->eldest_0equi > g->eldest || r->city2->eldest_0equi == 99999)
                        {
                            r->city2->eldest_0equi = g->eldest;
                        }
                        if(r->city2->eldest_0equi > r->age)
                        {
                            r->city2->eldest_0equi = r->age;
                        }

                    }
                    else
                    {
                        if(r->city2->dist == -1)
                        {
                            if(g->eldest < r->age)
                            {
                                r->city2->eldest_0equi = g->eldest;
                            }
                            else
                            {
                                r->city2->eldest_0equi = r->age;
                            }
                        }
                    }
                    if(r->city2->dist == -1 || dist < r->city2->dist ||
                            (r->city2->dist == dist && r->city2->eldest < r->age && r->city2->eldest < g->eldest))
                    {
                        r->city2->equivalence = true;
                        r->city2->prev_road = r;
                        r->city2->dist = dist;
                        if(g->eldest < r->age)
                        {
                            r->city2->eldest = g->eldest;
                        }
                        else
                        {
                            r->city2->eldest = r->age;
                        }
                    }

                    if(r->city2->dist == dist &&
                            (r->city2->eldest == r->age || r->city2->eldest == g->eldest) &&
                            (r->city2->eldest <= r->age && r->city2->eldest <= g->eldest) &&
                            r->city2->prev_road != r)
                    {
                        r->city2->equivalence = false;
                    }

                    addCityQ(r->city2, q);
                }
                r = r->next_road_city1;
            }
            else
            {
                if(r->city1->dist == -1 || r->city1->dist >= dist)
                {
                    if(r->city1->dist >= dist &&
                    (r->city1->prev_road != r || r->city1->dist > dist))
                    {
                        if(r->city1->dist == dist)
                        {
                            r->city1->equivalence_l = false;
                        }
                        else
                        {
                            r->city1->equivalence_l = true;
                        }
                        if(r->city1->eldest_0equi > g->eldest || r->city1->eldest_0equi == 99999)
                        {
                            r->city1->eldest_0equi = g->eldest;
                        }
                        if(r->city1->eldest_0equi > r->age)
                        {
                            r->city1->eldest_0equi = r->age;
                        }

                    }
                    else
                    {
                        if(r->city1->dist == -1)
                        {
                            if(g->eldest < r->age)
                            {
                                r->city1->eldest_0equi = g->eldest;
                            }
                            else
                            {
                                r->city1->eldest_0equi = r->age;
                            }
                        }
                    }
                    if(r->city1->dist == -1 || dist < r->city1->dist ||
                            (r->city1->dist == dist && r->city1->eldest < r->age && r->city1->eldest < g->eldest))
                    {
                        r->city1->equivalence = true;
                        r->city1->prev_road = r;
                        r->city1->dist = dist;
                        if(g->eldest < r->age)
                        {
                            r->city1->eldest = g->eldest;
                        }
                        else
                        {
                            r->city1->eldest = r->age;
                        }
                    }

                    if(r->city1->dist == dist &&
                            (r->city1->eldest == r->age || r->city1->eldest == g->eldest) &&
                            (r->city1->eldest <= r->age && r->city1->eldest <= g->eldest) &&
                            r->city1->prev_road != r)
                    {
                        r->city1->equivalence = false;
                    }

                    addCityQ(r->city1, q);
                }
                r = r->next_road_city2;
            }
        }
    }
    free(q);
}
/**@brief Dijkstra's algorithm
 * changes attributes of cities:
 * eldest, distm equivalence, prev_road
 * cities from route which id equals routeId
 * can't be added to queue of cities, this is only
 * difference betweeen in compare to Dkstra(),
 * @param[in] start - pointer to city from wich algorithm starts
 * @param[in] routeId - id of "forbidden" route
 */
void Dkstra2(City *start, int routeId)
{
    CityQ *q = newCityQ();
    addCityQ(start, q);
    start->prev_road = NULL;
    start->dist = 0;
    start->equivalence = true;
    City *g;
    // length of route, -1 means idnefined yet
    int dist;
    while(!emptyCityQ(q))
    {
        takeCityQ(&g, q);
        Road *r = g->first_road_L;
        while(r != NULL)
        {
            dist = r->length + g->dist;
            if(compare_names(g->name, r->city1->name) == 0)
            {
                if(r->city2->dist == -1 || r->city2->dist >= dist)
                {
                    if(r->city2->dist >= dist &&
                    (r->city2->prev_road != r || r->city2->dist > dist))
                    {
                        if(r->city2->dist == dist)
                        {
                            r->city2->equivalence_l = false;
                        }
                        else
                        {
                            r->city2->equivalence_l = true;
                        }
                        if(r->city2->eldest_0equi > g->eldest || r->city2->eldest_0equi == 99999)
                        {
                            r->city2->eldest_0equi = g->eldest;
                        }
                        if(r->city2->eldest_0equi > r->age)
                        {
                            r->city2->eldest_0equi = r->age;
                        }

                    }
                    else
                    {
                        if(r->city2->dist == -1)
                        {
                            if(g->eldest < r->age)
                            {
                                r->city2->eldest_0equi = g->eldest;
                            }
                            else
                            {
                                r->city2->eldest_0equi = r->age;
                            }
                        }
                    }
                    if(r->city2->dist == -1 || dist < r->city2->dist ||
                            (r->city2->dist == dist && r->city2->eldest < r->age && r->city2->eldest < g->eldest))
                    {
                        r->city2->equivalence = true;
                        r->city2->prev_road = r;
                        r->city2->dist = dist;
                        if(g->eldest < r->age)
                        {
                            r->city2->eldest = g->eldest;
                        }
                        else
                        {
                            r->city2->eldest = r->age;
                        }
                    }

                    if(r->city2->dist == dist &&
                            (r->city2->eldest == r->age || r->city2->eldest == g->eldest) &&
                            (r->city2->eldest <= r->age && r->city2->eldest <= g->eldest) &&
                            r->city2->prev_road != r)
                    {
                        r->city2->equivalence = false;
                    }
                    if(!r->city2->routes[routeId])
                    {
                        addCityQ(r->city2, q);
                    }
                }
                r = r->next_road_city1;
            }
            else
            {
                if(r->city1->dist == -1 || r->city1->dist >= dist)
                {
                    if(r->city1->dist >= dist &&
                    (r->city1->prev_road != r || r->city1->dist > dist))
                    {
                        if(r->city1->dist == dist)
                        {
                            r->city1->equivalence_l = false;
                        }
                        else
                        {
                            r->city1->equivalence_l = true;
                        }
                        if(r->city1->eldest_0equi > g->eldest || r->city1->eldest_0equi == 99999)
                        {
                            r->city1->eldest_0equi = g->eldest;
                        }
                        if(r->city1->eldest_0equi > r->age)
                        {
                            r->city1->eldest_0equi = r->age;
                        }

                    }
                    else
                    {
                        if(r->city1->dist == -1)
                        {
                            if(g->eldest < r->age)
                            {
                                r->city1->eldest_0equi = g->eldest;
                            }
                            else
                            {
                                r->city1->eldest_0equi = r->age;
                            }
                        }
                    }
                    if(r->city1->dist == -1 || dist < r->city1->dist ||
                            (r->city1->dist == dist && r->city1->eldest < r->age && r->city1->eldest < g->eldest))
                    {
                        r->city1->equivalence = true;
                        r->city1->prev_road = r;
                        r->city1->dist = dist;
                        if(g->eldest < r->age)
                        {
                            r->city1->eldest = g->eldest;
                        }
                        else
                        {
                            r->city1->eldest = r->age;
                        }
                    }

                    if(r->city1->dist == dist &&
                            (r->city1->eldest == r->age || r->city1->eldest == g->eldest) &&
                            (r->city1->eldest <= r->age && r->city1->eldest <= g->eldest) &&
                            r->city1->prev_road != r)
                    {
                        r->city1->equivalence = false;
                    }
                    if(!r->city1->routes[routeId])
                    {
                        addCityQ(r->city1, q);
                    }
                }
                r = r->next_road_city2;
            }
        }
    }
    free(q);
}

/**@brief Dijkstra's algorithm
 * changes attributes of cities:
 * eldest, distm equivalence, prev_road
 * cities from route which id equals routeId
 * can't be added to queue of cities,
 * road is omitted, this
 * is only difference in compare to Dkstra2()
 * @param[in] start - pointer to city from wich algorithm starts
 * @param[in] routeId - id of "forbidden" route
 * @param[in] road - pointer to "forbidden" road
 */
void Dkstra3(City *start, int routeId, Road *road)
{
    CityQ *q = newCityQ();
    addCityQ(start, q);
    start->prev_road = NULL;
    start->dist = 0;
    start->equivalence = true;
    City *g;
    // length of route, -1 means idnefined yet
    int dist;
    while(!emptyCityQ(q))
    {
        takeCityQ(&g, q);
        Road *r = g->first_road_L;
        while(r != NULL)
        {
            dist = r->length + g->dist;
            if(compare_names(g->name, r->city1->name) == 0)
            {
                if(r == road)
                {
                    goto skip1;
                }
                if(r->city2->dist == -1 || r->city2->dist >= dist)
                {

                    if(r->city2->dist >= dist &&
                    (r->city2->prev_road != r || r->city2->dist > dist))
                    {
                        if(r->city2->dist == dist)
                        {
                            r->city2->equivalence_l = false;
                        }
                        else
                        {
                            r->city2->equivalence_l = true;
                        }
                        if(r->city2->eldest_0equi > g->eldest || r->city2->eldest_0equi == 99999)
                        {
                            r->city2->eldest_0equi = g->eldest;
                        }
                        if(r->city2->eldest_0equi > r->age)
                        {
                            r->city2->eldest_0equi = r->age;
                        }

                    }
                    else
                    {
                        if(r->city2->dist == -1)
                        {
                            if(g->eldest < r->age)
                            {
                                r->city2->eldest_0equi = g->eldest;
                            }
                            else
                            {
                                r->city2->eldest_0equi = r->age;
                            }
                        }
                    }
                    if(r->city2->dist == -1 || dist < r->city2->dist ||
                            (r->city2->dist == dist && r->city2->eldest < r->age && r->city2->eldest < g->eldest))
                    {
                        r->city2->equivalence = true;
                        r->city2->prev_road = r;
                        r->city2->dist = dist;
                        if(g->eldest < r->age)
                        {
                            r->city2->eldest = g->eldest;
                        }
                        else
                        {
                            r->city2->eldest = r->age;
                        }
                    }

                    if(r->city2->dist == dist &&
                            (r->city2->eldest == r->age || r->city2->eldest == g->eldest) &&
                            (r->city2->eldest <= r->age && r->city2->eldest <= g->eldest) &&
                            r->city2->prev_road != r)
                    {
                        r->city2->equivalence = false;
                    }
                    if(!r->city2->routes[routeId])
                    {
                        addCityQ(r->city2, q);
                    }
                }
                skip1:
                r = r->next_road_city1;
            }
            else
            {
                if(r == road)
                {
                    goto skip2;
                }
                if(r->city1->dist == -1 || r->city1->dist >= dist)
                {
                    if(r->city1->dist >= dist &&
                    (r->city1->prev_road != r || r->city1->dist > dist))
                    {
                        if(r->city1->dist == dist)
                        {
                            r->city1->equivalence_l = false;
                        }
                        else
                        {
                            r->city1->equivalence_l = true;
                        }
                        if(r->city1->eldest_0equi > g->eldest || r->city1->eldest_0equi == 99999)
                        {
                            r->city1->eldest_0equi = g->eldest;
                        }
                        if(r->city1->eldest_0equi > r->age)
                        {
                            r->city1->eldest_0equi = r->age;
                        }

                    }
                    else
                    {
                        if(r->city1->dist == -1)
                        {
                            if(g->eldest < r->age)
                            {
                                r->city1->eldest_0equi = g->eldest;
                            }
                            else
                            {
                                r->city1->eldest_0equi = r->age;
                            }
                        }
                    }
                    if(r->city1->dist == -1 || dist < r->city1->dist ||
                            (r->city1->dist == dist && r->city1->eldest < r->age && r->city1->eldest < g->eldest))
                    {
                        r->city1->equivalence = true;
                        r->city1->prev_road = r;
                        r->city1->dist = dist;
                        if(g->eldest < r->age)
                        {
                            r->city1->eldest = g->eldest;
                        }
                        else
                        {
                            r->city1->eldest = r->age;
                        }
                    }

                    if(r->city1->dist == dist &&
                            (r->city1->eldest == r->age || r->city1->eldest == g->eldest) &&
                            (r->city1->eldest <= r->age && r->city1->eldest <= g->eldest) &&
                            r->city1->prev_road != r)
                    {
                        r->city1->equivalence = false;
                    }
                    if(!r->city1->routes[routeId])
                    {
                        addCityQ(r->city1, q);
                    }
                }
                skip2:
                r = r->next_road_city2;
            }
        }
    }
    free(q);
}
/** @brief Łączy dwa różne miasta drogą krajową.
 * Tworzy drogę krajową pomiędzy dwoma miastami i nadaje jej podany numer.
 * Wśród istniejących odcinków dróg wyszukuje najkrótszą drogę. Jeśli jest
 * więcej niż jeden sposób takiego wyboru, to dla każdego wariantu wyznacza
 * wśród wybranych w nim odcinków dróg ten, który był najdawniej wybudowany lub
 * remontowany i wybiera wariant z odcinkiem, który jest najmłodszy.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta.
 * @return Wartość @p true, jeśli droga krajowa została utworzona.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * wartość, istnieje już droga krajowa o podanym numerze, któreś z podanych
 * miast nie istnieje, obie podane nazwy miast są identyczne, nie można
 * jednoznacznie wyznaczyć drogi krajowej między podanymi miastami lub nie udało
 * się zaalokować pamięci.
 */
bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2)
{
    if(map == NULL ||
            routeId < 1 ||
            routeId > 999 ||
            !check_name(city1) ||
            !check_name(city2) ||
            compare_names(city1, city2) == 0)
    {
        return false;
    }
    if(map->routes[routeId] != NULL)
    {
        return false;
    }
    set_attr(map);
    City *start = find_or_create_City(map, city1, false);
    City *finish = find_or_create_City(map, city2, false);
    if(start == NULL || finish == NULL)
    {
        return false;
    }
    Dkstra(start);
    City *check = finish;
    if(!finish->equivalence)
    {
        return false;
    }
    while(check->prev_road != NULL)
    {
        if(!check->equivalence)
        {
            return false;
        }
        if(!check->equivalence_l && check != finish)
        {
            if(finish->eldest <= check->eldest_0equi)
            {
                return false;
            }
        }
        if(check == check->prev_road->city1)
        {
            check = check->prev_road->city2;
        }
        else
        {
            check = check->prev_road->city1;
        }
    }

    map->routes[routeId] = malloc(sizeof(Route));
    start->routes[routeId] = true;
    map->routes[routeId]->next = NULL;
    map->routes[routeId]->prev = NULL;
    map->routes[routeId]->city = finish;
    map->routes[routeId]->prev_road = finish->prev_road;
    Route *route = map->routes[routeId];
    City *c = finish;

    c->routes[routeId] = true;
    c->prev_road->routes[routeId] = true;
    if(c->prev_road->city1 == c)
    {
        c = c->prev_road->city2;
    }
    else
    {
        c = c->prev_road->city1;
    }
    //adding finish city to route
    while(c != start)
    {
        c->routes[routeId] = true;
        c->prev_road->routes[routeId] = true;
        route->prev = malloc(sizeof(Route));
        route->prev->next = route;
        route = route->prev;
        route->city = c;
        route->prev_road = c->prev_road;
        route->prev = NULL;
        if(c == start)
        {
            break;
        }
        if(c->prev_road->city1 == c)
        {
            c = c->prev_road->city2;
        }
        else
        {
            c = c->prev_road->city1;
        }
    }
// adding cities to route with start city

    c->routes[routeId] = true;
    route->prev = malloc(sizeof(Route));
    route->prev->next = route;
    route = route->prev;
    route->city = c;
    route->prev = NULL;
    route->prev_road = NULL;
    // adding start city

    return true;
}

/**
 * free each element in list of routes
 * it is used only in extendRoute() to free r1
 * param[in] r - pointer to first elem in list of routes
 */
void FREE1(Route *r)
{
    if(r != NULL)
    {
        assert(r->prev == NULL);
        while(r!= NULL)
        {
            Route *help = r;
            r = r->next;
            free(help);
        }
    }
}

/**
 * free each element in list of routes
 * it is used in extendRoute() to free r2
 * param[in] r - pointer to last elem in list of routes
 */
void FREE2(Route *r)
{
    if(r != NULL)
    {
        assert(r->next == NULL);
        while(r!= NULL)
        {
            Route *help = r;
            r = r->prev;
            free(help);
        }
    }
}

/**@brief links two lists of routes
 * links r1 to the end of route
 * @param[in] map - poiner to the map
 * @param[in] routeId - id of the route that is going to be extended
 * @param[in] r1 - pointer to first elem in a list of routes
 * @param[in] route - pointer to last elem in a list of routes
 */
void LINK1(Map *map, int routeId, Route *r1, Route *route)
{
    //this part links r1 with existing route
    r1->prev = route;
    assert(route != NULL);
    route->next = r1;
    r1->prev = route;
    while(route->prev != NULL)
    {
        route = route->prev;
    }
    while(route->next != NULL)
    {
        route->city->routes[routeId] = true;
        route = route->next;
        route->prev_road->routes[routeId] = true;
    }
    route->city->routes[routeId] = true;
    while(map->routes[routeId]-> next != NULL)
    {
        map->routes[routeId] = map->routes[routeId]->next;
    }
}
/** links two lists of routes
 * links r2 to the beginning of route
 * @param[in] map - poiner to the map
 * @param[in] routeId - id of the route that is going to be extended
 * @param[in] r2 - pointer to last elem in list of routes
 * @param[in] r - pointer to road that connects last city in
 * route r2 with first city in route which id equals to routeId
 * @param[in] route - pointer to last elem in a list of routes
 */
void LINK2(Map *map, int routeId, Route *r2, Road *r, Route *route)
{
    while(route->prev != NULL)
    {
        route = route->prev;
    }
    r->routes[routeId] = true;
    route->prev = r2;
    r2->next = route;
    r2 = r2->next;
    r2->prev_road = r;
    while(r2->prev != NULL)
    {
        r2->city->routes[routeId] = true;
        r2->city->prev_road->routes[routeId] = true;
        r2 = r2->prev;
    }
    r2->city->routes[routeId] = true;
    while(map->routes[routeId]->next != NULL)
    {
        map->routes[routeId] = map->routes[routeId]->next;
    }
}
/** @brief Wydłuża drogę krajową do podanego miasta.
 * Dodaje do drogi krajowej nowe odcinki dróg do podanego miasta w taki sposób,
 * aby nowy fragment drogi krajowej był najkrótszy. Jeśli jest więcej niż jeden
 * sposób takiego wydłużenia, to dla każdego wariantu wyznacza wśród dodawanych
 * odcinków dróg ten, który był najdawniej wybudowany lub remontowany i wybiera
 * wariant z odcinkiem, który jest najmłodszy.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej;
 * @param[in] city       – wskaźnik na napis reprezentujący nazwę miasta.
 * @return Wartość @p true, jeśli droga krajowa została wydłużona.
 * Wartość @p false, jeśli wystąpił błąd: któryś z parametrów ma niepoprawną
 * nazwę, nie istnieje droga krajowa o podanym numerze, nie ma miasta o podanej
 * nazwie, przez podane miasto już przechodzi droga krajowa o podanym numerze,
 * podana droga krajowa kończy się w podanym mieście, nie można jednoznacznie
 * wyznaczyć nowego fragmentu drogi krajowej lub nie udało się zaalokować
 * pamięci.
 */
bool extendRoute(Map *map, unsigned routeId, const char *city)
{
    if(map == NULL ||
            routeId < 1 ||
            routeId > 999 ||
            !check_name(city))
    {
        return false;
    }
    if(map->routes[routeId] == NULL)
    {
        return false;
    }
    City *finish = find_or_create_City(map, city, false);
    if(finish == NULL)
    {
        return false;
    }
    Route *w = map->routes[routeId];
    while(w != NULL)
    {
        w = w->prev;
    }
    if(finish->routes[routeId])
    {
        return false;
    }
    City *c;
    set_attr(map);
    City *start = map->routes[routeId]->city;
    Dkstra2(start, routeId);
    bool r1_exists = false;
    bool r1_equivalence = false;
    int r1_age = 0;
    int r1_length = 0;
    Route *r1 = NULL;
    if(finish->prev_road != NULL)
    {
        r1_equivalence = finish->equivalence;
        City *check = finish;
        while(check->prev_road != NULL)
        {
            if(!check->equivalence)
            {
                r1_equivalence = false;
            }
            if(!check->equivalence_l && finish != check)
            {
                if(finish->eldest <= check->eldest_0equi)
                {
                    r1_equivalence = false;
                }
            }
            if(check == check->prev_road->city1)
            {
                check = check->prev_road->city2;
            }
            else
            {
                check = check->prev_road->city1;
            }
        }
        if(r1_equivalence)
        {
            r1 = malloc(sizeof(Route));
            r1->next = NULL;
            r1_length = 0;
            r1_age = finish->eldest;
            c = finish;
            while(c != start)
            {
                if(c->prev_road == NULL)
                {
                    break;
                }
                r1->city = c;
                r1->prev_road = c->prev_road;
                r1_length = r1_length + c->prev_road->length;
                r1->prev = NULL;
                if(r1->prev_road != NULL)
                {
                    if(c == c->prev_road->city1)
                    {
                        c = c->prev_road->city2;
                    }
                    else
                    {
                        c = c->prev_road->city1;
                    }
                    if(c != start)
                    {
                        r1->prev = malloc(sizeof(Route));
                        r1->prev->next = r1;
                        r1 = r1->prev;
                    }
                }
            }
            r1_exists = true;
        }
    }
    else
    {
        r1_equivalence = false;
    }
// making route that can be added at the end to the existing route

    set_attr(map);
    start = finish;
    Route *h = map->routes[routeId];
    while(h->prev != NULL)
    {

        h = h->prev;
    }
    finish = h->city;
    // want to get first city in route

    Dkstra2(start, routeId);
    bool r2_exists = false;
    bool r2_equivalence = false;
    int r2_age = 0;
    int r2_length = 0;
    Route *r2 = NULL;
    Road *road2 = NULL;
    start->prev_road = NULL;
    if(finish->prev_road != NULL)
    {
        r2_equivalence = finish->equivalence;
        City *check = finish;
        while(check->prev_road != NULL)
        {
            if(!check->equivalence)
            {
                r2_equivalence = false;
            }
            if(!check->equivalence_l && check != finish)
            {
                if(finish->eldest <= check->eldest_0equi)
                {
                    r2_equivalence = false;
                }
            }

            if(check == check->prev_road->city1)
            {
                check = check->prev_road->city2;
            }
            else
            {
                check = check->prev_road->city1;
            }
        }
        if(r2_equivalence)
        {
            r2 = malloc(sizeof(Route));
            r2->next = NULL;
            r2_length = 0;
            r2_age = finish->eldest;
            c = finish;
            road2 = c->prev_road;
            do
            {
                r2_length = r2_length + c->prev_road->length;
                if(c == c->prev_road->city1)
                {
                    c = c->prev_road->city2;
                }
                else
                {
                    c = c->prev_road->city1;
                }
                r2->prev_road = c->prev_road;
                r2->city = c;
                r2->prev = NULL;
                if(c != start)
                {
                    r2->prev = malloc(sizeof(Route));
                    r2->prev->next = r2;
                    r2 = r2->prev;
                }
            }
            while(c != start);
            while(r2->next != NULL)
            {
                r2 = r2->next;
            }
            r2_exists = true;
        }
    }
    else
    {
        r2_equivalence = false;
    }
// making route that can be added at the beginning to the existing route
    if(!r1_exists && !r2_exists)
    {
        return false;
    }
    if(r1_exists && !r2_exists)
    {
        if(r1_equivalence)
        {
            LINK1(map, routeId, r1, map->routes[routeId]);
            return true;
        }
        else
        {
            FREE1(r1);
        }
        return false;
    }
    if(r2_exists && !r1_exists)
    {
        if(r2_equivalence)
        {
            LINK2(map, routeId, r2, road2, map->routes[routeId]);
            return true;
        }
        else
        {
            FREE2(r2);
        }
        return false;
    }
// if r2 will be a part of the route it will be linked by end of list
    if(r1_length < r2_length)
    {
        if(!r1_equivalence)
        {
            FREE1(r1);
            FREE2(r2);
            return false;
        }
        else
        {
            LINK1(map, routeId, r1, map->routes[routeId]);
            FREE2(r2);
        }
    }
    if(r1_length == r2_length)
    {
        if(r1_age > r2_age)
        {
            if(!r1_equivalence)
            {
                FREE1(r1);
                FREE2(r2);
                return false;
            }
            else
                LINK1(map, routeId, r1, map->routes[routeId]);
            FREE2(r2);
        }
        if(r1_age == r2_age)
        {
            FREE1(r1);
            FREE2(r2);
            return false;
        }
        if(r1_age < r2_age)
        {
            if(!r2_equivalence)
            {
                FREE1(r1);
                FREE2(r2);
                return false;
            }
            else
            {
                LINK2(map, routeId, r2, road2, map->routes[routeId]);
                FREE1(r1);
            }
        }
    }

    if(r1_length > r2_length)
    {
        if(!r2_equivalence)
        {
            FREE1(r1);
            FREE2(r2);
            return false;
        }
        else
        {
            LINK2(map, routeId, r2, road2, map->routes[routeId]);
            FREE1(r1);
        }
    }
    return true;
}
/** @brief looks for parent road in BST of roads
 * looks for parent road in BST of roads of city start
 * @param[in] start - pointer to city which BST is examined
 * @param[in] city - pointer to string, represents city which r links with other
 * @param[in] fixed_city - pointer to string, name of city start
 * @param[in] r - road which parent is wanted
 * @param[in] left - there info about last direction in tree is going to be written
 * @return NULL if there is no parent or pointer to parent road of r
 */
Road *find_parent_Road(City *start, const char *city, const char *fixed_city, Road *r, bool *left)
{
    Road *tree = start->first_road_BST;
    if(tree == r)
    {
        return NULL;
    }
    int one;
    //if one = 0 it means that certain road has fixed city as city1 in his struct
    int two;
    //if two = 0 it means that certain road has fixed city as city1 in his struct
    int alpha_order = 2;
    // if a_o = 1 the go to left son
    // if a_o = -1, rigth son
    // 0 - this is place where road from param can be
    Road *previous_road = NULL;
    while(tree != r)
    {
        one = compare_names(fixed_city, tree->city1->name);
        two = compare_names(fixed_city, tree->city2->name);
        if(one == 0)
        {
            alpha_order = compare_names(city, tree->city2->name);
        }
        if(two == 0)
        {
            alpha_order = compare_names(city, tree->city1->name);
        }
        if(alpha_order == 0)
        {
            break;
        }
        previous_road = tree;
        if(alpha_order == 1)
        {
            if(one == 0)
            {
                tree = tree->left_road_city1;
            }
            if(two == 0)
            {
                tree = tree->left_road_city2;
            }
        }
        if(alpha_order == -1)
        {
            if(one == 0)
            {
                tree = tree->right_road_city1;
            }
            if(two == 0)
            {
                tree = tree->right_road_city2;
            }
        }
        *left = (alpha_order == 1);
    }
    return previous_road;
}

/** @brief removes Road form everything
 * removes road from BST's, and List's
 * @param[in] map - pointer to map
 * @param[in] r - road to remove
 */
void deleteRoad(Map *map, Road *r)
{
    bool Fleft;
    bool Sleft;
    City *f = r->city1;
    City *s = r->city2;
    Road *F = find_parent_Road(r->city1, r->city2->name, r->city1->name, r, &Fleft);
    Road *S = find_parent_Road(r->city2, r->city1->name, r->city2->name, r, &Sleft);
    Road *rigth = r->right_road_city1;
    Road *left = r->left_road_city1;
    if(F == NULL)
    {
        r->city1->first_road_BST = left;
        if(rigth != NULL)
        {
            if(rigth->city1 == r->city1)
            {
                insert_Road(r->city1, rigth->city2->name, r->city1->name, rigth);
            }
            else
            {
                insert_Road(r->city1, rigth->city1->name, r->city1->name, rigth);
            }
        }
    }
    else
    {
        if(F->city1 == r->city1)
        {
            if(Fleft)
            {
                F->left_road_city1 = NULL;
            }
            else
            {
                F->right_road_city1 = NULL;
            }
        }
        else
        {
            if(Fleft)
            {
                F->left_road_city2 = NULL;
            }
            else
            {
                F->right_road_city2 = NULL;
            }
        }
        // set NULL instead of pointer to r of parent F

        if(left != NULL)
        {
            if(left->city1 == r->city1)
            {
                insert_Road(r->city1, left->city2->name, r->city1->name, left);
            }
            else
            {
                insert_Road(r->city1, left->city1->name, r->city1->name, left);
            }
        }
        if(rigth != NULL)
        {
            if(rigth->city1 == r->city1)
            {
                insert_Road(r->city1, rigth->city2->name, r->city1->name, rigth);
            }
            else
            {
                insert_Road(r->city1, rigth->city1->name, r->city1->name, rigth);
            }
        }
    }
// r->city1 as fixed city, removing r from BST of of roads of f

    rigth = r->right_road_city2;
    left = r->left_road_city2;

    if(S == NULL)
    {
        r->city2->first_road_BST = left;
        if(rigth != NULL)
        {
            if(rigth->city1 == r->city2)
            {
                insert_Road(r->city2, rigth->city2->name, r->city2->name, rigth);
            }
            else
            {
                insert_Road(r->city2, rigth->city1->name, r->city2->name, rigth);
            }
        }
    }
    else
    {
        if(S->city1 == r->city2)
        {
            if(Sleft)
            {
                S->left_road_city1 = NULL;
            }
            else
            {
                S->right_road_city1 = NULL;
            }
        }
        else
        {
            if(Sleft)
            {
                S->left_road_city2 = NULL;
            }
            else
            {
                S->right_road_city2 = NULL;
            }
        }

        // set NULL instead of pointer to r of parent S
        if(left != NULL)
        {
            if(left->city1 == r->city2)
            {
                insert_Road(r->city2, left->city2->name, r->city2->name, left);
            }
            else
            {
                insert_Road(r->city2, left->city1->name, r->city2->name, left);
            }
        }
        if(rigth != NULL)
        {
            if(rigth->city1 == r->city2)
            {
                insert_Road(r->city2, rigth->city2->name, r->city2->name, rigth);
            }
            else
            {
                insert_Road(r->city2, rigth->city1->name, r->city2->name, rigth);
            }
        }
    }
// r->city2 as fixed city, removing r from BST of roads of s


    Road *help = f->first_road_L;
    if(help == r)
    {
        f->first_road_L = help->next_road_city1;
    }
    else
    {
        bool stop = false;
        while(!stop)
        {
            if(help->next_road_city1 == r)
            {
                help->next_road_city1 = r->next_road_city1;
                stop = true;
            }
            if(help->next_road_city2 == r)
            {
                help->next_road_city2 = r->next_road_city1;
                stop = true;
            }
            if(help->city1 == f)
            {
                help = help->next_road_city1;
            }
            else
            {
                help = help->next_road_city2;
            }
        }
    }
//r->city1 fixed, deleting r from list of roads of f city

    help = s->first_road_L;
    if(help == r)
    {
        s->first_road_L = help->next_road_city2;
    }
    else
    {
        bool stop = false;
        while(!stop)
        {

            if(help->next_road_city1 == r)
            {
                help->next_road_city1 = r->next_road_city2;
                stop = true;
            }
            if(help->next_road_city2 == r)
            {
                help->next_road_city2 = r->next_road_city2;
                stop = true;
            }
            if(help->city1 == s)
            {
                help = help->next_road_city1;
            }
            else
            {
                help = help->next_road_city2;
            }
        }
    }
    //r->city2 fixed city, deleting r from list of roads of city s

    help = map->first_road;
    if(help == r)
    {
        map->first_road = help->next_road;
        free(help);
    }
    else
    {
        bool stop = false;
        while(!stop)
        {
            if(help->next_road == r)
            {
                help->next_road = r->next_road;
                free(r);
                stop = true;
            }
            help = help->next_road;
        }
    }
// deleting from list of all roads

}

// r->city2 as fixed city
/** @brief Usuwa odcinek drogi między dwoma różnymi miastami.
 * Usuwa odcinek drogi między dwoma miastami. Jeśli usunięcie tego odcinka drogi
 * powoduje przerwanie ciągu jakiejś drogi krajowej, to uzupełnia ją
 * istniejącymi odcinkami dróg w taki sposób, aby była najkrótsza. Jeśli jest
 * więcej niż jeden sposób takiego uzupełnienia, to dla każdego wariantu
 * wyznacza wśród dodawanych odcinków drogi ten, który był najdawniej wybudowany
 * lub remontowany i wybiera wariant z odcinkiem, który jest najmłodszy.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] city1      – wskaźnik na napis reprezentujący nazwę miasta;
 * @param[in] city2      – wskaźnik na napis reprezentujący nazwę miasta.
 * @return Wartość @p true, jeśli odcinek drogi został usunięty.
 * Wartość @p false, jeśli z powodu błędu nie można usunąć tego odcinka drogi:
 * któryś z parametrów ma niepoprawną wartość, nie ma któregoś z podanych miast,
 * nie istnieje droga między podanymi miastami, nie da się jednoznacznie
 * uzupełnić przerwanego ciągu drogi krajowej lub nie udało się zaalokować
 * pamięci.
 */
bool removeRoad(Map *map, const char *city1, const char *city2)
{
    if(map == NULL ||
            !check_name(city1) ||
            !check_name(city2))
    {
        return false;
    }
    City *c1 = find_or_create_City(map, city1, false);
    City *c2 = find_or_create_City(map, city2, false);
    if(c1 == NULL || c2 == NULL)
    {
        return false;
    }
    Road *r = find_or_insert_Road(c1, city2, city1, false, NULL);
    if(r == NULL)
    {
        return false;
    }
    Route* new_routes[1000];
    for(int i=1; i<1000; i++)
    {
        new_routes[i] = NULL;
    }

    for(int i=1; i<1000; i++)
    {
        City *start = NULL;
        City *finish = NULL;
        if(r->routes[i])
        {

            assert(map->routes[i]->next == NULL);
            while(map->routes[i]->prev != NULL)
            {
                if(c1 == map->routes[i]->city)
                {
                    start = c2;
                    finish = c1;
                    break;
                }
                if(c2 == map->routes[i]->city)
                {
                    start = c1;
                    finish = c2;
                    break;
                }
                map->routes[i] = map->routes[i]->prev;
            }

            while(map->routes[i]->next != NULL)
            {
                map->routes[i] = map->routes[i]->next;
            }


            set_attr(map);
            Dkstra3(start, i, r);
            City *check = finish;
            if(!finish->equivalence)
            {
                goto skip;
            }
            while(check->prev_road != NULL)
            {
                if(!check->equivalence)
                {
                    goto skip;
                }
                if(!check->equivalence_l && check != finish)
                {

                    if(finish->eldest <= check->eldest_0equi)
                    {
                        goto skip;
                    }
                }
                if(check == check->prev_road->city1)
                {
                    check = check->prev_road->city2;
                }
                else
                {
                    check = check->prev_road->city1;
                }
            }

            Route *route;
            Road *road;
            if(finish->prev_road == NULL)
            {
                goto skip;
            }
            route = malloc(sizeof(Route));
            if(r == NULL)
            {
                goto skip;
            }

            route->next = NULL;
            City *c = finish;
            road = c->prev_road;
            if(c == c->prev_road->city1)
            {
                c = c->prev_road->city2;
            }
            else
            {
                c = c->prev_road->city1;
            }

            while(c != start)
            {
                route->prev_road = c->prev_road;
                route->city = c;
                route->prev = NULL;
                if(c == c->prev_road->city1)
                {
                    c = c->prev_road->city2;
                }
                else
                {
                    c = c->prev_road->city1;
                }
                if(c != start)
                {
                    route->prev = malloc(sizeof(Route));
                    if(route->prev == NULL)
                    {
                        return false;
                    }
                    route->prev->next = route;
                    route = route->prev;
                    route->prev = NULL;
                }
            }

            while(route->next != NULL)
            {

                route = route->next;
            }
            Route *help = map->routes[i];
            new_routes[i] = malloc(sizeof(Route));
            new_routes[i]->next = NULL;
            do
            {
                new_routes[i]->city = help->city;
                new_routes[i]->prev = NULL;
                if(help->prev_road != r)
                {
                    new_routes[i]->prev_road = help->prev_road;
                    new_routes[i]->prev = malloc(sizeof(Route));
                    new_routes[i]->prev->next = new_routes[i];
                    new_routes[i] = new_routes[i]->prev;
                    help = help->prev;

                }
                else
                {
                    break;
                }
            }
            while(1);
// sorry for that while(1), I had no time, break works

            help = help->prev;
            new_routes[i]->prev_road = road;
            new_routes[i]->prev = route;
            route->next = new_routes[i];
            while(new_routes[i]->prev != NULL)
            {
                new_routes[i] = new_routes[i]->prev;
            }


            new_routes[i]->prev = malloc(sizeof(Route));
            new_routes[i]->prev->next = new_routes[i];
            new_routes[i] = new_routes[i]->prev;

            while(help != NULL)
            {
                new_routes[i]->city = help->city;
                new_routes[i]->prev_road = help->prev_road;
                new_routes[i]->prev = NULL;
                if(help->prev != NULL)
                {
                    new_routes[i]->prev = malloc(sizeof(Route));
                    if(new_routes[i]->prev == NULL)
                    {
                        goto skip;
                    }
                    new_routes[i]->prev->next = new_routes[i];
                    new_routes[i] = new_routes[i]->prev;
                }
                help = help->prev;
            }
            while(new_routes[i]->next != NULL)
            {
                new_routes[i] = new_routes[i]->next;
            }
        }
    }
    if(0 == 1)
    {
skip:
// if removing failed
        for(int i=1; i<1000; i++)
        {
            if(new_routes[i] != NULL)
            {
                while(new_routes[i]->next != NULL)
                {
                    new_routes[i] = new_routes[i]->next;
                }
                while(new_routes[i] != NULL)
                {
                    Route *pom = new_routes[i];
                    new_routes[i] = new_routes[i]->prev;
                    free(pom);
                }
            }
        }
        return false;
    }
    for(int i=1; i<1000; i++)
    {
        if(r->routes[i])
        {
            //chenges only these routes which lost road
            Route *delete = map->routes[i];
            while(delete->prev != NULL)
            {
                delete->city->routes[i] = false;
                delete->prev_road->routes[i] = false;
                Route *keep = delete;
                delete = delete->prev;
                free(keep);
            }
            delete->city->routes[i] = false;
            free(delete);

            map->routes[i] = new_routes[i];
            while(new_routes[i]->prev != NULL)
            {
                new_routes[i]->prev_road->routes[i] = true;
                new_routes[i]->city->routes[i] = true;
                new_routes[i] = new_routes[i]->prev;
            }
            new_routes[i]->prev_road = NULL;
            new_routes[i]->city->routes[i] = true;
        }
    }
    deleteRoad(map, r);
    return true;
}

/** @brief creates new route between two cities
 * creates new route between two cities,
 * this route cosists of one road
 * @param[in, out] map - pointer to struct that represents map
 * @param[in] routeId - unsigned that represents id of route
 * @param[in] city1 - pointer to a string that represents a name of city
 * @param[in] city2 - pointer to a string that represents a name of city
 * @return true if new route is made,
 * false if parameters are bad, or malloc failed
 */
bool makeRoute(Map *map, unsigned routeId, const char *city1, const char *city2)
{
  if(map == NULL ||
          !check_name(city1) ||
          !check_name(city2) ||
          routeId == 0 ||
          routeId > 999)
  {
      return false;
  }
  if(map->routes[routeId] != NULL)
  {
    return false;
  }
  City *c1 = find_or_create_City(map, city1, false);
  City *c2 = find_or_create_City(map, city2, false);
  if(c1 == NULL || c2 == NULL)
  {
      return false;
  }
  Road *r = find_or_insert_Road(c1, city2, city1, false, NULL);
  if(r == NULL)
  {
    return false;
  }
  Route *new = malloc(sizeof(Route));
  if(new == NULL){
    return false;
  }
  new->prev_road = NULL;
  new->prev = NULL;
  new->next = malloc(sizeof(Route));
  new->city = c1;
  new->next->prev = new;
  new = new->next;
  new->prev_road = r;
  new->next = NULL;
  new->city = c2;
  map->routes[routeId] = new;
  return true;
}
/** @brief checks if the routeId is ok
 * checks if route with given routeId exists
 * and if it is between 0 and 999
 * @param[in] map - pointer to struct that represents the map
 * @param[in] routeId - unsigned tthat represents Id of route
 * @return true if the route with given routeId doesnt exist
 * else returns false
 */
bool checkRouteId(Map *map, unsigned routeId)
{
  if(map == NULL ||
          routeId == 0 ||
          routeId > 999)
  {
    return false;
  }
  return (map->routes[routeId] == NULL);
}

/** @brief adds one road to route
 * extends route to given city in params,
 * to do this, it uses only one road
 * param[in, out] map - pointer to struct that represents map
 * param[in] routeId - unsigned that represents id of route
 * param[in] city - pointer to a string that represents a name of city
 */
void add_to_Route(Map *map, unsigned routeId, const char *city)
{
  if(map == NULL ||
          !check_name(city) ||
          routeId == 0 ||
          routeId > 999)
  {
      return;
  }
  if(map->routes[routeId] == NULL)
  {
    return;
  }
  City *c = find_or_create_City(map, city, false);
  City *finish = map->routes[routeId]->city;
  Road *r = find_or_insert_Road(c, finish->name, city, false, NULL);
  map->routes[routeId]->next = malloc(sizeof(Route));
  map->routes[routeId]->next->prev = map->routes[routeId];
  map->routes[routeId] = map->routes[routeId]->next;
  map->routes[routeId]->next = NULL;
  map->routes[routeId]->prev_road = r;
  map->routes[routeId]->city = c;
}

/** @brief removes route with given Id from the map
 * removes route with given in params Id,
 * if there is no route with given Id it doesn,t change the map
 * @param[in,out] map - pointer to struct that represents the map
 * @param[in] routeId - unsigned that represents Id of route
 * @return true if route with given Id has been removed,
 * else returns false
 */
bool removeRoute(Map *map, unsigned routeId){
  if(map == NULL ||
          routeId == 0 ||
          routeId > 999)
  {
    return false;
  }
  if(map->routes[routeId] == NULL)
  {
    return false;
  }
    Route *help = map->routes[routeId];
    Route *r = help;
    if(r == NULL)
    {
      return false;
    }
    while(r->prev != NULL){
      r->city->routes[routeId] = false;
      r->prev_road->routes[routeId] = false;
      help = r;
      r = r->prev;
      free(help);
    }
    r->city->routes[routeId] = false;
    free(r);

  map->routes[routeId] = NULL;
  return true;
}


/** @brief Udostępnia informacje o drodze krajowej.
 * Zwraca wskaźnik na napis, który zawiera informacje o drodze krajowej. Alokuje
 * pamięć na ten napis. Zwraca pusty napis, jeśli nie istnieje droga krajowa
 * o podanym numerze. Zaalokowaną pamięć trzeba zwolnić za pomocą funkcji free.
 * Informacje wypisywane są w formacie:
 * numer drogi krajowej;nazwa miasta;długość odcinka drogi;rok budowy lub
 * ostatniego remontu;nazwa miasta;długość odcinka drogi;rok budowy lub
 * ostatniego remontu;nazwa miasta;…;nazwa miasta.
 * Kolejność miast na liście jest taka, aby miasta @p city1 i @p city2, podane
 * w wywołaniu funkcji @ref newRoute, które utworzyło tę drogę krajową, zostały
 * wypisane w tej kolejności.
 * @param[in,out] map    – wskaźnik na strukturę przechowującą mapę dróg;
 * @param[in] routeId    – numer drogi krajowej.
 * @return Wskaźnik na napis lub NULL, gdy nie udało się zaalokować pamięci.
 */
char const* getRouteDescription(Map *map, unsigned routeId)
{
    if(map == NULL)
    {
        return NULL;
    }
    if(routeId > 999 || routeId < 1)
    {
        return calloc(sizeof(char), 1);
        // if routeId is bad there is no chance to do it rigth
    }
    if(map->routes[routeId] == NULL)
    {
        return calloc(sizeof(char), 1);
    }
    set_attr(map);
    int i = 1;
    // i counts how many chars is going to be in string
    i = i + Int_length(routeId);
    Route *r = map->routes[routeId];
    while(r != NULL)
    {
        i = i + Char_length(r->city->name);
        i++;
        if(r->prev_road != NULL)
        {
            i = i + Int_length(r->prev_road->length);
            i = i + Int_length(r->prev_road->age);
            i = i+2;
        }
        if(r->prev == NULL)
        {
            break;
        }
        r = r->prev;
    }
    char *new = calloc(sizeof(char), i);
    if(new == NULL)
    {
        return NULL;
    }
    for(int s=0; s<i; s++)
    {
        *(new+s) = '\0';
        // just in case
    }

    char *help = new;
    i = 0;
    Int_to_Char(&help, routeId);
    *help = ';';
    help = help+1;
    while(r != NULL)
    {
        if(r->prev_road != NULL)
        {
            Int_to_Char(&help, r->prev_road->length);
            *help = ';';
            help = help+1;
            Int_to_Char(&help, r->prev_road->age);
            *help = ';';
            help = help+1;
        }
        Char_to_Char(&help, r->city->name);
        r = r->next;
        if(r != NULL)
        {
            *help = ';';
            help = help+1;
        }
    }
    // fulfilling the string
    return new;
}
#endif
