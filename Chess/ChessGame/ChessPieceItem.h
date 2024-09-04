#ifndef CHESSPIECEITEM_H
#define CHESSPIECEITEM_H

#include <QGraphicsPixmapItem>
#include <memory>
#include "ChessPiece.h"
#include <QObject>
#include <QGraphicsSceneMouseEvent> // For mouse events
#include <QGraphicsEffect>          // For highlight effects

// Forward declaration to resolve circular dependency
class ChessPiece;

class ChessPieceItem : public QObject, public QGraphicsPixmapItem, public std::enable_shared_from_this<ChessPieceItem> {
    Q_OBJECT // This macro must be included to enable signals and slots

public:
    explicit ChessPieceItem(std::shared_ptr<ChessPiece> logicPiece, QGraphicsItem *parent = nullptr);

    void setChessPiece(std::shared_ptr<ChessPiece> piece);
    std::shared_ptr<ChessPiece> getChessPiece() const;

    void setHighlight(QColor highlight);

Q_SIGNALS:
    void pieceSelected(std::shared_ptr<ChessPiece> piece);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    std::shared_ptr<ChessPiece> chessPiece; // Pointer to the logic piece
};


#endif // CHESSPIECEITEM_H
