#ifndef JEU_HPP
#define JEU_HPP

#include <fstream>
#include <list>

enum Case { VIDE, MUR, FRUIT };
enum Direction { GAUCHE, DROITE, HAUT, BAS };

class Position {
public:
    int x, y;
    Position();
    Position(int, int);
    bool operator==(const Position &) const;
    bool operator!=(const Position &) const;
};

class Jeu {
protected:
    Case *terrain;
    int largeur, hauteur;
    std::list<Position> snake;
    Direction dirSnake;

public:
    Jeu();
    Jeu(const Jeu &);
    ~Jeu();
    Jeu &operator=(const Jeu &);

    bool init();
    void evolue();
    void increaseSnakeLength();

    // High score methods
    void saveHighScore();
    void loadHighScore();

    int getNbCasesX() const;
    int getNbCasesY() const;
    Case getCase(const Position &) const;
    const std::list<Position> &getSnake() const;
    bool posValide(const Position &) const;
    void setDirection(Direction);
    void setCase(const Position, Case);
    bool hasCollision() const;
    bool hasSelfCollision() const;
    Direction getDirection() const;

public:
    int score;
    int highscore;
    int level;

public:
    void resetLevel();
    void increaseLevel();
    int getLevel() const;
};

#endif
