// InteractiveSquare.h
#ifndef INTERACTIVESQUARE_H
#define INTERACTIVESQUARE_H

#include <QObject>
#include <QGraphicsRectItem>

class InteractiveSquare : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    InteractiveSquare(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = nullptr);

signals:
    void squareClicked(int x, int y);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int posX, posY; // Store the chessboard coordinates
};

#endif // INTERACTIVESQUARE_H
