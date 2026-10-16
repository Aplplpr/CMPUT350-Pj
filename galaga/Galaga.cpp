#include "Galaga.h"
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "EnemyTypes.h"
#include "Stars.h"
#include "Keyboard.h"

Galaga::Galaga() : Galaga(768, 1024)
{
}
Galaga::Galaga(int width, int height)
    : mWidth(width), mHeight(height)
{
}
void Galaga::Initialize(CMPUT350::GameContext* context)
{
    mContext = context;
    mSelf = context->CurrObject;
    context->mNotificationManager->Register(mSelf, "Coin");
    context->mNotificationManager->Register(mSelf, "Start");
    context->mNotificationManager->Register(mSelf, "PlayerDied");
    context->mNotificationManager->Register(mSelf, "EnemyDied");
    context->mEngineView->InstallKeyDownNotification( CMPUT350::Key::Num5, "Coin");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num1, "Start");
    context->mEngineView->InstallKeyDownNotification( CMPUT350::Key::A, "MoveLeftDown");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, "MoveLeftUp");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, "MoveRightDown" );
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, "MoveRightUp");
    context->mEngineView->InstallKeyDownNotification( CMPUT350::Key::Left, "MoveLeftDown");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Left, "MoveLeftUp");
    context->mEngineView->InstallKeyDownNotification( CMPUT350::Key::Right, "MoveRightDown");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Right, "MoveRightUp");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, "Fire");
}

void Galaga::Update(CMPUT350::GameContext* context)
{
    if (!mStarsAdded){
        context->mEngineView->AddGameObject(std::make_shared<Stars>( 250, CMPUT350::Rect(0, 0, mWidth, mHeight)));
        mStarsAdded = true;
    }
    if (mState == State::Waiting){
        return;
    }
    mStateFrame++;
    if (mState == State::Startup){
        if (mStateFrame >= 60){
            mState = State::Waves;
            mStateFrame = 0;
            mWaveFrame = 40;
        }
    }
    else if (mState == State::Waves){
        mWaveFrame++;
        if (mWave < 5 && mWaveFrame >= 40){
            SpawnWave();
            mWaveFrame = 0;
        }
        if (mWave == 5 && AllEnemiesInFormation()){
            mState = State::Gameplay;
            mStateFrame = 0;
            mAttackFrame = 0;
        }
    }
    else if (mState == State::Gameplay){
        if (mLivingEnemies == 0){
            StartNextLevel();
            return;
        }
        if (!mWaitingRespawn){
            mAttackFrame++;
            if (mAttackFrame >= 80){
                ChooseAttacker();
                mAttackFrame = 0;
            }
        }
    }
    else if (mState == State::GameOver){
        if (mStateFrame >= 150){
            ResetGame();
        }
        return;
    }
    if (mWaitingRespawn){
        mRespawnFrame++;
        if (mRespawnFrame >= 45 && AllEnemiesInFormation()){
            SpawnPlayer();
        }
    }
}
void Galaga::ReceiveNotification(const std::string &key)
{
    if (key == "Coin"){
        if (mState == State::Waiting){
            mCredits++;
        }
    }
    else if (key == "Start"){
        if (mState == State::Waiting && mCredits > 0){
            StartGame();
        }
    }
    else if (key == "EnemyDied"){
        if (mState == State::Waves || mState == State::Gameplay){
            CountEnemyDeaths();
        }
    }
    else if (key == "PlayerDied"){
        if (mState == State::Startup ||
            mState == State::Waves ||
            mState == State::Gameplay){
            if (mWaitingRespawn){
                return;
            }
            mLives--;
            if (mLives <= 0){
                mState = State::GameOver;
                mStateFrame = 0;
                ClearGameObjects();
            }
            else{
                mWaitingRespawn = true;
                mRespawnFrame = 0;
            }
        }
    }
}
 
void Galaga::StartGame()
{
    mCredits--;
    mScore = 0;
    mLives = 3;
    mLevel = 1;
    mWave = 0;
    mWaveFrame = 0;
    mAttackFrame = 0;
    mNextAttacker = 0;
    mLivingEnemies = 0;
    mRespawnFrame = 0;
    mWaitingRespawn = false;
    mEnemies.clear();
    mScored.clear();
    mState = State::Startup;
    mStateFrame = 0;
    SpawnPlayer();
}

void Galaga::SpawnPlayer()
{
    auto currentPlayer = mPlayer.lock();
    if (currentPlayer && currentPlayer->IsAlive()){
        return;
    }
    auto player = std::make_shared<Player>(CMPUT350::Point2D(mWidth / 2.0f, mHeight - 124.0f));
    mPlayer = player;
    mContext->mEngineView->AddGameObject(player);
    mWaitingRespawn = false;
    mRespawnFrame = 0;
}

void Galaga::SpawnWave()
{
    int row = mWave;
    for (int column = 0; column < 8; column++){
        CMPUT350::Point2D home(80 + column * 85,140 + row * 65);
        CMPUT350::Point2D start;
        if (column % 2 == 0){
            start = CMPUT350::Point2D(-60, -80 - row * 30);
        }
        else{
            start = CMPUT350::Point2D(mWidth + 60, -80 - row * 30);
        }
        float side = -90.0f;
        if (column % 2 != 0){
            side = 90.0f;
        }
        std::vector<CMPUT350::Point2D> path = {
            start,
            CMPUT350::Point2D(start.x, home.y - 70),
            CMPUT350::Point2D(home.x + side, home.y + 90),
            home
        };
        std::shared_ptr<Enemy> enemy;
        if (row == 0){
            enemy = std::make_shared<Enemy3>(home, path);
        }
        else if (row == 1){
            enemy = std::make_shared<Enemy2>(home, path);
        }
        else{
            enemy = std::make_shared<Enemy1>(home, path);
        }
        mEnemies.push_back(enemy);
        mScored.push_back(false);
        mLivingEnemies++;
        mContext->mEngineView->AddGameObject(enemy);
    }
    mWave++;
}

