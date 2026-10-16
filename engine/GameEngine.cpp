#include "GameEngine.h"
#include <cstdio>
#include "GraphicsObject.h"
#include "CollisionObject.h"
#include <algorithm>
#include <cmath>

/// @brief
namespace CMPUT350 {
#include "FontData.h"

/**
 * @brief Creates and initializes the game engine.
 *
 * @param width The width of the game window in pixels.
 * @param height The height of the game window in pixels.
 * @param name The title displayed on the game window.
 *
 * This constructor creates the SFML window, limits the frame rate,
 * loads the embedded font, creates the drawing context, and connects
 * the game context to the engine and drawing system.
 *
 * AI-generated documentation.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mWindow = std::make_shared<sf::RenderWindow>(
        sf::VideoMode({width, height}),
        name
    );

    mWindow->setFramerateLimit(30);

    mFont = std::make_shared<sf::Font>();

    if (!mFont->openFromMemory(&_font, _font_len)) {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    mScreenDrawContext = std::make_shared<DrawContext>(mWindow, mFont);
    mGUIDrawContext = std::make_shared<DrawContext>(mWindow, mFont);

    mGameContext.mEngineView = this;
    mGameContext.ScreenContext = mScreenDrawContext.get();
    mGameContext.GUIContext = mGUIDrawContext.get();

    mGameContext.mNotificationManager = &mNotificationManager;

    mGameContext.currentFrame = 0;
}

/**
 * @brief Destroys the game engine and closes the render window.
 *
 * AI-generated documentation.
 */
GameEngine::~GameEngine() {
    // Cleanup resources
    if (mWindow != nullptr){
        mWindow->close();
    }
}

/**
 * @brief Queues a game object to be added to the engine.
 *
 * @param gameObject Shared pointer to the game object being added.
 *
 * The object is stored in the pending-object collection and is not
 * activated until the beginning of the next frame.
 *
 * AI-generated documentation.
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewGameObjects.push_back(gameObject);
}

/**
 * @brief Associates a key press with a notification.
 *
 * @param key The keyboard scancode to register.
 * @param notification The notification sent when the key is pressed.
 *
 * Multiple notifications may be registered for the same key.
 *
 * AI-generated documentation.
 */
void GameEngine::InstallKeyDownNotification(int key, const std::string& notification)
{
    mKeyDownNotifications[key].push_back(notification);
}

/**
 * @brief Associates a key release with a notification.
 *
 * @param key The keyboard scancode to register.
 * @param notification The notification sent when the key is released.
 *
 * Multiple notifications may be registered for the same key.
 *
 * AI-generated documentation.
 */
void GameEngine::InstallKeyUpNotification(int key, const std::string& notification)
{
    mKeyUpNotifications[key].push_back(notification);
}

/**
 * @brief Returns a constant iterator to the beginning of the active objects.
 *
 * @return A constant iterator to the first active GameObject.
 *
 * This function returns mGameObjects.cbegin() because EngineView exposes
 * only const_iterator, allowing callers to iterate over objects without
 * modifying the engine's internal container through the iterator.
 * AI-generated documentation.
 */
GameEngine::const_iterator GameEngine::begin() const {
    return mGameObjects.cbegin();
}

/**
 * @brief Returns a constant iterator past the end of the active objects.
 *
 * @return A constant iterator past the final active GameObject.
 * 
 * AI-generated documentation.
 */
GameEngine::const_iterator GameEngine::end() const {
    return mGameObjects.cend();
}

GameEngine::const_iterator GameEngine::cbegin() const {
    return mGameObjects.cbegin();
}

GameEngine::const_iterator GameEngine::cend() const {
    return mGameObjects.cend();
}

/**
 * @brief Transforms a line from object-local coordinates to world coordinates.
 *
 * @param line The line defined in the object's local coordinate system.
 * @param object The object whose position and rotation are applied.
 * @return The transformed line in world coordinates.
 *
 * AI-generated documentation.
 */
Line GameEngine::TransformLine(const Line& line, const std::shared_ptr<CollisionObject>& object){
    mScreenDrawContext->SetContextOffset(object->GetLocation());
    mScreenDrawContext->SetContextRotation(object->GetRotation());

    Point2D p1 = mScreenDrawContext->Transform(line.p1);
    Point2D p2 = mScreenDrawContext->Transform(line.p2);

    return Line(p1, p2);
}

/**
 * @brief Transforms a circle from object-local coordinates to world coordinates.
 *
 * @param circle The circle defined in the object's local coordinate system.
 * @param object The object whose position and rotation are applied.
 * @return The transformed circle in world coordinates.
 *
 * The center is transformed while the radius remains unchanged.
 *
 * AI-generated documentation.
 */
Circle GameEngine::TransformCircle(const Circle& circle, const std::shared_ptr<CollisionObject>& object){
    mScreenDrawContext->SetContextOffset(object->GetLocation());
    mScreenDrawContext->SetContextRotation(object->GetRotation());

    Point2D center = mScreenDrawContext->Transform(circle.center);

    return Circle(center, circle.radius);
}

/**
 * @brief Transforms a local collision rectangle into a world-space bounding box.
 *
 * @param rect The rectangle in the object's local coordinate system.
 * @param object The object whose position and rotation are applied.
 * @return The axis-aligned bounding box of the transformed rectangle.
 *
 * AI-generated documentation.
 */
Rect GameEngine::TransformCollisionRect(const Rect& rect, const std::shared_ptr<CollisionObject>& object){
    mScreenDrawContext->SetContextOffset(object->GetLocation());
    mScreenDrawContext->SetContextRotation(object->GetRotation());

    return mScreenDrawContext->Transform(rect);
}

/**
 * @brief Tests whether two circles overlap.
 *
 * @param a The first circle.
 * @param b The second circle.
 * @return True if the circles overlap or touch, otherwise false.
 *
 * AI-generated documentation.
 */
bool GameEngine::CircleCircleCollision(const Circle& a, const Circle& b){
    float distance = a.center.Distance(b.center);

    return distance <= a.radius + b.radius;
}

/**
 * @brief Tests two line segments for intersection and returns the collision point.
 *
 * @param a The directed line being tested.
 * @param b The other line segment.
 * @param collisionPoint Receives the first intersection point along line a.
 * @return True if the line segments intersect, otherwise false.
 *
 * AI-generated documentation.
 */
bool GameEngine::LineLineCollision(const Line& a, const Line& b, Point2D& collisionPoint){
    return a.Crosses(b, collisionPoint);
}

/**
 * @brief Tests a directed line segment against a circle.
 *
 * @param line The directed line from p1 to p2.
 * @param circle The circle being tested.
 * @param collisionPoint Receives the first collision point along the line.
 * @return True if the line intersects or begins inside the circle, otherwise false.
 *
 * If p1 is already inside the circle, p1 is returned as the first collision
 * point. Otherwise, the line-circle intersection equation is solved and the
 * valid intersection closest to p1 is selected.
 *
 * AI-generated documentation.
 */
bool GameEngine::LineCircleCollision(const Line& line, const Circle& circle, Point2D& collisionPoint){
    // if p1 is in circle
    if (line.p1.Distance(circle.center) <= circle.radius){
        collisionPoint = line.p1;
        return true;
    }
    Point2D d = line.p2 - line.p1;
    Point2D f = line.p1 - circle.center;

    float a = Point2D::Dot(d, d);
    float b = 2.0f * Point2D::Dot(f, d);
    float c = Point2D::Dot(f, f) - circle.radius * circle.radius;

    if (a == 0.0f) return false;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) return false;

