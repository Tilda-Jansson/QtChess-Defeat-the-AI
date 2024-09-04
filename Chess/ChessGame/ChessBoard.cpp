#include "ChessBoard.h"
#include "King.h" //
#include "Queen.h" //
#include "Rook.h"  //
#include "Bishop.h" //
#include "Knight.h" //
#include "Pawn.h" //
#include <locale>
#include <codecvt>

ChessBoard::ChessBoard(size_t rows, size_t cols)
    : m_rows(rows), m_cols(cols), lastMove(-1, -1, -1, -1, nullptr) {}


void ChessBoard::movePiece(ChessMove chess_move) {
    shared_ptr<ChessPiece> captured_piece = getPieceAt(chess_move.to_x, chess_move.to_y);
    lastMove = chess_move;

    // Check if a piece was captured
    if (captured_piece != nullptr) {
        // Remove the captured piece from the vector
        lastCapturedPiece = captured_piece;

        auto it = find(m_pieces.begin(), m_pieces.end(), captured_piece);
        if (it != m_pieces.end()) {
            m_pieces.erase(it);
        }
    }else {
        // No piece was captured
        lastCapturedPiece = nullptr;
    }

    // Update the piece position
    chess_move.piece->setX(chess_move.to_x);
    chess_move.piece->setY(chess_move.to_y);
}


vector<ChessMove> ChessBoard::capturingMoves(bool is_white) {
    vector<ChessMove> moves;

    for (const auto &piece : m_pieces) {
        // If the piece color is the same as 'is_white'
        if (piece->isWhite() == is_white) {
            vector<ChessMove> pieceMoves = piece->capturingMoves();
            moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }

    return moves;
}

vector<ChessMove> ChessBoard::nonCapturingMoves(bool is_white) {
    vector<ChessMove> moves;

    for (const auto &piece : m_pieces) {
        // If the piece color is the same as 'is_white'
        if (piece->isWhite() == is_white) {
            vector<ChessMove> pieceMoves = piece->nonCapturingMoves();
            moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }

    return moves;
}


void ChessBoard::setupDefaultBoard() {
    //m_pieces.push_back(piece);    // Add it to the list
    // Setup the Black pieces
    setPieceAt(0, 0, std::make_shared<Rook>(0, 0, false, this));
    setPieceAt(0, 7, std::make_shared<Rook>(0, 7, false, this));
    setPieceAt(0, 1, std::make_shared<Knight>(0, 1, false, this));
    setPieceAt(0, 6, std::make_shared<Knight>(0, 6, false, this));
    setPieceAt(0, 2, std::make_shared<Bishop>(0, 2, false, this));
    setPieceAt(0, 5, std::make_shared<Bishop>(0, 5, false, this));
    setPieceAt(0, 3, std::make_shared<Queen>(0, 3, false, this));
    setPieceAt(0, 4, std::make_shared<King>(0, 4, false, this));
    for(int i = 0; i < 8; ++i) {
        setPieceAt(1, i, std::make_shared<Pawn>(1, i, false, this));
    }

    // Setup the White pieces
    setPieceAt(7, 0, std::make_shared<Rook>(7, 0, true, this));
    setPieceAt(7, 7, std::make_shared<Rook>(7, 7, true, this));
    setPieceAt(7, 1, std::make_shared<Knight>(7, 1, true, this));
    setPieceAt(7, 6, std::make_shared<Knight>(7, 6, true, this));
    setPieceAt(7, 2, std::make_shared<Bishop>(7, 2, true, this));
    setPieceAt(7, 5, std::make_shared<Bishop>(7, 5, true, this));
    setPieceAt(7, 3, std::make_shared<Queen>(7, 3, true, this));
    setPieceAt(7, 4, std::make_shared<King>(7, 4, true, this));
    for(int i = 0; i < 8; ++i) {
        setPieceAt(6, i, std::make_shared<Pawn>(6, i, true, this));
    }
}


ChessBoard & operator<<(ostream & os, ChessBoard & cb) {
    size_t rows = cb.getRows();
    size_t cols = cb.getCols();
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            shared_ptr<ChessPiece> piece = cb.getPieceAt(row, col);
            if (piece) {
                //os << piece->getUtfRepresentation();
                char32_t utfChar = piece->getUtfRepresentation();
                wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                string utf8String = converter.to_bytes(utfChar);
                os << utf8String;

            } else {
                os << '.';
            }
        }
        os << endl;
    }
    return cb;
}


shared_ptr<ChessPiece> ChessBoard::getPieceAt(size_t row, size_t col) const {
    for (const auto& piece : m_pieces) {
        if (piece->getX() == row && piece->getY() == col) {
            return piece;
        }
    }
    return nullptr;

}

