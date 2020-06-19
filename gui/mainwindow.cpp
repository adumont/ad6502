#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QTime>
#include <QApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

#include "Vtop.h"
#include "verilated_save.h"
#include "verilated_vcd_c.h"

//#include "Vtop_hrmcpu.h"
//#include "Vtop_PROG.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Clock Initialization
    clk = 0;

    bgColor = QString("background-color: rgb(128, 213, 255);");

    // Create our design model with Verilator
    top = new Vtop;

    // Verilated::internalsDump();  // See scopes to help debug

    // init trace dump
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    //tfp->spTrace()->set_time_resolution ("1 ps");
    tfp->open ("ad6502.vcd");

    topEval(); // first eval, will load initialize rom

    int k = QFontDatabase::addApplicationFont(":/font/assets/font/HD44780.ttf");

    QFont font = QFont("HD44780");

    ui->lcd->setFont(font);

    gpio_in_list<<ui->gpio_in0;
    gpio_in_list<<ui->gpio_in1;
    gpio_in_list<<ui->gpio_in2;
    gpio_in_list<<ui->gpio_in3;
    gpio_in_list<<ui->gpio_in4;
    gpio_in_list<<ui->gpio_in5;
    gpio_in_list<<ui->gpio_in6;
    gpio_in_list<<ui->gpio_in7;

    connect(ui->gpio_in0, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in1, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in2, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in3, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in4, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in5, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in6, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));
    connect(ui->gpio_in7, SIGNAL(stateChanged(int)), this, SLOT(on_any_gpio_in_toggled()));

    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(clkTick()));
    // m_timer->start( ui->clkPeriod->value() );

    mem_base_addr=256*(16*mem_page_high+mem_page_low);

    QStringList LIST;

    // RAM table, set headers
    LIST.clear();
    for(int i=0; i<16; i++){ LIST.append(QString("%1").arg( i,1,16,QChar('0'))); }
    ui->tblRAM->setVerticalHeaderLabels(LIST);
    LIST.clear();
    for(int i=0; i<16; i++){ LIST.append(QString("_%1").arg(i,1,16,QChar('0'))); }
    ui->tblRAM->setHorizontalHeaderLabels(LIST);

    // Initialize RAM table
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            ui->tblRAM->setItem(j,i,new QTableWidgetItem( formatData( top->top__DOT__ram_mem[mem_base_addr + 16*j+i] ) ));
//            ui->tblRAM->item(j, i)->setForeground(Qt::gray);
        }
    }
#ifdef KK
    for(int i=0; i<256; i++){ LIST.append(formatData(i)); }
    ui->tblPROG->setVerticalHeaderLabels(LIST);
    ui->tblPROG->setHorizontalHeaderLabels(QStringList("Data"));


    // PROG table, fill with current program
    for(int i=0; i<256; i++){
        ui->tblPROG->setItem(0,i,new QTableWidgetItem( formatData( top->hrmcpu__DOT__program0__DOT__rom[i] ) ));
    }

    // INBOX table
    for(int i=0; i<32; i++){
        ui->tblINBOX->setItem(i,0,new QTableWidgetItem( formatData( top->hrmcpu__DOT__INBOX__DOT__fifo[i] ) ));
        ui->tblINBOX->item(i, 0)->setForeground(Qt::gray);
    }
    ui->tblINBOX->setVerticalHeaderLabels(LIST.mid(0,32));

    // OUTBOX table
    for(int i=0; i<32; i++){
        ui->tblOUTBOX->setItem(i,0,new QTableWidgetItem( formatData( top->hrmcpu__DOT__OUTB__DOT__fifo[i] ) ));
        ui->tblOUTBOX->item(i, 0)->setForeground(Qt::gray);
    }
    ui->tblOUTBOX->setVerticalHeaderLabels(LIST.mid(0,32));


    ttr_pbPUSH = 0;

    // we force a clock cycle before we give control to the user
//    top->i_rst = true;
//    clkTick();
//    clkTick();
#endif
    top->i_rst = false;
    updateUI();
}

MainWindow::~MainWindow()
{
    // we close vcd:
    if(ui->actionGenerate_Trace->isChecked()) tfp->flush(); // any impact on perf? not relevant here
    tfp->close();

    delete ui;
}