bool Galaga::AllEnemiesInFormation() const
{
    for (const auto &item : mEnemies){
        std::shared_ptr<Enemy> enemy = item.lock();
        if (enemy && enemy->IsAlive() && !enemy->IsInFormation()){
            return false;
        }
    }
    return true;
}

void Galaga::ChooseAttacker()
{
    std::shared_ptr<Player> player = mPlayer.lock();
    if (!player || !player->IsAlive()){
        return;
    }
    std::vector<std::shared_ptr<Enemy>> available;
    for (auto it = mContext->mEngineView->cbegin();
        it != mContext->mEngineView->cend(); ++it){
        std::shared_ptr<Enemy> enemy =std::dynamic_pointer_cast<Enemy>(*it);
        if (enemy && enemy->IsAlive() && enemy->IsInFormation()){
            available.push_back(enemy);
        }
    }
    if (available.empty()){
        return;
    }
    int index = mNextAttacker % static_cast<int>(available.size());
    mNextAttacker++;
    available[index]->Attack(mContext, player->GetLocation());
}
void Galaga::CountEnemyDeaths()
{
    for (std::size_t i = 0; i < mEnemies.size(); i++){
        if (mScored[i]){
            continue;
        }
        std::shared_ptr<Enemy> enemy = mEnemies[i].lock();
        if (enemy && !enemy->IsAlive()){
            mScore += enemy->GetScore();
            mScored[i] = true;
            mLivingEnemies--;
        }
    }
}
void Galaga::StartNextLevel()
{
    mLevel++;
    mWave = 0;
    mWaveFrame = 0;
    mAttackFrame = 0;
    mNextAttacker = 0;
    mLivingEnemies = 0;
    mEnemies.clear();
    mScored.clear();
    mState = State::Startup;
    mStateFrame = 0;
    SpawnPlayer();
}

void Galaga::ClearGameObjects()
{
    for (const auto &item : mEnemies){
        std::shared_ptr<Enemy> enemy = item.lock();
        if (enemy){
            enemy->Kill();
        }
    }
    for (auto it = mContext->mEngineView->cbegin(); it != mContext->mEngineView->cend(); ++it){
        std::shared_ptr<CMPUT350::GameObject> object = *it;
        if (std::dynamic_pointer_cast<Player>(object) ||
            std::dynamic_pointer_cast<Enemy>(object) ||
            std::dynamic_pointer_cast<Bullet>(object)){
            object->Kill();
        }
    }
}

void Galaga::ResetGame()
{
    ClearGameObjects();
    mPlayer.reset();
    mEnemies.clear();
    mScored.clear();
    mCredits = 0;
    mScore = 0;
    mLives = 3;
    mLevel = 1;
    mWave = 0;
    mWaveFrame = 0;
    mAttackFrame = 0;
    mNextAttacker = 0;
    mLivingEnemies = 0;
    mRespawnFrame = 0;
    mWaitingRespawn = false;
    mState = State::Waiting;
    mStateFrame = 0;
}

void Galaga::RenderUI(CMPUT350::GameContext* context)
{
    float centerX = static_cast<float>(context->GUIContext->GetWindowWidth()) / 2.0f;
    if (mState == State::Waiting){
        context->GUIContext->DrawCenteredText("GALAGA", 52,CMPUT350::Point2D(centerX, 300), CMPUT350::Colors::cyan);
        context->GUIContext->DrawCenteredText("PRESS 5 TO INSERT COIN", 25, CMPUT350::Point2D(centerX, 430),CMPUT350::Colors::yellow );
        context->GUIContext->DrawCenteredText( "PRESS 1 TO START", 25, CMPUT350::Point2D(centerX, 480), CMPUT350::Colors::white);
        context->GUIContext->DrawCenteredText( "CREDITS: " + std::to_string(mCredits), 22, CMPUT350::Point2D(centerX, 550), CMPUT350::Colors::white );
        return;
    }
    context->GUIContext->DrawText("SCORE: " + std::to_string(mScore), 24,CMPUT350::Point2D(20, 20),CMPUT350::Colors::white);
    context->GUIContext->DrawText("LIVES: " + std::to_string(mLives), 24,CMPUT350::Point2D(20, 55),CMPUT350::Colors::white);
    context->GUIContext->DrawText( "LEVEL: " + std::to_string(mLevel), 24, CMPUT350::Point2D(570, 20), CMPUT350::Colors::white);
    if (mState == State::Startup){
        context->GUIContext->DrawCenteredText("LEVEL " + std::to_string(mLevel), 40,CMPUT350::Point2D(centerX, 480),CMPUT350::Colors::yellow );
    }
    else if (mState == State::Waves){
        context->GUIContext->DrawCenteredText("ENEMIES ARRIVING", 25, CMPUT350::Point2D(centerX, 480),CMPUT350::Colors::yellow);
    }
    else if (mState == State::GameOver){
        context->GUIContext->DrawCenteredText( "GAME OVER", 48, CMPUT350::Point2D(centerX, 470), CMPUT350::Colors::red);
    }
    else if (mWaitingRespawn){
        context->GUIContext->DrawCenteredText( "GET READY", 32,CMPUT350::Point2D(centerX, 550), CMPUT350::Colors::yellow);
    }
}