void ChessBoard::setPieceAt(size_t row, size_t col, shared_ptr<ChessPiece> piece) {
    if (row < 0 || row >= getRows() || col < 0 || col >= getCols()) {
        throw std::out_of_range("Index out of range");
    }

    // Removing any existing piece at the given location
    auto it = std::remove_if(m_pieces.begin(), m_pieces.end(), [row, col](const auto& p) {
        return p->getX() == row && p->getY() == col;
    });
    m_pieces.erase(it, m_pieces.end());

    // Update the piece position or add it to the list if it's not there
    if (piece != nullptr) {
        piece->setX(row);
        piece->setY(col);

        auto piece_it = std::find(m_pieces.begin(), m_pieces.end(), piece);
        if (piece_it == m_pieces.end()) { // If the piece is not already in the list
            m_pieces.push_back(piece);    // Add it to the list
        }
    }
}


bool ChessBoard::isEmpty(int x, int y) const {
    return getPieceAt(x, y) == nullptr;
}

bool ChessBoard::isOpponent(int x, int y, bool is_white) const {
    shared_ptr<ChessPiece> piece = getPieceAt(x, y);
    return piece && piece->isWhite() != is_white;
}

void ChessBoard::addPiece(shared_ptr<ChessPiece> piece) {
    m_pieces.push_back(piece);
}

vector<shared_ptr<ChessPiece>> ChessBoard::getPieces(bool is_white) const {
    vector<shared_ptr<ChessPiece>> result;
    for (const auto &piece : m_pieces) {
        if (piece->isWhite() == is_white) {
            result.push_back(piece);
        }
    }
    return result;
}



bool ChessBoard::gameOver(bool is_white){
    const vector<shared_ptr<ChessPiece>> &pieces = getPieces(is_white);
    bool no_remaining_pieces = pieces.size() == 0;

    vector<ChessMove> moves;
    vector<ChessMove> capturing_moves = capturingMoves(is_white);
    vector<ChessMove> non_capturing_moves = nonCapturingMoves(is_white);

    moves.insert(moves.end(), capturing_moves.begin(), capturing_moves.end());
    moves.insert(moves.end(), non_capturing_moves.begin(), non_capturing_moves.end());

    bool no_legal_moves = moves.empty();

    return no_legal_moves || no_remaining_pieces;
}

void ChessBoard::updatePieceLists(shared_ptr<ChessPiece> pawn, shared_ptr<ChessPiece> promoted_move) {
    auto& piecelist = m_pieces;
    auto it = find(piecelist.begin(), piecelist.end(), pawn);
    if (it != piecelist.end()) {
        piecelist.erase(it);
    }
    //add piece
    piecelist.push_back(promoted_move);
    cout << "A PAWN WAS PROMOTED";

    emit pawnPromoted(pawn, promoted_move);
}

// Helper
void ChessBoard::printPieceLists() {
    cout << "White pieces: ";
    for (const auto& piece : m_pieces) {
        if (piece->isWhite()) {
            cout << piece->getLatin1Representation() << " ";
        }
    }
    cout << endl;

    cout << "Black pieces: ";
    for (const auto& piece : m_pieces) {
        if (!piece->isWhite()) {
            cout << piece->getLatin1Representation() << " ";
        }
    }
    cout << endl;
}

size_t ChessBoard::getRows() const {
    return m_rows;
}

size_t ChessBoard::getCols() const {
    return m_cols;
}




ChessMove ChessBoard::promotePawn(const ChessMove& move, char promotionPiece) {
    // Assuming move.piece points to the pawn that is to be promoted
    int x = move.to_x;
    int y = move.to_y;
    bool isWhite = move.piece->isWhite();
    std::shared_ptr<ChessPiece> promoted_piece;
    cout << promotionPiece;

    switch (promotionPiece) {
    case 'K':
        promoted_piece = std::make_shared<King>(x, y, isWhite, this);
        break;
    case 'Q':
        promoted_piece = std::make_shared<Queen>(x, y, isWhite, this);
        break;
    case 'R':
        promoted_piece = std::make_shared<Rook>(x, y, isWhite, this);
        break;
    case 'B':
        promoted_piece = std::make_shared<Bishop>(x, y, isWhite, this);
        break;
    case 'N':
        promoted_piece = std::make_shared<Knight>(x, y, isWhite, this);
        break;
    default:
        promoted_piece = std::make_shared<Queen>(x, y, isWhite, this); // Default to queen if unspecified
    }
    ChessMove promoted_move = ChessMove(move.from_x, move.from_y, x, y, promoted_piece);

    // Remove the pawn and add the promoted piece to the respective pieces list
    this->updatePieceLists(move.piece, promoted_move.piece);
    this->setPieceAt(promoted_move.from_x, promoted_move.from_y, promoted_move.piece);
    return promoted_move;
}


void ChessBoard::resetBoard() {
    m_pieces.clear();
}