void MainWindow::clkTick()
{
    clk = ! clk;
    main_time++;

    topEval();

    clk = ! clk;
    main_time++;

    topEval();

    updateUI();
}

void MainWindow::on_pbA_pressed()
{
    clkTick();
}

void MainWindow::on_pbB_toggled(bool checked)
{
    if(checked) {
        ui->pbA->setDisabled(true);
        ui->pbINSTR->setDisabled(true);
//        ui->pbReset->setDisabled(true);
        m_timer->start( ui->clkPeriod->value() );
    } else {
        m_timer->stop();
        ui->pbA->setEnabled(true);
        if(!ui->pbReset->isChecked())
           ui->pbINSTR->setEnabled(true);
//        ui->pbReset->setEnabled(true);

    }
}

void MainWindow::topEval()
{
    top->clk = clk;
    top->eval();
    if(ui->actionGenerate_Trace->isChecked()) {
        tfp->dump(main_time);
        tfp->flush();
    }
}

void MainWindow::updateUI()
{

    // update INPUTS before we EVAL()
    // for now, all input are assigned in their respective UI slots

#ifdef KK
    top->cpu_in_wr = ui->pbPUSH->isChecked();
    top->cpu_dmp_chip_select = ui->dmp_module->value();
    top->cpu_dmp_fifo_pos = ui->dmp_position->value();

//    int x;
//    std::stringstream ss;
//    ss << std::hex << ui->editINdata->text();
//    ss >> top->cpu_in_data;

    bool toUintSuccess;
    top->cpu_in_data = ui->editINdata->text().toUInt(&toUintSuccess,16); //ui->editINdata->text().toInt();
#endif

    // Control Block
    ui->clk->setState( clk );
    ui->main_time->setText( QString("%1").arg( main_time ) );
    ui->led_i_rst->setState(top->i_rst);

    // State Name
    ui->cu_statename->setText( verilatorString( top->top__DOT__CPU__DOT__statename ) );
    ui->led_cpu_reset->setState(top->top__DOT__CPU__DOT__reset);

    // Processor flags
    ui->P_flag->setText(formatData( top->top__DOT__CPU__DOT__P ));
    ui->led_C->setState(top->top__DOT__CPU__DOT__C);
    ui->led_Z->setState(top->top__DOT__CPU__DOT__Z);
    ui->led_I->setState(top->top__DOT__CPU__DOT__I);
    ui->led_D->setState(top->top__DOT__CPU__DOT__D);
    ui->led_V->setState(top->top__DOT__CPU__DOT__V);
    ui->led_N->setState(top->top__DOT__CPU__DOT__N);

    // PC
    ui->PC_PC->setText(formatData( top->top__DOT__CPU__DOT__PC));

    // Registers AXYS
    ui->R_A->setText(formatData( top->top__DOT__CPU__DOT__A ));
    ui->R_X->setText(formatData( top->top__DOT__CPU__DOT__X ));
    ui->R_Y->setText(formatData( top->top__DOT__CPU__DOT__Y ));
    ui->R_S->setText(formatData( top->top__DOT__CPU__DOT__S ));
//    ui->led_R_wR->setState( top->hrmcpu__DOT__register0__DOT__wR );
//    highlightLabel(ui->R_R, top->hrmcpu__DOT__register0__DOT__wR);

    // Chip Select leds
    ui->led_cs_ram->setState( top->top__DOT__cs_ram );
    ui->led_cs_rom->setState( top->top__DOT__cs_rom );
    ui->led_cs_gpio->setState( top->top__DOT__cs_gpio );
    ui->led_cs_acia->setState( top->top__DOT__cs_acia );

    // IR Instruction Register
    ui->IR_IR->setText(formatData( top->top__DOT__CPU__DOT__IR ));
    ui->IR_IRHOLD->setText(formatData( top->top__DOT__CPU__DOT__IRHOLD ));
    ui->led_IRHOLD_valid->setState( top->top__DOT__CPU__DOT__IRHOLD_valid );

    // Data & Address Buses
    ui->DI->setText(formatData( top->top__DOT__CPU__DOT__DI ));
    ui->DO->setText(formatData( top->top__DOT__CPU__DOT__DO ));
    ui->AB->setText(formatData( top->top__DOT__CPU__DOT__AB ));
    ui->led_WE->setState( top->top__DOT__CPU__DOT__WE );

    // GPIO OUT
    ui->val_gpio_out->setText(formatData( top->gpio_o ));
    ui->led0->setState( top->gpio_o >> 0 & 1 );
    ui->led1->setState( top->gpio_o >> 1 & 1 );
    ui->led2->setState( top->gpio_o >> 2 & 1 );
    ui->led3->setState( top->gpio_o >> 3 & 1 );
    ui->led4->setState( top->gpio_o >> 4 & 1 );
    ui->led5->setState( top->gpio_o >> 5 & 1 );
    ui->led6->setState( top->gpio_o >> 6 & 1 );
    ui->led7->setState( top->gpio_o >> 7 & 1 );

    // GPIO IN
    ui->val_gpio_in->setText(QString("%1").arg( gpio_in, 2, 16, QChar('0')).toUpper());

    // Memory
    bool sel = ( mem_page_high==0 && top->top__DOT__cs_ram ) || ( mem_page_high==15 && top->top__DOT__cs_rom );
    ui->mem_start->setText( QString("%1").arg( mem_base_addr      , 4, 16, QChar('0')).toUpper() );
    ui->mem_end->setText(   QString("%1").arg( mem_base_addr + 255, 4, 16, QChar('0')).toUpper() );
    ui->led_mem_sel->setState( sel );

    // fill RAM table with current values
    int base = 256*mem_page_low;
    CData* mem;

    if(mem_page_high==0)
        mem=top->top__DOT__ram_mem;
    else
        mem=top->top__DOT__rom_mem;

    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            ui->tblRAM->item(j,i)->setText(formatData( mem[base + 16*j+i ] ));

            if(sel && ( base+16*j+i == (top->top__DOT__CPU__DOT__AB & 4095) ))
                ui->tblRAM->item(j, i)->setBackground(QColor(128, 213, 255, 255));
            else
                ui->tblRAM->item(j, i)->setBackground(Qt::white);

//            // IIF we're reading/writing to RAM (mmio=0), then hightlight corresponding cell
//            if( !top->hrmcpu__DOT__MEMORY0__DOT__mmio && top->hrmcpu__DOT__MEMORY0__DOT__AR_q == (16*j+i) )
//            {
//                if( clk==0 && top->hrmcpu__DOT__MEMORY0__DOT__wM ) {
//                    ui->tblRAM->item(j, i)->setBackground(QColor(128, 213, 255, 255));
//                    ui->tblRAM->item(j, i)->setForeground(Qt::black);
//                }
//                else if( clk==0 && top->hrmcpu__DOT__MEMORY0__DOT__wM == 0 ) {
//                    ui->tblRAM->item(j, i)->setBackground(Qt::white);
//                }
//            }
        }
    }

