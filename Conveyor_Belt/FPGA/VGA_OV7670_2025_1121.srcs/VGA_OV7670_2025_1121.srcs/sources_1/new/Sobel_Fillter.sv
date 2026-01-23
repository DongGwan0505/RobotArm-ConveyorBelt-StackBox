`timescale 1ns / 1ps

module Morphological_Boundary_Extraction (
    input logic       clk,
    input logic       reset,
    input logic       de_in,
    input logic [9:0] x_in,
    input logic [9:0] y_in,
    input logic       d_in,   // 가우시안 필터의 출력 (1비트)

    output logic       de_out,
    output logic [9:0] x_out,
    output logic [9:0] y_out,
    output logic       d_out    // 최종 엣지 데이터 (1비트)
);
    localparam int H_RES = 320;

    // 1비트 라인 버퍼 (2줄 저장)
    logic [0:H_RES-1] line_buf0;
    logic [0:H_RES-1] line_buf1;
    logic [0:2][0:2] w;  // 3x3 윈도우

    // 파이프라인 지연용 신호
    logic [9:0] x_d, y_d;
    logic de_d;

    // 1. 라인 버퍼 및 3x3 윈도우 구성
    always_ff @(posedge clk) begin
        if (reset) begin
            de_d <= 1'b0;
        end else if (de_in && (x_in < 320)) begin
            // 데이터 시프트
            line_buf0[x_in] <= line_buf1[x_in];
            line_buf1[x_in] <= d_in;

            // 윈도우 구성
            w[0][0] <= w[0][1];
            w[0][1] <= w[0][2];
            w[0][2] <= line_buf0[x_in];
            w[1][0] <= w[1][1];
            w[1][1] <= w[1][2];
            w[1][2] <= line_buf1[x_in];
            w[2][0] <= w[2][1];
            w[2][1] <= w[2][2];
            w[2][2] <= d_in;

            // 좌표 및 제어신호 1단계 지연
            x_d <= x_in;
            y_d <= y_in;
            de_d <= de_in;
        end else begin
            de_d <= 1'b0;
        end
    end

    // 2. Sobel 커널 연산 (Gx, Gy)
    // 입력이 1비트이므로 연산 결과는 매우 작은 범위 내에서 나옵니다.
    // logic signed [3:0] gx, gy;
    // logic [3:0] abs_mag;

    // always_comb begin
    //     // Gx = [-1 0 1; -2 0 2; -1 0 1]
    //     gx = $signed({3'b0, w[0][2]}) - $signed({3'b0, w[0][0]}) +
    //          ($signed({3'b0, w[1][2]}) <<< 1) - ($signed({3'b0, w[1][0]}) <<< 1) +
    //          $signed({3'b0, w[2][2]}) - $signed({3'b0, w[2][0]});

    //     // Gy = [-1 -2 -1; 0 0 0; 1 2 1]
    //     gy = $signed({3'b0, w[0][0]}) + ($signed({3'b0, w[0][1]}) <<< 1) + $signed({3'b0, w[0][2]}) -
    //          ($signed({3'b0, w[2][0]}) + ($signed({3'b0, w[2][1]}) <<< 1) + $signed({3'b0, w[2][2]}));

    //     // 절대값 합산 (근사치)
    //     abs_mag = (gx[3] ? -gx : gx) + (gy[3] ? -gy : gy);
    // end

    // // 3. 최종 출력 (2단계 지연 완료)
    // always_ff @(posedge clk) begin
    //     if (reset) begin
    //         de_out <= 1'b0;
    //         d_out  <= 1'b0;
    //     end else begin
    //         de_out <= de_d;
    //         x_out  <= x_d;
    //         y_out  <= y_d;
    //         // 이진 영상에서는 미세한 차이만 나도 엣지로 간주 (임계값 1)
    //         d_out  <= (abs_mag >= 4); 
    //     end
    // end

    // Sobel 커널 연산 대신 아래 코드를 넣으세요.
    logic edge_detected;

    always_comb begin
        // 핵심 원리: 중앙 픽셀(w[1][1])이 '물체(1)'인데, 
        // 상하좌우 중 하나라도 '배경(0)'이면 그곳이 바로 엣지입니다.
        // 이 방식은 소벨보다 선이 가늘고 선명하게 나옵니다.
        if (w[1][1] == 1'b1) begin
            if (w[0][1] == 1'b0 || w[2][1] == 1'b0 || 
                w[1][0] == 1'b0 || w[1][2] == 1'b0)
                edge_detected = 1'b1;
            else edge_detected = 1'b0;
        end else begin
            edge_detected = 1'b0;
        end
    end

    // 최종 출력 할당부
    always_ff @(posedge clk) begin
        if (reset) begin
            de_out <= 1'b0;
            d_out  <= 1'b0;
        end else begin
            de_out <= de_d;
            x_out  <= x_d;
            y_out  <= y_d;
            d_out  <= edge_detected;  // 계산된 엣지 신호 출력
        end
    end

endmodule
