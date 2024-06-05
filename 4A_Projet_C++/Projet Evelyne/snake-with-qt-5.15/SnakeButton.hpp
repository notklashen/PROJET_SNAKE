#ifndef SNAKEBUTTON_HPP
#define SNAKEBUTTON_HPP

#include <QPushButton>
#include <QKeyEvent>
#include <QCoreApplication>

class SnakeButton : public QPushButton {
public:
    using QPushButton::QPushButton; // Utilisez le constructeur de QPushButton

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // SNAKEBUTTON_HPP