#ifdef KK
    ui->led_halt->setState(top->hrmcpu__DOT__cu_halt);

    ui->led_PC_branch->setState( top->hrmcpu__DOT__PC0_branch );
    ui->led_PC_ijump->setState( top->hrmcpu__DOT__PC0_ijump );
    ui->led_PC_wPC->setState( top->hrmcpu__DOT__PC0_wPC );
    ui->tblPROG->setCurrentCell(top->hrmcpu__DOT__PC0_PC, 0);
    highlightLabel(ui->PC_PC, top->hrmcpu__DOT__PC0_wPC);

    // PROG
    ui->PROG_ADDR->setText(formatData( top->hrmcpu__DOT__PC0_PC ));
    ui->PROG_DATA->setText(formatData( top->hrmcpu__DOT__program0__DOT__Data ));
    // PROG table
    for(int i=0; i<256; i++){
        ui->tblPROG->setItem(0,i,new QTableWidgetItem( formatData( top->hrmcpu__DOT__program0__DOT__rom[i] ) ));
    }

    // IR Instruction Register
    ui->IR_INSTR->setText(formatData( top->hrmcpu__DOT__IR0_rIR ));
    ui->led_IR_wIR->setState( top->hrmcpu__DOT__cu_wIR );
    ui->IR_INSTR_name->setText( verilatorString( top->hrmcpu__DOT__ControlUnit0__DOT__instrname ) );
    highlightLabel(ui->IR_INSTR, top->hrmcpu__DOT__cu_wIR);

    // Register R
    ui->R_R->setText(formatData( top->hrmcpu__DOT__R_value ));
    ui->led_R_wR->setState( top->hrmcpu__DOT__register0__DOT__wR );
    highlightLabel(ui->R_R, top->hrmcpu__DOT__register0__DOT__wR);

    // MEMORY / RAM
    ui->MEM_AR->setText( formatData( top->hrmcpu__DOT__MEMORY0__DOT__AR_q ) );
    ui->led_MEM_wAR->setState( top->hrmcpu__DOT__MEMORY0__DOT__wAR );
    highlightLabel(ui->MEM_AR, top->hrmcpu__DOT__MEMORY0__DOT__wAR);

    ui->MEM_ADDR->setText( formatData( top->hrmcpu__DOT__MEMORY0__DOT__ADDR ) );
    ui->MEM_DATA->setText( formatData( top->hrmcpu__DOT__MEMORY0__DOT__M ) );
    ui->led_MEM_srcA->setState( top->hrmcpu__DOT__MEMORY0__DOT__srcA );
    ui->led_MEM_wM->setState( top->hrmcpu__DOT__MEMORY0__DOT__wM );
    ui->led_MEM_mmio->setState( top->hrmcpu__DOT__MEMORY0__DOT__mmio );

    // fill RAM table with current values
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            ui->tblRAM->item(j,i)->setText(formatData( top->hrmcpu__DOT__MEMORY0__DOT__mem_wrapper0__DOT__ram0__DOT__mem[16*j+i] ));

            // IIF we're reading/writing to RAM (mmio=0), then hightlight corresponding cell
            if( !top->hrmcpu__DOT__MEMORY0__DOT__mmio && top->hrmcpu__DOT__MEMORY0__DOT__AR_q == (16*j+i) )
            {
                if( clk==0 && top->hrmcpu__DOT__MEMORY0__DOT__wM ) {
                    ui->tblRAM->item(j, i)->setBackground(QColor(128, 213, 255, 255));
                    ui->tblRAM->item(j, i)->setForeground(Qt::black);
                }
                else if( clk==0 && top->hrmcpu__DOT__MEMORY0__DOT__wM == 0 ) {
                    ui->tblRAM->item(j, i)->setBackground(Qt::white);
                }
            }
        }
    }
    //ui->tblRAM->setCurrentCell( (int)( top->hrmcpu__DOT__MEMORY0__DOT__AR_q / 16 ), top->hrmcpu__DOT__MEMORY0__DOT__AR_q % 16 );

    // Pushbuttons release logic (must happen BEFORE INBOX / OUTBOX)
    if( main_time==ttr_pbPUSH && ui->pbPUSH->isChecked() ) {
        ui->pbPUSH->setChecked(false); // release
        // increment value after each PUSH to Inbox
        // ui->editINdata->setText( QString("%1").arg(ui->editINdata->text().toUInt(&toUintSuccess,16)+1 ,2,16,QChar('0')));
    }
    if( main_time==ttr_pbPOP && ui->pbPOP->isChecked() ) {
        ui->pbPOP->setChecked(false); // release
    }

    // udpate INBOX leds and Labels
    ui->led_INBOX_empty->setState( ! top->hrmcpu__DOT__INBOX_empty_n );
    ui->led_INBOX_full->setState( top->hrmcpu__DOT__INBOX_full );
    ui->led_INBOX_rd->setState( top->hrmcpu__DOT__INBOX_i_rd );
    ui->INBOX_data->setText( formatData( top->hrmcpu__DOT__INBOX_o_data ) );

    ui->pbPUSH->setEnabled(!top->hrmcpu__DOT__INBOX_full);

    // udpate INBOX table
    for(int i=0; i<32; i++){
        ui->tblINBOX->item(i,0)->setText(formatData( top->hrmcpu__DOT__INBOX__DOT__fifo[i] ));

        // Highlight in black foreground the FIFO elements
        if( top->hrmcpu__DOT__INBOX__DOT__r_last < top->hrmcpu__DOT__INBOX__DOT__r_first  )
            if( i >= top->hrmcpu__DOT__INBOX__DOT__r_last && i< top->hrmcpu__DOT__INBOX__DOT__r_first )
                ui->tblINBOX->item(i,0)->setForeground(Qt::black);
            else
                ui->tblINBOX->item(i,0)->setForeground(Qt::gray);
        else if( top->hrmcpu__DOT__INBOX__DOT__r_last > top->hrmcpu__DOT__INBOX__DOT__r_first  )
            if( i >= top->hrmcpu__DOT__INBOX__DOT__r_last || i< top->hrmcpu__DOT__INBOX__DOT__r_first )
                ui->tblINBOX->item(i,0)->setForeground(Qt::black);
            else
                ui->tblINBOX->item(i,0)->setForeground(Qt::gray);
        else
            ui->tblINBOX->item(i,0)->setForeground(Qt::gray);

        if( top->hrmcpu__DOT__INBOX_empty_n && i == top->hrmcpu__DOT__INBOX__DOT__r_last )
            // Head of FIFO in yellow background
            ui->tblINBOX->item(i,0)->setBackground(Qt::yellow);
        else
            ui->tblINBOX->item(i,0)->setBackground(Qt::white);
    }

    // udpate OUTBOX leds and Labels
    ui->led_OUTBOX_empty->setState( ! top->hrmcpu__DOT__OUTB_empty_n );
    ui->led_OUTBOX_full->setState( top->hrmcpu__DOT__OUTB_full);

    // OUTBOX table
    for(int i=0; i<32; i++){
        ui->tblOUTBOX->item(i,0)->setText(formatData( top->hrmcpu__DOT__OUTB__DOT__fifo[i] ));

        // Highlight in black foreground the FIFO elements
        if( top->hrmcpu__DOT__OUTB__DOT__r_last < top->hrmcpu__DOT__OUTB__DOT__r_first  )
            if( i >= top->hrmcpu__DOT__OUTB__DOT__r_last && i< top->hrmcpu__DOT__OUTB__DOT__r_first )
                ui->tblOUTBOX->item(i,0)->setForeground(Qt::black);
            else
                ui->tblOUTBOX->item(i,0)->setForeground(Qt::gray);
        else if( top->hrmcpu__DOT__OUTB__DOT__r_last > top->hrmcpu__DOT__OUTB__DOT__r_first  )
            if( i >= top->hrmcpu__DOT__OUTB__DOT__r_last || i< top->hrmcpu__DOT__OUTB__DOT__r_first )
                ui->tblOUTBOX->item(i,0)->setForeground(Qt::black);
            else
                ui->tblOUTBOX->item(i,0)->setForeground(Qt::gray);
        else
            ui->tblOUTBOX->item(i,0)->setForeground(Qt::gray);

        // Head of FIFO in yellow background
        if( top->hrmcpu__DOT__OUTB_empty_n && i == top->hrmcpu__DOT__OUTB__DOT__r_last )
            ui->tblOUTBOX->item(i,0)->setBackground(Qt::yellow);
        else
            ui->tblOUTBOX->item(i,0)->setBackground(Qt::white);
    }


    // Update debug (dump) values
    ui->led_dmp_valid->setState( top->cpu_dmp_valid );
    ui->dmp_value->setText(formatData( top->cpu_dmp_data ));
