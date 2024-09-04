#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsScene> // Include QGraphicsScene
#include <QGraphicsRectItem> // For drawing squares
#include "ChessBoard.h"
#include "ChessPieceItem.h"
#include "ChessAI.h"
#include "RandomAI.h"
#include "OneStepAheadAI.h"
#include "InteractiveSquare.h"

class Game : public QGraphicsView {
    Q_OBJECT // Use Q_OBJECT macro for any class that defines signals or slots

public:
    // Constructor declaration
    explicit Game(QWidget *parent = nullptr);

    // Destructor
    virtual ~Game() {}

    void showStartMenu();
    //void setupGame(bool is_white, const QString& aiMode);


    void onPieceSelected(std::shared_ptr<ChessPiece> piece);
    void clearHighlightedPieces();
    void highlightSquare(int x, int y, const QColor &color);
    ChessPieceItem *findChessPieceItem(const std::shared_ptr<ChessPiece> &piece);
    void playerMove(int toX, int toY);
    void makeAIMove();
    void updateUI();
    void toggleTurn();

public slots:
    void setupGame(bool isPlayerWhite, const QString& aiMode);
    void handlePawnPromotion(std::shared_ptr<ChessPiece> pawn, std::shared_ptr<ChessPiece> promotedPiece);

private:
    ChessPieceItem* currentlySelectedItem = nullptr;
    QList<InteractiveSquare*> highlightedSquares; // Container for highlighted squares
    QGraphicsScene *scene; // Hold a pointer to the scene
    std::unique_ptr<ChessBoard> board;
    std::shared_ptr<ChessAI> aiOpponent; // AI opponent
    bool is_white;
    void highlightPieces();
    bool user_turn;
    std::array<std::array<QGraphicsRectItem*, 8>, 8> squares; // Store pointers to square items
    void drawChessboard();
    void initializeChessPieces();
    QGraphicsRectItem* getSquareAt(int x, int y);
    std::shared_ptr<ChessPiece> selectedPiece;
    bool aiMovePending;

    void promotePawnItem(std::shared_ptr<ChessPiece> newPiece);
    ChessMove CheckPlayerPawnPromotion(const ChessMove &move);
    void clearSquares();

    int xOffset = 200;
    QList<ChessPieceItem*> capturedWhitePieces;
    QList<ChessPieceItem*> capturedBlackPieces;
    void addCapturedPiece(ChessPieceItem *item);
    void clearChessboardSquares();
    void resetGame();
};

#endif // GAME_H
