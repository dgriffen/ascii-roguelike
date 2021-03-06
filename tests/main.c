#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include <collections/heap.h>
#include <util/distance.h>
#include <dungeon/dungeon.h>
#include <io.h>

int compare_int(void *this, void *to) {
    return *(int *)this - *(int *)to;
}

int test_heap_build() {
    int init_array[10] = {5, 77, 23, 9, 15, 48, 2, 2, 63, 33};
    int sort_array[10] = {2, 2, 5, 9, 15, 23, 33, 48, 63, 77};
    Heap heap = init_heap(compare_int, sizeof(int));

    for(int i = 0; i < 10; i++) {
        heap_push(&heap, (void *)&init_array[i]);
    }

    for(int i = 0; i < 10; i++) {
        int n = *(int *)unwrap(heap_pop(&heap), 1);

        if(n != sort_array[i]) {
            return 1;
        }
    }

    return 0;
}

int test_heap_starts_empty() {
    Heap heap = init_heap(compare_int, sizeof(int));
    if (!heap_empty(&heap)) {
        return 1;
    }

    return 0;
}

int test_heap_not_empty() {
    Heap heap = init_heap(compare_int, sizeof(int));
    int i = 1;
    heap_push(&heap, (void *)&i);

    if (heap_empty(&heap)) {
        return 1;
    }
    return 0;
}

int test_heap_becomes_empty() {
    Heap heap = init_heap(compare_int, sizeof(int));
    int i = 1;
    heap_push(&heap, (void *)&i);
    heap_pop(&heap);

    if (!heap_empty(&heap)) {
        return 1;
    }
    return 0;
}


static int _len(void *context, Coordinate *this, Coordinate *to) {
    (void)(context);
    (void)(this);
    (void)(to);
    return 1;
}

static int _length_no_tunnel(void *context, Coordinate *this, Coordinate *to) {
    Dungeon *dungeon = (Dungeon *)context;
    (void)(this);
    DungeonBlock b_to = dungeon->blocks[to->row][to->col];
    if (b_to.type == ROCK || b_to.type == PILLAR) {
        return INT_MAX;
    } else {
        return 1;
    }
}

static int _length_tunnel(void *context, Coordinate *this, Coordinate *to) {
    Dungeon *dungeon = (Dungeon *)context;
    (void)(this);
    DungeonBlock b_to = dungeon->blocks[to->row][to->col];
    if (b_to.type == ROCK || b_to.type == PILLAR) {
        if(b_to.immutable) {
            return INT_MAX;
        }

        uint8_t hardness = b_to.hardness;
        if (hardness < HARDNESS_TIER_1) {
            return 1;
        } else if (hardness < HARDNESS_TIER_2) {
            return 2;
        } else if (hardness < HARDNESS_TIER_3) {
            return 3;
        } else if (hardness < HARDNESS_TIER_MAX) {
            return 4;
        } else {
            return INT_MAX;
        }
    } else {
        return 1;
    }
}

int test_dijkstra_no_segfault() {
    dijkstra(NULL, 60, 60, _len);

    return 0;
}

static int _dijkstra_descent_helper(int from_row, int from_col, Distances *d) {
    for(int row = 0; row < DUNGEON_HEIGHT; row++) {
        for(int col = 0; col < DUNGEON_WIDTH; col++) {
            int distance = d->d[row][col];
            bool lower = false;
            
            if((row == from_row && col == from_col) || distance == INT_MAX) {
                continue;
            }

            relative_array(1, row, col, DUNGEON_HEIGHT, DUNGEON_WIDTH, );
            int adjacent[8][2] = {{top, left}   , {top, col}   , {top, right},
                                  {row, left}   ,                {row, right},
                                  {bottom, left}, {bottom, col}, {bottom, right}};
            
            for(int i = 0; i < 8; i++) {
                int adj_row = adjacent[i][0];
                int adj_col = adjacent[i][1];
                lower |= d->d[adj_row][adj_col] < distance;
            }

            if (!lower) {
                printf("row: %d, col: %d\n", row, col);
                return 1;
            }
        }
    }
    return 0;
}

int test_dijkstra_always_descend_empty() {
    Distances d = dijkstra(NULL, 60, 60, _len);

    return _dijkstra_descent_helper(60, 60, &d);
}

int test_dijkstra_always_descend_no_tunnel() {
    srand(10);
    Options options;
    options.monsters = 30;
    options.room_tries = 1000;
    options.min_rooms = 10;
    options.hardness = 50;
    options.windiness = 30;
    options.max_maze_size = 2000;
    options.imperfection = 2000;

    Dungeon dungeon = create_dungeon(options); 
    Entity *player = unwrap(entity_retrieve(&dungeon.store, dungeon.player_id), 1);
    Distances d = dijkstra(&dungeon, player->player.row, player->player.col, _length_no_tunnel);

    return _dijkstra_descent_helper(player->player.row, player->player.col, &d);
}

int test_dijkstra_always_descend_tunnel() {
    srand(10);

    Options options;
    options.monsters = 30;
    options.room_tries = 1000;
    options.min_rooms = 10;
    options.hardness = 50;
    options.windiness = 30;
    options.max_maze_size = 2000;
    options.imperfection = 2000;

    Dungeon dungeon = create_dungeon(options); 
    Entity *player = unwrap(entity_retrieve(&dungeon.store, dungeon.player_id), 1);
    Distances d = dijkstra(&dungeon, player->player.row, player->player.col, _length_tunnel);

    return _dijkstra_descent_helper(player->player.row, player->player.col, &d);
}

#define test(test) ({ \
    printf("%s...", #test); \
    int result = test(); \
    if(result) { \
        printf("\033[31;40mFAIL\033[0m\n"); \
    } else { \
        printf("\033[32;40mOK\033[0m\n"); \
    } \
    result; \
})

int main() {
    int status = 0;
    status |= test(test_heap_build) << 0;
    status |= test(test_heap_starts_empty) << 1;
    status |= test(test_heap_not_empty) << 2;
    status |= test(test_heap_becomes_empty) << 3;
    status |= test(test_dijkstra_no_segfault) << 4;
    status |= test(test_dijkstra_always_descend_empty) << 5;
    status |= test(test_dijkstra_always_descend_no_tunnel) << 6;
    status |= test(test_dijkstra_always_descend_tunnel) << 7;

    return status;
}

