#include <iostream>
#include <cstdlib>
#include "snakewindow.hpp"

using namespace std;

SnakeWindow::SnakeWindow(QWidget *pParent, Qt::WindowFlags flags) : QFrame(pParent, flags)
{
    int largeurCase, hauteurCase;

    if (!pixmapCorps.load("./data/snake_corps.png")) {
        cerr << "Impossible d'ouvrir snake_corps.png" << endl;
        exit(-1);
    }

    if (!pixmapTete.load("./data/snake_tete.png")) {
        cerr << "Impossible d'ouvrir snake_tete.png" << endl;
        exit(-1);
    }

    if (!pixmapMur.load("./data/mur.bmp")) {
        cerr << "Impossible d'ouvrir mur.bmp" << endl;
        exit(-1);
    }

    if (!pixmapFruit.load("./data/apple1.png")) {
        cerr << "Impossible d'ouvrir apple.png" << endl;
        exit(-1);
    }

    if (!pixmapTerre.load("./data/terre.png")) {
        cerr << "Impossible d'ouvrir terre.png" << endl;
        exit(-1);
    }

    jeu.init();
    spawnFruit();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SnakeWindow::handleTimer);
    timer->start(100);

    largeurCase = pixmapMur.width();
    hauteurCase = pixmapMur.height();

    resize(jeu.getNbCasesX() * largeurCase, jeu.getNbCasesY() * hauteurCase + 50);

    btnAddWall = new SnakeButton(this);
    btnAddWall->setText("Ajout mur");
    btnAddWall->setGeometry(10, 10, 100, 30);
    connect(btnAddWall, &QPushButton::clicked, this, &SnakeWindow::addWall);

    btnRemoveWall = new SnakeButton(this);
    btnRemoveWall->setText("Suppr mur");
    btnRemoveWall->setGeometry(130, 10, 120, 30);
    connect(btnRemoveWall, &QPushButton::clicked, this, &SnakeWindow::removeWall);

    scoreLabel = new QLabel(this);
    scoreLabel->setText("Score: 0");
    scoreLabel->setGeometry(500, 10, 120, 30);

    highscoreLabel = new QLabel(this);
    highscoreLabel->setText("Highscore: " + QString::number(jeu.highscore));
    highscoreLabel->setGeometry(550, 10, 120, 30);

    levelLabel = new QLabel(this);
    levelLabel->setText("Level: 1");
    levelLabel->setGeometry(450, 10, 120, 30);

    btnPause = new SnakeButton(this);
    btnPause->setText("Pause");
    btnPause->setGeometry(270, 10, 100, 30);
    connect(btnPause, &QPushButton::clicked, this, &SnakeWindow::togglePause);
}

void SnakeWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int largeurCase = pixmapMur.width();
    int hauteurCase = pixmapMur.height();

    for (int y = 0; y < jeu.getNbCasesY(); y++) {
        for (int x = 0; x < jeu.getNbCasesX(); x++) {
            Position pos(x, y);
            switch (jeu.getCase(pos)) {
                case MUR:
                    painter.drawPixmap(x * largeurCase, y * hauteurCase + 50, pixmapMur);
                    break;
                case FRUIT:
                    painter.drawPixmap(x * largeurCase, y * hauteurCase + 50, pixmapFruit);
                    break;
                case VIDE:
                    painter.drawPixmap(x * largeurCase, y * hauteurCase + 50, pixmapTerre);
                    break;
                default:
                    break;
            }
        }
    }

    const list<Position>& snake = jeu.getSnake();
    if (!snake.empty()) {
        auto itSnake = snake.begin();
        const Position& posTete = snake.front();
        painter.drawPixmap(posTete.x * largeurCase, posTete.y * hauteurCase + 50, pixmapTete);

        for (++itSnake; itSnake != snake.end(); ++itSnake)
            painter.drawPixmap(itSnake->x * largeurCase, itSnake->y * hauteurCase + 50, pixmapCorps);
    }
}

void SnakeWindow::keyPressEvent(QKeyEvent *event)
{
    Direction currentDirection = jeu.getDirection();
    Direction newDirection = currentDirection;

    if (event->key() == Qt::Key_Left && currentDirection != DROITE)
        newDirection = GAUCHE;
    else if (event->key() == Qt::Key_Right && currentDirection != GAUCHE)
        newDirection = DROITE;
    else if (event->key() == Qt::Key_Up && currentDirection != BAS)
        newDirection = HAUT;
    else if (event->key() == Qt::Key_Down && currentDirection != HAUT)
        newDirection = BAS;

    if (newDirection != currentDirection) {
        jeu.setDirection(newDirection);
    }

    update();
}

