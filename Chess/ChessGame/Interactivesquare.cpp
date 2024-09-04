// InteractiveSquare.cpp
#include "InteractiveSquare.h"
#include <QGraphicsSceneMouseEvent>

InteractiveSquare::InteractiveSquare(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : QObject(), QGraphicsRectItem(x + 200, y, width, height, parent), posX(x/width), posY(y/height) {
    // Enable mouse events for this item
    setAcceptHoverEvents(true);
}

void InteractiveSquare::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);
    emit squareClicked(posX, posY);
}
