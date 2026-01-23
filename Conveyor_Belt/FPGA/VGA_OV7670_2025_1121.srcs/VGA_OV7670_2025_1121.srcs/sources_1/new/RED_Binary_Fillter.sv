`timescale 1ns / 1ps
module Red_Binary_Filter (
    input  logic [3:0] r_in,
    input  logic [3:0] g_in,
    input  logic [3:0] b_in,
    output logic       bin_out  // 빨간색이면 1, 아니면 0
);

    assign bin_out = (r_in > 4'd5) &&           // 1. 최소 밝기 대폭 완화 (8 -> 5)
        (r_in > g_in) &&  // 2. 일단 R이 G보다는 커야 함 (기본)
        (r_in > b_in) &&  // 3. 일단 R이 B보다는 커야 함 (기본)
        (r_in > (g_in + 4'd2)) &&  // 4. 차이 조건 완화 (5 -> 2)
        (r_in > (b_in + 4'd2)) &&  // 5. 차이 조건 완화 (4 -> 2)
        (g_in < 4'd9) &&          // 6. G가 너무 높으면 연주황/흰색이므로 상한선 설정
        (b_in < 4'd9);  // 7. B가 너무 높으면 보라/흰색 방지
    // assign bin_out = (r_in > 4'd6) &&           
    //              (r_in > g_in) &&  
    //              (r_in > b_in) &&  
    //              // [수정 1] R과 G의 차이를 2에서 3으로 살짝 강화
    //              // 살색이나 나무색은 G가 R을 꽤 따라오기 때문에, 
    //              // 차이를 3으로 벌리면 이들이 먼저 탈락합니다.
    //              (r_in > (g_in + 4'd3)) &&  
    //              (r_in > (b_in + 4'd3)) &&  
    //              // [수정 2] G의 상한선을 10에서 7~8로 하향
    //              // 나무색이나 밝은 살색은 G 성분이 8~10까지 올라갑니다. 
    //              // 이걸 억제하면 붉은색만 남습니다.
    //              (g_in < 4'd10) &&           
    //              (b_in < 4'd10);
endmodule