#endif
}

void MainWindow::highlightLabel(QWidget *qw, bool signal) {
    if( clk==0 && signal ) {
        qw->setStyleSheet(bgColor);
    } else if (clk==0 && signal == 0) {
        qw->setStyleSheet("");
    }
}

void MainWindow::on_clkPeriod_valueChanged(int period)
{
    m_timer->setInterval(period);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_F5) {
        ui->pbB->toggle();
    } else if(e->key() == Qt::Key_F3) {
        ui->pbA->click();
    } else if(e->key() == Qt::Key_F4) {
        ui->pbINSTR->click();
    }

}

void MainWindow::on_pbReset_toggled(bool checked)
{
    ui->pbINSTR->setDisabled(checked);

    top->i_rst=checked;
    updateUI();
}

void MainWindow::on_pbSave_pressed()
{
    VerilatedSave os;
//    os.open(filenamep);
//    os << main_time;  // user code must save the timestamp, etc
//    os << *topp;

}

void MainWindow::on_pbPUSH_toggled(bool checked)
{
#ifdef KK
    top->cpu_in_wr = checked;
    if(checked)
        ttr_pbPUSH = main_time+2; // release in 2 ticks
#endif
}

void MainWindow::on_pbPOP_toggled(bool checked)
{
#ifdef KK
    top->cpu_out_rd = checked;
    ttr_pbPOP = main_time+2;
#endif
}

