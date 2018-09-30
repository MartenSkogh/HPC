#ifndef SEPARATE_FILE
#define SEPARATE_FILE

// Compelx multiplicaion of two complex numbers
void mul_cpx_separatefile(double * a_re, double * a_im, 
                          double * b_re, double * b_im,
                          double * c_re, double * c_im)
{
    // b = b_re + i*b_im
    // c = c_re + i*c_im
    // a = b*c = (b_re + i*b_im)*(c_re + i*c_im)
    *a_re = (*b_re)*(*c_re) - (*b_im)*(*c_im);
    *a_im = (*b_im)*(*c_re) + (*b_re)*(*c_im);
}


#endif
