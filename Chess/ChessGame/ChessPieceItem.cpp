#include "ChessPieceItem.h"
#include <QPixmap>
#include <QGraphicsDropShadowEffect>

// Constructor
ChessPieceItem::ChessPieceItem(std::shared_ptr<ChessPiece> logicPiece, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), chessPiece(logicPiece) {
    // Determine the file name based on the type and color of the logic piece
    char pieceSymbol = logicPiece->getLatin1Representation();
    QString colorPrefix = logicPiece->isWhite() ? "w" : "b"; // Assuming 'w' for white, 'b' for black
    QString typeName;

    // Assign a type name based on the Latin1 representation
    switch (pieceSymbol) {
    case 'K': typeName = "king"; break;
    case 'Q': typeName = "queen"; break;
    case 'R': typeName = "rook"; break;
    case 'B': typeName = "bishop"; break;
    case 'N': typeName = "knight"; break;
    case 'P': typeName = "pawn"; break;
    case 'k': typeName = "king"; break;
    case 'q': typeName = "queen"; break;
    case 'r': typeName = "rook"; break;
    case 'b': typeName = "bishop"; break;
    case 'n': typeName = "knight"; break;
    case 'p': typeName = "pawn"; break;
    default:
        typeName = "unknown";
        break;
    }

    // Build the image path from the piece's type and color
    QString imagePath = QString(":/img/%1_%2.png").arg(colorPrefix, typeName);

    // Set the item's pixmap to the image
    setPixmap(QPixmap(imagePath));
}


void ChessPieceItem::setChessPiece(std::shared_ptr<ChessPiece> piece) {
    this->chessPiece = piece;
    // Set the pixmap based on piece type and color
}

std::shared_ptr<ChessPiece> ChessPieceItem::getChessPiece() const {
    return chessPiece;
}


void ChessPieceItem::setHighlight(QColor color) {
    auto effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(20); // Adjust the blur radius for the glow effect
    effect->setColor(color); // Choose a color for the glow
    effect->setOffset(0); // Glow should be centered around the item
    this->setGraphicsEffect(effect);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);

}


void ChessPieceItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (this->flags() & QGraphicsItem::ItemIsSelectable) {
        QGraphicsPixmapItem::mousePressEvent(event); // Call base class implementation
        emit pieceSelected(chessPiece); // Emit signal if the piece is selectable
    }
}
