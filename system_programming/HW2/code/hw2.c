// Assignment 2 - Efficient C Programming
// System Programming, DGIST, Prof. Yeseong Kim
// 
// YOU WILL TURN IN THIS FILE.
// Read the provided instruction carefully.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmplib.h"
#include "hw2.h"

// This implementation is simply copied from "main.c".
// Your job is to modify and optimize it for better performance!

void filter_optimized(void* args[]) {
    unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = args[2];
    Pixel* output = args[3];
    float* filter = args[4];
    int int_filter[9];

    int padded_img_size = (width+2) * (height+2) * sizeof(int);
    int* padded_r = (int *)malloc(padded_img_size);
    int* padded_g = (int *)malloc(padded_img_size);
    int* padded_b = (int *)malloc(padded_img_size);

    memset(padded_r, 0, padded_img_size);
    memset(padded_g, 0, padded_img_size);
    memset(padded_b, 0, padded_img_size);

    // make zero padded r, g, b channel img
    register int idx_o;
    register int idx_p;
    for (int px = 0; px < height; px++){
        for (int py = 0; py < width; py+=8){
            idx_o = py+px*width; // idx in orig img
            idx_p = idx_o+px+px+width+3;// idx in padded img

            padded_r[idx_p] = input[idx_o].r;
            padded_g[idx_p] = input[idx_o].g;
            padded_b[idx_p] = input[idx_o].b;
            padded_r[idx_p+1] = input[idx_o+1].r;
            padded_g[idx_p+1] = input[idx_o+1].g;
            padded_b[idx_p+1] = input[idx_o+1].b;
            padded_r[idx_p+2] = input[idx_o+2].r;
            padded_g[idx_p+2] = input[idx_o+2].g;
            padded_b[idx_p+2] = input[idx_o+2].b;
            padded_r[idx_p+3] = input[idx_o+3].r;
            padded_g[idx_p+3] = input[idx_o+3].g;
            padded_b[idx_p+3] = input[idx_o+3].b;
            padded_r[idx_p+4] = input[idx_o+4].r;
            padded_g[idx_p+4] = input[idx_o+4].g;
            padded_b[idx_p+4] = input[idx_o+4].b;
            padded_r[idx_p+5] = input[idx_o+5].r;
            padded_g[idx_p+5] = input[idx_o+5].g;
            padded_b[idx_p+5] = input[idx_o+5].b;
            padded_r[idx_p+6] = input[idx_o+6].r;
            padded_g[idx_p+6] = input[idx_o+6].g;
            padded_b[idx_p+6] = input[idx_o+6].b;
            padded_r[idx_p+7] = input[idx_o+7].r;
            padded_g[idx_p+7] = input[idx_o+7].g;
            padded_b[idx_p+7] = input[idx_o+7].b;
        }
    }

    for(int i=0; i < 9; i++){
        int_filter[i] = (int)((filter[i]) * 144.0); // gaussian use 1/16, box_blur use 1/9. 144 comes from lcm(16, 9).
    }

    int x;
    int y;
    register int m_c;
    register int u_c;
    register int d_c;
    
    for (x = 0; x < height; ++x) {
        for (y = 0; y < width; y+=4) {
            int flat_idx = y+x*width;
            m_c = flat_idx + x + x+ width+3;
            u_c = m_c-width-2;
            d_c = m_c+width+2;

            int r0 = 0;
            int g0 = 0;
            int b0 = 0;
            int r1 = 0;
            int g1 = 0;
            int b1 = 0;
            int r2 = 0;
            int g2 = 0;
            int b2 = 0;
            int r3 = 0;
            int g3 = 0;
            int b3 = 0;

            r0 += padded_r[u_c-1]*int_filter[0]+padded_r[u_c]*int_filter[1]+padded_r[u_c+1]*int_filter[2] + padded_r[m_c-1]*int_filter[3]+padded_r[m_c]*int_filter[4]+padded_r[m_c+1]*int_filter[5] + padded_r[d_c-1]*int_filter[6]+padded_r[d_c]*int_filter[7]+padded_r[d_c+1]*int_filter[8];
            g0 += padded_g[u_c-1]*int_filter[0]+padded_g[u_c]*int_filter[1]+padded_g[u_c+1]*int_filter[2] + padded_g[m_c-1]*int_filter[3]+padded_g[m_c]*int_filter[4]+padded_g[m_c+1]*int_filter[5] + padded_g[d_c-1]*int_filter[6]+padded_g[d_c]*int_filter[7]+padded_g[d_c+1]*int_filter[8];
            b0 += padded_b[u_c-1]*int_filter[0]+padded_b[u_c]*int_filter[1]+padded_b[u_c+1]*int_filter[2] + padded_b[m_c-1]*int_filter[3]+padded_b[m_c]*int_filter[4]+padded_b[m_c+1]*int_filter[5] + padded_b[d_c-1]*int_filter[6]+padded_b[d_c]*int_filter[7]+padded_b[d_c+1]*int_filter[8];

            r1 += padded_r[u_c]*int_filter[0]+padded_r[u_c+1]*int_filter[1]+padded_r[u_c+2]*int_filter[2] + padded_r[m_c]*int_filter[3]+padded_r[m_c+1]*int_filter[4]+padded_r[m_c+2]*int_filter[5] + padded_r[d_c]*int_filter[6]+padded_r[d_c+1]*int_filter[7]+padded_r[d_c+2]*int_filter[8];
            g1 += padded_g[u_c]*int_filter[0]+padded_g[u_c+1]*int_filter[1]+padded_g[u_c+2]*int_filter[2] + padded_g[m_c]*int_filter[3]+padded_g[m_c+1]*int_filter[4]+padded_g[m_c+2]*int_filter[5] + padded_g[d_c]*int_filter[6]+padded_g[d_c+1]*int_filter[7]+padded_g[d_c+2]*int_filter[8];
            b1 += padded_b[u_c]*int_filter[0]+padded_b[u_c+1]*int_filter[1]+padded_b[u_c+2]*int_filter[2] + padded_b[m_c]*int_filter[3]+padded_b[m_c+1]*int_filter[4]+padded_b[m_c+2]*int_filter[5] + padded_b[d_c]*int_filter[6]+padded_b[d_c+1]*int_filter[7]+padded_b[d_c+2]*int_filter[8];
     
            r2 += padded_r[u_c+1]*int_filter[0]+padded_r[u_c+2]*int_filter[1]+padded_r[u_c+3]*int_filter[2] + padded_r[m_c+1]*int_filter[3]+padded_r[m_c+2]*int_filter[4]+padded_r[m_c+3]*int_filter[5] + padded_r[d_c+1]*int_filter[6]+padded_r[d_c+2]*int_filter[7]+padded_r[d_c+3]*int_filter[8];
            g2 += padded_g[u_c+1]*int_filter[0]+padded_g[u_c+2]*int_filter[1]+padded_g[u_c+3]*int_filter[2] + padded_g[m_c+1]*int_filter[3]+padded_g[m_c+2]*int_filter[4]+padded_g[m_c+3]*int_filter[5] + padded_g[d_c+1]*int_filter[6]+padded_g[d_c+2]*int_filter[7]+padded_g[d_c+3]*int_filter[8];
            b2 += padded_b[u_c+1]*int_filter[0]+padded_b[u_c+2]*int_filter[1]+padded_b[u_c+3]*int_filter[2] + padded_b[m_c+1]*int_filter[3]+padded_b[m_c+2]*int_filter[4]+padded_b[m_c+3]*int_filter[5] + padded_b[d_c+1]*int_filter[6]+padded_b[d_c+2]*int_filter[7]+padded_b[d_c+3]*int_filter[8];

            r3 += padded_r[u_c+2]*int_filter[0]+padded_r[u_c+3]*int_filter[1]+padded_r[u_c+4]*int_filter[2] + padded_r[m_c+2]*int_filter[3]+padded_r[m_c+3]*int_filter[4]+padded_r[m_c+4]*int_filter[5] + padded_r[d_c+2]*int_filter[6]+padded_r[d_c+3]*int_filter[7]+padded_r[d_c+4]*int_filter[8];
            g3 += padded_g[u_c+2]*int_filter[0]+padded_g[u_c+3]*int_filter[1]+padded_g[u_c+4]*int_filter[2] + padded_g[m_c+2]*int_filter[3]+padded_g[m_c+3]*int_filter[4]+padded_g[m_c+4]*int_filter[5] + padded_g[d_c+2]*int_filter[6]+padded_g[d_c+3]*int_filter[7]+padded_g[d_c+4]*int_filter[8];
            b3 += padded_b[u_c+2]*int_filter[0]+padded_b[u_c+3]*int_filter[1]+padded_b[u_c+4]*int_filter[2] + padded_b[m_c+2]*int_filter[3]+padded_b[m_c+3]*int_filter[4]+padded_b[m_c+4]*int_filter[5] + padded_b[d_c+2]*int_filter[6]+padded_b[d_c+3]*int_filter[7]+padded_b[d_c+4]*int_filter[8];

            r0 = r0/144;
            g0 = g0/144;
            b0 = b0/144;
            r1 = r1/144;
            g1 = g1/144;
            b1 = b1/144;
            r2 = r2/144;
            g2 = g2/144;
            b2 = b2/144;
            r3 = r3/144;
            g3 = g3/144;
            b3 = b3/144;

            if (r0 > 255) r0 = 255;
            if (r0 < 0) r0 = 0;
            if (g0 > 255) g0 = 255;
            if (g0 < 0) g0 = 0;
            if (b0 > 255) b0 = 255;
            if (b0 < 0) b0 = 0;
            
            if (r1 > 255) r1 = 255;
            if (r1 < 0) r1 = 0;
            if (g1 > 255) g1 = 255;
            if (g1 < 0) g1 = 0;
            if (b1 > 255) b1 = 255;
            if (b1 < 0) b1 = 0;

            if (r2 > 255) r2 = 255;
            if (r2 < 0) r2 = 0;
            if (g2 > 255) g2 = 255;
            if (g2 < 0) g2 = 0;
            if (b2 > 255) b2 = 255;
            if (b2 < 0) b2 = 0;

            if (r3 > 255) r3 = 255;
            if (r3 < 0) r3 = 0;
            if (g3 > 255) g3 = 255;
            if (g3 < 0) g3 = 0;
            if (b3 > 255) b3 = 255;
            if (b3 < 0) b3 = 0;

            output[flat_idx].r = r0;
            output[flat_idx].g = g0;
            output[flat_idx].b = b0;
            output[flat_idx+1].r = r1;
            output[flat_idx+1].g = g1;
            output[flat_idx+1].b = b1;
            output[flat_idx+2].r = r2;
            output[flat_idx+2].g = g2;
            output[flat_idx+2].b = b2;
            output[flat_idx+3].r = r3;
            output[flat_idx+3].g = g3;
            output[flat_idx+3].b = b3;
        }
    }
    free(padded_r);
    free(padded_g);
    free(padded_b);
}