void MainWindow::LoadProgramFromFile(QString fileName)
{
#ifdef KK
    if (fileName.isEmpty()) {
         qDebug() << "filename is empty, not loading";
        return;
    }

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return;
    }

    QTextStream instream(&file);

    QString line = instream.readLine();

//    qDebug() << "first line: " << line;

    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

//    qDebug() << list;

    bool success;

    for (int i = 0; i < list.size(); ++i)
    {
        top->hrmcpu__DOT__program0__DOT__rom[i] = list.at(i).toUInt(&success,16);
    }
    file.close();

    updateUI();
#endif
}

void MainWindow::on_pbLoad_pressed()
{

}

QString MainWindow::verilatorString(QData &data )
{
    QString s;

    char* p;
    p=(char*)&data;

    for(int i=6; i>0; i--) {
        if( p[i-1]>0 ) s.append( p[i-1] );
    }

    return s;
}

QString MainWindow::verilatorString( WData data[] )
{
    QString s;

    char* p;
    p=(char*)data;

    for(int i=12; i>0; i--) {
        if( p[i-1]>0 ) s.append( p[i-1] );
    }

    return s;
}

void MainWindow::on_pbLoadPROG_pressed()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Program to load"), "",
        tr("Program files (program);;Hex files (*.hex);;All Files (*)"));

    LoadProgramFromFile(fileName);
}

