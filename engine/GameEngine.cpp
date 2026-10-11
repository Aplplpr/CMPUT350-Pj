#include "GameEngine.h"
#include <cstdio>
#include "GraphicsObject.h"
#include "CollisionObject.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

/**
 * @brief Creates and initializes the game engine.
 *
 * @param width The width of the game window in pixels.
 * @param height The height of the game window in pixels.
 * @param name The title displayed on the game window.
 *
 * This constructor creates the SFML window, limits the frame rate,
 * loads the embedded font, creates the drawing context, and connects
 * the game context to the engine and drawing system.
 *
 * AI-generated documentation.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mWindow = std::make_shared<sf::RenderWindow>(
        sf::VideoMode({width, height}),
        name
    );

    mWindow->setFramerateLimit(30);

    mFont = std::make_shared<sf::Font>();

    if (!mFont->openFromMemory(&_font, _font_len)) {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    mDrawContext = std::make_shared<DrawContext>(mWindow, mFont);

    mGameContext.mEngineView = this;
    mGameContext.ScreenContext = mDrawContext.get();
}

/**
 * @brief Destroys the game engine and closes the render window.
 *
 * AI-generated documentation.
 */
GameEngine::~GameEngine() {
    // Cleanup resources
    if (mWindow != nullptr){
        mWindow->close();
    }
}

/**
 * @brief Queues a game object to be added to the engine.
 *
 * @param gameObject Shared pointer to the game object being added.
 *
 * The object is stored in the pending-object collection and is not
 * activated until the beginning of the next frame.
 *
 * AI-generated documentation.
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewGameObjects.push_back(gameObject);
}

/**
 * @brief Runs the main game loop until the window is closed.
 *
 * Each frame removes inactive objects, activates newly added objects,
 * processes input events, updates game objects, checks collisions,
 * performs late updates, renders the scene, and displays the frame.
 *
 * AI-generated documentation.
 */
void GameEngine::Run() {
    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        // Compact the active object vector in place by copying
        // only living objects toward the front, then resize once. This avoids repeated
        // vector erase operations and prevents O(n^2) shifting.
        // This method is suggested by ai, as it could improve the the efficiency of removing dead objects
        size_t writeIndex = 0;
        for (size_t readIndex = 0; readIndex < mGameObjects.size(); ++readIndex) {
            if (mGameObjects[readIndex]->IsAlive()) {
                mGameObjects[writeIndex] = mGameObjects[readIndex];
                ++writeIndex;
            }
        }

        mGameObjects.resize(writeIndex);

        // 1. Activate and initialize any objects added during the last frame
        for (auto &gameObject : mNewGameObjects) {
            mGameObjects.push_back(gameObject);
            gameObject->Initialize(&mGameContext);
        }

        mNewGameObjects.clear();

        // 2. Process events
        while (const std::optional event = mWindow->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }
        }

        // 3. Update game objects
        for (auto &gameObject : mGameObjects) {
            gameObject->Update(&mGameContext);
        }

        // 4. Process collision events
        for (size_t a = 0; a < mGameObjects.size(); ++a) {
            auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[a]);

            if (objA == nullptr) {
                continue;
            }

            for (size_t b = a + 1; b < mGameObjects.size(); ++b) { //avoids checking an object against itself
                auto objB =
                    std::dynamic_pointer_cast<CollisionObject>(mGameObjects[b]);

                if (objB == nullptr) {
                    continue;
                }

                if (objA == objB) {
                    continue;
                }

                const Rect &boundsA = objA->GetBounds();
                const Rect &boundsB = objB->GetBounds();

                float aLeft = boundsA.topLeft.x;
                float aRight = boundsA.topLeft.x + boundsA.width;
                float aTop = boundsA.topLeft.y;
                float aBottom = boundsA.topLeft.y + boundsA.height;

                float bLeft = boundsB.topLeft.x;
                float bRight = boundsB.topLeft.x + boundsB.width;
                float bTop = boundsB.topLeft.y;
                float bBottom = boundsB.topLeft.y + boundsB.height;

                bool overlap =
                    !(aRight < bLeft ||
                    bRight < aLeft ||
                    aBottom < bTop ||
                    bBottom < aTop);

                if (overlap) {
                    objA->CollisionEnter(objB);
                    objB->CollisionEnter(objA);
                }
            }
        }

        // 5. Late updates
        for (auto &gameObject : mGameObjects) {
            gameObject->LateUpdate(&mGameContext);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto &gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject != nullptr) {
                graphicsObject->RenderBackground(&mGameContext);
            }
        }

        // 7. Render foreground
        for (auto &gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject != nullptr) {
                graphicsObject->RenderForeground(&mGameContext);
            }
        }

        // Actually render to window
        mWindow->display();
    }
}

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
//{
//	while (const std::optional event = mWindow->pollEvent())
//	{
//		if (event->is<sf::Event::Closed>())
//		{
//		}
//		else if (event->is<sf::Event::Resized>())
//		{
//		}
//		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
//		{
//			// use keyPressed->unicode to get character
//		}
//	}
// }

}  // namespace CMPUT350
