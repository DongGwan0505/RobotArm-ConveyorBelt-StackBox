// `timescale 1ns / 1ps
// module Gaussian_filter (
//     input  logic        clk,
//     input  logic        reset,
//     input  logic        de_in,
//     input  logic [9:0]  x_in,
//     input  logic [9:0]  y_in,
//     input  logic        d_in,

//     output logic        de_out,
//     output logic [9:0]  x_out,
//     output logic [9:0]  y_out,
//     output logic        d_out
// );
//     localparam int H_RES = 320;
//     logic [0:H_RES-1] line_buf0, line_buf1;
//     logic [0:2][0:2]  w;

//     // 데이터 연산 지연에 맞춰 좌표/제어 신호도 지연 (파이프라인)
//     logic [9:0] x_d, y_d;
//     logic de_d;

//     always_ff @(posedge clk) begin
//         if (de_in&& (x_in < 320)) begin
//             line_buf0[x_in] <= line_buf1[x_in];
//             line_buf1[x_in] <= d_in;

//             w[0][0] <= w[0][1]; w[0][1] <= w[0][2]; w[0][2] <= line_buf0[x_in];
//             w[1][0] <= w[1][1]; w[1][1] <= w[1][2]; w[1][2] <= line_buf1[x_in];
//             w[2][0] <= w[2][1]; w[2][1] <= w[2][2]; w[2][2] <= d_in;
            
//             // 좌표 저장 (데이터가 윈도우 중앙에 올 때의 좌표를 유지)
//             x_d <= x_in;
//             y_d <= y_in;
//             de_d <= de_in;
//         end else begin
//             de_d <= 1'b0;
//         end
//     end

//     // 합산 및 출력 (1클럭 추가 지연)
//     always_ff @(posedge clk) begin
//         de_out <= de_d;
//         x_out  <= x_d;
//         y_out  <= y_d;
//         d_out  <= (w[0][0]+w[0][1]+w[0][2]+w[1][0]+w[1][1]+w[1][2]+w[2][0]+w[2][1]+w[2][2] >= 5);
//     end
// endmodule
`timescale 1ns / 1ps

module Gaussian_filter (
    input  logic        clk,
    input  logic        reset,
    input  logic        de_in,
    input  logic [9:0]  x_in,
    input  logic [9:0]  y_in,
    input  logic        d_in,

    output logic        de_out,
    output logic [9:0]  x_out,
    output logic [9:0]  y_out,
    output logic        d_out
);
    localparam int H_RES = 320;
    
    // 라인 버퍼는 보통 RAM으로 합성되므로 초기값만 설정하거나 그대로 둡니다.
    logic [0:H_RES-1] line_buf0, line_buf1;
    logic [0:2][0:2]  w;

    // 파이프라인 지연용 신호
    logic [9:0] x_d, y_d;
    logic de_d;

    // 1단계: 라인 버퍼 저장 및 윈도우 구성 (1 clk 지연)
    always_ff @(posedge clk) begin
        if (reset) begin
            w    <= '0;
            x_d  <= '0;
            y_d  <= '0;
            de_d <= 1'b0;
        end else if (de_in && (x_in < H_RES)) begin
            line_buf0[x_in] <= line_buf1[x_in];
            line_buf1[x_in] <= d_in;

            // 윈도우 슬라이딩
            w[0][0] <= w[0][1]; w[0][1] <= w[0][2]; w[0][2] <= line_buf0[x_in];
            w[1][0] <= w[1][1]; w[1][1] <= w[1][2]; w[1][2] <= line_buf1[x_in];
            w[2][0] <= w[2][1]; w[2][1] <= w[2][2]; w[2][2] <= d_in;
            
            x_d  <= x_in;
            y_d  <= y_in;
            de_d <= de_in;
        end else begin
            de_d <= 1'b0; // Valid 신호가 없으면 de_d를 내려서 오작동 방지
        end
    end

    // 2단계: 필터링 연산 결과 출력 (추가 1 clk 지연, 총 2 clk)
    always_ff @(posedge clk) begin
        if (reset) begin
            de_out <= 1'b0;
            x_out  <= '0;
            y_out  <= '0;
            d_out  <= 1'b0;
        end else begin
            de_out <= de_d;
            x_out  <= x_d;
            y_out  <= y_d;
            // 3x3 윈도우 내 픽셀 합이 5 이상이면 1 (이진 영상 가우시안/미디언 효과)
            d_out  <= (w[0][0] + w[0][1] + w[0][2] +
                       w[1][0] + w[1][1] + w[1][2] +
                       w[2][0] + w[2][1] + w[2][2] >= 5);
        end
    end
endmodule