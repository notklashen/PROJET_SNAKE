#include <iostream>
#include "snakewindow.hpp"

using namespace std;

SnakeWindow::SnakeWindow(QWidget *pParent, Qt::WindowFlags flags):QFrame(pParent, flags)
{
    // Taille des cases en pixels
    int largeurCase, hauteurCase;
	
	// Demander à l'utilisateur de choisir le mode de jeu
    ModeJeu modeChoisi = demanderModeJeu();

    // Initialisez le jeu avec le mode choisi
	jeu.init(modeChoisi);

    // chargement des ressources graphiques pour les éléments du jeu 
	
	if (pixmapCorps.load("./data/snake_corps.png")==false)
    {
        cout<<"Impossible d'ouvrir snake_corps.png"<<endl;
        exit(-1);
    }

    if (pixmapTete.load("./data/snake_tete.png")==false)
    {
        cout<<"Impossible d'ouvrir snake_tete.png"<<endl;
        exit(-1);
    }

    if (pixmapMur.load("./data/mur.bmp")==false)
    {
        cout<<"Impossible d'ouvrir mur.bmp"<<endl;
        exit(-1);
    }
	
	if (pixmapPeche.load("./data/peche.png")==false) {
        cout << "Impossible d'ouvrir peche.png" << endl;
        exit(-1);
    }
	
	if (pixmapBombe.load("./data/bombe.png") == false) {
    cout << "Impossible d'ouvrir bombe.png" << endl;
    exit(-1);
	}
	
	// création et configuration du timer principal du jeu -> contrôler les mises à jour
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &SnakeWindow::handleTimer); 

    //bouton ajouter mur
	SnakeButton *btnAjout = new SnakeButton(this);
    btnAjout->setFixedSize(100, 25);
    btnAjout->setText("Ajout mur");
    btnAjout->move(10, 10);

    //bouton supprimer mur
	SnakeButton *btnSuppr = new SnakeButton(this);
    btnSuppr->setFixedSize(100, 25);
    btnSuppr->setText("Suppr mur");
    btnSuppr->move(120, 10);
	
	//boutn recommencer
	SnakeButton *btnRecommencer = new SnakeButton(this);
    btnRecommencer->setFixedSize(100, 25);
    btnRecommencer->setText("Recommencer");
    btnRecommencer->move(270, 10);
	
	isPaused = false;
	
	//bouton pause
	SnakeButton *btnPause = new SnakeButton(this);
	btnPause->setFixedSize(100, 25);
	btnPause->setText("Pause");
	btnPause->move(380, 10); 
	
	// ajout du bouton de changement de style du serpent
	SnakeButton *btnChangeStyle = new SnakeButton(this);
	btnChangeStyle->setFixedSize(180, 25); // taille du bouton
	btnChangeStyle->setText("Changer Style Serpent"); // Corrigez ceci pour affecter le bon bouton
	btnChangeStyle->move(120, 45); // position du bouton
	
	// bouton instructions
	SnakeButton *btnInstructions = new SnakeButton(this);
	btnInstructions->setFixedSize(100, 25);
	btnInstructions->setText("Instructions");
	btnInstructions->move(10, 45);
	
	// bouton quitter
	SnakeButton *btnQuit = new SnakeButton(this);
	btnQuit->setFixedSize(100, 25);
	btnQuit->setText("Quitter");
	btnQuit->move(490, 10);

    // connecte les boutons à leur slots respectifs 
    connect(btnInstructions, &QPushButton::clicked, this, &SnakeWindow::showInstructions);
    connect(btnQuit, &QPushButton::clicked, this, &SnakeWindow::quitApplication);
	connect(btnPause, &QPushButton::clicked, this, &SnakeWindow::handleButtonPause);
    connect(btnAjout, &QPushButton::clicked, this, &SnakeWindow::handleButtonAjout);
    connect(btnSuppr, &QPushButton::clicked, this, &SnakeWindow::handleButtonSuppr);
	connect(btnRecommencer, &QPushButton::clicked, this, &SnakeWindow::handleButtonRecommencer);
	connect(btnChangeStyle, &QPushButton::clicked, this, &SnakeWindow::handleChangeStyle);
	
    largeurCase = pixmapMur.width();
    hauteurCase = pixmapMur.height();

    decalageY = 80;
    resize(jeu.getNbCasesX()*largeurCase, jeu.getNbCasesY()*hauteurCase+decalageY); 
}

void SnakeWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    
    Position pos;
    
    // Taille des cases en pixels
    int largeurCase, hauteurCase;

    largeurCase = pixmapMur.width();
    hauteurCase = pixmapMur.height();

    // Dessine les cases
    for (pos.y=0;pos.y<jeu.getNbCasesY();pos.y++)
        for (pos.x=0;pos.x<jeu.getNbCasesX();pos.x++)
			if (jeu.getCase(pos)==MUR)
                painter.drawPixmap(pos.x*largeurCase, pos.y*hauteurCase+decalageY, pixmapMur);

    // Dessine le serpent
    const list<Position> &snake = jeu.getSnake();
    if (!snake.empty())
    {
        list<Position>::const_iterator itSnake;
        const Position &posTete = snake.front();
        painter.drawPixmap(posTete.x*largeurCase, posTete.y*hauteurCase+decalageY, pixmapTete);

        for (itSnake=++snake.begin(); itSnake!=snake.end(); itSnake++)
            painter.drawPixmap(itSnake->x*largeurCase, itSnake->y*hauteurCase+decalageY, pixmapCorps);
    }
	
	// Dessine les fruits
    for (int y = 0; y < jeu.getNbCasesY(); y++) {
        for (int x = 0; x < jeu.getNbCasesX(); x++) {
            Position pos(x, y);
            if (jeu.getCase(pos) == FRUIT) {
                painter.drawPixmap(x * largeurCase, y * hauteurCase + decalageY, pixmapPeche);
            }
        }
    }
	
	// Dessine les bombes	
	for (int y = 0; y < jeu.getNbCasesY(); y++) {
		for (int x = 0; x < jeu.getNbCasesX(); x++) {
			Position pos(x, y);
			if (jeu.getCase(pos) == BOMBE) {
				painter.drawPixmap(x * largeurCase, y * hauteurCase + decalageY, pixmapBombe);
			}
		}
	}

	if (isPaused) 
	{
        // Configuration pour le texte de pause
        QFont font("Arial", 20, QFont::Bold);
        painter.setFont(font);
        painter.setPen(Qt::red);
        QRect rect = this->rect();
        QRect pauseRect(rect.width() / 4, rect.height() / 2 - 50, rect.width() / 2, 100);
        painter.drawText(pauseRect, Qt::AlignCenter, "Jeu en Pause");
    }
	
	 // Dessine le serpent avec les nouvelles formes
	 
	 // Vérifie si le serpent n'est pas vide
    if (!snake.empty()) {
        // Dessine la tête du serpent avec son image spécifique
        const Position& posTete = snake.front();
        painter.drawPixmap(posTete.x * largeurCase, posTete.y * hauteurCase + decalageY, pixmapTete);

        // Définit la couleur de remplissage pour le corps
        painter.setBrush(QBrush(corpsColors[styleIndex]));

        // Commence à dessiner le corps du serpent à partir du second élément
        auto itSnake = ++snake.begin();
        for (; itSnake != snake.end(); ++itSnake) {
            QRect rect(itSnake->x * largeurCase, itSnake->y * hauteurCase + decalageY, largeurCase, hauteurCase);
            switch (styleIndex) {
                case 0: // Ellipse
                    painter.drawEllipse(rect);
                    break;
				case 1: // Rectangle
                    painter.drawRect(rect);
                    break;
                case 2: // Rond barré (Cercle + Ligne)
                    painter.drawEllipse(rect);
                    painter.drawLine(rect.topLeft(), rect.bottomRight());
                    painter.drawLine(rect.bottomLeft(), rect.topRight());
                    break;
            }
        }	
    }

	// Dessine l'écran de démarrage
	if (jeu.getEtat() == Demarrage) 
	{
        // Configuration pour le texte de démarrage
        QFont font("Arial", 8, QFont::Bold);
        painter.setFont(font);
        painter.setPen(Qt::blue);
        QRect rect = this->rect();
        QRect textRect(rect.width() / 4, rect.height() / 2 - 50, rect.width() / 2, 100);
        painter.drawText(textRect, Qt::AlignCenter, "Appuyez sur Entrée (ou retour à la ligne) pour commencer"); // touche entrée / retour à la igne suivant le type d'ordinateur 

        return; 
    }
} 

// gest° des commandes du joueur via les touches du clavier
void SnakeWindow::keyPressEvent(QKeyEvent *event) {
    if (jeu.getEtat() == Demarrage && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) 
	{
        jeu.setEtat(EnCours);
        gameTimer->start(jeu.vitesse); // Démarrer le jeu
    } else if (jeu.getEtat() == EnCours) 
	{ // Assurez-vous de gérer les entrées seulement si le jeu est en cours
        if (event->key() == Qt::Key_Left)
            jeu.setDirection(GAUCHE);
        else if (event->key() == Qt::Key_Right)
            jeu.setDirection(DROITE);
        else if (event->key() == Qt::Key_Up)
            jeu.setDirection(HAUT);
        else if (event->key() == Qt::Key_Down)
            jeu.setDirection(BAS);
    }
    update(); // Force le redessin de la fenêtre après la gestion de l'événement
}

