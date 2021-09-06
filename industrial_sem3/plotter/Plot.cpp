#include <cassert>
#include <math.h>
#include "Plot.hpp"

#define MAX_LABEL_LEN 20

static const float ARROW_WIDTH  = 7.0f;
static const float ARROW_HEIGHT = 10.0f;

static const float GRAPH_THICKNESS  = 2.0f;
static const float AXIS_THICKNESS   = 1.0f;
static const float VECTOR_THICKNESS = 1.0f;

static const float LABEL_Y_HORIZ_INDENT     = 7.0f;
static const float LABEL_X_VERT_INDENT      = 1.0f;
static const float LABEL_BOTTOM_VERT_INDENT = -25.0f;
static const float LABEL_LEFT_HORIZ_INDENT  = 7.0f;
static const float LABEL_RIGHT_HORIZ_INDENT = -7.0f;

static const double GRAPH_STEP = 1.0;

static const double DEFAULT_VIEWPORT_SIZE = 200.0;
static const double DEFAULT_SCALE_FACTOR  = 1 / 10.0;

static const int LABEL_FONT_SIZE = 20;
static const int DOT_VERT_COUNT  = 10;

static const int MAX_VEC_COUNT = 128;

static double defaultPlotFunc(double x);


Plot::Plot()
{
    vectors = new AppliedVector[MAX_VEC_COUNT];
    vecCount = 0;

    // Drawers setup

    dotDrawer.setRadius(GRAPH_THICKNESS);
    dotDrawer.setOrigin(GRAPH_THICKNESS, GRAPH_THICKNESS);
    
    dotDrawer.setFillColor(sf::Color::Black);
    dotDrawer.setPointCount(DOT_VERT_COUNT);

    textDrawer.setFillColor(sf::Color::Black);
    textDrawer.setCharacterSize(LABEL_FONT_SIZE);

    // Default params

    setFunction(defaultPlotFunc);

    setViewportSize(sf::Vector2f(DEFAULT_VIEWPORT_SIZE, DEFAULT_VIEWPORT_SIZE));
    setViewportOrigin(Vec2(0, 0));
    setScaleFactor(DEFAULT_SCALE_FACTOR);
}

Plot::~Plot()
{
    delete[] vectors;
}

void Plot::setFont(sf::Font &font)
{
    textDrawer.setFont(font);
}

void Plot::setViewportOrigin(Vec2 origin)
{
    plotOrigin = origin;
}

void Plot::setViewportSize(sf::Vector2f size)
{
    viewportSize = size;
    background.setSize(size);
}

void Plot::setScaleFactor(double scaleFactor)
{
    this->scaleFactor = scaleFactor;
}

void Plot::setFunction(PlotFunction function)
{
    assert(function != NULL);

    this->function = function;
}

void Plot::moveViewport(Vec2 offset)
{
    plotOrigin += offset * scaleFactor;
}

void Plot::scale(double scale)
{
    scaleFactor *= scale;
}

void Plot::addVector(AppliedVector vec)
{
    if (vecCount >= MAX_VEC_COUNT)
        return;

    vectors[vecCount++] = vec;
}

void Plot::draw(sf::RenderWindow &canvas, sf::Vector2f viewportPos)
{
    // Precalculation of some common-used values

    plotSize = Vec2(viewportSize) * scaleFactor;
    plotOffset = plotOrigin + Vec2(-plotSize.x, plotSize.y) / 2.0;

    background.setPosition(viewportPos);
    canvas.draw(background);

    drawAxis   (canvas, viewportPos);
    drawPlot   (canvas, viewportPos);
    drawVectors(canvas, viewportPos);
}

