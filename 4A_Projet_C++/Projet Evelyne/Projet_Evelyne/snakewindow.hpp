#ifndef SNAKEWINDOW_HPP
#define SNAKEWINDOW_HPP

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "jeu.hpp"

class SnakeWindow : public QFrame
{
  protected:
    Jeu jeu;
    QPixmap pixmapCorps, pixmapTete, pixmapMur, pixmapPeche, pixmapBombe;
	QColor serpentColor;
    QString serpentForme;
    int decalageY;

  public:
    SnakeWindow(QWidget *pParent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
	ModeJeu demanderModeJeu();

  public slots:
	void handleChangeStyle();

  private:
	QTimer* gameTimer;
	bool isPaused;
	QWidget *menuWidget; // conteneur pour les boutons du menu
    QPushButton *btnNewGame;
    QPushButton *btnInstructions;
    QPushButton *btnQuit;
    QVBoxLayout *menuLayout; // layout pour organiser les boutons

  protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);
    void handleTimer();
    void handleButtonAjout();
    void handleButtonSuppr();
	void handleButtonRecommencer();
	void handleButtonPause();
	void afficherMenuDemarrage();
	void handleNewGame();
    void showInstructions();
    void quitApplication();

    int styleIndex = 0; // indice pour parcourir les styles de dessin
    QVector<QColor> corpsColors = {Qt::red, Qt::green, Qt::blue, Qt::yellow}; // Couleurs du corps
};

class SnakeButton : public QPushButton
{
  public:
    SnakeButton(QWidget *pParent=0):QPushButton(pParent) {}

  protected:
    void keyPressEvent(QKeyEvent *e)
    {
        if (parent()!=NULL)
            QCoreApplication::sendEvent(parent(), e);
    }
};

#endif