void SnakeWindow::handleTimer()
{
    jeu.evolue();

    if (jeu.hasCollision()) {
        QMessageBox::information(this, "Game Over", "You hit a wall! Game Over.");
        jeu.saveHighScore();
        jeu.init();
        spawnFruit();
        updateScoreLabel();
        updateHighScoreLabel();
        timer->start(100);
        jeu.resetLevel();
        updateLevelLabel();
        return;
    }

    if (jeu.hasSelfCollision()) {
        QMessageBox::information(this, "Game Over", "You collided with yourself! Game Over.");
        jeu.saveHighScore();
        jeu.init();
        spawnFruit();
        updateScoreLabel();
        updateHighScoreLabel();
        timer->start(100);
        jeu.resetLevel();
        updateLevelLabel();
        return;
    }

    const Position& headPosition = jeu.getSnake().front();
    if (headPosition == fruitPosition) {
        spawnFruit();
        jeu.increaseSnakeLength();
        jeu.score += 10;
        updateScoreLabel();

        if (jeu.score > jeu.highscore) {
            jeu.highscore = jeu.score;
            updateHighScoreLabel();
        }

        if (jeu.score % 100 == 0) {
            jeu.increaseLevel();
            updateLevelLabel();
            updateTimerInterval();
        }
    }

    update();
}

void SnakeWindow::updateTimerInterval()
{
    int newInterval = 100 - ((jeu.getLevel() - 1) * 10);
    if (newInterval < 30) {
        newInterval = 30;
    }
    timer->setInterval(newInterval);
}

void SnakeWindow::addWall()
{
    int x, y;
    bool positionOk = false;

    for (int i = 0; i < 100; ++i) {
        x = rand() % jeu.getNbCasesX();
        y = rand() % jeu.getNbCasesY();

        Position newWallPosition(x, y);

        if (jeu.getCase(newWallPosition) == VIDE) {
            const list<Position>& snake = jeu.getSnake();
            positionOk = true;
            for (const Position& part : snake) {
                if (part == newWallPosition) {
                    positionOk = false;
                    break;
                }
            }
            if (positionOk) {
                jeu.setCase(newWallPosition, MUR);
                update();
                return;
            }
        }
    }
}

void SnakeWindow::removeWall()
{
    vector<Position> wallsToRemove;

    for (int y = 0; y < jeu.getNbCasesY(); ++y) {
        for (int x = 0; x < jeu.getNbCasesX(); ++x) {
            if (jeu.getCase(Position(x, y)) == MUR) {
                wallsToRemove.push_back(Position(x, y));
            }
        }
    }

    if (!wallsToRemove.empty()) {
        int index = rand() % wallsToRemove.size();
        Position wallToRemove = wallsToRemove[index];
        jeu.setCase(wallToRemove, VIDE);
        update();
    }
}

void SnakeWindow::spawnFruit()
{
    jeu.setCase(fruitPosition, VIDE);

    int x, y;
    bool positionOk;

    do {
        x = rand() % jeu.getNbCasesX();
        y = rand() % jeu.getNbCasesY();
        Position newFruitPosition(x, y);

        positionOk = (jeu.getCase(newFruitPosition) == VIDE);

        if (positionOk) {
            const list<Position>& snake = jeu.getSnake();
            for (const Position& part : snake) {
                if (part == newFruitPosition) {
                    positionOk = false;
                    break;
                }
            }
        }
    } while (!positionOk);

    fruitPosition = Position(x, y);
    jeu.setCase(fruitPosition, FRUIT);
}

void SnakeWindow::updateScoreLabel()
{
    scoreLabel->setText("Score: " + QString::number(jeu.score));
}

void SnakeWindow::updateHighScoreLabel()
{
    highscoreLabel->setText("Highscore: " + QString::number(jeu.highscore));
}

void SnakeWindow::updateLevelLabel()
{
    levelLabel->setText("Level: " + QString::number(jeu.getLevel()));
}

void SnakeWindow::togglePause()
{
    if (isPaused) {
        timer->start();
        btnPause->setText("Pause");
    } else {
        timer->stop();
        btnPause->setText("Resume");
    }
    isPaused = !isPaused;
}