    float root = std::sqrt(discriminant);
    float t1 = (-b - root) / (2.0f * a);
    float t2 = (-b + root) / (2.0f * a);

    float t = 2.0f;

    if (t1 >= 0.0f && t1 <= 1.0f) t = t1;
    if (t2 >= 0.0f && t2 <= 1.0f && t2 < t) t = t2;

    if (t > 1.0f) return false;

    collisionPoint = line.p1 + d * t;
    return true;
}

/**
 * @brief Tests a circle against an axis-aligned rectangle.
 *
 * @param circle The circle being tested.
 * @param rect The axis-aligned rectangle being tested.
 * @return True if the circle overlaps or touches the rectangle, otherwise false.
 *
 * The closest point on the rectangle to the circle center is used to determine
 * whether the distance is within the circle radius.
 *
 * AI-generated documentation.
 */
bool GameEngine::CircleRectCollision(const Circle& circle, const Rect& rect){
    float closestX = std::max(rect.topLeft.x, std::min(circle.center.x, rect.topLeft.x + rect.width));
    float closestY = std::max(rect.topLeft.y, std::min(circle.center.y, rect.topLeft.y + rect.height));

    Point2D closestPoint(closestX, closestY);
    float distance = circle.center.Distance(closestPoint);

    return distance <= circle.radius;
}

