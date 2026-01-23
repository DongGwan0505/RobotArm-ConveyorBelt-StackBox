`timescale 1ns / 1ps

module Erosion_3x3 (
    input  logic       clk, reset,
    input  logic       de_in,
    input  logic [9:0] x_in, y_in,
    input  logic       d_in,

    output logic       de_out,
    output logic [9:0] x_out,
    output logic [9:0] y_out,
    output logic       d_out
);
    localparam int H_RES = 320;
    logic [0:H_RES-1] line_buf0, line_buf1;
    logic [0:2][0:2]  w;
    logic [9:0] x_d, y_d;
    logic de_d;

    always_ff @(posedge clk) begin
        if (reset) begin
            de_d <= 0; w <= '0;
        end else if (de_in && x_in < H_RES) begin
            line_buf0[x_in] <= line_buf1[x_in];
            line_buf1[x_in] <= d_in;

            w[0][0] <= w[0][1]; w[0][1] <= w[0][2]; w[0][2] <= line_buf0[x_in];
            w[1][0] <= w[1][1]; w[1][1] <= w[1][2]; w[1][2] <= line_buf1[x_in];
            w[2][0] <= w[2][1]; w[2][1] <= w[2][2]; w[2][2] <= d_in;

            x_d <= x_in; y_d <= y_in; de_d <= de_in;
        end else de_d <= 0;
    end

    always_ff @(posedge clk) begin
        de_out <= de_d; x_out <= x_d; y_out <= y_d;
        // 3x3 모든 칸이 1일 때만 1 (AND 연산)
        d_out <= (w[0][0] && w[0][1] && w[0][2] &&
                  w[1][0] && w[1][1] && w[1][2] &&
                  w[2][0] && w[2][1] && w[2][2]);
    end
endmodule

module Dilation_3x3 (
    input  logic       clk, reset,
    input  logic       de_in,
    input  logic [9:0] x_in, y_in,
    input  logic       d_in,

    output logic       de_out,
    output logic [9:0] x_out,
    output logic [9:0] y_out,
    output logic       d_out
);
    localparam int H_RES = 320;
    logic [0:H_RES-1] line_buf0, line_buf1;
    logic [0:2][0:2]  w;
    logic [9:0] x_d, y_d;
    logic de_d;

    always_ff @(posedge clk) begin
        if (reset) begin
            de_d <= 0; w <= '0;
        end else if (de_in && x_in < H_RES) begin
            line_buf0[x_in] <= line_buf1[x_in];
            line_buf1[x_in] <= d_in;

            w[0][0] <= w[0][1]; w[0][1] <= w[0][2]; w[0][2] <= line_buf0[x_in];
            w[1][0] <= w[1][1]; w[1][1] <= w[1][2]; w[1][2] <= line_buf1[x_in];
            w[2][0] <= w[2][1]; w[2][1] <= w[2][2]; w[2][2] <= d_in;

            x_d <= x_in; y_d <= y_in; de_d <= de_in;
        end else de_d <= 0;
    end

    always_ff @(posedge clk) begin
        de_out <= de_d; x_out <= x_d; y_out <= y_d;
        // 3x3 중 하나라도 1이면 1 (OR 연산)
        d_out <= (w[0][0] || w[0][1] || w[0][2] ||
                  w[1][0] || w[1][1] || w[1][2] ||
                  w[2][0] || w[2][1] || w[2][2]);
    end
endmodule