QString MainWindow::formatData(CData data) {
    return QString("%1").arg( data, 2, 16, QChar('0')).toUpper();
}

QString MainWindow::formatData(SData data) {
    return QString("%1").arg( data, 4, 16, QChar('0')).toUpper();
    // ASCII mode --> return QString("%1").arg( QChar(data) );
}

void MainWindow::on_actionLoad_Program_triggered()
{
    on_pbLoadPROG_pressed();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_pbINSTR_pressed()
{
#ifdef KK
    if( top->hrmcpu__DOT__IR0_rIR == 0 && !top->hrmcpu__DOT__INBOX_empty_n ) {
        clkTick();
        clkTick();
        if( !top->cpu_in_wr || ui->editINdata->text().isEmpty() ) {
            return;
        }
    }
    while( top->hrmcpu__DOT__ControlUnit0__DOT__state == 9 /* = DECODE */ && top->hrmcpu__DOT__IR0_rIR != 240 /* != HALT */ ) {
        clkTick();
    }
    while( top->hrmcpu__DOT__ControlUnit0__DOT__state != 9 /* DECODE */ && top->hrmcpu__DOT__IR0_rIR != 240 /* HALT */ ) {
        clkTick();
    }
#endif
}

void MainWindow::on_pbHold_toggled(bool checked)
{
    ui->pbINSTR->setDisabled(checked);

    // store value in Verilator model
    top->hold=ui->pbHold->isChecked();
}

void MainWindow::on_any_gpio_in_toggled()
{
    // https://stackoverflow.com/a/24261056/728281

    QCheckBox* pCheckBox = qobject_cast<QCheckBox*>(sender());
    if (!pCheckBox) // this is just a safety check
        return;

    unsigned char i = gpio_in_list.indexOf(pCheckBox);

    gpio_in += (1<<i)*( pCheckBox->isChecked() ? 1 : -1 );

    ui->val_gpio_in->setText(QString("%1").arg( gpio_in, 2, 16, QChar('0')).toUpper());

    // store value in Verilator model
    top->gpio_i=gpio_in;
}

void MainWindow::on_mem_page_valueChanged(int base)
{
    mem_page_low = base;
    mem_base_addr=256*(16*mem_page_high+mem_page_low);
    updateUI();
}

void MainWindow::on_radioRam_toggled(bool checked)
{
    if(checked) {
        mem_page_high=0;
        mem_base_addr=256*(16*mem_page_high+mem_page_low);
        updateUI();
    }
}

void MainWindow::on_radioRom_toggled(bool checked)
{
    if(checked) {
        mem_page_high=15;
        mem_base_addr=256*(16*mem_page_high+mem_page_low);
        updateUI();
    }
}
