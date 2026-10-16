#ifndef GALAGA_H
#define GALAGA_H

#include "GameObject.h"
#include "GameContext.h"
#include <memory>
#include <vector>
#include <string>

class Player;
class Enemy;

class Galaga : public CMPUT350::GameObject
{
public:
    Galaga();
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string &key) override;

private:
    enum class State
    {
        Waiting,
        Startup,
        Waves,
        Gameplay,
        GameOver
    };
    void StartGame();
    void SpawnPlayer();
    void SpawnWave();
    void ChooseAttacker();
    void CountEnemyDeaths();
    void StartNextLevel();
    void ClearGameObjects();
    void ResetGame();
    bool AllEnemiesInFormation() const;
    CMPUT350::GameContext* mContext = nullptr;
    std::weak_ptr<CMPUT350::GameObject> mSelf;
    std::weak_ptr<Player> mPlayer;
    std::vector<std::weak_ptr<Enemy>> mEnemies;
    std::vector<bool> mScored;
    State mState = State::Waiting;
    int mCredits = 0;
    int mScore = 0;
    int mLives = 3;
    int mLevel = 1;
    int mStateFrame = 0;
    int mWave = 0;
    int mWaveFrame = 0;
    int mAttackFrame = 0;
    int mNextAttacker = 0;
    int mLivingEnemies = 0;
    int mRespawnFrame = 0;
    bool mWaitingRespawn = false;
    bool mStarsAdded = false;
};

#endif
