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
	EtatJeu etat; // stocke l'�tat actuel du jeu
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

    // Retourne la case � une position donn�e
    Case getCase(const Position &) const;
	// D�finit le type de case � la position donn�e
	void setCase(const Position& pos, Case type);

    // Retourne la liste des �l�ments du serpent en lecture seule
    const std::list<Position> &getSnake() const;

    // Indique si la case � une position donn�e existe et est libre
    bool posValide(const Position &) const;

    // Modifie la direction
    void setDirection(Direction);

    // ajouter et supprimer mur
	void ajoutMur();
    void suppressionMur();
	
	// fruits
	void genererFruit(); // g�n�re 1 fruit � 1 position al�atoire
	// bool serpentMangerFruit(); // v�rifie si le serpent mange le fruit
	bool serpentMangerFruit(const Position& posTete);
	int getFruitsManges() const { return fruitsManges; } // retourne la valeur de "fruitsManges"  

	bool posDansTerrain(const Position& pos) const;

	// r�duit serpent 
	void reduireSerpent(); 
	
	// m�thode pour acc�der � l'�tat jeu 
	bool gameOver() const; 
	
	// g�n�re bombes
	void genererBombe(); 
	
	// M�thode pour obtenir l'�tat du jeu
    EtatJeu getEtat() const;
    // M�thode pour d�finir l'�tat du jeu
    void setEtat(EtatJeu); 
	
	Position getNextHeadPosition();  
	
};

#endif