/**
 * @brief Tests a directed line segment against an axis-aligned rectangle.
 *
 * @param line The directed line from p1 to p2.
 * @param rect The rectangle being tested.
 * @param collisionPoint Receives the first collision point along the line.
 * @return True if the line intersects or begins inside the rectangle, otherwise false.
 *
 * If p1 is already inside the rectangle, p1 is returned. Otherwise, all four
 * rectangle edges are tested and the intersection closest to p1 is selected.
 *
 * AI-generated documentation.
 */
bool GameEngine::LineRectCollision(const Line& line, const Rect& rect, Point2D& collisionPoint){
    Point2D topLeft = rect.topLeft;
    Point2D topRight(rect.topLeft.x + rect.width, rect.topLeft.y);
    Point2D bottomLeft(rect.topLeft.x, rect.topLeft.y + rect.height);
    Point2D bottomRight(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height);

    Line edges[4] = {
        Line(topLeft, topRight),
        Line(topRight, bottomRight),
        Line(bottomRight, bottomLeft),
        Line(bottomLeft, topLeft)
    };

    if (PointInRect(line.p1, rect)){
        collisionPoint = line.p1;
        return true;
    }

    bool found = false;
    float bestDistance = line.Length();

    for (int i = 0; i < 4; ++i){
        Point2D point;

        if (line.Crosses(edges[i], point)){
            float distance = line.p1.Distance(point);

            if (!found || distance < bestDistance){
                bestDistance = distance;
                collisionPoint = point;
                found = true;
            }
        }
    }

    return found;
}

/**
 * @brief Tests whether a point lies inside an axis-aligned rectangle.
 *
 * @param point The point to test.
 * @param rect The rectangle being tested.
 * @return True if the point lies inside or on the boundary of the rectangle.
 *
 * This case is suggested by ai.
 * AI-generated documentation.
 */
bool GameEngine::PointInRect(const Point2D& point, const Rect& rect){
    return point.x >= rect.topLeft.x &&
           point.x <= rect.topLeft.x + rect.width &&
           point.y >= rect.topLeft.y &&
           point.y <= rect.topLeft.y + rect.height;
}

/**
 * @brief Tests whether a world-space point lies inside an object's local rectangle.
 *
 * @param point The point in world coordinates.
 * @param rect The rectangle in the object's local coordinate system.
 * @param object The object whose transform defines the rectangle's world position.
 * @return True if the transformed point lies inside the rectangle.
 *
 * The world-space point is converted into the object's local coordinate system
 * before performing the rectangle containment test.
 *
 * AI-generated documentation.
 */
bool GameEngine::PointInObjectRect(const Point2D& point, const Rect& rect, const std::shared_ptr<CollisionObject>& object){
    mScreenDrawContext->SetContextOffset(object->GetLocation());
    mScreenDrawContext->SetContextRotation(object->GetRotation());

    Point2D localPoint = mScreenDrawContext->ReverseTransform(point);

    return PointInRect(localPoint, rect);
}

/**
 * @brief Returns the four world-space corners of an object's rectangle.
 *
 * @param rect The rectangle in the object's local coordinate system.
 * @param object The object whose position and rotation are applied.
 * @return A vector containing the transformed corners in perimeter order.
 *
 * AI-generated documentation.
 */
std::vector<Point2D> GameEngine::GetRectCorners(const Rect& rect, const std::shared_ptr<CollisionObject>& object){
    mScreenDrawContext->SetContextOffset(object->GetLocation());
    mScreenDrawContext->SetContextRotation(object->GetRotation());

    Point2D topLeft = mScreenDrawContext->Transform(rect.topLeft);
    Point2D topRight = mScreenDrawContext->Transform(Point2D(rect.topLeft.x + rect.width, rect.topLeft.y));
    Point2D bottomRight = mScreenDrawContext->Transform(Point2D(rect.topLeft.x + rect.width, rect.topLeft.y + rect.height));
    Point2D bottomLeft = mScreenDrawContext->Transform(Point2D(rect.topLeft.x, rect.topLeft.y + rect.height));

    return {topLeft, topRight, bottomRight, bottomLeft};
}

