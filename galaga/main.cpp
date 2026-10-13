#include <memory>
#include "GameEngine.h"
#include "Galaga.h"

int main()
{
    CMPUT350::GameEngine engine(768, 1024, "Galaga");
    engine.AddGameObject(std::make_shared<Galaga>());
    engine.Run();
    return 0;
}
