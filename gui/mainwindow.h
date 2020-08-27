#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QCheckBox>
#include <QFontDatabase>
#include "Vtop.h"
#include "verilated_save.h"
#include "verilated_vcd_c.h"

extern vluint64_t main_time;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void clkTick();

    void on_pbB_toggled(bool checked);

    void on_pbA_pressed();

    void on_clkPeriod_valueChanged(int period);

    void on_pbReset_toggled(bool checked);

    void on_pbSave_pressed();

    void on_pbPUSH_toggled(bool checked);

    void on_pbPOP_toggled(bool checked);

    void on_pbLoad_pressed();

    void on_pbLoadPROG_pressed();

    void on_actionLoad_Program_triggered();

    void on_actionExit_triggered();

    void on_pbINSTR_pressed();

    void on_pbHold_toggled(bool checked);

    void on_any_gpio_in_toggled();

    void on_mem_page_valueChanged(int arg1);

    void on_radioRam_toggled(bool checked);

    void on_radioRom_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QTimer *m_timer;
    bool clk;
    int counter;
    unsigned char mem_page_high = 0; // 0: ram, f: rom
    unsigned char mem_page_low  = 0; // 0 to f
    unsigned int  mem_base_addr = 0;
    QString bgColor;

    Vtop * top;
    VerilatedVcdC* tfp;

    vluint64_t ttr_pbPUSH; // time to release pbPUSH
    vluint64_t ttr_pbPOP;  // time to release pbPOP

    QStringList m_TableHeader;

    void updateUI();
    void LoadProgramFromFile(QString fileName);

    QString formatData(CData);
    QString formatData(SData);
    QString verilatorString(WData[]);
    QString verilatorString(QData&);

    void highlightLabel(QWidget*, bool);

    void topEval();

    unsigned char gpio_in = 0 ;
    QList<QCheckBox*> gpio_in_list;

    unsigned char instr[255];
    unsigned char mode[255];
    const char *modeName[12];
    const char *instrName[56];

};

#endif // MAINWINDOW_H