/**
 * @brief Tests two potentially rotated object rectangles for collision.
 *
 * @param rectA The local rectangle belonging to the first object.
 * @param objA The first collision object.
 * @param rectB The local rectangle belonging to the second object.
 * @param objB The second collision object.
 * @return True if the transformed rectangles overlap, otherwise false.
 *
 * The transformed rectangle edges are tested for intersection. Containment is
 * also checked so collisions are detected when one rectangle lies completely
 * inside the other without crossing an edge.
 *
 * AI-generated documentation.
 */
bool GameEngine::RectRectCollision(const Rect& rectA, const std::shared_ptr<CollisionObject>& objA,
                                   const Rect& rectB, const std::shared_ptr<CollisionObject>& objB){
    std::vector<Point2D> a = GetRectCorners(rectA, objA);
    std::vector<Point2D> b = GetRectCorners(rectB, objB);

    Line aEdges[4] = {
        Line(a[0], a[1]),
        Line(a[1], a[2]),
        Line(a[2], a[3]),
        Line(a[3], a[0])
    };

    Line bEdges[4] = {
        Line(b[0], b[1]),
        Line(b[1], b[2]),
        Line(b[2], b[3]),
        Line(b[3], b[0])
    };

    Point2D collisionPoint;

    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            if (aEdges[i].Crosses(bEdges[j], collisionPoint)) return true;
        }
    }
    if (PointInObjectRect(a[0], rectB, objB)) return true;
    if (PointInObjectRect(b[0], rectA, objA)) return true;

    return false;
}

/**
 * @brief Converts a world-space point into an object's local coordinate system.
 *
 * @param point The point in world coordinates.
 * @param object The object whose transform should be reversed.
 * @return The corresponding point in the object's local coordinates.
 *
 * AI-generated documentation.
 */
Point2D GameEngine::WorldToLocal(const Point2D& point, const std::shared_ptr<CollisionObject>& object){
    mScreenDrawContext->SetContextOffset(object->GetLocation());
    mScreenDrawContext->SetContextRotation(object->GetRotation());

    return mScreenDrawContext->ReverseTransform(point);
}

/**
 * @brief Tests two collision shapes using the appropriate shape-pair algorithm.
 *
 * @param shapeA The collision shape belonging to the first object.
 * @param objA The object that owns shapeA.
 * @param shapeB The collision shape belonging to the second object.
 * @param objB The object that owns shapeB.
 * @param hasCollisionPoint Set to true when a precise collision point is available.
 * @param collisionPoint Receives the collision point in world coordinates when available.
 * @return True if the two shapes collide, otherwise false.
 *
 * Local collision shapes are transformed as needed before testing. Line
 * collisions provide the first collision point along the directed line, while
 * non-line collisions only report whether an overlap occurred.
 *
 * AI-generated documentation.
 */
