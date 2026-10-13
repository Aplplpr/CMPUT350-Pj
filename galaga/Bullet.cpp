#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player):
mLocation(location), mPreviousLocation(location),
mHeading(heading),mPlayer(player),
mAlive(true), mBounds(location, location)
{
}

bool Bullet::IsPlayerBullet()
{
    // TODO: Update
    return mPlayer;
}

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
               std::weak_ptr<CMPUT350::GameObject> parent):
Bullet(location, heading, player)
{
    mParent = parent;
    mHasParent = true;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

void Bullet::Update(CMPUT350::GameContext* context)
{
    if (!IsAlive()){
        Kill();
        return;
    }

    mPreviousLocation = mLocation;
    mLocation += mHeading;
    mBounds = CMPUT350::Rect(mPreviousLocation, mLocation);
    //if bullet is out of screen, kill it
    if (mLocation.x < 0 || mLocation.x > context->ScreenContext->GetWindowWidth() || mLocation.y < 0 || mLocation.y > context->ScreenContext->GetWindowHeight()){
        Kill();
    }
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}


void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    if (mHasParent || mHit){
        CMPUT350::RGBColor color = CMPUT350::Colors::yellow;
        if (mPlayer){
            color = CMPUT350::Colors::cyan;
        }
        if (mHit){
            context->ScreenContext->DrawCircle(mImpactPoint, 4.0f, color);
            return;
        }

        if (!mAlive){
            return;
        }
        CMPUT350::Point2D start = mPreviousLocation - mLocation;
        CMPUT350::Point2D end(0, 0);
        context->ScreenContext->DrawLine(start, end, 2.0f, color);
        return;
    }
    context->ScreenContext->DrawLine( mPreviousLocation, mLocation, 2.0f, CMPUT350::Colors::white );
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (mHasParent)
    {
        CollisionEnter(obj, CMPUT350::Point2D(0, 0));
        return;
    }

    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);
    if (mPlayer && player){
        return;
    }
    Kill();
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                            const CMPUT350::Point2D &collisionPoint)
{
    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);
    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (mPlayer && player){
        return;
    }
    if (!mPlayer && enemy){
        return;
    }
    if (!player && !enemy){
        return;
    }
    if (!mHit || collisionPoint.y > mImpactPoint.y){
        mImpactPoint = collisionPoint;
    }
    mHit = true;
    Kill();
}

void Bullet::Kill()
{
    mAlive = false;
}

bool Bullet::IsAlive() const
{
    if (mHasParent){
        std::shared_ptr<CMPUT350::GameObject> parent = mParent.lock();
        if (parent == nullptr || !parent->IsAlive()){
            return false;
        }
    }
    // TODO: Update code
    return mAlive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    // TODO: Update code
    return mBounds;
}

const std::vector<CMPUT350::Shape> &Bullet::GetShapes()
{
    mShapes.clear();
    CMPUT350::Point2D start = mPreviousLocation- mLocation;
    CMPUT350::Point2D end(0, 0);
    mShapes.emplace_back(CMPUT350::Line(start, end));
    return mShapes;
}

CMPUT350::Point2D Bullet::GetLocation() const
{
    return mLocation;
}

float Bullet::GetRotation() const
{
    return 0.0f;
}
