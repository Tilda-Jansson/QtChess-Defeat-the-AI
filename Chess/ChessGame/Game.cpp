#include <QGraphicsScene>
#include "Game.h"
#include "ChessPieceItem.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include "RandomAI.h"
#include "OneStepAheadAI.h"
#include "InteractiveSquare.h"
#include "StartMenuDialog.h"
#include <QInputDialog>
#include <QApplication>


Game::Game(QWidget *parent): QGraphicsView(parent), board(std::make_unique<ChessBoard>(8, 8)){
    // create scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800+ 2*xOffset, 800);

    // add a view
    this->setScene(scene);

    setFixedSize(1000+xOffset, 802);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void Game::setupGame(bool isPlayerWhite, const QString& aiMode) {
    qDebug() << "Setting up game. Player is white:" << isPlayerWhite << ", AI mode:" << aiMode;
    is_white = isPlayerWhite;

    user_turn = is_white ? true : false;

    if (aiMode == "Random") {
        aiOpponent = std::make_shared<RandomAI>();
    } else if (aiMode == "OneStepAhead") {
        aiOpponent = std::make_shared<OneStepAheadAI>();
    }

    drawChessboard();
    // Setup the board with the initial configuration
    board->setupDefaultBoard();
    // Initialize chess pieces on the board based on player's color
    initializeChessPieces();

    connect(board.get(), &ChessBoard::pawnPromoted, this, &Game::handlePawnPromotion);

    // Immediately update UI to reflect the initial state
    if (user_turn){
        qDebug() << "user turn";
        qDebug() << is_white;
        highlightPieces();
    }
    else{
        qDebug() << "ai turn";
        if (aiOpponent) {
            qDebug() << "ai is set";
        }
        aiMovePending = true; // Indicate that an AI move is pending
        QTimer::singleShot(500, this, [this]() {
            qDebug() << aiMovePending;
            if (aiMovePending) { // Check if an AI move is still expected
                makeAIMove();
                aiMovePending = false; // Reset flag after making AI move
            }
        });
    }
}

void Game::toggleTurn() {
    user_turn = !user_turn;
    if (!user_turn) {
        // Assuming you have a bool member `aiMovePending` initialized to false
        aiMovePending = true; // Indicate that an AI move is pending
        QTimer::singleShot(500, this, [this]() {
            if (aiMovePending) { // Check if an AI move is still expected
                makeAIMove();
                aiMovePending = false; // Reset flag after making AI move
            }
        });
    } else {
        highlightPieces(); // Highlight player moves when it's their turn
    }
}

void Game::playerMove(int toX, int toY) {
    clearHighlightedPieces();
    clearSquares();
    currentlySelectedItem = nullptr;
    //if (!user_turn || !selectedPiece) return;

    auto validCaptures = selectedPiece->capturingMoves();
    auto validMoves = selectedPiece->nonCapturingMoves();
    bool captureAvailable = !validCaptures.empty();

    ChessMove move(selectedPiece->getX(), selectedPiece->getY(), toX, toY, selectedPiece);
    qDebug() << toX;
    qDebug() << toY;
    qDebug() << selectedPiece->getX();
    qDebug() << selectedPiece->getY();

    // Check if the move is a valid capture when captures are available, or a valid move otherwise.
    bool isValidMove = captureAvailable ? std::find(validCaptures.begin(), validCaptures.end(), move) != validCaptures.end() :
                           std::find(validMoves.begin(), validMoves.end(), move) != validMoves.end();

    if (!isValidMove) {
        // Invalid move logic here
        qDebug() << "Invalid move";
        return;
    }

    ChessMove potentialPromotedMove = CheckPlayerPawnPromotion(move);

    board->movePiece(potentialPromotedMove); // Move the piece on the board

    updateUI();
}

ChessMove Game::CheckPlayerPawnPromotion(const ChessMove& move){
    size_t rows = board->getRows() - 1;
    if ((!is_white && move.to_x == rows) || (is_white && move.to_x == 0)) {
        if (move.piece->getLatin1Representation() == (is_white ? 'P' : 'p')) {
            QStringList pieces;
            pieces << "Queen" << "King" << "Rook" << "Bishop" << "Knight";

            bool ok;
            QString selected = QInputDialog::getItem(this, "Pawn Promotion", "Choose a piece:", pieces, 0, false, &ok);

            if (ok && !selected.isEmpty()) {
                qDebug() << "PROMOTED PAWN!!!!!!!!!!";
                char promotionPiece = 'Q'; // Default to Queen
                if (selected == "Rook") promotionPiece = 'R';
                else if (selected == "Bishop") promotionPiece = 'B';
                else if (selected == "Knight") promotionPiece = 'N';
                else if (selected == "King") promotionPiece = 'K';

                qDebug() << selected;

                auto item = findChessPieceItem(move.piece);
                if (item) {
                    qDebug() << "PAWN ITEM EXISTS";
                }

                // Use the promotion logic that is part of your board class.
                ChessMove new_move = board->promotePawn(move, promotionPiece);
                return new_move;
            }
        }
    }
    return move;
}




void Game::drawChessboard() {
    int squareSize = 100; // Size of a square on the chessboard
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            // Create a new square
            QGraphicsRectItem *square = new QGraphicsRectItem(xOffset + squareSize * i, squareSize * j, squareSize, squareSize);
            square->setZValue(0);
            // Alternate colors
            if ((i + j) % 2 == 0)
                square->setBrush(Qt::white);
            else
                square->setBrush(Qt::darkGray);

            scene->addItem(square); // Add square to the scene
            squares[i][j] = square; // Store the square in the array
        }
    }
}