void SnakeWindow::handleTimer() {
    jeu.evolue(); // mise à jour de l'état du jeu
    gameTimer->setInterval(jeu.vitesse); // ajuste la vitesse du timer

    if (jeu.gameOver()) {
        gameTimer->stop(); // arrête le timer si le jeu est terminé

        // Construit le message de Game Over avec le score basé sur le nombre de fruits mangés
        QString messageGameOver = QString("Le jeu est terminé. Votre score est : %1. Voulez-vous recommencer ?")
                                  .arg(jeu.getFruitsManges()); // Utilisez getFruitsManges pour récupérer le score

        int reponse = QMessageBox::question(this, "Game Over", messageGameOver, QMessageBox::Yes | QMessageBox::No);
        
        if (reponse == QMessageBox::Yes) {
            ModeJeu modeChoisi = demanderModeJeu(); // Demande à nouveau le mode de jeu pour la nouvelle partie
            jeu.init(modeChoisi); // Réinitialise le jeu avec le mode choisi
            gameTimer->start(jeu.vitesse);
        } else {
            this->close(); // Ferme la fenêtre de jeu
        }
    } else {
        update(); // Force le redessin pour afficher l'état actuel du jeu
    }
}


void SnakeWindow::handleButtonAjout()
{
    jeu.ajoutMur();
    update();
}

void SnakeWindow::handleButtonSuppr()
{
    jeu.suppressionMur();
    update();
}

void SnakeWindow::handleButtonRecommencer() 
{
    gameTimer->stop(); // arrête le timer du jeu pour éviter des évolutions pendant la réinitialisation
    
    // réinitialise l'état du jeu pour une nouvelle partie
    ModeJeu modeChoisi = demanderModeJeu(); // demande au joueur de choisir à nouveau le mode
    jeu.init(modeChoisi); // réinitialise le jeu avec le nouveau mode choisi
    
    // s'assure que le jeu n'est pas en état de pause
    isPaused = false;
    
    // redémarre le jeu avec le nouveau mode et réinitialise le timer avec la vitesse initiale
    gameTimer->start(jeu.vitesse);
    
    update(); // force le redessin pour mettre à jour l'affichage
}


void SnakeWindow::handleButtonPause()
{
    if (gameTimer->isActive()) 
	{
        gameTimer->stop(); // met le jeu en pause
        isPaused = true; // mise à jour de l'état de pause
    } else 
	{
        gameTimer->start(jeu.vitesse); // reprend le jeu
        isPaused = false; // le jeu n'est plus en pause
    }
    update(); // force le redessin pour afficher/masquer le message de pause
}

void SnakeWindow::handleChangeStyle() 
{
    styleIndex = (styleIndex + 1) % 4; // Change le style pour le prochain dessin
    update(); // Force le redessin pour afficher le nouveau style
}


// Les slots pour gérer les actions des boutons
void SnakeWindow::handleNewGame() 
{
    jeu.init(ModeJeu::ZEN); // initialise et démarre un nouveau jeu
    menuWidget->hide(); // cache le menu lorsqu'un nouveau jeu commence
}

void SnakeWindow::showInstructions() 
{
	 // vérifie si le jeu est en cours et le met en pause
    if (jeu.getEtat() == EnCours && !isPaused) 
	{
        gameTimer->stop(); // met le jeu en pause
        isPaused = true; // mise à jour de l'état de pause
    }
	
	// affichage de la fenêtre d'instructions
    QMessageBox::information(this, "Instructions", "Utilisez les flèches du clavier pour déplacer le serpent.\nMangez des fruits pour marquer des points.\nÉvitez les murs et les bombes ! Si le serpent se mord la queue, c'est perdu ! ");

	// reprise du jeu après que l'utilisateur ait cliqué sur OK
    if (isPaused) 
	{
        gameTimer->start(jeu.vitesse); // reprend le jeu
        isPaused = false; // le jeu n'est plus en pause
    }
}


void SnakeWindow::quitApplication() 
{
    QApplication::quit(); // quitte l'application
}

ModeJeu SnakeWindow::demanderModeJeu() 
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Sélectionnez le Mode de Jeu");
    msgBox.setText("Choisissez votre mode de jeu :");
    QPushButton *zenButton = msgBox.addButton("Zen", QMessageBox::AcceptRole);
    QPushButton *challengeButton = msgBox.addButton("Challenge", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == zenButton) {
        return ModeJeu::ZEN;
    } else if (msgBox.clickedButton() == challengeButton) {
        return ModeJeu::CHALLENGE;
    }
	
    return ModeJeu::ZEN; // Mode par défaut
}


