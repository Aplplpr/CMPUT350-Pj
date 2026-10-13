#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionObject.h"
#include <memory>
#include <vector>
#include <string>

class Bullet;
class Player : public CMPUT350::CollisionObject
{
public:
    Player(CMPUT350::Point2D loc);
    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string &key) override;
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
private:
    CMPUT350::Point2D mLocation;
    CMPUT350::Rect mBounds;
    std::weak_ptr<CMPUT350::GameObject> mSelf;
    CMPUT350::GameContext* mContext = nullptr;
    bool mAlive;
    bool mMoveLeft = false;
    bool mMoveRight = false;
    std::vector<std::weak_ptr<Bullet>> mBullets;
};

#endif