void Plot::drawAxis(sf::RenderWindow &canvas, sf::Vector2f viewportPosition)
{
    char buf[MAX_LABEL_LEN + 1] = {0};

    double viewportXAxisOffs = viewportSize.y / plotSize.y * plotOffset.y;
    
    if (viewportXAxisOffs > 0 && viewportXAxisOffs < viewportSize.x)
    {
        // X axis must be drawn
        
        sf::Vector2f XAxisBegin = viewportPosition + sf::Vector2f(0             , viewportXAxisOffs);
        sf::Vector2f XAxisEnd   = viewportPosition + sf::Vector2f(viewportSize.x, viewportXAxisOffs);

        drawArrow(canvas, XAxisBegin, XAxisEnd, AXIS_THICKNESS, sf::Color::Black);

        // Right X label

        snprintf(buf, MAX_LABEL_LEN, "%d", (int)(plotOffset.x + plotSize.x));
        textDrawer.setString(buf);

        sf::FloatRect textBound = textDrawer.getLocalBounds();

        sf::Vector2f rightLabelPos = viewportPosition +
                                     sf::Vector2f(viewportSize.x - textBound.width + LABEL_RIGHT_HORIZ_INDENT,
                                                  viewportXAxisOffs + LABEL_X_VERT_INDENT);

        textDrawer.setPosition(rightLabelPos);
        canvas.draw(textDrawer);

        // Left X label

        snprintf(buf, MAX_LABEL_LEN, "%d", (int)plotOffset.x);
        textDrawer.setString(buf);

        sf::Vector2f leftLabelPos = viewportPosition +
                                    sf::Vector2f(LABEL_LEFT_HORIZ_INDENT,
                                                 viewportXAxisOffs + LABEL_X_VERT_INDENT);
        
        textDrawer.setPosition(leftLabelPos);
        canvas.draw(textDrawer);
    }

    double viewportYAxisOffs = viewportSize.y / plotSize.y * (-plotOffset.x);
    
    if (viewportYAxisOffs > 0 && viewportYAxisOffs < viewportSize.y)
    {
        // Y axis must be drawn
        
        sf::Vector2f YAxisBegin = viewportPosition + sf::Vector2f(viewportYAxisOffs, viewportSize.y);
        sf::Vector2f YAxisEnd   = viewportPosition + sf::Vector2f(viewportYAxisOffs, 0);

        drawArrow(canvas, YAxisBegin, YAxisEnd, AXIS_THICKNESS, sf::Color::Black);

        // Top Y label

        snprintf(buf, MAX_LABEL_LEN, "%d", (int)plotOffset.y);
        textDrawer.setString(buf);

        sf::Vector2f topLabelPos = viewportPosition + sf::Vector2f(viewportYAxisOffs + LABEL_Y_HORIZ_INDENT, 0);


        textDrawer.setPosition(topLabelPos);
        canvas.draw(textDrawer);

        // Bottom Y label

        snprintf(buf, MAX_LABEL_LEN, "%d", (int)(plotOffset.y - plotSize.y));
        textDrawer.setString(buf);

        sf::Vector2f bottomLabelPos = viewportPosition +
                                      sf::Vector2f(viewportYAxisOffs + LABEL_Y_HORIZ_INDENT,
                                                   viewportSize.y + LABEL_BOTTOM_VERT_INDENT);

        textDrawer.setPosition(bottomLabelPos);
        canvas.draw(textDrawer);
    }
}

void Plot::drawPlot(sf::RenderWindow &canvas, sf::Vector2f viewportPos)
{
    sf::Vector2f prevViewportOffs = sf::Vector2f(-1.0, -1.0);
    bool prevOutside = false;

    bool mustStop = false;

    for (double xOffs = 0; !mustStop; xOffs += GRAPH_STEP * scaleFactor)
    {
        if (xOffs >= plotSize.x)
        {
            xOffs = plotSize.x;
            mustStop = true;
        }

        double x = plotOffset.x + xOffs;
        double y = function(x);

        double yOffs = plotOffset.y - y;

        sf::Vector2f viewportOffs = sf::Vector2f(viewportSize.x  / plotSize.x * xOffs,
                                                 viewportSize.y  / plotSize.y * yOffs);

        sf::Vector2f pointPos     = viewportPos + viewportOffs;
        sf::Vector2f prevPointPos = viewportPos + prevViewportOffs;

        if (viewportOffs.y < 0 || viewportOffs.y > viewportSize.y)
        {
            // current point is outside the viewport

            if (!prevOutside && prevViewportOffs.x > -0.5)
            {
                // if previous point is inside the viewport, line must be clipped by viewport border
                
                sf::Vector2f lineVec = pointPos - prevPointPos;
                float clipFactor = 1.0;

                if (viewportOffs.y < 0)
                {
                    // clipped at the top
                    clipFactor = prevViewportOffs.y / (prevViewportOffs.y - viewportOffs.y);
                }
                else if (viewportOffs.y > 0)
                {
                    // clipped at the bottom
                    clipFactor = (viewportSize.y - prevViewportOffs.y) /
                                 (viewportOffs.y - prevViewportOffs.y);
                }

                lineVec *= clipFactor;
                pointPos = prevPointPos + lineVec;
                drawLine(canvas, prevPointPos, pointPos, GRAPH_THICKNESS, sf::Color::Black);
            }
            
            prevViewportOffs = viewportOffs;
            prevOutside = true;
            continue;
        }

        // current point is inside the viewport

        dotDrawer.setPosition(pointPos);
        canvas.draw(dotDrawer);

        if (prevOutside)
        {
            // if previous point is outside the viewport, line must be clipped by viewport border

            sf::Vector2f lineVec = prevPointPos - pointPos;
            float clipFactor = 1.0f;

            if (prevViewportOffs.y < 0)
            {
                // clipped at the top
                clipFactor = viewportOffs.y / (viewportOffs.y - prevViewportOffs.y);
            }
            else
            {
                // clipped at the bottom
                clipFactor = (viewportSize.y - viewportOffs.y) /
                             (prevViewportOffs.y - viewportOffs.y);
            }

            lineVec *= clipFactor;
            prevPointPos = pointPos + lineVec;
            drawLine(canvas, prevPointPos, pointPos, GRAPH_THICKNESS, sf::Color::Black);
        }
        else if (prevViewportOffs.x > -0.5)
            drawLine(canvas, prevPointPos, pointPos, GRAPH_THICKNESS, sf::Color::Black);

        prevViewportOffs = viewportOffs;
        prevOutside = false;
    }
}

