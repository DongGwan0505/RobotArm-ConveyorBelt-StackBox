module shape_detector (
    input  wire        clk,
    input  wire        wclk,            // PCLK
    input  wire        vsync,           // 카메라 VSYNC
    input  wire        href,            // 카메라 HREF (we 대신 사용)
    input  wire [15:0] wData,           // RGB565 데이터
    input  wire        btn_capture,     // 캡쳐 버튼 (디바운싱 권장)
    output reg  [ 1:0] final_shape,     // 0:None, 1:Tri, 2:Sqr, 3:Cir
    output reg  [ 3:0] state_led,       // 상태 확인용 LED
    output reg         shape_done_out,
    output reg  [15:0] outData
);

    // FSM 상태 정의 (Parameter 사용)
    parameter IDLE = 2'b00;
    parameter WAIT_FRAME = 2'b01;
    parameter ANALYZE = 2'b10;
    parameter RESULT = 2'b11;

    typedef enum {
        BTN_IDLE,
        BTN_WAIT
    } btn_state_t;

    btn_state_t btn_state;

    reg [1:0] state = IDLE;

    reg shape_done_reg, shape_done_next;
    reg shape_done;
    reg btn_capture_out;
    // 좌표 및 특징 추출용 레지스터
    reg [9:0] x_cnt, y_cnt;
    reg [18:0] pixel_count;
    reg [9:0] x_min, x_max, x_min_reg, x_max_reg;
    reg [8:0] y_min, y_max, y_min_reg, y_max_reg;
    reg [9:0] max_width, curr_line_width;
    reg   [ 8:0] max_width_y;

    logic [24:0] sum_x;
    logic [24:0] sum_y;

    // 계산용 중간 레지스터 (automatic 대신 선언)
    reg [9:0] W, H;
    reg [18:0] box_area;
    reg [ 9:0] center_y;
    reg [ 9:0] box_center_y;
    reg [19:0] area;
    reg [24:0] ratio;

    reg [18:0] top_pixels;  // 상단 절반 영역의 픽셀 수
    reg [9:0]
        width_q1, width_q2, width_q3;  // 1/4, 2/4, 3/4 지점의 가로폭

    // 색상 필터링 (빨간색 기준)
    wire is_target;
    assign is_target = (wData[15:11] > 5'd11) && (wData[10:5] < 6'd22);

    reg prev_vsync;
    always @(posedge wclk) prev_vsync <= vsync;

    wire frame_done = (!prev_vsync && vsync);


    wire is_border = ((x_cnt >= x_min_reg && x_cnt <= x_max_reg) && (y_cnt == y_min_reg || y_cnt == y_max_reg)) ||
                     ((y_cnt >= y_min_reg && y_cnt <= y_max_reg) && (x_cnt == x_min_reg || x_cnt == x_max_reg));

    always @(posedge wclk) begin
        case (state)
            IDLE: begin
                shape_done <= 0;
                state_led  <= 4'b0001;
                if (btn_capture_out) state <= WAIT_FRAME;
            end

            WAIT_FRAME: begin
                state_led <= 4'b0010;
                if (vsync) begin  // VSYNC 시작 시 초기화
                    pixel_count <= 0;
                    sum_x <= 0;
                    sum_y <= 0;
                    x_min <= 319;
                    x_max <= 0;
                    y_min <= 239;
                    y_max <= 0;
                    x_cnt <= 0;
                    y_cnt <= 0;
                    state <= ANALYZE;
                end
            end
            ANALYZE: begin
                state_led <= 4'b0100;
                if (href) begin
                    x_cnt <= x_cnt + 1;
                    if (is_target) begin
                        pixel_count <= pixel_count + 1;
                        if (x_cnt < x_min) x_min <= x_cnt;
                        if (x_cnt > x_max) x_max <= x_cnt;
                        if (y_cnt < y_min) y_min <= y_cnt;
                        if (y_cnt > y_max) y_max <= y_cnt;
                        curr_line_width <= curr_line_width + 1;
                    end
                end else begin
                    if (x_cnt > 0) begin
                        x_cnt <= 0;
                        y_cnt <= y_cnt + 1;
                        // 특정 지점의 가로폭 샘플링 (H와 y_min은 이전 프레임 값 사용)
                        if (y_cnt == (y_min_reg + (H >> 2)))
                            width_q1 <= curr_line_width;  // 25% 지점
                        if (y_cnt == (y_min_reg + (H >> 1)))
                            width_q2 <= curr_line_width;  // 50% 지점
                        if (y_cnt == (y_min_reg + (H * 3 >> 2)))
                            width_q3 <= curr_line_width;  // 75% 지점
                        curr_line_width <= 0;
                    end
                    if (frame_done && pixel_count > 400) begin
                        x_min_reg <= x_min;
                        x_max_reg <= x_max;
                        y_min_reg <= y_min;
                        y_max_reg <= y_max;
                        H <= (y_max > y_min) ? (y_max - y_min + 1) : 1;
                        W <= (x_max > x_min) ? (x_max - x_min + 1) : 1;
                        state <= RESULT;
                    end
                end
            end

            RESULT: begin
                state_led <= 4'b1000;
                box_area = W * H;

                // 1. 노이즈 컷
                if (pixel_count < 500) begin
                    final_shape <= 2'b00;
                end else begin
                    // [핵심 로직] 폭의 비율 분석
                    // 삼각형 판별: 위쪽 폭이 아래쪽보다 2배 이상 크거나(역세모), 작을 때(정세모)
                    if ((width_q1 > (width_q3 << 1)) || (width_q3 > (width_q1 << 1))) begin
                        final_shape <= 2'b01;  // [삼각형]
                    end
                    // 원 판별: 중간폭이 상하폭보다 확실히 클 때 (둥근 모양)
                    else if ((width_q2 > width_q1 + 5) && (width_q2 > width_q3 + 5)) begin
                        // 점유율이 너무 높으면 원, 적당하면 사각형
                        if ((pixel_count << 8) > (box_area * 210))
                            final_shape <= 2'b11;  // [원]
                        else final_shape <= 2'b10;  // [사각형]
                    end  // 그 외: 폭이 일정하게 유지되면 사각형
                    else begin
                        final_shape <= 2'b10;  // [사각형]
                    end
                end

                shape_done <= 1;
                state <= IDLE;
            end
        endcase
    end

    always @(posedge clk) begin
        shape_done_reg  <= shape_done;
        shape_done_next <= shape_done_reg;
    end

    assign shape_done_out = shape_done_reg & !shape_done_next;

    always_ff @(posedge clk) begin
        case (btn_state)
            BTN_IDLE:
            if (btn_capture) begin
                btn_state <= BTN_WAIT;
                btn_capture_out <= 1;
            end
            BTN_WAIT:
            if (state != IDLE) begin
                btn_state <= BTN_IDLE;
                btn_capture_out <= 0;
            end
        endcase
    end
endmodule