bool GameEngine::ShapesCollide(const Shape& shapeA, const std::shared_ptr<CollisionObject>& objA,
                               const Shape& shapeB, const std::shared_ptr<CollisionObject>& objB,
                               bool& hasCollisionPoint, Point2D& collisionPoint){
    hasCollisionPoint = false;

    if (shapeA.t == ShapeType::kCircle && shapeB.t == ShapeType::kCircle){
        Circle a = TransformCircle(shapeA.shape.circle, objA);
        Circle b = TransformCircle(shapeB.shape.circle, objB);
        return CircleCircleCollision(a, b);
    }

    if (shapeA.t == ShapeType::kLine && shapeB.t == ShapeType::kLine){
        Line a = TransformLine(shapeA.shape.line, objA);
        Line b = TransformLine(shapeB.shape.line, objB);

        if (LineLineCollision(a, b, collisionPoint)){
            hasCollisionPoint = true;
            return true;
        }

        return false;
    }

    if (shapeA.t == ShapeType::kLine && shapeB.t == ShapeType::kCircle){
        Line line = TransformLine(shapeA.shape.line, objA);
        Circle circle = TransformCircle(shapeB.shape.circle, objB);

        if (LineCircleCollision(line, circle, collisionPoint)){
            hasCollisionPoint = true;
            return true;
        }

        return false;
    }

    if (shapeA.t == ShapeType::kCircle && shapeB.t == ShapeType::kLine){
        Line line = TransformLine(shapeB.shape.line, objB);
        Circle circle = TransformCircle(shapeA.shape.circle, objA);

        if (LineCircleCollision(line, circle, collisionPoint)){
            hasCollisionPoint = true;
            return true;
        }

        return false;
    }

    if (shapeA.t == ShapeType::kCircle && shapeB.t == ShapeType::kRect){
        Circle circle = TransformCircle(shapeA.shape.circle, objA);
        circle.center = WorldToLocal(circle.center, objB);

        return CircleRectCollision(circle, shapeB.shape.rect);
    }

    if (shapeA.t == ShapeType::kRect && shapeB.t == ShapeType::kCircle){
        Circle circle = TransformCircle(shapeB.shape.circle, objB);
        circle.center = WorldToLocal(circle.center, objA);

        return CircleRectCollision(circle, shapeA.shape.rect);
    }

    if (shapeA.t == ShapeType::kLine && shapeB.t == ShapeType::kRect){
        Line worldLine = TransformLine(shapeA.shape.line, objA);

        Line localLine(
            WorldToLocal(worldLine.p1, objB),
            WorldToLocal(worldLine.p2, objB)
        );

        Point2D localPoint;

        if (LineRectCollision(localLine, shapeB.shape.rect, localPoint)){
            mScreenDrawContext->SetContextOffset(objB->GetLocation());
            mScreenDrawContext->SetContextRotation(objB->GetRotation());

            collisionPoint = mScreenDrawContext->Transform(localPoint);
            hasCollisionPoint = true;
            return true;
        }

        return false;
    }

    if (shapeA.t == ShapeType::kRect && shapeB.t == ShapeType::kLine){
        Line worldLine = TransformLine(shapeB.shape.line, objB);

        Line localLine(
            WorldToLocal(worldLine.p1, objA),
            WorldToLocal(worldLine.p2, objA)
        );

        Point2D localPoint;

        if (LineRectCollision(localLine, shapeA.shape.rect, localPoint)){
            mScreenDrawContext->SetContextOffset(objA->GetLocation());
            mScreenDrawContext->SetContextRotation(objA->GetRotation());

            collisionPoint = mScreenDrawContext->Transform(localPoint);
            hasCollisionPoint = true;
            return true;
        }

        return false;
    }

    if (shapeA.t == ShapeType::kRect && shapeB.t == ShapeType::kRect){
        return RectRectCollision(shapeA.shape.rect, objA, shapeB.shape.rect, objB);
    }

    return false;
}

/**
 * @brief Runs the main game loop until the window is closed.
 *
 * Each frame removes inactive objects, activates newly added objects,
 * processes input events, updates game objects, checks collisions,
 * performs late updates, renders the scene, and displays the frame.
 *
 * AI-generated documentation.
 */
