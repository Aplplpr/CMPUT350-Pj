#include "DrawContext.h"
#include <algorithm>
#include <cmath>

namespace CMPUT350 {

/**
 * @brief Creates a drawing context for rendering to a window.
 *
 * @param window Shared pointer to the SFML render window.
 * @param font Shared pointer to the font used for text rendering.
 *
 * AI-generated documentation.
 */
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font): mWindow(window), mFont(font), mContextOffset(0, 0), mContextRotation(0.0f) {}

/**
 * @brief Draws text centered at the given position.
 *
 * @param text The text to draw.
 * @param pixelSize The character size in pixels.
 * @param p The position that should be the center of the text.
 * @param c The text color.
 *
 * AI-generated documentation.
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text drawable(*mFont, text, pixelSize);
    drawable.setFillColor(sf::Color(c.r, c.g, c.b));

    sf::FloatRect bounds = drawable.getLocalBounds();

    drawable.setOrigin({
        bounds.position.x + bounds.size.x / 2.0f,
        bounds.position.y + bounds.size.y / 2.0f
    });

    Point2D transformedP = Transform(p);

    drawable.setPosition({transformedP.x, transformedP.y});
    drawable.setRotation(sf::radians(mContextRotation));

    mWindow->draw(drawable);
}

/**
 * @brief Draws text starting at the given position.
 *
 * @param text The text to draw.
 * @param pixelSize The character size in pixels.
 * @param p The position of the text.
 * @param c The text color.
 *
 * AI-generated documentation.
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text drawable(*mFont, text, pixelSize);
    drawable.setFillColor(sf::Color(c.r, c.g, c.b));

    Point2D transformedP = Transform(p);

    drawable.setPosition({transformedP.x, transformedP.y});
    drawable.setRotation(sf::radians(mContextRotation));

    mWindow->draw(drawable);
}

/**
 * @brief Draws a filled circle centered at the given point.
 *
 * @param p The center position of the circle.
 * @param radius The circle radius in pixels.
 * @param c The fill color of the circle.
 *
 * AI-generated documentation.
 */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    Point2D transformedP = Transform(p);

    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    circle.setPosition({transformedP.x - radius, transformedP.y - radius}); //top left corner
    mWindow->draw(circle);
}

/**
 * @brief Draws a filled rectangle.
 *
 * @param r The rectangle position and size.
 * @param c The fill color of the rectangle.
 *
 * AI-generated documentation.
 */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    Point2D p1 = Transform(r.topLeft);

    Point2D p2 = Transform(Point2D(
        r.topLeft.x + r.width,
        r.topLeft.y
    ));

    Point2D p3 = Transform(Point2D(
        r.topLeft.x + r.width,
        r.topLeft.y + r.height
    ));

    Point2D p4 = Transform(Point2D(
        r.topLeft.x,
        r.topLeft.y + r.height
    ));

    sf::ConvexShape rect;
    rect.setPointCount(4);

    rect.setPoint(0, {p1.x, p1.y});
    rect.setPoint(1, {p2.x, p2.y});
    rect.setPoint(2, {p3.x, p3.y});
    rect.setPoint(3, {p4.x, p4.y});
    rect.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(rect);
}

/**
 * @brief Draws the outline of a rectangle without filling its interior.
 *
 * @param r The rectangle position and size.
 * @param width The thickness of the outline in pixels.
 * @param c The outline color.
 *
 * AI-generated documentation.
 */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    Point2D p1 = r.topLeft;

    Point2D p2(
        r.topLeft.x + r.width,
        r.topLeft.y
    );

    Point2D p3(
        r.topLeft.x + r.width,
        r.topLeft.y + r.height
    );

    Point2D p4(
        r.topLeft.x,
        r.topLeft.y + r.height
    );

    DrawLine(p1, p2, width, c); //DrawLine will transform the points
    DrawLine(p2, p3, width, c);
    DrawLine(p3, p4, width, c);
    DrawLine(p4, p1, width, c);
}

/**
 * @brief Draws a line segment with a specified width and color.
 *
 * @param from The starting point of the line.
 * @param to The ending point of the line.
 * @param width The width of the line in pixels.
 * @param c The line color.
 *
 * The line is represented by a four-point convex shape. A perpendicular
 * offset is calculated from the line direction so the shape can support
 * horizontal, vertical, and angled lines.
 *
 * AI-generated documentation.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    from = Transform(from);
    to = Transform(to);

    float dx = to.x - from.x;
    float dy = to.y - from.y;

    float length = std::sqrt(dx * dx + dy * dy);

    if (length == 0) {
        return;
    }

    float halfWidth = width / 2.0f;

    float nx = -dy / length * halfWidth;
    float ny =  dx / length * halfWidth;

    sf::ConvexShape line;
    line.setPointCount(4);

    line.setPoint(0, {from.x + nx, from.y + ny});
    line.setPoint(1, {to.x + nx,   to.y + ny});
    line.setPoint(2, {to.x - nx,   to.y - ny});
    line.setPoint(3, {from.x - nx, from.y - ny});

    line.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(line);
}

void DrawContext::SetContextOffset(Point2D p) {
    mContextOffset = p;
}

void DrawContext::SetContextRotation(float rotation) {
    mContextRotation = rotation;
}

Point2D DrawContext::Transform(Point2D p) {
    float cosRotation = std::cos(mContextRotation);
    float sinRotation = std::sin(mContextRotation);

    float x = p.x * cosRotation - p.y * sinRotation;
    float y = p.x * sinRotation + p.y * cosRotation;

    return Point2D(x + mContextOffset.x, y + mContextOffset.y);
}

Point2D DrawContext::ReverseTransform(Point2D p) {
    float x = p.x - mContextOffset.x;
    float y = p.y - mContextOffset.y;

    float cosRotation = std::cos(mContextRotation);
    float sinRotation = std::sin(mContextRotation);

    float localX = x * cosRotation + y * sinRotation;
    float localY = -x * sinRotation + y * cosRotation;

    return Point2D(localX, localY);
}

Rect DrawContext::Transform(Rect r) {
    Point2D p1 = Transform(r.topLeft);

    Point2D p2 = Transform(
        Point2D(r.topLeft.x + r.width, r.topLeft.y)
    );

    Point2D p3 = Transform(
        Point2D(r.topLeft.x + r.width,
                r.topLeft.y + r.height)
    );

    Point2D p4 = Transform(
        Point2D(r.topLeft.x,
                r.topLeft.y + r.height)
    );

    float minX = std::min(std::min(p1.x, p2.x), std::min(p3.x, p4.x));
    float maxX = std::max(std::max(p1.x, p2.x), std::max(p3.x, p4.x));
    float minY = std::min(std::min(p1.y, p2.y), std::min(p3.y, p4.y));
    float maxY = std::max(std::max(p1.y, p2.y), std::max(p3.y, p4.y));

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
