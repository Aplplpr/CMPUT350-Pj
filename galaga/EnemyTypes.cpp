#include "EnemyTypes.h"
Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):Enemy(loc, path, 1, 100)
{
}
bool Enemy1::Attack(CMPUT350::GameContext* context,const CMPUT350::Point2D &target)
{
    if (!IsInFormation()){
        return false;
    }

    mTarget = target;
    mAttackFrame = 0;
    CMPUT350::Point2D start = GetLocation();
    std::vector<CMPUT350::Point2D> path = {start,start + CMPUT350::Point2D(0, 130), target + CMPUT350::Point2D(-40, -160),target + CMPUT350::Point2D(0, -90)};
    StartAttackPath(path);
    return true;
}
void Enemy1::Update(CMPUT350::GameContext* context)
{
    Enemy::Update(context);
    if (!IsAttacking()){
        return;
    }
    mAttackFrame++;
    if (mAttackFrame == 25){
        FireBullet(context, mTarget, 8.0f);
    }
}
void Enemy1::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::Point2D top(0, -20);
    CMPUT350::Point2D left(-20, 20);
    CMPUT350::Point2D right(20, 20);
    context->ScreenContext->DrawLine(top, left, 3.0f, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(left, right, 3.0f, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(right, top, 3.0f, CMPUT350::Colors::green);
    context->ScreenContext->DrawLine(CMPUT350::Point2D(0, 0), CMPUT350::Point2D(0, 20),2.0f,CMPUT350::Colors::green);
}
Enemy2::Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):Enemy(loc, path, 2, 200)
{
}
bool Enemy2::Attack(CMPUT350::GameContext* context,const CMPUT350::Point2D &target)
{
    if (!IsInFormation()){
        return false;
    }
    mTarget = target;
    mAttackFrame = 0;
    CMPUT350::Point2D start = GetLocation();
    std::vector<CMPUT350::Point2D> path = { start,start + CMPUT350::Point2D(120, 100),target + CMPUT350::Point2D(120, -180), target + CMPUT350::Point2D(50, -110)};
    StartAttackPath(path);
    return true;
}
void Enemy2::Update(CMPUT350::GameContext* context)
{
    Enemy::Update(context);
    if (!IsAttacking()){
        return;
    }
    mAttackFrame++;
    if (mAttackFrame == 20 || mAttackFrame == 45){
        FireBullet(context, mTarget, 7.0f);
    }
}
void Enemy2::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = CMPUT350::Colors::magenta;
    if (GetHealth() < 2){
        color = CMPUT350::Colors::yellow;
    }
    context->ScreenContext->DrawRect(CMPUT350::Rect(-16, -14, 32, 28), color);
    context->ScreenContext->DrawLine( CMPUT350::Point2D(-16, -14), CMPUT350::Point2D(-20, 18), 3.0f, color);
    context->ScreenContext->DrawLine( CMPUT350::Point2D(16, -14),CMPUT350::Point2D(20, 18), 3.0f, color);
    context->ScreenContext->DrawRect(CMPUT350::Rect(-6, -8, 12, 14),CMPUT350::Colors::black);
}
Enemy3::Enemy3(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):Enemy(loc, path, 2, 300)
{
}
bool Enemy3::Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D &target)
{
    if (!IsInFormation()){
        return false;
    }
    mTarget = target;
    mAttackFrame = 0;
    CMPUT350::Point2D start = GetLocation();
    CMPUT350::Point2D middle((start.x + target.x) / 2.0f, (start.y + target.y)/ 2.0f);
    std::vector<CMPUT350::Point2D> path = { start, start + CMPUT350::Point2D(120, 90), middle + CMPUT350::Point2D(120, -80), middle, 
        middle + CMPUT350::Point2D(-120, 80), target + CMPUT350::Point2D(-120, -170), target + CMPUT350::Point2D(0, -100)};
    StartAttackPath(path);
    return true;
}
void Enemy3::Update(CMPUT350::GameContext* context)
{
    Enemy::Update(context);
    if (!IsAttacking()){
        return;
    }
    mAttackFrame++;
    if (mAttackFrame == 30){
        FireBullet(context, mTarget + CMPUT350::Point2D(-80, 0), 9.0f);
        FireBullet(context, mTarget, 9.0f);
        FireBullet(context,mTarget + CMPUT350::Point2D(80, 0), 9.0f);
    }
}
void Enemy3::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::RGBColor color = CMPUT350::Colors::cyan;
    if (GetHealth() < 2){
        color = CMPUT350::Colors::red;
    }
    CMPUT350::Point2D top(0, -20);
    CMPUT350::Point2D left(-20, 14);
    CMPUT350::Point2D middleLeft(-8, 10);
    CMPUT350::Point2D bottom(0, 20);
    CMPUT350::Point2D middleRight(8, 10);
    CMPUT350::Point2D right(20, 14);
    context->ScreenContext->DrawLine(top, left, 3.0f, color);
    context->ScreenContext->DrawLine(left, middleLeft, 3.0f, color);
    context->ScreenContext->DrawLine(middleLeft, bottom, 3.0f, color);
    context->ScreenContext->DrawLine(bottom, middleRight, 3.0f, color);
    context->ScreenContext->DrawLine(middleRight, right, 3.0f, color);
    context->ScreenContext->DrawLine(right, top, 3.0f, color);
    context->ScreenContext->DrawLine(CMPUT350::Point2D(-8, 0), CMPUT350::Point2D(8, 0), 3.0f, color);
}
