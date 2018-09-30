#include <stdio.h>
#include <stdlib.h>
//#include <complexmath.h>

// Compelx multiplicaion of two complex numbers
void mul_cpx(double * a_re, double * a_im, 
             double * b_re, double * b_im,
             double * c_re, double * c_im)
{
    // b = b_re + i*b_im
    // c = c_re + i*c_im
    // a = b*c = (b_re + i*b_im)*(c_re + i*c_im)
    *a_re = (*b_re)*(*c_re) - (*b_im)*(*c_im);
    *a_im = (*b_im)*(*c_re) + (*b_re)*(*c_im);
}

int main() 
{
    // Test
    double a_re, a_im, b_re, b_im, c_re, c_im;

    b_re = 2.0;
    b_im = 3.0;

    c_re = 7.0;
    c_im = 8.0;

    mul_cpx(&a_re, &a_im, &b_re, &b_im, &c_re, &c_im);

    printf("b = %g + i%g, c = %g + i%g", b_re, b_im, c_re, c_im);
    printf("a = b*c = %g + i%g", a_re, a_im);

    return 0;
}
