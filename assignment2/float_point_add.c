#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0
typedef union{
float floating_value_in_32_bits; 
struct sign_exp_mantissa{
        unsigned mantissa:23; 
        unsigned exponent:8; 
        unsigned sign:1;
} f_bits;
struct single_bits{
        unsigned b0 :1;  unsigned b1 :1;  unsigned b2 :1;  unsigned b3 :1;
        unsigned b4 :1;  unsigned b5 :1;  unsigned b6 :1;  unsigned b7 :1;
        unsigned b8 :1;  unsigned b9 :1;  unsigned b10:1;  unsigned b11:1;
        unsigned b12:1;  unsigned b13:1;  unsigned b14:1;  unsigned b15:1;
        unsigned b16:1;  unsigned b17:1;  unsigned b18:1;  unsigned b19:1;
        unsigned b20:1;  unsigned b21:1;  unsigned b22:1;  unsigned b23:1;
        unsigned b24:1;  unsigned b25:1;  unsigned b26:1;  unsigned b27:1;
        unsigned b28:1;  unsigned b29:1;  unsigned b30:1;  unsigned b31:1;
   
    }bit; 
}FLOAT_UN;

static void bitstring_grouped(FLOAT_UN f, char out[64]) {
    int p = 0;
    // sign
    out[p++] = (char)('0' + (f.f_bits.sign & 1));
    out[p++] = ' ';
    // exponent MSB→LSB; space after 4 bits
    for (int i = 7; i >= 0; --i) {
        out[p++] = (char)('0' + ((f.f_bits.exponent >> i) & 1));
        if (i == 4) out[p++] = ' ';
    }
    out[p++] = ' ';
    // mantissa 23 bits: first 3, then groups of 4
    for (int i = 22; i >= 0; --i) {
        out[p++] = (char)('0' + ((f.f_bits.mantissa >> i) & 1));
        if (i == 20 || i == 16 || i == 12 || i == 8 || i == 4) out[p++] = ' ';
    }
    out[p] = '\0';
}

int main(int argc, char* argv[]) 
{
FLOAT_UN float_32_s1,float_32_s2,float_32_rslt, fun_arg;
 /******** local helper variables ***********/
float the_hardware_result;
int mant_s1, mant_s2, mant_res, exp_s1, exp_s2; 
int i, j, k, shift_count;
int de_norm_s1 = TRUE, de_norm_s2 = TRUE;

/******** request two floating point numbers ********/
    printf("Please enter two positive floating point values each with: - no more than 6 significant digits\n");
    printf("- a value between + 10**37 and 10**-37\n");
    printf("Enter Float 1: "); 
    if (scanf("%g", &float_32_s1.floating_value_in_32_bits) != 1);

    printf("Enter float 2: "); 
    if (scanf("%g", &float_32_s2.floating_value_in_32_bits) != 1);

/****** generate the floating point hardware result ********/

the_hardware_result = float_32_s2.floating_value_in_32_bits + 
float_32_s1.floating_value_in_32_bits;

 /******* get the mantissa and exponent components ****/
/******* into the helper variables****//*000000 .. XXXX .. XXXX
| 8 bits | 00000000 .. XXXX XXXX*/
mant_s1 = float_32_s1.f_bits.mantissa; 
mant_s2 = float_32_s2.f_bits.mantissa; 
exp_s1 = float_32_s1.f_bits.exponent; 
exp_s2 = float_32_s2.f_bits.exponent;

unsigned sign1 = float_32_s1.f_bits.mantissa;
unsigned sign2 = float_32_s1.f_bits.mantissa;

//check both for normalization and slam in the hidden bit if normalized
if(exp_s1){
mant_s1 |= (1 << 23);
de_norm_s1 = FALSE; //set normalization Boolean
}
if (exp_s2) {
        mant_s2 |= (1 << 23);
        de_norm_s2 = FALSE; // normalized
    }

int res_exp;
 /******* check exponent diff and who's the smallest ****/
if((shift_count = exp_s1 - exp_s2) < 0){ 
    shift_count = -(shift_count); //keep diff + if here, need to shift mant_s1
    if(shift_count >24) shift_count = 24;//don’t over-shift .. shift 24 bits max
    int actual_shift = shift_count - (de_norm_s1 ? 1 : 0);
    if (actual_shift < 0) actual_shift = 0;
    mant_s1 = (int)((unsigned) mant_s1 >> actual_shift);
    res_exp = exp_s2; 
} else if (shift_count > 0){
    if( shift_count > 24) shift_count = 24;
    int actual_shift = shift_count - (de_norm_s2 ? 1 : 0);
    if (actual_shift < 0) actual_shift = 0;
    mant_s2 = (int)((unsigned) mant_s2 >> actual_shift);
    res_exp = exp_s1;
}else {
    res_exp = exp_s1;
}
/*before actually shifting, check exp_s1 for normalization if denormalized number, shift 1 less than shift_count
else, shift by shift_count
set resulting exponent value into result struct*/
//if here, need to shift mant_s2and result exponent is exp_s1


/**** finally ready to add helper mantissa variables ****/ 
mant_res = mant_s1 + mant_s2; //integer addition
/*check if the addition overflowed 24 bits, since mantissa with hidden bit can only be 24 bits significant if we need to right shift, must increase the exp
and then clear the slammed hidden bit in the
mantissa helper to get to 23 live bits and put these
23 bits into the mantissa bit field of the resu1lt*/
if(mant_res & (1<<24)){
    mant_res= (int)((unsigned) mant_res >> 1);
    res_exp += 1;
}
//else just clear HB, leaving 23 live bits for the result mantissa
/**** check for infinity exponent pattern (0xFF) ****/ 
/**** cannot have NAN from addition, so clear mantissa ****/

float_32_rslt.f_bits.sign = 0;  // (sign1 & sign2) == 0 for positives
 if (res_exp >= 255) {
        float_32_rslt.f_bits.exponent = 0xFF;
        float_32_rslt.f_bits.mantissa = 0;
    } else if (res_exp <= 0) {
        // Underflow to denormal/zero: store what’s left without hidden bit
        float_32_rslt.f_bits.exponent = 0;
        float_32_rslt.f_bits.mantissa = (unsigned)(mant_res & 0x7FFFFF);
    } else {
        float_32_rslt.f_bits.exponent = (unsigned)res_exp & 0xFF;
        // clear hidden bit (bit 23) leaving 23 live bits for the mantissa field
        float_32_rslt.f_bits.mantissa = (unsigned)(mant_res & ~(1 << 23));
    }
    char s1[64], s2[64], sr[64];
    bitstring_grouped(float_32_s1, s1);
    bitstring_grouped(float_32_s2, s2);
    bitstring_grouped(float_32_rslt, sr);

    printf("\nOriginal pattern of Float 1: %s\n", s1);
    printf("Original pattern of Float 2: %s\n", s2);
    printf("Bit pattern of result : %s\n", sr);

    printf("EMULATED FLOATING RESULT FROM PRINTF ==>>> %.10g\n", float_32_rslt.floating_value_in_32_bits);
    printf("HARDWARE FLOATING RESULT FROM PRINTF ==>>> %.10g\n", the_hardware_result);
    printf("**********************************************************************\n");

    return 0;
}