void Game::initializeChessPieces() {
    int squareSize = 100; // Assuming square size
    for (auto &piece : board->getPieces(true)) { // For white pieces
        auto item = new ChessPieceItem(piece);
        item->setZValue(2);
        scene->addItem(item);
        item->setPos(xOffset + piece->getX() * squareSize, piece->getY() * squareSize);
        item->setFlag(QGraphicsItem::ItemIsSelectable, false); // Make unselectable initially
        connect(item, &ChessPieceItem::pieceSelected, this, &Game::onPieceSelected);
    }
    for (auto &piece : board->getPieces(false)) { // For black pieces
        auto item = new ChessPieceItem(piece);
        item->setZValue(2);
        scene->addItem(item);
        item->setPos(xOffset + piece->getX() * squareSize, piece->getY() * squareSize);
        item->setFlag(QGraphicsItem::ItemIsSelectable, false); // Make unselectable initially
        connect(item, &ChessPieceItem::pieceSelected, this, &Game::onPieceSelected);
    }
}



void Game::highlightPieces() {
    qDebug() << "Highlighting pieces";
    qDebug() << is_white;
    auto pieces = board->getPieces(is_white);

    qDebug() << "[highlightPieces] Total pieces to consider for highlighting:" << pieces.size();

    // Determine if any pieces have capturing moves
    bool hasCapturingMoves = std::any_of(pieces.begin(), pieces.end(), [](const auto& piece) {
        return !piece->capturingMoves().empty();
    });

    qDebug() << "[highlightPieces] Has capturing moves:" << hasCapturingMoves;

    // Highlight pieces based on the presence of capturing moves
    for (const auto &piece : pieces) {
        auto moves = hasCapturingMoves ? piece->capturingMoves() : piece->nonCapturingMoves();
        if (!moves.empty()) {
            auto item = findChessPieceItem(piece);
            if (item != nullptr) {
                item->setHighlight(Qt::green);
                //qDebug() << "Highlighting piece type:" << piece->getLatin1Representation()
                //         << " at position:" << piece->getX() << "," << piece->getY()
                //         << " with moves:" << moves.size();
            }else {
                qDebug() << "[highlightPieces] No ChessPieceItem found for piece type:" << piece->getLatin1Representation()
                         << " at position:" << piece->getX() << piece->getY();
            }
        } // Add no valid moves - Game over! redan hanterat på AI sidan
    }
}




ChessPieceItem *Game::findChessPieceItem(const std::shared_ptr<ChessPiece>& piece) {
    for (auto item : scene->items()) {
        // Attempt to cast the QGraphicsItem to ChessPieceItem
        auto chessItem = dynamic_cast<ChessPieceItem*>(item);
        if (chessItem && chessItem->getChessPiece() == piece) {
            // Found the matching ChessPieceItem
            return chessItem;
        }
    }
    // Return nullptr if no matching item was found
    qDebug() << "NULLPTR piece Item!!!!!";
    return nullptr;
}



