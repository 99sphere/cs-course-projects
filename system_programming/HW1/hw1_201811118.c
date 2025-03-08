#include <stdio.h>
#include <stdlib.h>

typedef unsigned char* pointer;

// Problem 1
void print_bit(pointer a, int len) {
    // Implement here
    for (int i = 0; i < len; i++) {
        unsigned char byte = *(a + i);
        for (int j = 7; j >= 0; j--) {
            printf("%d", (byte >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}


// Problem 2
void reverse_bit(pointer a, int len) {
    // Implement here
    unsigned char u_char_arr[len];
    pointer addrs[len];
    // save reversed value
    for (size_t i = 0; i < len; i++) {
        unsigned char byte = *(a + i);
        unsigned char rev_char = 0;
        int k = 1;
        for(int j=7; j>=0; j--){
            rev_char += (((byte >> j) & 0x01) * k);
            k *= 2;
        }
        u_char_arr[i] = rev_char;
    }
    // save address
    for (size_t i = 0; i < len; i++) {
        pointer addr = a + i;
        addrs[i] = addr;
    }
    // update each value in addrs
    for (size_t i = 0; i < len; i++) {
        pointer temp = addrs[i];
        *temp = u_char_arr[len-1-i];
    }
}

// Problem 3
void split_bit(pointer a, pointer out1, pointer out2, int len) {
    // Implement here
    int half_len = len/2;

    unsigned char even_1 = 0;
    unsigned char even_2 = 0;
    unsigned char odd_1 = 0;
    unsigned char odd_2 = 0;
    unsigned char mask = 1;
    for (size_t i = 0; i < half_len; i++){
        // calculation for continuous 2-bit
        unsigned char orig_1 = *(a + 2*i);
        unsigned char orig_2 = *(a + 2*i + 1);
        unsigned char result_even = 0;
        unsigned char result_odd = 0;

        // masking
        odd_1 = orig_1 & 0xAA; // & 10101010(2) = 0xAA
        odd_2 = orig_2 & 0xAA; // 
        even_1 = orig_1 & 0x55; // & 01010101(2) = 0x55
        even_2 = orig_2 & 0x55; //

        // make odd result (sampling odd-index value) ex) 00110101(2) -> 0100(2)
        int cnt_1 = 0;
        int cnt_2 = 4;
        for(int j=7; j>=0; j--){
            if (j > 3){
                if((odd_1 & (1 << (j - cnt_1))) == (1 << (j - cnt_1))) // j: 7 6 5 4 / cnt: 0 1 2 3 / target: 7 5 3 1
                    result_odd += (1 << j);
                    cnt_1 += 1;
            }
            else {
                if((odd_2 & (1 << (j + cnt_2))) == (1 << (j + cnt_2))) // j: 3 2 1 0 / cnt: 4 3 2 1 / target: 7 5 3 1
                    result_odd += (1 << j);
                    cnt_2 -= 1;
            }
        }

        // make even result (sampling even-index value) ex) 00110101(2) -> 0111(2)
        cnt_1 = 1;
        cnt_2 = 3;
        for(int j=7; j>=0; j--){
            if (j > 3){
                if((even_1 & (1 << (j - cnt_1))) == (1 << (j - cnt_1))) // j: 7 6 5 4 / cnt_1: 1 2 3 4 / target: 6 4 2 0 
                    result_even += (1 << j);
                    cnt_1 += 1;
            }
            else {
                if((even_2 & (1 << (j + cnt_2))) == (1 << (j + cnt_2))) // j: 3 2 1 0 / cnt_2: 3 2 1 0 / target: 6 4 2 0
                    result_even += (1 << j);
                    cnt_2 -= 1;
            }
        }

        pointer odd_addr = out1 + i;
        pointer even_addr = out2 + i;

        *odd_addr = result_odd;
        *even_addr = result_even;
    }    
}

// Problem 4
unsigned int convert_endian(unsigned int a) {
    // Implement here
    int len = sizeof(a);
    unsigned int* uint_addr_a = &a;
    unsigned char* uchar_addr_a = (pointer) uint_addr_a;
    unsigned int result = 0;

    for (size_t i = 0; i < len; i++) {
        unsigned char byte = *(uchar_addr_a + i);
        result += (byte << ((len-1-i)*8));
    }
    return result;
}

// Problem 5
void get_date(unsigned int date, int* pYear, int* pMonth, int* pDay) {
    // Implement here
    *pYear = (date >> 9); // modify this
    *pMonth = (date << 23) >> 28; // modify this
    *pDay = (date << 27) >> 27; // modify this
}

int main() {
    // You don't need to touch the main function
    printf("Problem 1\n");
    unsigned int v1 = 0x1234CDEF;
    print_bit((pointer)&v1, sizeof(v1));
    reverse_bit((pointer)&v1, sizeof(v1));
    print_bit((pointer)&v1, sizeof(v1));

    printf("Problem 2\n");
    unsigned int v2 = 0x1234CDEF;
    unsigned short out1 = 0, out2 = 0;
    print_bit((pointer)&v2, sizeof(v2));
    split_bit((pointer)&v2, (pointer)&out1, (pointer)&out2, sizeof(v2));
    print_bit((pointer)&out1, sizeof(out1));
    print_bit((pointer)&out2, sizeof(out2));

    printf("Problem 3\n");
    unsigned int v4 = 0x12345678;
    unsigned int v4_ret = convert_endian(v4);
    print_bit((pointer)&v4, sizeof(v4));
    print_bit((pointer)&v4_ret, sizeof(v4_ret));

    printf("Problem 4\n");
    unsigned int date = 1035391;
    int year, month, day;
    print_bit((pointer)&date, sizeof(date));
    get_date(date, &year, &month, &day);
    printf("%d -> %d/%d/%d\n", date, year, month, day);

    return 0;
}