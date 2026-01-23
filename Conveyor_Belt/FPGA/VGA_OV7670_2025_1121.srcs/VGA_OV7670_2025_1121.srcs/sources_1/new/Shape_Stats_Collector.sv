`timescale 1ns / 1ps
module Shape_Stats_Collector (
    input logic       clk,
    input logic       reset,
    input logic       vsync,            // 카메라 vsync (프레임 구분)
    input logic       de_in,            // 필터 거친 Valid 신호
    input logic       d_area_in,        // 가우시안 결과 (면적 픽셀)
    input logic       d_edge_in,        // 소벨 결과 (엣지 픽셀)
    input logic [9:0] x_in,
    input logic [9:0] y_in,
    input logic       btn_capture,
    input logic       rx_done_capture,
    input logic [7:0] rx_data,
    input logic IR_done,

    output logic [19:0] area_cnt,    // 면적 총합
    output logic [19:0] edge_cnt,    // 둘레 총합
    output logic [ 9:0] box_w,
    output logic [ 9:0] box_h,       // 바운딩 박스 가로/세로
    output logic        frame_done,  // 한 프레임 수집 완료 신호
    output logic [ 3:0] state_led,
    output logic [ 9:0] dist_x,      // 추가: 중심 이탈 거리 X
    output logic [ 9:0] dist_y       // 추가: 중심 이탈 거리 Y
);
    logic [9:0] min_x, max_x, min_y, max_y;
    logic [29:0]
        sum_x, sum_y;  // 추가: 좌표 누적합 (충분히 큰 비트수)
    logic prev_vsync;
    logic prev_rx_done;
    logic prev_IR_done;

    typedef enum logic [1:0] {
        IDLE,
        WAIT_START,
        COLLECT,
        DONE
    } state_t;
    state_t state;

    always_ff @(posedge clk) begin
        prev_vsync <= vsync;
        prev_rx_done <= rx_done_capture;
        prev_IR_done <= IR_done;
    end


    // Rising/Falling Edge 검출
    wire vsync_rising = (vsync && !prev_vsync);
    wire vsync_falling = (!vsync && prev_vsync);
    wire rx_done_rising = (rx_done_capture && !prev_rx_done);
    wire IR_done_rising = (IR_done && !prev_IR_done);

    always_ff @(posedge clk) begin
        if (reset) begin
            state <= IDLE;
            frame_done <= 0;
        end else begin
            case (state)
                // 1. 대기: 버튼을 누르면 다음 프레임의 시작을 기다리러 감
                IDLE: begin
                    state_led  <= 4'b0001;
                    frame_done <= 0;
                    if (btn_capture || IR_done_rising) state <= WAIT_START;
                end

                // 2. 싱크 대기: VSYNC가 올라오는(프레임 시작) 순간 리셋하고 수집 시작
                WAIT_START: begin
                    state_led <= 4'b0010;
                    if (vsync_rising) begin
                        area_cnt <= 0;
                        edge_cnt <= 0;
                        min_x <= 320;
                        max_x <= 0;
                        min_y <= 240;
                        max_y <= 0;
                        state <= COLLECT;
                    end
                end

                // 3. 수집: VSYNC가 떨어질 때(프레임 끝)까지 데이터를 꽉 채움
                COLLECT: begin
                    state_led <= 4'b0100;
                    if (de_in) begin
                        if (d_area_in) begin
                            area_cnt <= area_cnt + 1;
                            sum_x <= sum_x + x_in;  // 추가: X좌표 누적
                            sum_y <= sum_y + y_in;  // 추가: Y좌표 누적

                            if (x_in < min_x) min_x <= x_in;
                            if (x_in > max_x) max_x <= x_in;
                            if (y_in < min_y) min_y <= y_in;
                            if (y_in > max_y) max_y <= y_in;
                        end
                        if (d_edge_in) edge_cnt <= edge_cnt + 1;
                    end

                    if (vsync_falling) begin
                        // 1. 바운딩 박스 크기 계산
                        box_w <= (max_x > min_x) ? (max_x - min_x) : 0;
                        box_h <= (max_y > min_y) ? (max_y - min_y) : 0;

                        // 2. 무게중심 및 중심 이탈 거리 계산 (추가)
                        // 나눗셈을 직접 하기보다, 판별기에서 곱셈으로 비교하는게 좋지만
                        // 여기서 미리 계산해두면 편리합니다.
                        if (area_cnt > 0) begin
                            logic [9:0] center_x, center_y;
                            logic [9:0] mid_x, mid_y;

                            center_x = sum_x / area_cnt;
                            center_y = sum_y / area_cnt;
                            mid_x = (min_x + max_x) >> 1;
                            mid_y = (min_y + max_y) >> 1;

                            dist_x <= (center_x > mid_x) ? (center_x - mid_x) : (mid_x - center_x);
                            dist_y <= (center_y > mid_y) ? (center_y - mid_y) : (mid_y - center_y);
                        end

                        frame_done <= 1;
                        state <= DONE;
                    end
                end
                // 4. 완료: 판별기가 데이터를 가져갈 수 있게 한 클럭 신호를 주고 다시 IDLE로
                DONE: begin
                    state_led <= 4'b1000;
                    frame_done <= 0;
                    state <= IDLE;
                end
            endcase
        end
    end

    // always @(posedge clk) begin
    //     frame_done_reg  <= frame_done;
    //     frame_done_next <= frame_done_reg;
    // end

    // assign frame_done_out = frame_done_reg & !frame_done_next;
endmodule
