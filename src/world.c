#include "world.h"

#define WORLD_RENDER_THREADS 4

void world_init(struct World* world, struct WorldData data) {
    world->roomCount = data.roomCount;

    world->rooms = malloc(data.roomCount * sizeof(struct Room));

    for (int i = 0; i < data.roomCount; i++) {
        room_init(&world->rooms[i], data.rooms[i]);
    }

    world->camera.fovRad = 90.0f * 3.1415926535 / 180.0f;
}



struct WorldDrawBatchData {
    struct World* world;
    struct Display* display;
    int room;
    int iteration;
};

DWORD WINAPI world_draw_columns_batch(LPVOID lpParam) {
    struct WorldDrawBatchData* batch = (struct WorldDrawBatchData*)lpParam;

    for (int x = batch->iteration; x < batch->display->width; x += WORLD_RENDER_THREADS) {
        room_draw_column(&batch->world->rooms[batch->room], batch->display, x);
    }
}

void world_draw(struct World* world, struct Display* display)
{
    for (int i = 0; i < world->roomCount; i++) {
        room_project_vertices(&world->rooms[i], &world->camera, display);
    }

    // TODO: find the room we're currently in to render it
    int currRoom = 0;


    struct WorldDrawBatchData* pDataArray[WORLD_RENDER_THREADS];
    DWORD dwThreadIdArray[WORLD_RENDER_THREADS];
    HANDLE hThreadArray[WORLD_RENDER_THREADS];

    
    for (int i = 0; i < WORLD_RENDER_THREADS; i++)
    {
        pDataArray[i] = (struct WorldDrawBatchData*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(struct WorldDrawBatchData));

        *pDataArray[i] = (struct WorldDrawBatchData){
            .world = world,
            .display = display,
            .room = currRoom,
            .iteration = i,
        };

        hThreadArray[i] = CreateThread(NULL, 0, world_draw_columns_batch, pDataArray[i], 0, &dwThreadIdArray[i]);
    }

    WaitForMultipleObjects(WORLD_RENDER_THREADS, hThreadArray, TRUE, INFINITE);
}
