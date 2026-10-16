#include "Bezier.h"
#include <cassert>

namespace CMPUT350 {

Bezier::Bezier(const std::vector<Point2D> &pts):mPoints(pts){
    assert(mPoints.size() >= 4 && (mPoints.size() - 1) % 3 == 0);
}

Point2D Bezier::GetPoint(float t) const{
    return GetPoint(mPoints, t);
}

Point2D Bezier::GetSlope(float t) const{
    return GetSlope(mPoints, t);
}

Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t){
    assert(pts.size() >= 4 && (pts.size() - 1) % 3 == 0);
    int segmentCount = static_cast<int>((pts.size() - 1) / 3);
    assert(t >= 0.0f && t <= static_cast<float>(segmentCount));
    int segment = static_cast<int>(t);
    if (segment == segmentCount)
    {
        segment--;
    }
    int index = segment * 3;
    float localT = t - static_cast<float>(segment);
    float oneMinusT = 1.0f - localT;
    return pts[index] * (oneMinusT * oneMinusT * oneMinusT)+ pts[index + 1] * (3.0f * oneMinusT * oneMinusT * localT) + pts[index + 2] * (3.0f * oneMinusT * localT * localT) + pts[index + 3] * (localT * localT * localT);
}

Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t){
    assert(pts.size() >= 4 && (pts.size() - 1) % 3 == 0);

    int segmentCount = static_cast<int>((pts.size() - 1) / 3);
    assert(t >= 0.0f && t <= static_cast<float>(segmentCount));
    int segment = static_cast<int>(t);
    if (segment == segmentCount){
        segment--;
    }
    int index = segment * 3;
    float localT = t - static_cast<float>(segment);
    float oneMinusT = 1.0f - localT;
    return (pts[index + 1] - pts[index]) * (3.0f * oneMinusT * oneMinusT) + (pts[index + 2] - pts[index + 1]) * (6.0f * oneMinusT * localT) + (pts[index + 3] - pts[index + 2]) * (3.0f * localT * localT);
}

}