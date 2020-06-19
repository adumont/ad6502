#ifndef MLCD_H
#define MLCD_H

#include <QWidget>
#include "verilated.h"

extern vluint64_t main_time;

class MLcd : public QWidget
{
    Q_OBJECT
public:
    explicit MLcd(QWidget *parent = 0);

signals:

public slots:
    bool eval(unsigned char, unsigned char, bool, bool);

private slots:
    bool isBusy();
    void reset();

private:
    vluint64_t mtime;
    vluint64_t prev_main_time;
    vluint64_t busy_until;

    bool prev_clk;
    char* text;
    int curs; // cursor position
    bool mode;
};
#endif // MLCD_H
