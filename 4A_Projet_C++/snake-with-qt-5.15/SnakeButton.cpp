#include "SnakeButton.hpp"

void SnakeButton::keyPressEvent(QKeyEvent *event) {
    // Envoyez l'événement clavier au widget parent
    if (parent() != nullptr) {
        QCoreApplication::sendEvent(parent(), event);
    }
    else {
        // Appel au comportement par défaut si nécessaire
        QPushButton::keyPressEvent(event);
    }
}

