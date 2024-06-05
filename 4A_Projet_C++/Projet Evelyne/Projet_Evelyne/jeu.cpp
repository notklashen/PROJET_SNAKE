#include <iostream>
#include <cassert>
#include "jeu.hpp"

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

// constructeur par défaut : initialise variables du jeu
Jeu::Jeu() : isGameOver(false), fruitsManges(0), vitesse(400) // initialise gameOver, fruitsManges et vitesse
{
    terrain = nullptr;
    largeur = 0; hauteur = 0;
    dirSnake = DROITE;
	setEtat(Demarrage); // initialise l'état du jeu à Demarrage
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

// réinitialisation de l'état du jeu pour une nouvelle partie
bool Jeu::init(ModeJeu mode)
{
	this->modeJeu = mode; 
	fruitsManges = 0; // réinitialise le score pour la nouvelle par
	isGameOver = false; // pour que le jeu ne soit pas en état de gameOver au démarrage
	vitesse = 400; // réinitialise la vitesse = 400 ms
    dirSnake = DROITE; // réinitialisation de la direction du serpent à droite par défaut 
	
	snake.clear(); 
	
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

	// réinitialisation du terrain avec des murs et des espaces vides
	if (terrain != nullptr) {
		delete[] terrain; // libère l'espace mémoire si terrain était déjà alloué
	}
	terrain = new Case[largeur*hauteur];

    // parcourt le tableau de terrain par défaut pour initialiser le terrain du jeu
	int x, y;
	for(y=0;y<hauteur;++y)
		for(x=0;x<largeur;++x)
            if (terrain_defaut[y][x]=='#')
                terrain[y*largeur+x] = MUR;
            else
                terrain[y*largeur+x] = VIDE;

    // initialise la position du serpent au centre du terrain 
	int longueurSerpent = 5; // serpent de longueur 5 
    Position posTete; // position initiale tête 
    posTete.x = 15;
    posTete.y = 8; 
	for (int i=0; i<longueurSerpent; i++)
    {
        snake.push_back(posTete);
        posTete.x--;
    }
	
	genererFruit(); // place 1 premier fruit surle terrain 

	// En mode CHALLENGE, place une première bombe sur le terrain en zone libre
    if (modeJeu == CHALLENGE) {
        genererBombe();
    }
	 
    return true;
}

// pour faire évoluer l'état du jeu à chaque tick du timer
void Jeu::evolue() 
{
    Position nouvelleTete = snake.front();
    int depX[] = {-1, 1, 0, 0}; // Déplacement horizontal : gauche, droite
    int depY[] = {0, 0, -1, 1}; // Déplacement vertical : haut, bas

    // Calcule la position de la nouvelle tête en fonction de la direction du serpent
    nouvelleTete.x += depX[dirSnake];
    nouvelleTete.y += depY[dirSnake];
    
    Position prochaineTete = getNextHeadPosition(); // Calcule la nouvelle position de la tête

    // Vérifie si le serpent touche son propre corps
    for (const auto& segment : snake) {
        if (prochaineTete == segment) {
            isGameOver = true;
            return;
        }
    }
    
    // Fait apparaître le serpent de l'autre côté si nécessaire
    if (nouvelleTete.x < 0) nouvelleTete.x = largeur - 1;
    else if (nouvelleTete.x >= largeur) nouvelleTete.x = 0;
    if (nouvelleTete.y < 0) nouvelleTete.y = hauteur - 1;
    else if (nouvelleTete.y >= hauteur) nouvelleTete.y = 0;

    // Vérifie si la nouvelle tête touche un mur
    if (terrain[nouvelleTete.y * largeur + nouvelleTete.x] == MUR) {
        reduireSerpent();
        return; // Termine l'évolution pour ce tour
    }

    // Ajoute la nouvelle tête au début de la liste représentant le serpent
    snake.push_front(nouvelleTete);

    // Si le serpent mange un fruit
    if (serpentMangerFruit(nouvelleTete)) {
        genererFruit(); // Génère un nouveau fruit et ne retire pas le dernier élément (la queue grandit)
        if(modeJeu == CHALLENGE) {
            genererBombe(); // Génère une bombe uniquement en mode Challenge
            if(vitesse > 100) { // 100 ms = vitesse maximale 
                vitesse -= 50; // Réduit la vitesse (augmente la difficulté) uniquement en mode Challenge
            }
        }
    } else {
        // Si le serpent ne mange pas de fruit, retire le dernier élément pour simuler le mouvement
        snake.pop_back();
    }

    // Gérer la collision avec une bombe uniquement en mode Challenge
    if (modeJeu == CHALLENGE && terrain[nouvelleTete.y * largeur + nouvelleTete.x] == BOMBE) {
        isGameOver = true;
        return;
    }
}

// calcule et retourne la position de la tête du serpent au prochain mouvement 
Position Jeu::getNextHeadPosition()
{
    Position head = snake.front();
    switch (dirSnake) {
        // ajustent head.x pour se déplacer horizontalement
		case GAUCHE: head.x -= 1; break;
        case DROITE: head.x += 1; break;
        // ajustent head.y pour se déplacer verticalement
		case HAUT: head.y -= 1; break;
        case BAS: head.y += 1; break;
    }
    return head;
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

void Jeu::setCase(const Position& pos, Case type) {
    if(pos.x >= 0 && pos.x < largeur && pos.y >= 0 && pos.y < hauteur) {
        terrain[pos.y * largeur + pos.x] = type;
    }
}

const list<Position> &Jeu::getSnake() const
{
    return snake;
}

bool Jeu::posValide(const Position &pos) const // vérifie si 1 case = vide + pas occupée par le serpent
{
    if (pos.x>=0 && pos.x<largeur && pos.y>=0 && pos.y<hauteur // vérifie si la position donnée "pos" = à l'intérieur du terrain
        && (terrain[pos.y*largeur+pos.x]==VIDE)||terrain[pos.y*largeur+pos.x]==FRUIT) 
    {
        list<Position>::const_iterator itSnake; // itérateur "list" -> parcourir liste "snake" = posit° occupées par le serpent
        itSnake = snake.begin(); // initialisat° itérateur début liste "snake" 
        while (itSnake!=snake.end() && *itSnake!=pos)
            itSnake++;
        return (itSnake==snake.end());
    }
    else
        return false;
}

void Jeu::setDirection(Direction nouvelleDirection)
 {
    // empêche le serpent de faire demi-tour sur lui-même
    if ((dirSnake == GAUCHE && nouvelleDirection != DROITE) ||
        (dirSnake == DROITE && nouvelleDirection != GAUCHE) ||
        (dirSnake == HAUT && nouvelleDirection != BAS) ||
        (dirSnake == BAS && nouvelleDirection != HAUT)) {
        dirSnake = nouvelleDirection;
    }
}

void Jeu::ajoutMur()
{
    Position posMur;

    // trouve une case libre
    do {
        posMur.x = rand()%largeur;
        posMur.y = rand()%hauteur;
    } while (!posValide(posMur));
    terrain[posMur.y*largeur+posMur.x]=MUR;
}

void Jeu::suppressionMur()
{
    Position posMur;

    // trouve un mur
    do {
        posMur.x = rand()%largeur;
        posMur.y = rand()%hauteur;
    } while (terrain[posMur.y*largeur+posMur.x]!=MUR);
    terrain[posMur.y*largeur+posMur.x]=VIDE;
}

void Jeu::genererFruit() {
    int x, y;
    do {
        // // génère des coordonnées aléatoires pour le fruit jusqu'à ce qu'une case vide soit trouvée
		x = rand() % largeur;
        y = rand() % hauteur;
	// continue de chercher si la case n'est pas vide ou si elle est occupée par la tête du serpent
    } while (terrain[y*largeur + x] != VIDE || (x == snake.front().x && y == snake.front().y));
    
	// une fois une case vide trouvée, place un fruit à cette position
	terrain[y*largeur + x] = FRUIT;
}

bool Jeu::serpentMangerFruit(const Position& posTete) {
    if(terrain[posTete.y*largeur + posTete.x] == FRUIT) {
        terrain[posTete.y*largeur + posTete.x] = VIDE; // supprime le fruit du terrain
        fruitsManges++; // incrémente le compteur de fruits mangés
        return true;
    }
    return false;
}

bool Jeu::posDansTerrain(const Position& pos) const 
{
    return pos.x >= 0 && pos.x < largeur && pos.y >= 0 && pos.y < hauteur;
}

void Jeu::reduireSerpent() 
{
    const int tailleMinimale = 1; // taille minimale du serpent avant le game over
    if (snake.size() > tailleMinimale) {
        snake.pop_back(); // réduit la taille du serpent
    } else {
        isGameOver = true; // déclenche le game over
    }
}

bool Jeu::gameOver() const 
{
    return isGameOver; // retourne l'état de 'isGameOver'
}

void Jeu::genererBombe() {
    int x, y;
    do 
	{
        x = rand() % largeur;
        y = rand() % hauteur;
    } while ((terrain[y*largeur + x] != VIDE)|| (x == snake.front().x && y == snake.front().y)); // s'assure que la case est vide

    terrain[y*largeur + x] = BOMBE; // place une bombe sur le terrain
}

EtatJeu Jeu::getEtat() const 
{
    return etat;
}

void Jeu::setEtat(EtatJeu nouvelEtat) 
{
    etat = nouvelEtat;
}
