#ifndef CREATORS_H
#define CREATORS_H

#include <QDialog>

namespace Ui {
class Creators;
}

class Creators : public QDialog
{
    Q_OBJECT

public:
    explicit Creators(QWidget *parent = nullptr);
    ~Creators();

private:
    Ui::Creators *ui = nullptr;
};

#endif // CREATORS_H
