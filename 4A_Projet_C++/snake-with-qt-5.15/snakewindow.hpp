#ifndef SNAKEWINDOW_HPP
#define SNAKEWINDOW_HPP

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "jeu.hpp"

class SnakeButton : public QPushButton {
public:
    SnakeButton(QWidget *parent = nullptr) : QPushButton(parent) {}
    void keyPressEvent(QKeyEvent *e) override {
        if (parent() != nullptr)
            QCoreApplication::sendEvent(parent(), e);
    }
};

class SnakeWindow : public QFrame {
protected:
    Jeu jeu;
    QPixmap pixmapCorps, pixmapTete, pixmapMur, pixmapFruit, pixmapTerre;
    SnakeButton *btnAddWall;
    SnakeButton *btnRemoveWall;
    QTimer *timer;

public:
    SnakeWindow(QWidget *pParent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    void handleTimer();
    void spawnFruit();

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *) override;

private slots:
    void addWall();
    void removeWall();

private:
    Position fruitPosition;
    QLabel *scoreLabel;
    QLabel *highscoreLabel;
    QLabel *levelLabel;

    void updateScoreLabel();
    void updateHighScoreLabel();
    void updateTimerInterval();
    void updateLevelLabel();

private slots:
    void togglePause();
private:
    bool isPaused;
    QPushButton *btnPause;
};

#endif // SNAKEWINDOW_HPP
