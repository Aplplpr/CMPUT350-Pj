
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <unordered_map>
#include <string>

namespace CMPUT350 {
class GameEngine;
}

#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>
#include "DrawContext.h"
#include "GameContext.h"
#include "NotificationManager.h"

namespace CMPUT350 {

class DrawContext;

/**
 * @brief Main engine class that owns the game window, rendering resources,
 *        game context, and active game objects.
 *
 * The GameEngine manages the lifetime of engine resources and game objects,
 * and provides the main interface for adding objects and running the game.
 *
 * AI-generated documentation.
 */
class GameEngine : public EngineView {
public:
    GameEngine(unsigned int width, unsigned int height, const std::string& name);
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;

    void InstallKeyDownNotification(int key, const std::string& notification) override;
    void InstallKeyUpNotification(int key, const std::string& notification) override;

    const_iterator cbegin() const override;
    const_iterator cend() const override;
    const_iterator begin() const override;
    const_iterator end() const override;

    void Run();

private:
    std::shared_ptr<sf::RenderWindow> mWindow; // Shared SFML window used by the engine and drawing context
    std::shared_ptr<sf::Font> mFont; // Shared font resource
    std::shared_ptr<DrawContext> mScreenDrawContext;
    std::shared_ptr<DrawContext> mGUIDrawContext;  
    NotificationManager mNotificationManager;
    GameContext mGameContext; // Context passed to game objects during initialization, update, and rendering

    std::vector<std::shared_ptr<GameObject>> mGameObjects; // Game objects currently active in the engine
    std::vector<std::shared_ptr<GameObject>> mNewGameObjects; // Game objects waiting to be activated at the start of the next frame
    
    std::unordered_map<int, std::vector<std::string>> mKeyDownNotifications;
    std::unordered_map<int, std::vector<std::string>> mKeyUpNotifications;
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