void Game::onPieceSelected(std::shared_ptr<ChessPiece> piece) {
    if (!user_turn) return;
    selectedPiece = piece;
    qDebug() << piece->getLatin1Representation();

    // First, clear any existing highlights
    clearSquares();
    if (currentlySelectedItem) {
        currentlySelectedItem->setHighlight(Qt::green); // Reset to default highlight color
    }

    // Highlight the selected piece in red
    auto item = findChessPieceItem(piece);
    if (item) {
        item->setHighlight(Qt::red); // Call the new highlight method
        currentlySelectedItem = item;
    }

    vector<ChessMove> moves = piece->capturingMoves().empty() ? piece->nonCapturingMoves() : piece->capturingMoves();
    for (auto &move : moves) {
        //qDebug() << "Highlighting square";
        highlightSquare(move.to_x, move.to_y, Qt::red);
    }
}


void Game::clearSquares() {
    for (auto square : highlightedSquares) {
        scene->removeItem(square);
        delete square; // Delete the square to free up memory
    }
    highlightedSquares.clear(); // Clear the list after removing all squares
}


void Game::clearHighlightedPieces() {
    for (auto item : scene->items()) {
        item->setGraphicsEffect(nullptr); // Remove any effects, effectively clearing highlights
        item->setFlag(QGraphicsItem::ItemIsSelectable, false); // Make unselectable again
    }
}


void Game::highlightSquare(int x, int y, const QColor &color) {
    //qDebug() << "Attempting to highlight square at:" << x << y;
    // Create an interactive square
    InteractiveSquare* square = new InteractiveSquare( x * 100, y * 100, 100, 100);
    square->setBrush(color);
    square->setZValue(1);

    // Connect the squareClicked signal to the Game class slot or lambda function
    connect(square, &InteractiveSquare::squareClicked, this, [this](int x, int y) {
        playerMove(x, y);
    });

    scene->addItem(square);
    highlightedSquares.append(square);
}



QGraphicsRectItem* Game::getSquareAt(int x, int y) {
    if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        return squares[x][y]; // Return the square at the given coordinates
    } else {
        return nullptr; // Return null if coordinates are out of bounds
    }
}




void Game::makeAIMove() {
    if (!user_turn && aiMovePending && aiOpponent) {
            ChessMove aiMove = aiOpponent->getMove(*board, !is_white);
            if (aiMove.piece) {
                board->movePiece(aiMove);
                qDebug() << "AI MOVED ";
            }
    }
    qDebug() << "AI Passed";
    board->printPieceLists();
    updateUI();
}



void Game::updateUI() {
    ChessMove lastMove = board->getLastMove();
    qDebug() << "Moved Piece";
    qDebug() << lastMove.piece->getLatin1Representation();
    qDebug() << lastMove.piece->getX() << lastMove.piece->getY();

    auto lastCapturedPiece = board->getLastCapturedPiece();

    // If there was a captured piece, remove its corresponding item from the scene
    if (lastCapturedPiece != nullptr) {
        qDebug() << "capture";
        qDebug() << lastCapturedPiece->getLatin1Representation();
        auto item = findChessPieceItem(lastCapturedPiece);
        if (item) {
            qDebug() << "removed item";
            board->printPieceLists();
            //scene->removeItem(item);
            //delete item; // avoid memory leaks
            addCapturedPiece(item);
        }
    }

    // Move the piece that was moved to its new position
    auto movedPieceItem = findChessPieceItem(lastMove.piece);
    if (movedPieceItem) {
        qDebug() << "Moved piece to new pos";
        movedPieceItem->setPos(xOffset + lastMove.to_x * 100, lastMove.to_y * 100);
    }



    // After moving, check if the game is over
    qDebug() << user_turn;
    board->resetLastMoveAndCapture();

    if (board->gameOver(!is_white) || board->gameOver(is_white)) {
        QString winner = board->gameOver(!is_white) ? (!is_white ? "White wins!" : "Black wins!") : (is_white ? "White wins!" : "Black wins!");
        QMessageBox msgBox;
        msgBox.setWindowTitle("Game Over");
        msgBox.setText(winner + "\nWould you like to play again?");
        QAbstractButton* playAgainButton = msgBox.addButton(tr("Play Again"), QMessageBox::YesRole);
        msgBox.addButton(tr("Exit"), QMessageBox::NoRole);

        msgBox.exec();

        if (msgBox.clickedButton() == playAgainButton) {
            resetGame();
        } else {
            QApplication::quit();
        }
    }
    else {
        toggleTurn();
    }
}


