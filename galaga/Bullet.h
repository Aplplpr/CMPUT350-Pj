#ifndef BULLET_H
#define BULLET_H

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player);
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent);
    bool IsPlayerBullet();
    //GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;
    //Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
    //Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj, const CMPUT350::Point2D &collisionPoint) override;
    const CMPUT350::Rect& GetBounds() override;
    const std::vector<CMPUT350::Shape> &GetShapes() override;
private:
    std::vector<CMPUT350::Shape> mShapes;
    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mPreviousLocation;
    CMPUT350::Point2D mHeading;
    std::weak_ptr<CMPUT350::GameObject> mParent;
    CMPUT350::Point2D mImpactPoint{0, 0};
    bool mHit = false;
    bool mHasParent = false;
    bool mPlayer;
    bool mAlive;
    CMPUT350::Rect mBounds;
};
#endif // BULLET_H
