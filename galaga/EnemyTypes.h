#ifndef ENEMY_TYPES_H
#define ENEMY_TYPES_H

#include "Enemy.h"

class Enemy1 : public Enemy
{
public:
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    void Update(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D &target) override;
private:
    CMPUT350::Point2D mTarget;
    int mAttackFrame = 0;
};
class Enemy2 : public Enemy
{
public:
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    void Update(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D &target) override;
private:
    CMPUT350::Point2D mTarget;
    int mAttackFrame = 0;
};
class Enemy3 : public Enemy
{
public:
    Enemy3(CMPUT350::Point2D loc,const std::vector<CMPUT350::Point2D> &path);
    void Update(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;
    bool Attack(CMPUT350::GameContext* context,const CMPUT350::Point2D &target) override;
private:
    CMPUT350::Point2D mTarget;
    int mAttackFrame = 0;
};
#endif