void Game::handlePawnPromotion(std::shared_ptr<ChessPiece> pawn, std::shared_ptr<ChessPiece> promotedPiece) {
    auto item = findChessPieceItem(pawn);
    if (item) {
        scene->removeItem(item); // Unclear??
        delete item;
    }
    else{
        qDebug() << "COULDNT FIND PAWN ITEM TO DELETE";
    }
    auto promotedItem = new ChessPieceItem(promotedPiece);
    promotedItem->setZValue(2);
    scene->addItem(promotedItem);
    promotedItem->setPos(xOffset + promotedPiece->getX() * 100, promotedPiece->getY() * 100);
    connect(promotedItem, &ChessPieceItem::pieceSelected, this, &Game::onPieceSelected);
}



void Game::addCapturedPiece(ChessPieceItem* item) {
    if(item->getChessPiece()->isWhite()) {
        int index = capturedWhitePieces.count();
        int column = index % 2; // Number of pieces per row, adjust based on space
        int row = index / 2; // Change rows after every 4 pieces

        // Assuming the board's right edge is at x = 800, and we start placing captured pieces 20px to the right from the board's edge
        int x = xOffset + 800 + 20 + (column * 75); // 75px apart to account for piece size and some margin
        int y = 30 + (row * 75); // Start 20px from the top, each piece 75px apart vertically

        item->setPos(x, y);
        capturedWhitePieces.append(item);
    } else {
        int index = capturedBlackPieces.count();
        int column = index % 2; // Adjust the number of pieces per row as needed
        int row = index / 2;

        // Assuming the window's left edge is where we start, with a margin
        int x = 40 + (column * 75); // 20px from the left edge, each piece 75px apart horizontally
        int y = 30 + (row * 75); // Start 20px from the top, each piece 75px apart vertically

        item->setPos(x, y);
        capturedBlackPieces.append(item);
    }
}




void Game::resetGame() {
    // Clear the scene
    clearSquares();

    // Remove captured white pieces
    for (auto &item : capturedWhitePieces) {
        scene->removeItem(item);
        delete item;
    }
    capturedWhitePieces.clear(); // Clear the list

    // Remove captured black pieces
    for (auto &item : capturedBlackPieces) {
        scene->removeItem(item);
        delete item;
    }
    capturedBlackPieces.clear(); // Clear the list


    // Remove the piece's items and logical counterpart on the board:
    // Clear all white pieces
    auto whitePieces = board->getPieces(true);
    for (auto &piece : whitePieces) {
        auto item = findChessPieceItem(piece);
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }

    // Clear all black pieces
    auto blackPieces = board->getPieces(false);
    for (auto &piece : blackPieces) {
        auto item = findChessPieceItem(piece);
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }

    // Now that all pieces are removed from the scene, clear the logical board
    board->resetBoard();

    clearChessboardSquares();
    disconnect(board.get(), &ChessBoard::pawnPromoted, this, &Game::handlePawnPromotion);

    scene->clear();

    // Reset game state variables
    aiOpponent = nullptr;
    is_white = NULL; // Assuming white starts the game
    user_turn = NULL;
    aiMovePending = false;
    selectedPiece = nullptr;
    currentlySelectedItem = nullptr;

    showStartMenu();
    // Reinitialize the board logic (if needed, recreate or reset your board object)
    //board->setupDefaultBoard(); // Assumes your ChessBoard class has a method to reset itself

    // Re-draw the chessboard and initialize pieces
    //drawChessboard();
    //initializeChessPieces();

}


void Game::clearChessboardSquares() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            // Check if the pointer is not null
            if (squares[i][j] != nullptr) {
                scene->removeItem(squares[i][j]); // Remove from the scene
                delete squares[i][j]; // Delete the object
                squares[i][j] = nullptr; // Set the pointer to nullptr
            }
        }
    }
}


void Game::showStartMenu() {
    auto startMenu = new StartMenuDialog(this); // Consider managing the dialog's lifetime appropriately
    startMenu->setModal(true); // Still modal, but now using show() for non-blocking behavior
    startMenu->setAttribute(Qt::WA_DeleteOnClose); // Ensure the dialog is deleted automatically after closing

    // Connect dialog signals to appropriate slots
    connect(startMenu, &StartMenuDialog::gameSettingsSelected, this, &Game::setupGame);
    connect(startMenu, &QDialog::accepted, [this]() {
        this->show(); // Show game window after setting up the game
    });
    connect(startMenu, &QDialog::rejected, []() {
        QApplication::quit(); // Quit application if dialog is rejected
    });

    startMenu->show(); // Show the dialog without blocking
}
