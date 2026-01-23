`timescale 1ns / 1ps


module SCCB_Master (
    // global signals
    input  logic       clk,
    input  logic       reset,
    // internal signals
    input  logic       I2C_En,
    input  logic [7:0] tx_data,
    output logic       tx_done,
    output logic       tx_ready,
    //output logic [7:0] rx_data,
    //output logic       rx_done,
    input  logic       I2C_start,
    input  logic       I2C_stop,
    // external signals
    output logic       SCL,
    inout  logic       SDA
);

    typedef enum {
        IDLE,
        START1,
        START2,
        DATA1,
        DATA2,
        DATA3,
        DATA4,
        ACK1,
        ACK2,
        ACK3,
        ACK4,
        STOP1,
        STOP2,
        HOLD
    } I2C_state;

    I2C_state state, state_next;

    logic [8:0] clk_counter_reg, clk_counter_next;
    logic [2:0] bit_counter_reg, bit_counter_next;
    logic [7:0] tx_data_reg, tx_data_next;
    logic sda_en, sda_out;

    assign SDA = sda_en ? sda_out : 1'bz;

    always_ff @(posedge clk, posedge reset) begin
        if (reset) begin
            state           <= IDLE;
            clk_counter_reg <= 0;
            tx_data_reg     <= 0;
            bit_counter_reg <= 0;
        end else begin
            state           <= state_next;
            clk_counter_reg <= clk_counter_next;
            tx_data_reg     <= tx_data_next;
            bit_counter_reg <= bit_counter_next;
        end
    end

    always_comb begin
        state_next       = state;
        clk_counter_next = clk_counter_reg;
        tx_data_next     = tx_data_reg;
        bit_counter_next = bit_counter_reg;
        SCL              = 1'b1;
        sda_en           = 1'b1;
        sda_out          = 1'b1;
        tx_done          = 0;
        tx_ready         = 0;
        case (state)
            IDLE: begin
                sda_en   = 1;
                sda_out  = 1'b1;
                SCL      = 1'b1;
                tx_ready = 1;
                tx_done  = 0;
                if (I2C_En && I2C_start) begin
                    bit_counter_next = 0;
                    state_next = HOLD;
                end
            end

            HOLD: begin
                if (I2C_start && !I2C_stop) begin
                    SCL = 1'b0;
                    tx_ready = 0;
                    state_next = START1;
                end

                if (!I2C_start && I2C_stop) begin
                    SCL = 1'b1;
                    tx_ready = 0;
                    state_next = STOP1;
                end

                if (!I2C_start && !I2C_stop) begin
                    SCL = 1'b0;
                    tx_data_next = tx_data;
                    tx_ready = 0;
                    state_next = DATA1;
                end

                // READ 용
                // if (I2C_start && I2C_stop) begin
                //     state_next = read;
                // end
            end

            START1: begin
                sda_en  = 1;
                sda_out = 1'b0;
                SCL     = 1'b1;
                if (clk_counter_reg == 499) begin
                    clk_counter_next = 0;
                    state_next = START2;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            START2: begin
                sda_en  = 1;
                sda_out = 1'b0;
                SCL     = 1'b0;
                if (clk_counter_reg == 499) begin
                    clk_counter_next = 0;
                    state_next = HOLD;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            DATA1: begin
                sda_en  = 1;
                sda_out = tx_data_reg[7];
                SCL     = 1'b0;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    state_next = DATA2;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            DATA2: begin
                sda_en  = 1;
                sda_out = tx_data_reg[7];
                SCL     = 1'b1;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    state_next = DATA3;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            DATA3: begin
                sda_en  = 1;
                sda_out = tx_data_reg[7];
                SCL     = 1'b1;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    state_next = DATA4;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            DATA4: begin
                sda_en  = 1;
                sda_out = tx_data_reg[7];
                SCL     = 1'b0;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    if (bit_counter_reg == 7) begin
                        bit_counter_next = 0;
                        state_next = ACK1;
                    end else begin
                        bit_counter_next = bit_counter_reg + 1;
                        tx_data_next = {tx_data_reg[6:0], 1'b0};
                        state_next = DATA1;
                    end
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            ACK1: begin
                sda_en = 0;
                SCL    = 1'b0;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    state_next = ACK2;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            ACK2: begin
                sda_en = 0;
                SCL    = 1'b1;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    state_next = ACK3;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            ACK3: begin
                sda_en = 0;
                SCL    = 1'b1;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    state_next = ACK4;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            ACK4: begin
                sda_en = 0;
                SCL    = 1'b0;
                if (clk_counter_reg == 249) begin
                    clk_counter_next = 0;
                    tx_ready = 1;
                    state_next = HOLD;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            STOP1: begin
                sda_en  = 1;
                sda_out = 0;
                SCL     = 1;
                if (clk_counter_reg == 499) begin
                    clk_counter_next = 0;
                    state_next = STOP2;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end

            STOP2: begin
                sda_en  = 1;
                sda_out = 1;
                SCL     = 1;
                if (clk_counter_reg == 499) begin
                    clk_counter_next = 0;
                    tx_done = 1;
                    state_next = IDLE;
                end else begin
                    clk_counter_next = clk_counter_reg + 1;
                end
            end
        endcase
    end
endmodule


