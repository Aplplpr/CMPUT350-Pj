#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "Bezier.h"
#include <cmath>

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    mContext = context;
    mSelf = context->CurrObject;
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    if (!mAlive){
        return;
    }
    if (mState == State::Formation){
        float frame = static_cast<float>(context->currentFrame);
        mLocation.x = mHome.x + std::sin(frame * 0.03f) * 10.0f;
        mLocation.y = mHome.y + std::sin(frame * 0.02f) * 4.0f;
        mRotation = 0.0f;
    }
    else{
        MoveAlongPath();
        if (mDistance >= mPathLength){
            if (mState == State::Attacking){
                CMPUT350::Point2D start = mLocation;
                float middleY = (start.y + mHome.y) / 2.0f;
                std::vector<CMPUT350::Point2D> path = { start, CMPUT350::Point2D(start.x, middleY), CMPUT350::Point2D(mHome.x, middleY), mHome };
                StartPath(path, State::Returning);
            }
            else if (mState == State::Entering || mState == State::Returning){
                mState = State::Formation;
                mLocation = mHome;
                mRotation = 0.0f;
            }
        }
    }

    float halfSize = 20.0f *
        (std::fabs(std::cos(mRotation)) +
         std::fabs(std::sin(mRotation)));

    mBounds = CMPUT350::Rect(
        mLocation - halfSize, halfSize * 2.0f, halfSize * 2.0f
    );
}
void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}


void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(
        CMPUT350::Rect(-20, -20, 40, 40),
        CMPUT350::Colors::red
    );
}
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (!mAlive){
        return;
    }
    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);
    if (player){
        Kill();
        return;
    }
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet && bullet->IsPlayerBullet()){
        mHealth--;
        if (mHealth <= 0){
            Kill();
        }
    }
}
void Enemy::Kill()
{
    if (!mAlive){
        return;
    }
    mAlive = false;
    if (mContext != nullptr){
        mContext->mNotificationManager->Notify("EnemyDied");
    }
}
bool Enemy::IsAlive() const
{
    // TODO: Update code
    return mAlive;;
}
const CMPUT350::Rect& Enemy::GetBounds()
{
    // TODO: Update code
    return mBounds;
}

const std::vector<CMPUT350::Shape> &Enemy::GetShapes()
{
        // TODO: Update code
    static const std::vector<CMPUT350::Shape> shapes = {CMPUT350::Rect(-20, -20, 40, 40)};
    return shapes;
}

CMPUT350::Point2D Enemy::GetLocation() const
{
    return mLocation;
}

float Enemy::GetRotation() const
{
    return mRotation;
}


Enemy::Enemy(CMPUT350::Point2D loc,const std::vector<CMPUT350::Point2D> &path,int health, int score):mHome(loc), mLocation(loc), mBounds(loc - 20.0f, 40, 40),
mAlive(true),
mHealth(health), mScore(score), mState(State::Entering)
{
    StartPath(path, State::Entering);
}

bool Enemy::IsInFormation() const
{
    return mState == State::Formation;
}

bool Enemy::IsAttacking() const
{
    return mState == State::Attacking;
}

int Enemy::GetScore() const
{
    if (mState == State::Attacking){
        return mScore * 2;
    }
    return mScore;
}

void Enemy::StartAttackPath(const std::vector<CMPUT350::Point2D> &path)
{
    StartPath(path, State::Attacking);
}

int Enemy::GetHealth() const
{
    return mHealth;
}

void Enemy::FireBullet(CMPUT350::GameContext* context,
                       CMPUT350::Point2D target, float speed)
{
    CMPUT350::Point2D heading = target - mLocation;
    heading.Normalize();
    heading = heading * speed;

    auto bullet = std::make_shared<Bullet>(
        mLocation, heading, false, mSelf
    );

    context->mEngineView->AddGameObject(bullet);
}
void Enemy::StartPath(const std::vector<CMPUT350::Point2D> &path, State state)
{
    mPath = path;
    mState = state;
    mDistance = 0.0f;
    mPathLength = 0.0f;
    mSample = 0;
    mDistances.clear();
    mDistances.push_back(0.0f);
    int segmentCount = static_cast<int>((mPath.size() - 1) / 3);
    int samples = segmentCount * 40;
    CMPUT350::Point2D previous = CMPUT350::Bezier::GetPoint(mPath, 0.0f);
    for (int i = 1; i <= samples; i++){
        float t = static_cast<float>(i) / 40.0f;
        CMPUT350::Point2D current = CMPUT350::Bezier::GetPoint(mPath, t);
        mPathLength += static_cast<float>(previous.Distance(current));
        mDistances.push_back(mPathLength);
        previous = current;
    }
    mLocation = CMPUT350::Bezier::GetPoint(mPath, 0.0f);
}
void Enemy::MoveAlongPath()
{
    const float speed = 6.0f;
    mDistance += speed;
    if (mDistance >= mPathLength){
        float end = static_cast<float>((mPath.size() - 1) / 3);
        mLocation = CMPUT350::Bezier::GetPoint(mPath, end);
        return;
    }
    while (mSample + 1 < static_cast<int>(mDistances.size()) - 1 &&
           mDistance > mDistances[mSample + 1]){
        mSample++;
    }
    float length = mDistances[mSample + 1] - mDistances[mSample];
    float ratio = 0.0f;
    if (length > 0.0f){
        ratio = (mDistance - mDistances[mSample]) / length;
    }
    float t = (static_cast<float>(mSample) + ratio) / 40.0f;
    mLocation = CMPUT350::Bezier::GetPoint(mPath, t);
    CMPUT350::Point2D slope = CMPUT350::Bezier::GetSlope(mPath, t);
    if (slope.x != 0.0f || slope.y != 0.0f){
        mRotation = std::atan2(slope.y, slope.x) + 1.5707963f;
    }
}
