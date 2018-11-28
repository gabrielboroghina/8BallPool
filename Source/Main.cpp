#include <ctime>

#include <Core/Engine.h>
#include "8BallPool/BallPool.h"

using namespace std;

int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));

    // Create a window property structure
    WindowProperties wp;
    wp.resolution = glm::ivec2(1280, 720);
	wp.name = "8 Ball Pool";

    // Init the Engine and create a new window with the defined properties
    WindowObject *window = Engine::Init(wp);

    // Create a new 3D world and start running it
    World *world = new BallPool();
    world->Init();
    world->Run();

    // Signals to the Engine to release the OpenGL context
    Engine::Exit();

    return 0;
}
