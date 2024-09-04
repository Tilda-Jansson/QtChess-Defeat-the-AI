QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Bishop.cpp \
    ChessAI.cpp \
    ChessBoard.cpp \
    ChessMove.cpp \
    ChessPiece.cpp \
    ChessPieceItem.cpp \
    Game.cpp \
    Interactivesquare.cpp \
    King.cpp \
    Knight.cpp \
    OneStepAheadAI.cpp \
    Pawn.cpp \
    Queen.cpp \
    RandomAI.cpp \
    Rook.cpp \
    StartMenuDialog.cpp \
    main.cpp

HEADERS += \
    Bishop.h \
    ChessAI.h \
    ChessBoard.h \
    ChessMove.h \
    ChessPiece.h \
    ChessPieceItem.h \
    Game.h \
    InteractiveSquare.h \
    King.h \
    Knight.h \
    OneStepAheadAI.h \
    Pawn.h \
    Queen.h \
    RandomAI.h \
    Rook.h \
    StartMenuDialog.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
