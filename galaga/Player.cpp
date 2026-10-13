#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "GameContext.h"
#include <algorithm>
#include <SFML/Window/Keyboard.hpp>

Player::Player(CMPUT350::Point2D loc)
:mLocation(loc), mBounds(loc - 20.0f, 40, 40),mAlive(true)
{
    // TODO: Update code
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    mContext = context;
    mSelf = context->CurrObject;
    context->mNotificationManager->Register(mSelf, "MoveLeftDown");
    context->mNotificationManager->Register(mSelf, "MoveLeftUp");
    context->mNotificationManager->Register(mSelf, "MoveRightDown");
    context->mNotificationManager->Register(mSelf, "MoveRightUp");
    context->mNotificationManager->Register(mSelf, "Fire");
}

void Player::Update(CMPUT350::GameContext* context)
{
    const float speed = 10.0f;
    const float halfWidth = 20.0f;
    if (mMoveLeft){
        mLocation.x -= speed;
    }
    if (mMoveRight){
        mLocation.x += speed;
    }
    // Do not allow the player to leave the screen
    float screenWidth = context->ScreenContext->GetWindowWidth();
    if (mLocation.x < halfWidth){
        mLocation.x = halfWidth;
    }
    if (mLocation.x > screenWidth - halfWidth){
        mLocation.x = screenWidth - halfWidth;
    }
    mBounds = CMPUT350::Rect(mLocation- 20.0f,40,40);
}
void Player::LateUpdate(CMPUT350::GameContext* context)
{
}
void Player::ReceiveNotification(const std::string &key)
{
    if (!mAlive){
        return;
    }
    if (key == "MoveLeftDown"){
        mMoveLeft = true;
    }
    else if (key == "MoveLeftUp"){
        mMoveLeft = false;
    }
    else if (key == "MoveRightDown"){
        mMoveRight = true;
    }
    else if (key == "MoveRightUp"){
        mMoveRight = false;
    }
    else if (key == "Fire"){
        for (auto it = mBullets.begin(); it != mBullets.end();){
            std::shared_ptr<Bullet> bullet = it->lock();
            if (bullet == nullptr || !bullet->IsAlive()){
                it = mBullets.erase(it);
            }
            else{
                ++it;
            }
        }
        if (mBullets.size() >= 2){
            return;
        }
        auto bullet = std::make_shared<Bullet>( mLocation + CMPUT350::Point2D(0, -20),CMPUT350::Point2D(0, -12),true,mSelf);
        mContext->mEngineView->AddGameObject(bullet);
        mBullets.push_back(bullet);
    }
}
void Player::RenderBackground(CMPUT350::GameContext* context)
{
}
void Player::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::Point2D top(0, -20);
    CMPUT350::Point2D left(-20, 20);
    CMPUT350::Point2D right(20, 20);
    context->ScreenContext->DrawLine( top,left,3.0f,CMPUT350::Colors::cyan);
    context->ScreenContext->DrawLine(left,right,3.0f,CMPUT350::Colors::cyan);
    context->ScreenContext->DrawLine(right,top,3.0f,CMPUT350::Colors::cyan);
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet =std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet && bullet->IsPlayerBullet()){
        return;
    }
    if (bullet){
        Kill();
        return;
    }
    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);
    if (enemy){
        Kill();
    }
}
void Player::Kill()
{
    if (!mAlive){
        return;
    }
    mAlive = false;
    if (mContext != nullptr){
        mContext->mNotificationManager->Notify("PlayerDied");
    }
}
bool Player::IsAlive() const
{
    // TODO: Update code
    return mAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    // TODO: Update code
    return mBounds;
}

const std::vector<CMPUT350::Shape> &Player::GetShapes()
{
    static const std::vector<CMPUT350::Shape> shapes = {CMPUT350::Rect(-20, -20, 40, 40)};
    return shapes;
}

CMPUT350::Point2D Player::GetLocation() const
{
    return mLocation;
}

float Player::GetRotation() const
{
    return 0.0f;
}
