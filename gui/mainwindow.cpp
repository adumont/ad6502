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
    // ugly to put this here... will fix later
    instr[0]=10; mode[0]=0; // BRK SNGL
    instr[1]=34; mode[1]=5; // ORA INDX
    instr[5]=34; mode[5]=2; // ORA ZP
    instr[6]=2; mode[6]=2; // ASL ZP
    instr[8]=36; mode[8]=0; // PHP SNGL
    instr[9]=34; mode[9]=1; // ORA Imm
    instr[10]=2; mode[10]=0; // ASL SNGL
    instr[13]=34; mode[13]=8; // ORA ABS
    instr[14]=2; mode[14]=8; // ASL ABS
    instr[16]=9; mode[16]=7; // BPL BRA
    instr[17]=34; mode[17]=6; // ORA INDY
    instr[21]=34; mode[21]=3; // ORA ZPX
    instr[22]=2; mode[22]=3; // ASL ZPX
    instr[24]=13; mode[24]=0; // CLC SNGL
    instr[25]=34; mode[25]=10; // ORA ABSY
    instr[29]=34; mode[29]=9; // ORA ABSX
    instr[30]=2; mode[30]=9; // ASL ABSX
    instr[32]=28; mode[32]=8; // JSR ABS
    instr[33]=1; mode[33]=5; // AND INDX
    instr[36]=6; mode[36]=2; // BIT ZP
    instr[37]=1; mode[37]=2; // AND ZP
    instr[38]=39; mode[38]=2; // ROL ZP
    instr[40]=38; mode[40]=0; // PLP SNGL
    instr[41]=1; mode[41]=1; // AND Imm
    instr[42]=39; mode[42]=0; // ROL SNGL
    instr[44]=6; mode[44]=8; // BIT ABS
    instr[45]=1; mode[45]=8; // AND ABS
    instr[46]=39; mode[46]=8; // ROL ABS
    instr[48]=7; mode[48]=7; // BMI BRA
    instr[49]=1; mode[49]=6; // AND INDY
    instr[53]=1; mode[53]=3; // AND ZPX
    instr[54]=39; mode[54]=3; // ROL ZPX
    instr[56]=44; mode[56]=0; // SEC SNGL
    instr[57]=1; mode[57]=10; // AND ABSY
    instr[61]=1; mode[61]=9; // AND ABSX
    instr[62]=39; mode[62]=9; // ROL ABSX
    instr[64]=41; mode[64]=0; // RTI SNGL
    instr[65]=23; mode[65]=5; // EOR INDX
    instr[69]=23; mode[69]=2; // EOR ZP
    instr[70]=32; mode[70]=2; // LSR ZP
    instr[72]=35; mode[72]=0; // PHA SNGL
    instr[73]=23; mode[73]=1; // EOR Imm
    instr[74]=32; mode[74]=0; // LSR SNGL
    instr[76]=27; mode[76]=8; // JMP ABS
    instr[77]=23; mode[77]=8; // EOR ABS
    instr[78]=32; mode[78]=8; // LSR ABS
    instr[80]=11; mode[80]=7; // BVC BRA
    instr[81]=23; mode[81]=6; // EOR INDY
    instr[85]=23; mode[85]=3; // EOR ZPX
    instr[86]=32; mode[86]=3; // LSR ZPX
    instr[88]=15; mode[88]=0; // CLI SNGL
    instr[89]=23; mode[89]=10; // EOR ABSY
    instr[93]=23; mode[93]=9; // EOR ABSX
    instr[94]=32; mode[94]=9; // LSR ABSX
    instr[96]=42; mode[96]=0; // RTS SNGL
    instr[97]=0; mode[97]=5; // ADC INDX
    instr[101]=0; mode[101]=2; // ADC ZP
    instr[102]=40; mode[102]=2; // ROR ZP
    instr[104]=37; mode[104]=0; // PLA SNGL
    instr[105]=0; mode[105]=1; // ADC Imm
    instr[106]=40; mode[106]=0; // ROR SNGL
    instr[108]=27; mode[108]=11; // JMP IND
    instr[109]=0; mode[109]=8; // ADC ABS
    instr[110]=40; mode[110]=8; // ROR ABS
    instr[112]=12; mode[112]=7; // BVS BRA
    instr[113]=0; mode[113]=6; // ADC INDY
    instr[117]=0; mode[117]=3; // ADC ZPX
    instr[118]=40; mode[118]=3; // ROR ZPX
    instr[120]=46; mode[120]=0; // SEI SNGL
    instr[121]=0; mode[121]=10; // ADC ABSY
    instr[125]=0; mode[125]=9; // ADC ABSX
    instr[126]=40; mode[126]=9; // ROR ABSX
    instr[129]=47; mode[129]=5; // STA INDX
    instr[132]=49; mode[132]=2; // STY ZP
    instr[133]=47; mode[133]=2; // STA ZP
    instr[134]=48; mode[134]=2; // STX ZP
    instr[136]=22; mode[136]=0; // DEY SNGL
    instr[138]=53; mode[138]=0; // TXA SNGL
    instr[140]=49; mode[140]=8; // STY ABS
    instr[141]=47; mode[141]=8; // STA ABS
    instr[142]=48; mode[142]=8; // STX ABS
    instr[144]=3; mode[144]=7; // BCC BRA
    instr[145]=47; mode[145]=6; // STA INDY
    instr[148]=49; mode[148]=3; // STY ZPX
    instr[149]=47; mode[149]=3; // STA ZPX
    instr[150]=48; mode[150]=4; // STX ZPY
    instr[152]=55; mode[152]=0; // TYA SNGL
    instr[153]=47; mode[153]=10; // STA ABSY
    instr[154]=54; mode[154]=0; // TXS SNGL
    instr[157]=47; mode[157]=9; // STA ABSX
    instr[160]=31; mode[160]=1; // LDY Imm
    instr[161]=29; mode[161]=5; // LDA INDX
    instr[162]=30; mode[162]=1; // LDX Imm
    instr[164]=31; mode[164]=2; // LDY ZP
    instr[165]=29; mode[165]=2; // LDA ZP
    instr[166]=30; mode[166]=2; // LDX ZP
    instr[168]=51; mode[168]=0; // TAY SNGL
    instr[169]=29; mode[169]=1; // LDA Imm
    instr[170]=50; mode[170]=0; // TAX SNGL
    instr[172]=31; mode[172]=8; // LDY ABS
    instr[173]=29; mode[173]=8; // LDA ABS
    instr[174]=30; mode[174]=8; // LDX ABS
    instr[176]=4; mode[176]=7; // BCS BRA
    instr[177]=29; mode[177]=6; // LDA INDY
    instr[180]=31; mode[180]=3; // LDY ZPX
    instr[181]=29; mode[181]=3; // LDA ZPX
    instr[182]=30; mode[182]=4; // LDX ZPY
    instr[184]=16; mode[184]=0; // CLV SNGL
    instr[185]=29; mode[185]=10; // LDA ABSY
    instr[186]=52; mode[186]=0; // TSX SNGL
    instr[188]=31; mode[188]=9; // LDY ABSX
    instr[189]=29; mode[189]=9; // LDA ABSX
    instr[190]=30; mode[190]=10; // LDX ABSY
    instr[192]=19; mode[192]=1; // CPY Imm
    instr[193]=17; mode[193]=5; // CMP INDX
    instr[196]=19; mode[196]=2; // CPY ZP
    instr[197]=17; mode[197]=2; // CMP ZP
    instr[198]=20; mode[198]=2; // DEC ZP
    instr[200]=26; mode[200]=0; // INY SNGL
    instr[201]=17; mode[201]=1; // CMP Imm
    instr[202]=21; mode[202]=0; // DEX SNGL
    instr[204]=19; mode[204]=8; // CPY ABS
    instr[205]=17; mode[205]=8; // CMP ABS
    instr[206]=20; mode[206]=8; // DEC ABS
    instr[208]=8; mode[208]=7; // BNE BRA
    instr[209]=17; mode[209]=6; // CMP INDY
    instr[213]=17; mode[213]=3; // CMP ZPX
    instr[214]=20; mode[214]=3; // DEC ZPX
    instr[216]=14; mode[216]=0; // CLD SNGL
    instr[217]=17; mode[217]=10; // CMP ABSY
    instr[221]=17; mode[221]=9; // CMP ABSX
    instr[222]=20; mode[222]=9; // DEC ABSX
    instr[224]=18; mode[224]=1; // CPX Imm
    instr[225]=43; mode[225]=5; // SBC INDX
    instr[228]=18; mode[228]=2; // CPX ZP
    instr[229]=43; mode[229]=2; // SBC ZP
    instr[230]=24; mode[230]=2; // INC ZP
    instr[232]=25; mode[232]=0; // INX SNGL
    instr[233]=43; mode[233]=1; // SBC Imm
    instr[234]=33; mode[234]=0; // NOP SNGL
    instr[236]=18; mode[236]=8; // CPX ABS
    instr[237]=43; mode[237]=8; // SBC ABS
    instr[238]=24; mode[238]=8; // INC ABS
    instr[240]=5; mode[240]=7; // BEQ BRA
    instr[241]=43; mode[241]=6; // SBC INDY
    instr[245]=43; mode[245]=3; // SBC ZPX
    instr[246]=24; mode[246]=3; // INC ZPX
    instr[248]=45; mode[248]=0; // SED SNGL
    instr[249]=43; mode[249]=10; // SBC ABSY
    instr[253]=43; mode[253]=9; // SBC ABSX
    instr[254]=24; mode[254]=9; // INC ABSX

    modeName[0]="SNGL";
    modeName[1]="Imm";
    modeName[2]="ZP";
    modeName[3]="ZPX";
    modeName[4]="ZPY";
    modeName[5]="INDX";
    modeName[6]="INDY";
    modeName[7]="BRA";
    modeName[8]="ABS";
    modeName[9]="ABSX";
    modeName[10]="ABSY";
    modeName[11]="IND";

    instrName[0]="ADC";
    instrName[1]="AND";
    instrName[2]="ASL";
    instrName[3]="BCC";
    instrName[4]="BCS";
    instrName[5]="BEQ";
    instrName[6]="BIT";
    instrName[7]="BMI";
    instrName[8]="BNE";
    instrName[9]="BPL";
    instrName[10]="BRK";
    instrName[11]="BVC";
    instrName[12]="BVS";
    instrName[13]="CLC";
    instrName[14]="CLD";
    instrName[15]="CLI";
    instrName[16]="CLV";
    instrName[17]="CMP";
    instrName[18]="CPX";
    instrName[19]="CPY";
    instrName[20]="DEC";
    instrName[21]="DEX";
    instrName[22]="DEY";
    instrName[23]="EOR";
    instrName[24]="INC";
    instrName[25]="INX";
    instrName[26]="INY";
    instrName[27]="JMP";
    instrName[28]="JSR";
    instrName[29]="LDA";
    instrName[30]="LDX";
    instrName[31]="LDY";
    instrName[32]="LSR";
    instrName[33]="NOP";
    instrName[34]="ORA";
    instrName[35]="PHA";
    instrName[36]="PHP";
    instrName[37]="PLA";
    instrName[38]="PLP";
    instrName[39]="ROL";
    instrName[40]="ROR";
    instrName[41]="RTI";
    instrName[42]="RTS";
    instrName[43]="SBC";
    instrName[44]="SEC";
    instrName[45]="SED";
    instrName[46]="SEI";
    instrName[47]="STA";
    instrName[48]="STX";
    instrName[49]="STY";
    instrName[50]="TAX";
    instrName[51]="TAY";
    instrName[52]="TSX";
    instrName[53]="TXA";
    instrName[54]="TXS";
    instrName[55]="TYA";

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
    ui->PC_PC->setText(formatData( top->top__DOT__CPU__DOT__curr_PC ));

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
    ui->IR_IR->setText(formatData( top->top__DOT__CPU__DOT__curr_IR ));
    // qDebug() << top->top__DOT__CPU__DOT__curr_IR ;
    ui->IR_INSTR_name->setText( instrName[instr[ top->top__DOT__CPU__DOT__curr_IR ] ] );
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
