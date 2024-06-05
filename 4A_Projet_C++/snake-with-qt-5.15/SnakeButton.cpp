#include "SnakeButton.hpp"

void SnakeButton::keyPressEvent(QKeyEvent *event) {
    // Envoyez l'�v�nement clavier au widget parent
    if (parent() != nullptr) {
        QCoreApplication::sendEvent(parent(), event);
    }
    else {
        // Appel au comportement par d�faut si n�cessaire
        QPushButton::keyPressEvent(event);
    }
}

