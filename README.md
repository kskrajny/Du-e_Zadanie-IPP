# Duze_Zadanie-IPP
I część
Tegoroczne duże zadanie polega na zaimplementowaniu obsługi map dróg krajowych. 
Jako pierwszą część zadania należy zaimplementować moduł operacji na mapach drogowych. Opis interfejsu modułu znajduje się w pliku src/map.h w formacie komentarzy dla programu doxygen. Przykład użycia znajduje się w pliku src/map_main.c.

II część
Jako drugą część dużego zadania należy zaimplementować program, który, korzystając z modułu zaimplementowanego w części pierwszej, udostępnia operacje na mapie dróg. Program obsługuje tylko jedną mapę dróg. Ponadto należy zaimplementować skrypt w bashu.
Skrypt dla każdego podanego w argumentach numeru drogi krajowej po kolei szuka w podanym pliku informacji o tej drodze krajowej, wylicza jej długość i wypisuje jedną linię w formacie:

numer drogi krajowej;długość

Program akceptuje polecenia w formacie:
1.
numer drogi krajowej;nazwa miasta;długość odcinka drogi;rok budowy lub ostatniego remontu;nazwa miasta;długość odcinka drogi;rok budowy lub ostatniego remontu;nazwa miasta;…;nazwa miasta

To polecenie tworzy drogę krajową o podanym numerze i przebiegu. 
2.
addRoad;city1;city2;length;builtYear

3.
repairRoad;city1;city2;repairYear

4.
getRouteDescription;routeId


