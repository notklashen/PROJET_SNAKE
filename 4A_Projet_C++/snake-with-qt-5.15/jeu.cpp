#include <iostream>
#include <cassert>
#include "jeu.hpp"
#include <fstream>

using namespace std;

Position::Position()
{
}

Position::Position(int a, int b)
{
    x=a;
    y=b;
}

bool Position::operator==(const Position &pos) const
{
    return (x==pos.x && y==pos.y);
}

bool Position::operator!=(const Position &pos) const
{
    return (x!=pos.x || y!=pos.y);
}

Jeu::Jeu()
{
    terrain = nullptr;
    largeur = 0; hauteur = 0;
    dirSnake = DROITE;
}

Jeu::Jeu(const Jeu &jeu):snake(jeu.snake)
{
    largeur = jeu.largeur;
    hauteur = jeu.hauteur;
    dirSnake = jeu.dirSnake;

    if (jeu.terrain!=nullptr)
    {
        terrain = new Case[largeur*hauteur];
        for (int c=0; c<largeur*hauteur; c++)
            terrain[c] = jeu.terrain[c];
    }
    else
        terrain = nullptr;
}

Jeu::~Jeu()
{
    if (terrain!=nullptr)
        delete[] terrain;
}

Jeu &Jeu::operator=(const Jeu &jeu)
{
    if (terrain!=nullptr)
        delete[] terrain;

    largeur = jeu.largeur;
    hauteur = jeu.hauteur;
    dirSnake = jeu.dirSnake;
    snake = jeu.snake;

    if (jeu.terrain!=nullptr)
    {
        terrain = new Case[largeur*hauteur];
        for (int c=0; c<largeur*hauteur; c++)
            terrain[c] = jeu.terrain[c];
    }
    else
        terrain = nullptr;
    return *this;
}

bool Jeu::init()
{
    int x, y;

    const char terrain_defaut[15][21] = {
        "####..##############",
        "#........##........#",
        "#.#####..##...####.#",
        "#........##........#",
        "#..................#",
        "#..................#",
        "....................",
        "....................",
        "....................",
        "....................",
        "#..................#",
        "#..................#",
        "#.....#......#.....#",
        "#.....#......#.....#",
        "####..##############"
    };

    largeur = 20;
    hauteur = 15;

    terrain = new Case[largeur * hauteur];

    for (y = 0; y < hauteur; ++y)
        for (x = 0; x < largeur; ++x)
            if (terrain_defaut[y][x] == '#')
                terrain[y * largeur + x] = MUR;
            else
                terrain[y * largeur + x] = VIDE;

    dirSnake = DROITE;
    score = 0; // Reset the score to 0
    loadHighScore();
    resetLevel(); // Reset the level when the game starts
    int longueurSerpent = 5;
    snake.clear();

    Position posTete;
    posTete.x = 15;
    posTete.y = 8;
    for (int i = 0; i < longueurSerpent; i++)
    {
        snake.push_back(posTete);
        posTete.x--;
    }

    return true;
}

void Jeu::evolue() {
    Position posTest;
    list<Position>::iterator itSnake;

    int depX[] = {-1, 1, 0, 0};
    int depY[] = {0, 0, -1, 1};

    posTest.x = snake.front().x + depX[dirSnake];
    posTest.y = snake.front().y + depY[dirSnake];

    // Wrap around if the snake reaches the edge of the game board
    if (posTest.x < 0) {
        posTest.x = largeur - 1; // Wrap to the right edge
    } else if (posTest.x >= largeur) {
        posTest.x = 0; // Wrap to the left edge
    }
    if (posTest.y < 0) {
        posTest.y = hauteur - 1; // Wrap to the bottom edge
    } else if (posTest.y >= hauteur) {
        posTest.y = 0; // Wrap to the top edge
    }

    if (posValide(posTest)) {
        snake.pop_back();
        snake.push_front(posTest);
    }
}

int Jeu::getNbCasesX() const
{
    return largeur;
}

int Jeu::getNbCasesY() const
{
    return hauteur;
}

Case Jeu::getCase(const Position &pos) const
{
    assert(pos.x>=0 && pos.x<largeur && pos.y>=0 && pos.y<hauteur);
    return terrain[pos.y*largeur+pos.x];
}

const list<Position> &Jeu::getSnake() const
{
    return snake;
}

bool Jeu::posValide(const Position &pos) const {
    // Check if the position is within the boundaries of the game board
    if (pos.x >= 0 && pos.x < largeur && pos.y >= 0 && pos.y < hauteur) {
        // Check if the position is either empty or contains a fruit
        if (terrain[pos.y * largeur + pos.x] == VIDE || terrain[pos.y * largeur + pos.x] == FRUIT) {
            return true; // Position is valid
        } else if (terrain[pos.y * largeur + pos.x] == MUR) {
            return true; // Position is a wall, so it's a valid position for the snake to collide with
        } else {
            // Iterate over the snake's positions to check if the given position is occupied by the snake
            for (const auto &snakePos : snake) {
                if (snakePos == pos) {
                    return false; // Position is occupied by the snake
                }
            }
            return true; // Position is not occupied by the snake
        }
    }
    return false; // Position is out of bounds
}


void Jeu::setDirection(Direction dir)
{
    dirSnake = dir;
}

void Jeu::setCase(Position pos, Case type) {
    if (pos.x < 0 || pos.x >= largeur || pos.y < 0 || pos.y >= hauteur) {
        std::cerr << "Error: Attempting to set a position out of bounds: (" << pos.x << ", " << pos.y << ")" << std::endl;
        return;
    }
    terrain[pos.y * largeur + pos.x] = type;
}

void Jeu::increaseSnakeLength() {
    // Get the current tail position
    const Position &tailPosition = snake.back();

    // Add a new segment to the snake at the current tail position
    snake.push_back(tailPosition);
}

bool Jeu::hasCollision() const {
    // Get the position of the snake's head
    const Position &headPosition = snake.front();

    // Check if the head collides with a wall
    if (terrain[headPosition.y * largeur + headPosition.x] == MUR) {
        return true; // Collision with a wall detected
    }

    return false; // No collision with a wall
}

bool Jeu::hasSelfCollision() const {
    // Get the position of the snake's head
    const Position &headPosition = snake.front();

    // Check if the head collides with any part of its body
    for (auto it = ++snake.begin(); it != snake.end(); ++it) {
        if (*it == headPosition) {
            return true; // Collision with itself detected
        }
    }

    return false; // No collision with itself
}

Direction Jeu::getDirection() const {
    return dirSnake;
}

void Jeu::saveHighScore() {
    std::ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << highscore;
        outFile.close();
    } else {
        std::cerr << "Unable to open file for writing high score." << std::endl;
    }
}

void Jeu::loadHighScore() {
    std::ifstream inFile("highscore.txt");
    if (inFile.is_open()) {
        inFile >> highscore;
        inFile.close();
    } else {
        highscore = 0; // Default value if file doesn't exist
        std::cerr << "Unable to open file for reading high score, setting default to 0." << std::endl;
    }
}

void Jeu::resetLevel() {
    level = 1;
}

void Jeu::increaseLevel() {
    level++;
}

int Jeu::getLevel() const {
    return level;
}

