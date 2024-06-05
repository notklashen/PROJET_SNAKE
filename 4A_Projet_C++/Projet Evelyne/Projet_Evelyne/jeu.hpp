#ifndef JEU_HPP
#define JEU_HPP

#include <list>

typedef enum {VIDE, MUR, FRUIT, BOMBE} Case;
typedef enum {GAUCHE, DROITE, HAUT, BAS} Direction;
typedef enum {Demarrage, EnCours, GameOver} EtatJeu;
typedef enum {ZEN, CHALLENGE} ModeJeu; 


class Position
{
  public:
    int x, y;
    Position();
    Position(int,int);
    bool operator==(const Position &) const;
    bool operator!=(const Position &) const;
};

class Jeu
{
  protected:
    Case *terrain;
    int largeur, hauteur; // Nombre de cases en largeur et en hauteur
    std::list<Position> snake;
    Direction dirSnake;
	
  private: 
	bool isGameOver;
	int fruitsManges; 
	EtatJeu etat; // stocke l'état actuel du jeu
	ModeJeu modeJeu; 

  public:
    Jeu();
    Jeu(const Jeu &);
    ~Jeu();

    Jeu &operator=(const Jeu &);

    bool init(ModeJeu); 
    void evolue();
	int vitesse; // en millisecondes
	
    // Retourne les dimensions (en nombre de cases)
    int getNbCasesX() const;
    int getNbCasesY() const;

    // Retourne la case à une position donnée
    Case getCase(const Position &) const;
	// Définit le type de case à la position donnée
	void setCase(const Position& pos, Case type);

    // Retourne la liste des éléments du serpent en lecture seule
    const std::list<Position> &getSnake() const;

    // Indique si la case à une position donnée existe et est libre
    bool posValide(const Position &) const;

    // Modifie la direction
    void setDirection(Direction);

    // ajouter et supprimer mur
	void ajoutMur();
    void suppressionMur();
	
	// fruits
	void genererFruit(); // génère 1 fruit à 1 position aléatoire
	// bool serpentMangerFruit(); // vérifie si le serpent mange le fruit
	bool serpentMangerFruit(const Position& posTete);
	int getFruitsManges() const { return fruitsManges; } // retourne la valeur de "fruitsManges"  

	bool posDansTerrain(const Position& pos) const;

	// réduit serpent 
	void reduireSerpent(); 
	
	// méthode pour accéder à l'état jeu 
	bool gameOver() const; 
	
	// génère bombes
	void genererBombe(); 
	
	// Méthode pour obtenir l'état du jeu
    EtatJeu getEtat() const;
    // Méthode pour définir l'état du jeu
    void setEtat(EtatJeu); 
	
	Position getNextHeadPosition();  
	
};

#endif
