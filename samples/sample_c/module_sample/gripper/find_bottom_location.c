// #include <stdio.h>
// #include <math.h>

// int image_width = 240*2;
// int image_height = 310*2;
// float pxl2cm = 75.0/204.0;

// //Input_variables
// int x_pxld = 224, y_pxl_noflipd=120;


// int calculate_pos(int x_pxl,int y_pxl, float arr[]);

// int main(){

//     float arr[2];

//     calculate_pos(x_pxld,y_pxl_noflipd,arr);
//     printf("X \n");
//     printf("%.6f\n", arr[0]);//X in cm
//     printf("y \n");
//     printf("%.6f", arr[1]); //Y in cm

//     return 0;
// }

// int calculate_pos(int x_pxl,int y_pxl, float arr[]){
    
//     int y_pxl2 = image_height - y_pxl_noflipd;

//     if(y_pxl2<image_height/2){
//         printf("Image in under-half");
//         return 0;
        
//     }
//     else{

//         int imag2circle_center_x = image_width/2 - x_pxl;
    
//         int imag2circle_center_y = (image_height/4 - y_pxl2)*-1;

//         double x_pos = (float)imag2circle_center_x*pxl2cm/100;
//         double y_pos = (float)imag2circle_center_y * pxl2cm/100; 
    
//         arr[0] = x_pos; 
//         arr[1]= y_pos;
//         return 0;
//     }

// }
