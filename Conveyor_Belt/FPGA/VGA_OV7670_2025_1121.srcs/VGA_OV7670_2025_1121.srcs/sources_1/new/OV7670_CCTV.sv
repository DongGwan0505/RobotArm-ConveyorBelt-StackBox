`timescale 1ns / 1ps


module OV7670_CCTV (
    input  logic       clk,
    input  logic       reset,
    // ov7670 side
    output logic       xclk,
    input  logic       pclk,
    input  logic       href,
    input  logic       vsync,
    input  logic [7:0] data,
    input  logic       IR_tick,
    // vga port
    output logic       h_sync,
    output logic       v_sync,
    output logic [3:0] r_port,
    output logic [3:0] g_port,
    output logic [3:0] b_port,
    output logic       SCL,
    inout  logic       SDA,
    input  logic       btn,
    output logic [7:0] led,
    output             tx_stm,
    output             tx_pc
);

    logic        sys_clk;
    logic        DE;
    logic [ 9:0] x_pixel;
    logic [ 9:0] y_pixel;
    logic [16:0] rAddr;
    logic [15:0] rData;
    logic        we;
    logic [16:0] wAddr;
    logic [15:0] wData;

    logic        o_btn;
    logic [ 7:0] shape;
    logic        done;
    logic [ 3:0] w_img_read_r;
    logic [ 3:0] w_img_read_g;
    logic [ 3:0] w_img_read_b;
    logic        w_bin_out;
    logic        w_h_sync;
    logic        w_v_sync;
    logic [ 9:0] w_dist_x;
    logic [ 9:0] w_dist_y;
    logic        rx_done;
    logic [ 7:0] rx_data;

    // 디바운스용 싱크로나이저
    logic btn_sync_1, btn_sync_2;
    logic rx_sync_1, rx_sync_2, rx_sync_3;
    logic tx;

    // clk_100 도메인
    // logic [3:0] ext_cnt = 0;
    // logic rx_done_extended;

    // always_ff @(posedge clk) begin
    //     if (rx_done) begin
    //         ext_cnt <= 4'd8; // 25MHz 클럭 주기보다 훨씬 길게(80ns) 늘림
    //     end else if (ext_cnt > 0) begin
    //         ext_cnt <= ext_cnt - 1;
    //     end
    // end

    logic IR_sync_1, IR_sync_2, IR_sync_3;

    always_ff @(posedge sys_clk) begin
        if (reset) begin
            btn_sync_1 <= 0;
            btn_sync_2 <= 0;
            IR_sync_1  <= 0;
            IR_sync_2  <= 0;
            IR_sync_3  <= 0;
        end else begin
            // 버튼은 길게 눌리니까 기존 방식 유지
            btn_sync_1 <= o_btn;
            btn_sync_2 <= btn_sync_1;

            // rx_done은 짧으니까 토글 신호를 받아서 동기화
            IR_sync_1  <= IR_tick;
            IR_sync_2  <= IR_sync_1;  // 메타스테이빌리티 안정화
            IR_sync_3  <= IR_sync_2;
        end
    end




    assign xclk   = sys_clk;
    assign tx_pc  = tx;
    assign tx_stm = tx;

    SCCB_intf U_SCCB_TOP (
        .clk  (clk),
        .reset(reset),
        .SCL  (SCL),
        .SDA  (SDA)
    );

    pixel_clk_gen U_PXL_CLK_GEN (
        .clk  (clk),
        .reset(reset),
        .pclk (sys_clk)
    );

    VGA_Syncher U_VGA_Syncher (
        .clk    (sys_clk),
        .reset  (reset),
        .h_sync (w_h_sync),
        .v_sync (w_v_sync),
        .DE     (DE),
        .x_pixel(x_pixel),
        .y_pixel(y_pixel)
    );

    ImgMemReader U_IMG_Reader (
        .DE     (DE),
        .x_pixel(x_pixel),
        .y_pixel(y_pixel),
        .addr   (rAddr),
        .imgData(rData),
        .r_port (w_img_read_r),
        .g_port (w_img_read_g),
        .b_port (w_img_read_b)
    );


    logic [9:0] hsync_delay, vsync_delay;

    always_ff @(posedge sys_clk or posedge reset) begin
        if (reset) begin
            // 10비트 모두 1로 초기화 (Sync는 Active Low이므로)
            hsync_delay <= 10'h3FF;
            vsync_delay <= 10'h3FF;
        end else begin
            // 시프트 레지스터: 새 신호(w_h_sync)를 가장 낮은 비트에 넣고 위로 밀어냄
            hsync_delay <= {hsync_delay[8:0], w_h_sync};
            vsync_delay <= {vsync_delay[8:0], w_v_sync};
        end
    end

    Red_Binary_Filter U_Binary_Fillter (
        .r_in   (w_img_read_r),
        .g_in   (w_img_read_g),
        .b_in   (w_img_read_b),
        .bin_out(w_bin_out)      // 빨간색이면 1, 아니면 0
    );

    logic w_gau_de, w_gau_data;
    logic [9:0] w_gau_x, w_gau_y;

    Gaussian_filter U_Gaussian_Fillter (
        .clk   (sys_clk),
        .reset (reset),
        .de_in (DE),
        .x_in  (x_pixel),
        .y_in  (y_pixel),
        .d_in  (w_bin_out),
        .de_out(w_gau_de),
        .x_out (w_gau_x),
        .y_out (w_gau_y),
        .d_out (w_gau_data)
    );
    logic w_sobel_de, w_sobel_data;
    logic [9:0] w_sobel_x, w_sobel_y;
    logic [9:0] w_ero_x, w_ero_y, w_ero_de;
    logic w_ero_data;

    logic [9:0] w_ero_x1, w_ero_y1, w_ero_de1;
    logic w_ero_data1;

    logic [9:0] w_dil_x, w_dil_y, w_dil_de;
    logic w_dil_data;

    logic [9:0] w_dil_x1, w_dil_y1, w_dil_de1;
    logic w_dil_data1;

    // 2. 침식 모듈 (추가) - 100x7 잡음을 깎기 시작
    Erosion_3x3 u_erosion (
        .clk   (sys_clk),
        .reset (reset),
        .de_in (w_gau_de),
        .x_in  (w_gau_x),
        .y_in  (w_gau_y),
        .d_in  (w_gau_data),
        .de_out(w_ero_de),
        .x_out (w_ero_x),
        .y_out (w_ero_y),
        .d_out (w_ero_data)
    );

    Erosion_3x3 u_erosion_1 (
        .clk   (sys_clk),
        .reset (reset),
        .de_in (w_ero_de),
        .x_in  (w_ero_x),
        .y_in  (w_ero_y),
        .d_in  (w_ero_data),
        .de_out(w_ero_de1),
        .x_out (w_ero_x1),
        .y_out (w_ero_y1),
        .d_out (w_ero_data1)
    );

    // 3. 팽창 모듈 (추가) - 깎인 본체를 복구
    Dilation_3x3 u_dilation (
        .clk   (sys_clk),
        .reset (reset),
        .de_in (w_ero_de1),
        .x_in  (w_ero_x1),
        .y_in  (w_ero_y1),
        .d_in  (w_ero_data1),
        .de_out(w_dil_de),
        .x_out (w_dil_x),
        .y_out (w_dil_y),
        .d_out (w_dil_data)
    );

    Dilation_3x3 u_dilation_1 (
        .clk   (sys_clk),
        .reset (reset),
        .de_in (w_dil_de),
        .x_in  (w_dil_x),
        .y_in  (w_dil_y),
        .d_in  (w_dil_data),
        .de_out(w_dil_de1),
        .x_out (w_dil_x1),
        .y_out (w_dil_y1),
        .d_out (w_dil_data1)
    );

    // 팽창 후 나온 가우시안 결과를 엣지필터와의 싱크를 맞추기 위해 두클럭 늦춘 쉬프트 레지스터
    logic [1:0] area_delay_reg;
    logic final_area_for_stats;

    // 2. w_dil_data1을 2클럭 지연시킴
    always_ff @(posedge sys_clk) begin
        if (reset) begin
            area_delay_reg <= 2'b0;
        end else begin
            area_delay_reg <= {area_delay_reg[0], w_dil_data1};
        end
    end

    assign final_area_for_stats = area_delay_reg[1];

    Morphological_Boundary_Extraction U_Morphological_Boundary_Extraction (
        .clk   (sys_clk),
        .reset (reset),
        .de_in (w_dil_de1),
        .x_in  (w_dil_x1),
        .y_in  (w_dil_y1),
        .d_in  (w_dil_data1),  // 가우시안 필터의 출력 (1비트)
        .de_out(w_sobel_de),
        .x_out (w_sobel_x),
        .y_out (w_sobel_y),
        .d_out (w_sobel_data)   // 최종 엣지 데이터 (1비트)
    );


    // 3. 2번 거친 최종 데이터
    assign final_area_for_stats = area_delay_reg[1];

    logic [19:0] w_area_cnt, w_edge_cnt;
    logic [9:0] w_box_w, w_box_h;
    logic       w_frame_done;
    logic [1:0] w_shape_result;
    logic       display_area;

    Shape_Stats_Collector U_Stats_Collect (
        .clk(sys_clk),
        .reset(reset),
        .vsync(vsync_delay[9]),  // 카메라 vsync (프레임 구분)
        .de_in(display_area),  // 필터 거친 Valid 신호
        .d_area_in(area_delay_reg[1]),  // 가우시안 결과 (면적 픽셀)
        .d_edge_in(w_sobel_data),  // 소벨 결과 (엣지 픽셀)
        .x_in(w_sobel_x),
        .y_in(w_sobel_y),
        .btn_capture(btn_sync_2),
        .rx_done_capture(rx_sync_3),
        .rx_data(rx_data),
        .IR_done(IR_sync_3),
        .area_cnt(w_area_cnt),  // 면적 총합
        .edge_cnt(w_edge_cnt),  // 둘레 총합
        .box_w(w_box_w),
        .box_h(w_box_h),  // 바운딩 박스 가로/세로
        .frame_done(w_frame_done),  // 한 프레임 수집 완료 신호
        .state_led(led[7:4]),
        .dist_x(w_dist_x),
        .dist_y(w_dist_y)
    );

    Shape_Classifier U_Shape_Classifier (
        .clk          (sys_clk),
        .reset         (reset),
        .area_cnt      (w_area_cnt),    // Collector에서 받은 면적
        .edge_cnt      (w_edge_cnt),    // Collector에서 받은 둘레
        .box_w         (w_box_w),
        .box_h         (w_box_h),       // 바운딩 박스 크기
        .dist_x        (w_dist_x),
        .dist_y        (w_dist_y),
        .frame_done    (w_frame_done),  // Collector의 완료 신호 (Trig)
        .shape_type    (shape),
        .led_out       (led[3:0]),      // 결과 표시용 LED
        .shape_done_out(done)
    );

    // assign final_DE = w_sobel_de;
    // assign {r_port, g_port, b_port} = w_sobel_data ? 12'hFFF : 12'h000;

    frame_buffer U_Frame_Buffer (
        .wclk (sys_clk),
        .we   (we),
        .wAddr(wAddr),
        .wData(wData),
        .rclk (sys_clk),
        .oe   (1'b1),
        .rAddr(rAddr),
        .rData(rData)
    );

    // x 좌표가 320 미만이고, 최종 소벨 DE가 High일 때만 출력

    assign display_area = w_sobel_de && (w_sobel_x < 320);

    // 최종 VGA 포트 연결
    assign h_sync = hsync_delay[9];  // 데이터 지연만큼 늦춰진 싱크
    assign v_sync = vsync_delay[9];

    assign r_port = display_area ? (w_sobel_data ? 4'hF : 4'h0) : 4'h0;
    assign g_port = display_area ? (w_sobel_data ? 4'hF : 4'h0) : 4'h0;
    assign b_port = display_area ? (w_sobel_data ? 4'hF : 4'h0) : 4'h0;

    OV7670_Mem_controller U_OV7670_Mem_Controller (
        .pclk (pclk),
        .reset(reset),
        .href (href),
        .vsync(vsync),
        .data (data),
        .we   (we),
        .wAddr(wAddr),
        .wData(wData)
    );


    // shape_detector U_SHAPE_DETECTOR (
    //     .clk           (clk),
    //     .wclk          (pclk),   // PCLK
    //     .vsync         (vsync),  // 카메라 VSYNC
    //     .href          (href),   // 카메라 HREF (we 대신 사용)
    //     .wData         (wData),  // RGB565 데이터
    //     .btn_capture   (o_btn),  // 캡쳐 버튼 (디바운싱 권장)
    //     .final_shape   (shape),  // 0:None, 1:Tri, 2:Sqr, 3:Cir
    //     .state_led     (led),    // 상태 확인용 LED
    //     .shape_done_out(done)
    // );


    button_debounce U_BUTTON_DEBOUNCE (
        .clk  (clk),
        .rst  (reset),
        .i_btn(btn),
        .o_btn(o_btn)
    );

    uart_top U_UART (
        .clk(clk),
        .rst(reset),
        .tx_data(shape),
        .tx_start(done),
        .tx(tx)
    );
endmodule

module button_debounce (
    input  clk,
    input  rst,
    input  i_btn,
    output o_btn
);

    // 100M -> 1M
    reg [$clog2(100)-1:0] counter_reg;
    reg clk_reg;
    reg [7:0] q_reg, q_next;
    reg  edge_reg;
    wire debounce;

    //clock divider
    always @(posedge clk, posedge rst) begin
        if (rst) begin
            counter_reg <= 0;
            clk_reg     <= 1'b0;
        end else begin
            if (counter_reg == 99) begin
                counter_reg <= 0;
                clk_reg     <= 1'b1;
            end else begin
                counter_reg <= counter_reg + 1;
                clk_reg     <= 1'b0;
            end
        end
    end


    //debounce, shift register
    always @(posedge clk_reg, posedge rst) begin
        if (rst) begin
            q_reg <= 0;
        end else begin
            q_reg <= q_next;
        end
    end


    // serial input, parallel shift resister
    always @(*) begin
        q_next = {i_btn, q_reg[7:1]};

    end

    // 4 input AND
    assign debounce = &q_next;  // And Gate about all 4bit



    // Q5 output
    always @(posedge clk, posedge rst) begin
        if (rst) begin
            edge_reg <= 1'b0;

        end else begin
            edge_reg <= debounce;
        end

    end

    //edge output
    assign o_btn = ~edge_reg & debounce;


endmodule
