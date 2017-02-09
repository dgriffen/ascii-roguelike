#ifndef IO_H
#define IO_H

#include <dungeon/dungeon.h>
#include <util/distance.h>

void print_room(DungeonRoom *room);

void print_dungeon(Dungeon *dungeon);

void print_distance_map(Distances* distances);

void save_dungeon(Dungeon *dungeon, char *path);

Dungeon load_dungeon(char *path);

#endif