void Plot::drawVectors(sf::RenderWindow &canvas, sf::Vector2f viewportPos)
{
    for (int i = 0; i < vecCount; i++)
    {
        AppliedVector currVec = vectors[i];

        Vec2 vecStart = currVec.origin;
        Vec2 vecEnd   = vecStart + currVec.vector;

        Vec2 startOffs = Vec2(vecStart.x - plotOffset.x, plotOffset.y - vecStart.y);
        Vec2 endOffs   = Vec2(vecEnd.x   - plotOffset.x, plotOffset.y - vecEnd.y);

        sf::Vector2f startViewportOffs = sf::Vector2f(viewportSize.x  / plotSize.x  * startOffs.x,
                                                      viewportSize.y  / plotSize.y  * startOffs.y);

        sf::Vector2f endViewportOffs = sf::Vector2f(viewportSize.x  / plotSize.x  * endOffs.x,
                                                    viewportSize.y  / plotSize.y  * endOffs.y);

        if (startViewportOffs.x < 0 || startViewportOffs.x > viewportSize.x ||
            startViewportOffs.y < 0 || startViewportOffs.y > viewportSize.y ||
            endViewportOffs.x   < 0 || endViewportOffs.x   > viewportSize.x ||
            endViewportOffs.y   < 0 || endViewportOffs.y   > viewportSize.y)
        {
            continue;
        }

        sf::Vector2f vecStartViewport = viewportPos + startViewportOffs;
        sf::Vector2f vecEndViewport   = viewportPos + endViewportOffs;

        drawArrow(canvas, vecStartViewport, vecEndViewport, VECTOR_THICKNESS, sf::Color::Cyan);
    }
}

void Plot::drawLine(sf::RenderWindow &canvas,
                    sf::Vector2f p1, sf::Vector2f p2, float thickness, sf::Color color)
{
    sf::Vector2f line_vec = p2 - p1;
    float line_vec_len = sqrtf(line_vec.x * line_vec.x + line_vec.y * line_vec.y);

    sf::Vector2f normal = sf::Vector2f(-line_vec.y, line_vec.x) / line_vec_len;

    sf::Vector2f vert1 = p1 - normal * thickness;
    sf::Vector2f vert2 = p1 + normal * thickness;
    sf::Vector2f vert3 = p2 + normal * thickness;
    sf::Vector2f vert4 = p2 - normal * thickness;
    
    lineDrawer.setFillColor(color);

    lineDrawer.setPointCount(4);
    lineDrawer.setPoint(0, vert1);
    lineDrawer.setPoint(1, vert2);
    lineDrawer.setPoint(2, vert3);
    lineDrawer.setPoint(3, vert4);

    canvas.draw(lineDrawer);
}

void Plot::drawArrow(sf::RenderWindow &canvas,
                     sf::Vector2f p1, sf::Vector2f p2, float thickness, sf::Color color)
{
    drawLine(canvas, p1, p2, thickness, color);

    sf::Vector2f line_vec = p2 - p1;

    float line_vec_len = sqrtf(line_vec.x * line_vec.x + line_vec.y * line_vec.y);

    sf::Vector2f line_vec_norm = line_vec / line_vec_len;
    sf::Vector2f normal = sf::Vector2f(-line_vec_norm.y, line_vec_norm.x);

    sf::Vector2f vert1 = p2 - line_vec_norm * ARROW_HEIGHT - normal * ARROW_WIDTH;
    sf::Vector2f vert2 = p2 - line_vec_norm * ARROW_HEIGHT + normal * ARROW_WIDTH;

    lineDrawer.setFillColor(color);

    lineDrawer.setPointCount(3);
    lineDrawer.setPoint(0, p2);
    lineDrawer.setPoint(1, vert1);
    lineDrawer.setPoint(2, vert2);

    canvas.draw(lineDrawer);
}

static double defaultPlotFunc(double x)
{
    return x * x;
}