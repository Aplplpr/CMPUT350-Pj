#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"
#include <vector>

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(CMPUT350::Point2D loc,const std::vector<CMPUT350::Point2D> &path,int health = 1, int score = 100);
    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;
    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape> &GetShapes() override;
    virtual bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D &target) = 0;
    bool IsInFormation() const;
    bool IsAttacking() const;
    int GetScore() const;
    int GetHealth() const;
    void FireBullet(CMPUT350::GameContext* context, CMPUT350::Point2D target, float speed);
    void StartAttackPath(const std::vector<CMPUT350::Point2D> &path);
private:
    std::vector<CMPUT350::Point2D> mPath;
    std::vector<float> mDistances;
    float mDistance = 0.0f;
    float mPathLength = 0.0f;
    int mSample = 0;
    enum class State
    {
        Entering,
        Formation,
        Attacking,
        Returning
    };
    void StartPath(const std::vector<CMPUT350::Point2D> &path, State state);
    void MoveAlongPath();
    CMPUT350::Point2D mHome;
    CMPUT350::Point2D mLocation;
    CMPUT350::Rect mBounds;
    std::weak_ptr<CMPUT350::GameObject> mSelf;
    CMPUT350::GameContext* mContext = nullptr;
    float mRotation = 0.0f;
    bool mAlive;
    int mHealth = 1;
    int mScore = 100;
    State mState = State::Formation;
};
#endif