void GameEngine::Run() {
    while (mWindow->isOpen())  // window is open
    {
        mGameContext.currentFrame++;

        // 0. Remove any objects that are now dead
        // Compact the active object vector in place by copying
        // only living objects toward the front, then resize once. This avoids repeated
        // vector erase operations and prevents O(n^2) shifting.
        // This method is suggested by ai, as it could improve the the efficiency of removing dead objects
        size_t writeIndex = 0;
        for (size_t readIndex = 0; readIndex < mGameObjects.size(); ++readIndex) {
            if (mGameObjects[readIndex]->IsAlive()) {
                mGameObjects[writeIndex] = mGameObjects[readIndex];
                ++writeIndex;
            }
        }

        mGameObjects.resize(writeIndex);

        // 1. Activate and initialize any objects added during the last frame
        for (auto &gameObject : mNewGameObjects) {
            mGameObjects.push_back(gameObject);
            mGameContext.CurrObject = gameObject;
            gameObject->Initialize(&mGameContext);
        }

        mNewGameObjects.clear();

        // 2. Process events
        while (const std::optional event = mWindow->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mWindow->close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                int key = static_cast<int>(keyPressed->scancode);
                auto it = mKeyDownNotifications.find(key);
                if (it != mKeyDownNotifications.end()) {
                    for (const std::string& notification : it->second) {
                        mNotificationManager.Notify(notification);
                    }
                }
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                int key = static_cast<int>(keyReleased->scancode);
                auto it = mKeyUpNotifications.find(key);
                if (it != mKeyUpNotifications.end()) {
                    for (const std::string& notification : it->second) {
                        mNotificationManager.Notify(notification);
                    }
                }
            }
        }

        // 3. Update game objects
        for (auto &gameObject : mGameObjects) {
            mGameContext.CurrObject = gameObject;
            gameObject->Update(&mGameContext);
        }

        // 4. Process collision events
        for (size_t a = 0; a < mGameObjects.size(); ++a) {
            auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[a]);

            if (objA == nullptr) {
                continue;
            }

            for (size_t b = a + 1; b < mGameObjects.size(); ++b) { //avoids checking an object against itself
                auto objB =
                    std::dynamic_pointer_cast<CollisionObject>(mGameObjects[b]);

                if (objB == nullptr) {
                    continue;
                }

                if (objA == objB) {
                    continue;
                }

                Rect boundsA = objA->GetBounds();
                Rect boundsB = objB->GetBounds();

                float aLeft = boundsA.topLeft.x;
                float aRight = boundsA.topLeft.x + boundsA.width;
                float aTop = boundsA.topLeft.y;
                float aBottom = boundsA.topLeft.y + boundsA.height;

                float bLeft = boundsB.topLeft.x;
                float bRight = boundsB.topLeft.x + boundsB.width;
                float bTop = boundsB.topLeft.y;
                float bBottom = boundsB.topLeft.y + boundsB.height;

                bool overlap =
                    !(aRight < bLeft ||
                    bRight < aLeft ||
                    aBottom < bTop ||
                    bBottom < aTop);

                if (overlap){
                    bool shapeCollision = false;
                    bool hasCollisionPoint = false;
                    Point2D collisionPoint;

                    for (const Shape& shapeA : objA->GetShapes()){
                        for (const Shape& shapeB : objB->GetShapes()){
                            bool pointFound = false;
                            Point2D point;

                            if (ShapesCollide(shapeA, objA, shapeB, objB, pointFound, point)){
                                shapeCollision = true;

                                if (pointFound){
                                    hasCollisionPoint = true;
                                    collisionPoint = point;
                                }

                                break;
                            }
                        }

                        if (shapeCollision) break;
                    }

                    if (shapeCollision){
                        if (hasCollisionPoint){
                            Point2D localA = WorldToLocal(collisionPoint, objA);
                            Point2D localB = WorldToLocal(collisionPoint, objB);

                            mGameContext.CurrObject = objA;
                            objA->CollisionEnter(objB, localA);
                            mGameContext.CurrObject = objB;
                            objB->CollisionEnter(objA, localB);
                        }
                        else{
                            mGameContext.CurrObject = objA;
                            objA->CollisionEnter(objB);

                            mGameContext.CurrObject = objB;
                            objB->CollisionEnter(objA);
                        }
                    }
                }
            }
        }

        // 5. Late updates
        for (auto &gameObject : mGameObjects) {
            mGameContext.CurrObject = gameObject;
            gameObject->LateUpdate(&mGameContext);
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        for (auto &gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject != nullptr) {
                mGameContext.CurrObject = gameObject;
                // Reset the gameplay drawing context after object rendering
                mScreenDrawContext->SetContextOffset(graphicsObject->GetLocation());
                mScreenDrawContext->SetContextRotation(graphicsObject->GetRotation());
                graphicsObject->RenderBackground(&mGameContext);
            }
        }

        // 7. Render foreground
        for (auto &gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);
            if (graphicsObject != nullptr) {
                mGameContext.CurrObject = gameObject;
                // Reset the gameplay drawing context after object rendering
                mScreenDrawContext->SetContextOffset(graphicsObject->GetLocation());
                mScreenDrawContext->SetContextRotation(graphicsObject->GetRotation());
                graphicsObject->RenderForeground(&mGameContext);
            }
        }

        // 8. Render UI
        for (auto &gameObject : mGameObjects){
            mGameContext.CurrObject = gameObject;
            gameObject->RenderUI(&mGameContext);
        }

        // Actually render to window
        mWindow->display();
    }
}

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
//{
//	while (const std::optional event = mWindow->pollEvent())
//	{
//		if (event->is<sf::Event::Closed>())
//		{
//		}
//		else if (event->is<sf::Event::Resized>())
//		{
//		}
//		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
//		{
//			// use keyPressed->unicode to get character
//		}
//	}
// }

}  // namespace CMPUT350
