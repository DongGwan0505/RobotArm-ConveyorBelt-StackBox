module SCCB_Controller (
    input  logic        clk,
    input  logic        reset,
    // internal logic rom
    input  logic [15:0] rom_data,
    output       [ 7:0] addr,
    // internal logic master
    output logic [ 7:0] reg_data,
    //output logic [7:0] slave_addr,
    //output logic       I2C_En,
    output logic        I2C_start,
    output logic        I2C_stop,
    input  logic        tx_done,
    input  logic        tx_ready
);

    logic [11:0] clk_counter_reg, clk_counter_next;
    logic [8:0] addr_reg, addr_next;
    assign addr = addr_reg;

    typedef enum {
        IDLE,
        WAIT_SLAVE,
        SLAVE,
        WAIT_REGDATA,
        REGDATA,
        STOP,
        RACE
    } state_e;
    state_e state, state_next;

    always_ff @(posedge clk, posedge reset) begin
        if (reset) begin
            state <= IDLE;
            reg_data <= 0;
            addr_reg <= 0;
        end else begin
            state <= state_next;
            clk_counter_reg = clk_counter_next;
        end
    end

    always_comb begin
        clk_counter_next = clk_counter_reg;
        state_next = state;
        I2C_start = 1'b0;
        I2C_stop = 1'b0;
        case (state)
            IDLE: begin
                clk_counter_next = 0;
                if (tx_ready) begin
                    I2C_start  = 1'b1;
                    I2C_stop   = 1'b0;
                    state_next = SLAVE;
                end
            end

            WAIT_SLAVE: begin
                I2C_start = 1'b1;
                I2C_stop  = 1'b0;
                if (clk_counter_reg == 999) begin
                    I2C_start = 1'b0;
                    I2C_stop = 1'b0;
                    state_next = SLAVE;
                    clk_counter_next = 0;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            SLAVE: begin
                I2C_start = 1'b0;
                I2C_stop  = 1'b0;
                reg_data  = 8'h42;
            end

            WAIT_REGDATA: begin
                I2C_start = 1'b0;
                I2C_stop  = 1'b0;
                if (tx_done) begin
                    state_next = REGDATA;
                end
            end

            REGDATA: begin
                I2C_start = 1'b0;
                I2C_stop  = 1'b0;
                reg_data  = rom_data[15:8];
                if (tx_done) begin
                    reg_data = rom_data[7:0];
                    if (tx_done) begin
                        if (rom_data == 16'hff) begin
                            state_next = RACE;
                        end
                    end
                end
            end

            STOP: begin
                I2C_start  = 1'b0;
                I2C_stop   = 1'b1;
                addr_next  = addr_reg + 1;
                state_next = IDLE;
            end

            RACE: begin
            end
        endcase
    end

endmodule
