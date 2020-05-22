`ifndef __TOP_V__
`define __TOP_V__

`default_nettype none

`include "cpu.v"

// check connections to VGA adapter on https://github.com/Obijuan/MonsterLED/wiki

module top (
    input  wire       clk,       // System clock.

`ifndef SYNTHESIS
    // Simulation Only
    input  wire       i_rst,
    input  wire       hold,
    input  wire [7:0] gpio_i,
`endif
    output reg  [7:0] gpio_o
  );

`ifdef SYNTHESIS
  wire i_rst = 1'b 0;
  wire hold  = 1'b 0;

  wire [7:0] gpio_i = 8'b0;
`endif

  // //`ifdef BOARD_HAVE_BUTTONS
  // wire sw1_d; // pulse when sw pressed
  // wire sw1_u; // pulse when sw released
  // wire sw1_s; // sw state
  // debouncer db_sw1 (.clk(clk), .PB(sw1), .PB_down(sw1_d), .PB_up(sw1_u), .PB_state(sw1_s));

  // wire sw2_d; // pulse when sw pressed
  // wire sw2_u; // pulse when sw released
  // wire sw2_s; // sw state
  // debouncer db_sw2 (.clk(clk), .PB(sw2), .PB_down(sw2_d), .PB_up(sw2_u), .PB_state(sw2_s));
  // //`endif

  localparam baudsDivider=24'd104;

  // ---------------------------------------- //
  // Power-Up Reset
  // reset_n low for (2^reset_counter_size) first clocks
  wire reset_n;

`ifdef SYNTHESIS
  localparam reset_counter_size = 5;
`else
  localparam reset_counter_size = 1;
`endif

  reg [(reset_counter_size-1):0] reset_reg = 0;

  always @(posedge clk)
      reset_reg <= reset_reg + { {(reset_counter_size-1) {1'b0}} , !reset_n};

  assign reset_n = &reset_reg ;

  // ---------------------------------------- //

  // ---------------------------------------- //
  // CPU (cpu)
  //

  wire        i_CPU_clk;
  wire        i_CPU_reset;
  wire [15:0] o_CPU_AB;
  wire [ 7:0] i_CPU_DI;
  wire [ 7:0] o_CPU_DO;
  wire        o_CPU_WE;
  wire        i_CPU_IRQ;
  wire        i_CPU_NMI;
  wire        i_CPU_RDY;

  cpu CPU (
    //---- input ports ----
    .clk  (i_CPU_clk  ),
    .reset(i_CPU_reset),
    .DI   (i_CPU_DI   ),
    .IRQ  (i_CPU_IRQ  ),
    .NMI  (i_CPU_NMI  ),
    .RDY  (i_CPU_RDY  ),
    //---- output ports ----
    .AB   (o_CPU_AB   ),
    .DO   (o_CPU_DO   ),
    .WE   (o_CPU_WE   )
  );
  // Connect Inputs:
  assign i_CPU_clk   = clk ;
  assign i_CPU_reset = ~reset_n || i_rst;
  assign i_CPU_DI    = o_DataMux_dout ;
  assign i_CPU_IRQ   = 1'b0 ;
  assign i_CPU_NMI   = 1'b0 ;
  assign i_CPU_RDY   = ~hold ;
  // ---------------------------------------- //

	wire [3:0] cs = o_CPU_AB[15:12]; // 16 "devices"/ banks, $0 to $F
	wire cs_ram  = ( cs == 4'h 0 );   // RAM $0_000 to $0_FFF, $1000 = 4096 Bytes long (4KB)
	wire cs_gpio = ( cs == 4'h 1 );   // GPIO
	wire cs_acia = ( cs == 4'h 2 );   // ACIA
	wire cs_rom  = ( cs == 4'h F );   // ROM (program) $F000 to $FFFF, $1000 = 4096 Bytes long (4KB)

	// RAM @ pages $00 to $0F
	reg [7:0] ram_mem[4095:0];
	reg [7:0] ram_do;
	always @(posedge clk)
		if( o_CPU_WE && cs_ram )
			ram_mem[o_CPU_AB[11:0]] <= o_CPU_DO;
	always @(posedge clk)
		ram_do <= ram_mem[o_CPU_AB[11:0]];
	
	// GPIO @ page 10-1f
	reg [7:0] gpio_do;
	always @(posedge clk)
		if( o_CPU_WE && cs_gpio )
			gpio_o <= o_CPU_DO;
	always @(posedge clk)
		gpio_do <= gpio_i;
	
	// // ACIA at page 20-2f
	// wire [7:0] acia_do;
	// acia uacia(
	// 	.clk(clk),				// system clock
	// 	.rst(reset),			// system reset
	// 	.cs(cs_acia),				// chip select
	// 	.we(o_CPU_WE),			// write enable
	// 	.rs(o_CPU_AB[0]),			// register select
	// 	.rx(RX),				// serial receive
	// 	.din(o_CPU_DO),			// data bus input
	// 	.dout(acia_do),			// data bus output
	// 	.tx(TX),				// serial transmit
	// 	.irq(CPU_IRQ)			// interrupt request
	// );
	
	// ROM @ pages f0,f1... (Addr: $F0_00 to $FF_FF)
  reg [7:0] rom_mem[4095:0];
	reg [7:0] rom_do;
	initial
    $readmemh("rom.hex",rom_mem);
	always @(posedge clk)
		rom_do <= rom_mem[o_CPU_AB[11:0]];

	// data mux
  reg [ 7:0] o_DataMux_dout;
	reg [3:0] mux_sel;
	always @(posedge clk)
		mux_sel <= cs;

	always @(*)
  begin
		casez(mux_sel)
			4'h0: o_DataMux_dout = ram_do;
			4'h1: o_DataMux_dout = gpio_do;
			// 4'h2: o_DataMux_dout = acia_do;
			4'hf: o_DataMux_dout = rom_do;
			default: o_DataMux_dout = rom_do;
		endcase
  end

endmodule

`endif // __TOP_V__