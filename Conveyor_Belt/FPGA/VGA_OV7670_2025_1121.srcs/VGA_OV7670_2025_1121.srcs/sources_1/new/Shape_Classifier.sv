// `timescale 1ns / 1ps

// module Shape_Classifier (
//     input logic        clk,
//     input logic        reset,
//     input logic [19:0] area_cnt,   // Collector에서 받은 면적
//     input logic [19:0] edge_cnt,   // Collector에서 받은 둘레
//     input logic [ 9:0] box_w,
//     input logic [ 9:0] box_h,      // 바운딩 박스 크기
//     input logic        frame_done, // Collector의 완료 신호 (Trig)
//     input logic [9:0] dist_x,
//     input logic [9:0] dist_y,

//     output logic [ 1:0] shape_type, // 0:None, 1:Tri, 2:Sqr, 3:Cir
//     output logic [ 3:0] led_out     // 결과 표시용 LED
// );

//     // 나눗셈 대신 곱셈을 쓰기 위한 둘레 제곱 계산
//     // 20bit * 20bit = 40bit (넉넉하게)
//     logic [39:0] edge_sq;
//     assign edge_sq = edge_cnt * edge_cnt;

//     logic [31:0] box_area;
//     assign box_area = box_w * box_h;

// always_ff @(posedge clk or posedge reset) begin
//     if (reset) begin
//         shape_type <= 2'b00;
//         led_out    <= 4'b0000;
//     end 
//     else if (frame_done) begin
//     // 1. 노이즈 제거
//     if (area_cnt < 300) begin
//         shape_type <= 2'b00;
//         led_out    <= 4'b0000;
//     end 

//     // 2. [강력한 사각형/마름모 조건] 
//     // 점유율이 45%를 넘으면서 무게중심이 어느 정도 중앙(박스의 15% 이내)에 있다면 사각형/마름모입니다.
//     // 마름모는 무게중심이 중앙에 있어야 한다는 점을 이용합니다.
//     else if ((area_cnt * 10 > box_area * 4) && (dist_x * 7 <= box_w) && (dist_y * 7 <= box_h)) begin
//         shape_type <= 2'b10; // 사각형 (마름모 포함)
//         led_out    <= 4'b0100;
//     end

//     // 3. [삼각형 조건] 
//     // 점유율이 사각형보다 낮거나 비슷하더라도, 무게중심이 확실히 한쪽으로 쏠려 있어야 합니다.
//     // 마름모가 삼각형으로 인식된다면 여기서 dist 기준을 더 키우세요 (*5 -> *4 or *3)
//     else if ((dist_x * 4 > box_w) || (dist_y * 4 > box_h)) begin
//         shape_type <= 2'b01; // 삼각형
//         led_out    <= 4'b0010; 
//     end

//     // 4. [원 판별]
//     else if (edge_sq < area_cnt * 18) begin
//         shape_type <= 2'b11; // 원
//         led_out    <= 4'b1000;
//     end

//     else begin
//         // 기본값 사각형
//         shape_type <= 2'b10;
//         led_out    <= 4'b0100;
//     end
// end
// end
// endmodule
`timescale 1ns / 1ps

module Shape_Classifier (
    input logic        clk,
    input logic        reset,
    input logic [19:0] area_cnt,
    input logic [19:0] edge_cnt,
    input logic [ 9:0] box_w,
    input logic [ 9:0] box_h,
    input logic [ 9:0] dist_x,
    input logic [ 9:0] dist_y,
    input logic        frame_done,

    output logic [2:0] shape_type,  // 0:None, 1:Tri, 2:Sqr, 3:Cir
    output logic [3:0] led_out,
    output logic shape_done_out
);

    logic shape_done_reg, shape_done_next, shape_done;

    logic [39:0] edge_sq;
    assign edge_sq = edge_cnt * edge_cnt;

    logic [31:0] box_area;
    assign box_area = box_w * box_h;

    // 판별 보조 신호
    // 1. 점유율이 낮은가? (삼각형/마름모는 보통 55% 미만)
    logic is_low_occupancy;
    assign is_low_occupancy = (area_cnt * 100 < box_area * 55);

    // 2. 무게중심이 확실히 쏠렸는가? (직각삼각형의 치트키)
    // 박스 크기의 약 8~10% 이상 벗어나면 쏠린 것으로 간주
    logic is_centroid_offset;
    assign is_centroid_offset = (dist_x * 6 > box_w) || (dist_y * 6 > box_h);

    always_ff @(posedge clk or posedge reset) begin
        shape_done <= 0;
        if (reset) begin
            shape_type <= 3'b000;
            led_out    <= 4'b0000;
        end else if (frame_done) begin
            // 1. [노이즈 제거] 너무 작은 물체는 무시
            if (area_cnt < 400) begin
                shape_type <= 3'b000;
                led_out    <= 4'b0000;
            end 

        // 2. [원 판별] 기존 로직 유지 (가장 매끄러운 도형)
            else if (area_cnt * 100 > box_area * 60) begin
                // 계단 현상 대응: 회전된 사각형의 둘레 제곱값이 커질 수 있으므로 13 -> 11로 강화
                // (더 매끄러운 것만 원으로 인정하여 정사각형이 원으로 튀는 것 방지)
                if (edge_sq < area_cnt * 13) begin
                    shape_done <= 1;
                    shape_type <= 3'b010; // 원
                    led_out    <= 4'b1000;
                end else begin
                    shape_done <= 1;
                    shape_type <= 3'b001; // 사각형
                    led_out    <= 4'b0110;
                end
            end  // 3. [삼각형 vs 사각형 판별] 
                 // 핵심: 회전 시 수치가 변하는 dist 대신, 고유의 엣지 비율(edge_sq)로 결정
            else begin
                // 삼각형은 엣지가 길기 때문에 18.5를 기준으로 잡습니다.
                // (마름모는 보통 17~18 미만, 삼각형은 20 이상 나옵니다.)
                if ((edge_sq > area_cnt * 18) || is_centroid_offset) begin
                    shape_done <= 1;
                    shape_type <= 3'b100; // 삼각형
                    led_out    <= 4'b0010;
                end 
            
            // 엣지가 짧으면서 점유율이 일정 수준(45%) 이상이면 사각형/마름모
                else if (area_cnt * 100 > box_area * 45) begin
                    shape_done <= 1;
                    shape_type <= 3'b001; // 사각형 (마름모 포함)
                    led_out    <= 4'b1110;
                end  // 그 외 (너무 얇거나 깨진 도형)
                else begin
                    shape_done <= 1;
                    shape_type <= 3'b000;
                    led_out    <= 4'b0000;
                end
            end
        end
    end

    always @(posedge clk) begin
        shape_done_reg  <= shape_done;
        shape_done_next <= shape_done_reg;
    end

    assign shape_done_out = shape_done_reg & !shape_done_next;
endmodule
