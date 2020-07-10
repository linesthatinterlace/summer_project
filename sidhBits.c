#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sidhBits.h"


//FP

void fp_Add(const mp a, const mp b, mp c) {
    *c = (uint32_t)( ((uint64_t)*a + (uint64_t)*b ) % (uint64_t)MODULUS );
}

void fp_Clear(mp* q) {
    free(*q);
}


void fp_Constant(unsigned long a, mp b) {
    *b = a % MODULUS;
}

void fp_Copy(mp dst, const mp src) {
    memcpy(dst, src, 8);
}

void fp_Init (mp* q) {
    *q = calloc(1, 8);
}

int fp_IsEqual(const mp a, const mp b) {
    return (*a % MODULUS) == (*b % MODULUS);
}


int fp_IsEven(const mp a) {
    return !fp_IsBitSet(a, 0);
}

void fp_Invert(const mp a, mp b) {
    mp t1;
    fp_Init(&t1);
    fp_Constant(MODULUS - 2, t1);
    fp_Pow(a, t1, b);
    fp_Clear(&t1);
}


int fp_IsBitSet(const mp a, const unsigned long index) {
    return (*a & ( 1 << index )) >> index;
}

int fp_IsConstant(const mp a, const size_t constant) {
    return (*a % MODULUS) == (constant % MODULUS);
}

void fp_Multiply(const mp a, const mp b, mp c) {
    *c = (uint32_t)( ((uint64_t)*a * (uint64_t)*b ) % (uint64_t)MODULUS);
}


void fp_Negative(const mp a, mp b) {
    mp t1, t2;
    fp_Init(&t1);
    fp_Constant(*a, t1);
    *b = (*t1 == 0) ? 0 : MODULUS - (*a % MODULUS);
    fp_Clear(&t1);
}

void fp_Pow(const mp a, const mp b, mp c) {
    mp t1;
    fp_Init(&t1);
    fp_Unity(t1);
    for (int i = 31; i >= 0; i--)
    {                 
        fp_Square(t1, t1);

        if (fp_IsBitSet(b, i)) {
            fp_Multiply(t1, a, t1);
        }
    }
    fp_Copy(c, t1);
    fp_Clear(&t1);
}


int fp_QuadNonRes(const mp a) {
    mp t1, t2;
    fp_Init(&t1);
    fp_Init(&t2);
    fp_Sqrt(a, t1);
    fp_Square(t1, t1);
    if ( fp_IsEqual(a, t1) ) return 0 ; else return 1;
}

void fp_Square(const mp a, mp b) {
    fp_Multiply(a, a, b);
}

void fp_Sqrt(const mp a, mp b) {
    mp t1, p14;
    fp_Init(&p14);
    fp_Init(&t1);
    
    fp_Constant((MODULUS + 1) >> 2, p14);
    fp_Pow(a, p14, t1);
    if (fp_IsEven(t1)) {
        fp_Copy(b, t1);
    } else {
        fp_Negative(t1, t1);
        fp_Copy(b, t1);
    }
    fp_Clear(&p14);
    fp_Clear(&t1);

}

void fp_Subtract(const mp a, const mp b, mp c) {
    mp t1;
    fp_Init(&t1);
    fp_Negative(b, t1);
    fp_Add(a, t1, c);
    fp_Clear(&t1);
}

void fp_Unity(mp b) {
    fp_Constant(1, b);
}

void fp_Zero(mp b) {
    fp_Constant(0, b);
}

// FP2

void
fp2_Add( const fp2* a, const fp2* b, fp2* c )
{
  fp_Add(a->x0, b->x0, c->x0);
  fp_Add(a->x1, b->x1, c->x1);
}

void fp2_Clear( fp2* fp2)
{
  fp_Clear(&(fp2->x0));
  fp_Clear(&(fp2->x1));
}


void
fp2_Copy( const fp2* a, fp2* b )
{
  fp_Copy( b->x0, a->x0 );
  fp_Copy( b->x1, a->x1 );
}


void
fp2_Init( fp2* fp2 )
{
  fp_Init(&(fp2->x0));
  fp_Init(&(fp2->x1));
}

void
fp2_Init_set( fp2* fp2, unsigned long x0, unsigned long x1 )
{
  fp2_Init(fp2);
  fp_Constant(x0, fp2->x0);
  fp_Constant(x1, fp2->x1);
}

int
fp2_IsEqual( const fp2* a1, const fp2* a2 )
{
  return fp_IsEqual(a1->x0, a2->x0) && fp_IsEqual(a1->x1, a2->x1);
}

void
fp2_Set( fp2* fp2, unsigned long x0, unsigned long x1 )
{
  fp_Constant(x0, fp2->x0);
  fp_Constant(x1, fp2->x1);
}

void
fp2_Sub( const fp2* a, const fp2* b, fp2* c )
{
  fp_Subtract( a->x0, b->x0, c->x0 );
  fp_Subtract( a->x1, b->x1, c->x1 );
}


void
fp2_Multiply( const fp2*  a, const fp2*  b, fp2*  c )
{
  mp mul0;
  mp mul1;
  mp adda;
  mp addb;

  fp_Init(&mul0);
  fp_Init(&mul1);
  fp_Init(&adda);
  fp_Init(&addb);

  fp_Multiply( a->x0, b->x0, mul0 );
  fp_Multiply( a->x1, b->x1, mul1 );

  fp_Add( a->x0, a->x1, adda );
  fp_Add( b->x0, b->x1, addb );

  fp_Subtract( mul0, mul1, c->x0 );

  fp_Add( mul0, mul1, mul0 );
  fp_Multiply( adda, addb, mul1 );

  fp_Subtract( mul1, mul0, c->x1 );

  fp_Clear(&mul0);
  fp_Clear(&mul1);
  fp_Clear(&adda);
  fp_Clear(&addb);
}

void
fp2_Square( const fp2* a, fp2* b )
{
  fp2_Multiply( a, a, b );
}

void
fp2_Invert( const fp2* a, fp2* b )
{
  mp mul0;
  mp mul1;
  fp_Init(&mul0);
  fp_Init(&mul1);

  fp_Square( a->x0, mul0 );
  fp_Square( a->x1, mul1 );

  fp_Add( mul0, mul1, mul0 );
  fp_Invert( mul0, mul0 );

  fp_Negative( a->x1, mul1 );

  fp_Multiply( a->x0, mul0, b->x0 );
  fp_Multiply( mul1, mul0, b->x1 );

  fp_Clear(&mul0);
  fp_Clear(&mul1);
}

void
fp2_Negative( const fp2* a, fp2* b )
{
  fp_Negative( a->x0, b->x0 );
  fp_Negative( a->x1, b->x1 );
}

int
fp2_IsConst( const fp2* a, unsigned long x0, unsigned long x1 ) {
  return fp_IsConstant(a->x0, x0) && fp_IsConstant(a->x1, x1);
}


void fp2_Sqrt(const fp2* a, fp2* b)
{
    mp t0, t1;
    fp_Init(&t0);
    fp_Init(&t1);
    if (( fp_IsConstant(a->x1, 0) ) && (fp_QuadNonRes(a->x0))) {
            fp_Zero(t0);
            fp_Sqrt(a-> x0, t1);
            fp_Copy(b->x0, t0);
            fp_Copy(b->x1, t1);
    }
    else {
        mp t2, t3, p14, p34, inv2;
        fp_Init(&t2);
        fp_Init(&t3);
        fp_Init(&p14);
        fp_Init(&p34);
        fp_Init(&inv2);
        fp_Constant(2, inv2);

        // (p + 1) / 4
        fp_Constant((MODULUS + 1) >> 2, p14);

        // (p - 3) / 4
        fp_Constant((MODULUS - 3) >> 2, p34);

        fp_Invert(inv2, inv2);
        fp_Square(a->x0, t0); 
        fp_Square(a->x1, t1); 
        fp_Add(t0, t1, t0); 
        fp_Pow(t0, p14, t1); 
        fp_Add(a->x0, t1, t0); 
        fp_Multiply(t0, inv2, t0);

        //p->half(t0);
        fp_Pow(t0, p34, t2); 
        //fp_Multiply(t0, t2, t1);

        fp_Pow(t0, p14, t1);
        fp_Multiply(t2, a->x1, t2); 
        fp_Multiply(t2, inv2, t2); 

        fp_Square(t1, t3);
        if (!fp_IsEqual(t3, t0)) {
            fp_Copy(t0, t1);
            fp_Copy(t1, t2);
            fp_Negative(t0, t2);
        }
        if (fp_IsEven(t1)) {
            fp_Copy(b->x0, t1);
            fp_Copy(b->x1, t2);
        } else {
            fp_Negative(t1, b->x0);
            fp_Negative(t2, b->x1);
        }
        fp_Clear(&t2);
        fp_Clear(&t3);
        fp_Clear(&p14);
        fp_Clear(&p34);
        fp_Clear(&inv2);
    }
    fp_Clear(&t0);
    fp_Clear(&t1);
  }


// MONTGOMERY


void mont_curve_init(mont_curve_int_t* curve) {
  fp2_Init(&curve->a);
  fp2_Init(&curve->b);
}

void mont_curve_clear(mont_curve_int_t* curve) {
  fp2_Clear(&curve->a);
  fp2_Clear(&curve->b);
}

void mont_curve_copy(const mont_curve_int_t* curve, mont_curve_int_t* curvecopy) {
  if (curve != curvecopy) {
    fp2_Copy(&curve->a, &curvecopy->a);
    fp2_Copy(&curve->b, &curvecopy->b);
  }
}

void mont_pt_init(mont_pt_t* pt) {
  fp2_Init(&pt->x);
  fp2_Init(&pt->y);
}

void mont_pt_clear(mont_pt_t* pt) {
  fp2_Clear(&pt->x);
  fp2_Clear(&pt->y);
}

void public_key_init(sike_public_key_t* pk) {
  fp2_Init(&pk->xP);
  fp2_Init(&pk->xQ);
  fp2_Init(&pk->xR);
}

void public_key_clear(sike_public_key_t* pk) {
  fp2_Clear(&pk->xP);
  fp2_Clear(&pk->xQ);
  fp2_Clear(&pk->xR);

}

void mont_pt_copy(const mont_pt_t* src, mont_pt_t* dst) {
  if (src != dst) {
    fp2_Copy(&src->x, &dst->x);
    fp2_Copy(&src->y, &dst->y);
  }
}

/* infinity is represented as a point with (0, 0) */
void mont_set_inf_affine(const mont_curve_int_t* curve, mont_pt_t *P) {
  fp2_Set( &P->x, 0, 0 );
  fp2_Set( &P->y, 1, 0 );
}

/* returns 1 for True, 0 for False */
int mont_is_inf_affine(const mont_curve_int_t* curve, const mont_pt_t *P) {
  return (fp2_IsConst( &P->x, 0, 0 )) && (fp2_IsConst( &P->y, 1, 0 ));
}

void mont_double_and_add(const mont_curve_int_t* curve, const mp k, const mont_pt_t* P, mont_pt_t *Q, int msb) {

  int i;
  
  mont_pt_t kP = { 0 };
  mont_pt_init(&kP);

  mont_set_inf_affine(curve, &kP);

  for (i = msb - 1; i >= 0; i--) {
    xDBL(curve, &kP, &kP);
    if (fp_IsBitSet(k, i)) {
      xADD(curve, &kP, P, &kP);
    }
  }

  mont_pt_copy(&kP, Q);

  mont_pt_clear(&kP);
}

void xDBL(const mont_curve_int_t *curve, const mont_pt_t *P, mont_pt_t *R) {

  const fp2* a = &curve->a;
  const fp2* b = &curve->b;
 
  // x3 = b*(3*x1^2+2*a*x1+1)^2/(2*b*y1)^2-a-x1-x1
  // y3 = (2*x1+x1+a)*(3*x1^2+2*a*x1+1)/(2*b*y1)-b*(3*x1^2+2*a*x1+1)^3/(2*b*y1)^3-y1

  fp2 t0 = { 0 }, t1 = { 0 }, t2 = { 0 };
  fp2_Init(&t0);
  fp2_Init(&t1);
  fp2_Init(&t2);

  fp2_Negative(&P->y, &t0);

  if (mont_is_inf_affine(curve, P) ) {
    mont_set_inf_affine(curve, R);
  }
  else if ( fp2_IsEqual(&P->y, &t0) ) {
    /* P == -P */
    mont_set_inf_affine(curve, R);
  }
  else {

      fp2_Set(&t2, 1, 0);                        // t2 = 1

      fp2_Square(&P->x, &t0);                    // t0 = x1^2
      fp2_Add(&t0, &t0, &t1);                    // t1 = 2*x1^2
      fp2_Add(&t0, &t1, &t0);                    // t0 = 3*x1^2

      fp2_Multiply(a, &P->x, &t1);               // t1 = a*x1
      fp2_Add(&t1, &t1, &t1);                    // t1 = 2*a*x1


      fp2_Add(&t0, &t1, &t0);                    // t0 = 3*x1^2+2*a*x1
      fp2_Add(&t0, &t2, &t0);                    // t0 = 3*x1^2+2*a*x1+1

      fp2_Multiply(b, &P->y, &t1);               // t1 = b*y1
      fp2_Add(&t1, &t1, &t1);                    // t1 = 2*b*y1
      fp2_Invert(&t1, &t1);                      // t1 = 1 / (2*b*y1)

      fp2_Multiply(&t0, &t1, &t0);               // t0 = (3*x1^2+2*a*x1+1) / (2*b*y1)

      fp2_Square(&t0, &t1);                      // t1 = (3*x1^2+2*a*x1+1)^2 / (2*b*y1)^2

      fp2_Multiply(b, &t1, &t2);                 // t2 = b*(3*x1^2+2*a*x1+1)^2 / (2*b*y1)^2
      fp2_Sub(&t2, a, &t2);                      // t2 = b*(3*x1^2+2*a*x1+1)^2 / (2*b*y1)^2 - a
      fp2_Sub(&t2, &P->x, &t2);                  // t2 = b*(3*x1^2+2*a*x1+1)^2 / (2*b*y1)^2 - a - x1
      fp2_Sub(&t2, &P->x, &t2);                  // t2 = b*(3*x1^2+2*a*x1+1)^2 / (2*b*y1)^2 - a - x1 - x1

      fp2_Multiply(&t0, &t1, &t1);               // t1 = (3*x1^2+2*a*x1+1)^3 / (2*b*y1)^3
      fp2_Multiply(b, &t1, &t1);                 // t1 = b*(3*x1^2+2*a*x1+1)^3 / (2*b*y1)^3
      fp2_Add(&t1, &P->y, &t1);                  // t1 = b*(3*x1^2+2*a*x1+1)^3 / (2*b*y1)^3 + y1

      fp2_Add(&P->x, &P->x, &R->y);              // x3 = 2*x1
      fp2_Add(&R->y, &P->x, &R->y);              // y3 = 2*x1+x1
      fp2_Add(&R->y, a, &R->y);                  // y3 = 2*x1+x1+a
      fp2_Multiply(&R->y, &t0, &R->y);           // y3 = (2*x1+x1+a)*(3*x1^2+2*a*x1+1)/(2*b*y1)
      fp2_Sub(&R->y, &t1, &R->y);                // y3 = (2*x1+x1+a)*(3*x1^2+2*a*x1+1)/(2*b*y1) - (b*(3*x1^2+2*a*x1+1)^3/(2*b*y1)^3 + y1)

      fp2_Copy(&t2, &R->x);                      // x3 = b*(3*x1^2+2*a*x1+1)^2 / (2*b*y1)^2 - a - x1 - x1

  }
  fp2_Clear(&t0);
  fp2_Clear(&t1);
  fp2_Clear(&t2);

}

void xDBLe(const mont_curve_int_t *curve, const mont_pt_t *P, int e, mont_pt_t *R) {
  mont_pt_copy(P, R);
  for (int j = 0; j < e; ++j)
    xDBL(curve, R, R);
}

void xADD(const mont_curve_int_t *curve, const mont_pt_t *P, const mont_pt_t *Q, mont_pt_t *R) {

  // x3 = b*(y2-y1)^2/(x2-x1)^2-a-x1-x2
  // y3 = (2*x1+x2+a)*(y2-y1)/(x2-x1)-b*(y2-y1)^3/(x2-x1)^3-y1
  // y3 = ((2*x1)+x2+a) * ((y2-y1)/(x2-x1)) - b*((y2-y1)^3/(x2-x1)^3) - y1

  const fp2* a = &curve->a;
  const fp2* b = &curve->b;

  fp2 t0 = { 0 }, t1 = { 0 }, t2 = { 0 };
  fp2_Init(&t0);
  fp2_Init(&t1);
  fp2_Init(&t2);

  fp2_Negative(&Q->y, &t0);

  if (mont_is_inf_affine(curve, P) ) {
    mont_pt_copy(Q, R);
  }
  else if (mont_is_inf_affine(curve, Q) ) {
    mont_pt_copy(P, R);
  }
  else if ( fp2_IsEqual( &P->x, &Q->x) && fp2_IsEqual( &P->y, &Q->y ) ) {
    /* P == Q */
    xDBL(curve, P, R);
  }
  else if ( fp2_IsEqual( &P->x, &Q->x) && fp2_IsEqual(&P->y, &t0) ) {
    /* P == -Q */
    mont_set_inf_affine(curve, R);
  }
  else {
    /* P != Q or -Q  */

    fp2_Sub(&Q->y, &P->y, &t0);               // t0 = y2-y1
    fp2_Sub(&Q->x, &P->x, &t1);               // t1 = x2-x1
    fp2_Invert(&t1, &t1);                     // t1 = 1/(x2-x1)
    fp2_Multiply(&t0, &t1, &t0);              // t0 = (y2-y1)/(x2-x1)

    fp2_Square(&t0, &t1);                     // t1 = (y2-y1)^2/(x2-x1)^2

    fp2_Add(&P->x, &P->x, &t2);               // t2 = 2*x1
    fp2_Add(&t2, &Q->x, &t2);                 // t2 = 2*x1+x2
    fp2_Add(&t2, a, &t2);                     // t2 = 2*x1+x2+a
    fp2_Multiply(&t2, &t0, &t2);              // t2 = (2*x1+x2+a)*(y2-y1)/(x2-x1)

    fp2_Multiply(&t0, &t1, &t0);              // t0 = (y2-y1)^3/(x2-x1)^3
    fp2_Multiply(b, &t0, &t0);                // t0 = b*(y2-y1)^3/(x2-x1)^3
    fp2_Add(&t0, &P->y, &t0);                 // t0 = b*(y2-y1)^3/(x2-x1)^3+y1

    fp2_Sub(&t2, &t0, &t0);                   // t0 = (2*x1+x2+a)*(y2-y1)/(x2-x1)-b*(y2-y1)^3/(x2-x1)^3-y1

    fp2_Multiply(b, &t1, &t1);                // t1 = b*(y2-y1)^2/(x2-x1)^2
    fp2_Sub(&t1, a, &t1);                     // t1 = b*(y2-y1)^2/(x2-x1)^2-a
    fp2_Sub(&t1, &P->x, &t1);                 // t1 = b*(y2-y1)^2/(x2-x1)^2-a-x1

    fp2_Sub(&t1, &Q->x, &R->x);               // x3 = b*(y2-y1)^2/(x2-x1)^2-a-x1-x2

    fp2_Copy(&t0, &R->y);                     // y3 = (2*x1+x2+a)*(y2-y1)/(x2-x1)-(b*(y2-y1)^3/(x2-x1)^3+y1)

  }

  fp2_Clear(&t0);
  fp2_Clear(&t1);
  fp2_Clear(&t2);
}

void xTPL(const mont_curve_int_t *curve, const mont_pt_t *P, mont_pt_t *R) {
  mont_pt_t T = { 0 };
  mont_pt_init(&T);

  xDBL(curve, P, &T);
  xADD(curve, P, &T, R);

  mont_pt_clear(&T);
}

void xTPLe(const mont_curve_int_t *curve, const mont_pt_t *P, int e, mont_pt_t *R) {
  mont_pt_copy(P, R);
  for (int j = 0; j < e; ++j)
    xTPL(curve, R, R);
}


void j_inv(const mont_curve_int_t *E, fp2 *jinv) {
  const fp2 *a = &E->a;

  fp2 t0 = { 0 }, t1 = { 0 };
  fp2_Init(&t0);
  fp2_Init(&t1);

  fp2_Square(a, &t0);            // t0 = a^2
  fp2_Set(jinv, 3, 0);           // jinv = 3
  fp2_Sub(&t0, jinv, jinv);      // jinv = a^2-3
  fp2_Square(jinv, &t1);         // t1 = (a^2-3)^2
  fp2_Multiply(jinv, &t1, jinv); // jinv = (a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 2*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 4*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 8*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 16*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 32*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 64*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 128*(a^2-3)^3
  fp2_Add(jinv, jinv, jinv);     // jinv = 256*(a^2-3)^3

  fp2_Set(&t1, 4, 0);            // t1 = 4
  fp2_Sub(&t0, &t1, &t0);        // t0 = a^2-4
  fp2_Invert(&t0, &t0);          // t0 = 1/(a^2-4)
  fp2_Multiply(jinv, &t0, jinv); // jinv = 256*(a^2-3)^3/(a^2-4)

  fp2_Clear(&t0);
  fp2_Clear(&t1);
}

void get_xR(const mont_curve_int_t *curve, const mont_pt_t *P, const mont_pt_t *Q, sike_public_key_t *pk) {

  mont_pt_t R = { 0 };
  mont_pt_init(&R);

  mont_pt_copy(Q, &R);
  fp2_Negative(&R.y, &R.y);

  xADD(curve, P, &R, &R);
  fp2_Copy(&P->x, &pk->xP);
  fp2_Copy(&Q->x, &pk->xQ);
  fp2_Copy(&R.x, &pk->xR);

  mont_pt_clear(&R);
}

void get_yP_yQ_A_B(const sike_public_key_t *pk, mont_pt_t *P, mont_pt_t *Q, mont_curve_int_t *curve) {

  fp2* a = &curve->a;
  fp2* b = &curve->b;

  const fp2* xP = &pk->xP;
  const fp2* xQ = &pk->xQ;
  const fp2* xR = &pk->xR;

  mont_pt_t T = { 0 };
  mont_pt_init(&T);

  fp2 *t1 = &T.x, *t2 = &T.y;

  // a:=(1-xP*xQ-xP*xR-xQ*xR)^2/(4*xP*xQ*xR)-xP-xQ-xR;

  fp2_Multiply(xP, xQ, a);   // t1 = xP*xQ
  fp2_Multiply(a, xR, t1);   // t2 = xP*xQ*xR
  fp2_Add(t1, t1, t1);       // t2 = 2*xP*xQ*xR
  fp2_Add(t1, t1, t1);       // t2 = 4*xP*xQ*xR
  fp2_Invert(t1, t1);        // t2 = 1/(4*xP*xQ*xR)

  fp2_Set(t2, 1, 0);         // t3 = 1
  fp2_Sub(t2, a, a);         // t1 = 1-xP*xQ
  fp2_Multiply(xP, xR, t2);  // t3 = xP*xR
  fp2_Sub(a, t2, a);         // t1 = 1-xP*xQ-xP*xR
  fp2_Multiply(xQ, xR, t2);  // t3 = xQ*xR
  fp2_Sub(a, t2, a);         // t1 = 1-xP*xQ-xP*xR-xQ*xR
  fp2_Square(a, a);          // t1 = (1-xP*xQ-xP*xR-xQ*xR)^2

  fp2_Multiply(a, t1, a);    // t1 = (1-xP*xQ-xP*xR-xQ*xR)^2/(4*xP*xQ*xR)
  fp2_Sub(a, xP, a);         // t1 = (1-xP*xQ-xP*xR-xQ*xR)^2/(4*xP*xQ*xR)-xP
  fp2_Sub(a, xQ, a);         // t1 = (1-xP*xQ-xP*xR-xQ*xR)^2/(4*xP*xQ*xR)-xP-xQ
  fp2_Sub(a, xR, a);         // a = (1-xP*xQ-xP*xR-xQ*xR)^2/(4*xP*xQ*xR)-xP-xQ-xR


  fp2_Square(xP, t1);        // t1 = xP^2
  fp2_Multiply(xP, t1, t2);  // t2 = xP^3
  fp2_Multiply(a, t1, t1);   // t1 = a*xP^2
  fp2_Add(t2, t1, t1);       // t1 = xP^3+a*xP^2
  fp2_Add(t1, xP, t1);       // t1 = xP^3+a*xP^2+xP
  fp2_Sqrt(t1, &P->y);    // yP = sqrt(xP^3+a*xP^2+xP)

  fp2_Square(xQ, t1);        // t1 = xQ^2
  fp2_Multiply(xQ, t1, t2);  // t2 = xQ^3
  fp2_Multiply(a, t1, t1);   // t1 = a*xQ^2
  fp2_Add(t2, t1, t1);       // t1 = xQ^3+a*xQ^2
  fp2_Add(t1, xQ, t1);       // t1 = xQ^3+a*xQ^2+xQ
  fp2_Sqrt(t1, &Q->y);    // yQ = sqrt(xQ^3+a*xQ^2+xQ)

  fp2_Copy(xP, &P->x);
  fp2_Copy(xQ, &Q->x);

  fp2_Set(b, 1, 0);

  mont_pt_copy(Q, &T);
  fp2_Negative(&T.y, &T.y);
  xADD(curve, P, &T, &T);

  if (!fp2_IsEqual(&T.x, xR))
    fp2_Negative(&Q->y, &Q->y);

  mont_pt_clear(&T);
}

// PARAMS

const sidh_params_raw_t sidhRawParams = {
    .param_cA1 = CURVEA1,
    .param_cA2 = CURVEA2,
    .param_cB1 = CURVEB1,
    .param_cB2 = CURVEB2,
    .param_xp21 = XP21,
    .param_xp22 = XP22,
    .param_yp21 = YP21,
    .param_yp22 = YP22,
    .param_xq21 = XQ21,
    .param_xq22 = XQ22,
    .param_yq21 = YQ21,
    .param_yq22 = YQ22,
    .param_xp31 = XP31,
    .param_xp32 = XP32,
    .param_yp31 = YP31,
    .param_yp32 = YP32,
    .param_xq31 = XQ31,
    .param_xq32 = XQ32,
    .param_yq31 = YQ31,
    .param_yq32 = YQ32,
};

void sidh_setup_params(const sidh_params_raw_t *raw, sidh_params_t *params) {
    mont_curve_int_t* startingCurve = &params->startingCurve;
    mont_pt_t* param_P2 = &params->param_P2;
    mont_pt_t* param_Q2 = &params->param_Q2;
    mont_pt_t* param_P3 = &params->param_P3;
    mont_pt_t* param_Q3 = &params->param_Q3;
    mont_curve_init(startingCurve);
    mont_pt_init(param_P2);
    mont_pt_init(param_Q2);
    mont_pt_init(param_P3);
    mont_pt_init(param_Q3);
    fp2_Set(&startingCurve->a, raw->param_cA1, raw->param_cA2);
    fp2_Set(&startingCurve->b, raw->param_cB1, raw->param_cB2);
    fp2_Set(&param_P2->x, XP21, XP22);
    fp2_Set(&param_P2->y, YP21, YP22);
    fp2_Set(&param_Q2->x, XQ21, XQ22);
    fp2_Set(&param_Q2->y, YQ21, YQ22);    
    fp2_Set(&param_P3->x, XP31, XP32);
    fp2_Set(&param_P3->y, YP31, YP32);
    fp2_Set(&param_Q3->x, XQ31, XQ32);
    fp2_Set(&param_Q3->y, YQ31, YQ32); 
};

void sidh_teardown_params(sidh_params_t *params) {
    mont_curve_int_t* startingCurve = &params->startingCurve;
    mont_pt_t* param_P2 = &params->param_P2;
    mont_pt_t* param_Q2 = &params->param_Q2;
    mont_pt_t* param_P3 = &params->param_P3;
    mont_pt_t* param_Q3 = &params->param_Q3;
    mont_curve_clear(startingCurve);
    mont_pt_clear(param_P2);
    mont_pt_clear(param_Q2);
    mont_pt_clear(param_P3);
    mont_pt_clear(param_Q3);
}


// ISOGENIES

void eval_2_iso(const mont_pt_t *P2, const mont_pt_t *P, mont_pt_t *isoP) {

  const fp2* x2 = &P2->x;
  const fp2* x = &P->x;
  const fp2* y = &P->y;
  fp2* xx = &isoP->x;
  fp2* yy = &isoP->y;

  // xx:=(x^2*x2-x)/(x-x2);
  // yy:=y*(x^2*x2-2*x*x2^2+x2)/(x-x2)^2;

  fp2 t1 = { 0 }, t2 = { 0 }, t3 = { 0 }, one = { 0 };
  fp2_Init(&t1);
  fp2_Init(&t2);
  fp2_Init(&t3);
  fp2_Init_set(&one, 1, 0);

  fp2_Multiply(x, x2, &t1);         // t1:=x*x2;  // t1 = x*x2
  fp2_Multiply(x, &t1, &t2);        // t2:=x*t1;  // t2 = x^2*x2
  fp2_Multiply(&t1, x2, &t3);       // t3:=t1*x2; // t3 = x*x2^2
  fp2_Add(&t3, &t3, &t3);           // t3:=t3+t3; // t3 = 2*x*x2^2
  fp2_Sub(&t2, &t3, &t3);           // t3:=t2-t3; // t3 = x^2*x2-2*x*x2^2
  fp2_Add(&t3, x2, &t3);            // t3:=t3+x2; // t3 = x^2*x2-2*x*x2^2+x2
  fp2_Multiply(y, &t3, &t3);        // t3:=y*t3;  // t3 = y*(x^2*x2-2*x*x2^2+x2)
  fp2_Sub(&t2, x, &t2);             // t2:=t2-x;  // t2 = x^2*x2-x
  fp2_Sub(x, x2, &t1);              // t1:=x-x2;  // t1 = x-x2
  fp2_Invert(&t1, &t1);             // t1:=1/t1;  // t1 = 1/(x-x2)
  fp2_Multiply(&t2, &t1, xx);       // xx:=t2*t1; // xx = (x^2*x2-x)/(x-x2)
  fp2_Square(&t1, &t1);             // t1:=t1^2;  // t1 = 1/(x-x2)^2
  fp2_Multiply(&t3, &t1, yy);       // yy:=t3*t1; // yy = y*(x^2*x2-2*x*x2^2+x2)/(x-x2)^2

  fp2_Clear(&t1);
  fp2_Clear(&t2);
  fp2_Clear(&t3);
  fp2_Clear(&one);
}

void curve_2_iso(const mont_pt_t *P2, const mont_curve_int_t *E, mont_curve_int_t *isoE) {

  // aa:=2*(1-2*x2^2);
  // bb:=x2*b;

  const fp2* x2 = &P2->x;
  //const fp2* a = &E->a;
  const fp2* b = &E->b;

  fp2* aa = &isoE->a;
  fp2* bb = &isoE->b;

  fp2 t1 = { 0 }, one = { 0 };
  fp2_Init(&t1);
  fp2_Init_set(&one, 1, 0);

  fp2_Square(x2, &t1);     // t1 = x2^2
  fp2_Add(&t1, &t1, &t1);  // t1 = 2*x2^2
  fp2_Sub(&one, &t1, &t1); // t1 = 1-2*x2^2
  fp2_Add(&t1, &t1, aa);   // aa = 2*(1-2*x2^2)
  fp2_Multiply(x2, b, bb); // bb = x2*b

  fp2_Clear(&t1);
}

void eval_3_iso(const mont_pt_t *P3, const mont_pt_t *P, mont_pt_t *isoP) {

  const fp2* x3 = &P3->x;
  const fp2* x = &P->x;
  const fp2* y = &P->y;

  // xx:=x*(x*x3-1)^2/(x-x3)^2;
  // yy:=y*(x*x3-1)*(x^2*x3-3*x*x3^2+x+x3)/(x-x3)^3;

  fp2 t1 = { 0 }, t2 = { 0 }, t3 = { 0 }, t4 = { 0 }, one = { 0 };
  fp2_Init(&t1);
  fp2_Init(&t2);
  fp2_Init(&t3);
  fp2_Init(&t4);
  fp2_Init_set(&one, 1, 0);

  fp2_Square(x, &t1);             // t1 = x^2
  fp2_Multiply(&t1, x3, &t1);     // t1 = x^2*x3
  fp2_Square(x3, &t2);            // t2 = x3^2
  fp2_Multiply(x, &t2, &t2);      // t2 = x*x3^2
  fp2_Add(&t2, &t2, &t3);         // t3 = 2*x*x3^2
  fp2_Add(&t2, &t3, &t2);         // t2 = 3*x*x3^2
  fp2_Sub(&t1, &t2, &t1);         // t1 = x^2*x3-3*x*x3^2
  fp2_Add(&t1, x, &t1);           // t1 = x^2*x3-3*x*x3^2+x
  fp2_Add(&t1, x3, &t1);          // t1 = x^2*x3-3*x*x3^2+x+x3

  fp2_Sub(x, x3, &t2);            // t2 = x-x3
  fp2_Invert(&t2, &t2);           // t2 = 1/(x-x3)
  fp2_Square(&t2, &t3);           // t3 = 1/(x-x3)^2
  fp2_Multiply(&t2, &t3, &t2);    // t2 = 1/(x-x3)^3

  fp2_Multiply(x, x3, &t4);       // t4 = x*x3
  fp2_Sub(&t4, &one, &t4);        // t4 = x*x3-1

  fp2_Multiply(&t4, &t1, &t1);    // t1 = (x*x3-1)*(x^2*x3-3*x*x3^2+x+x3)
  fp2_Multiply(&t1, &t2, &t1);    // t1 = (x*x3-1)*(x^2*x3-3*x*x3^2+x+x3)/(x-x3)^3

  fp2_Square(&t4, &t2);           // t2 = (x*x3-1)^2
  fp2_Multiply(&t2, &t3, &t2);    // t2 = (x*x3-1)^2/(x-x3)^2

  fp2_Multiply(x, &t2, &isoP->x); // xx = x*(x*x3-1)^2/(x-x3)^2
  fp2_Multiply(y, &t1, &isoP->y); // yy = y*(x*x3-1)*(x^2*x3-3*x*x3^2+x+x3)/(x-x3)^3

  fp2_Clear(&t1);
  fp2_Clear(&t2);
  fp2_Clear(&t3);
  fp2_Clear(&t4);
  fp2_Clear(&one);
}

void curve_3_iso(const mont_pt_t *P3, const mont_curve_int_t *E, mont_curve_int_t *isoE) {

  // aa:=(a*x3-6*x3^2+6)*x3;
  // bb:=b*x3^2;

  const fp2* x3 = &P3->x;
  const fp2* a = &E->a;
  const fp2* b = &E->b;
  fp2* aa = &isoE->a;
  fp2* bb = &isoE->b;

  fp2 t1 = { 0 }, t2 = { 0 };
  fp2_Init(&t1);
  fp2_Init(&t2);

  fp2_Square(x3, &t1);         // t1 = x3^2
  fp2_Multiply(b, &t1, bb);    // bb = b*x3^2

  fp2_Add(&t1, &t1, &t1);      // t1 = 2*x3^2
  fp2_Add(&t1, &t1, &t2);      // t2 = 4*x3^2
  fp2_Add(&t1, &t2, &t1);      // t1 = 6*x3^2
  fp2_Set(&t2, 6, 0);          // t2 = 6
  fp2_Sub(&t1, &t2, &t1);      // t1 = 6*x3^2-6
  fp2_Multiply(a, x3, &t2);    // t2 = a*x3
  fp2_Sub(&t2, &t1, &t1);      // t1 = a*x3-6*x3^2+6
  fp2_Multiply(&t1, x3, aa);   // aa = (a*x3-6*x3^2+6)*x3

  fp2_Clear(&t1);
  fp2_Clear(&t2);
}

void eval_4_iso(const mont_pt_t *P4, const mont_pt_t *P, mont_pt_t *isoP) {

  const fp2* x4 = &P4->x;
  const fp2* x = &P->x;
  const fp2* y = &P->y;
  fp2* xx = &isoP->x;
  fp2* yy = &isoP->y;

  fp2 t1 = { 0 }, t2 = { 0 }, t3 = { 0 }, t4 = { 0 }, t5 = { 0 }, one = { 0 };
  fp2_Init(&t1);
  fp2_Init(&t2);
  fp2_Init(&t3);
  fp2_Init(&t4);
  fp2_Init(&t5);
  fp2_Init_set(&one, 1, 0);

  // xx:=-(x*x4^2+x-2*x4)*x*(x*x4-1)^2/((x-x4)^2*(2*x*x4-x4^2-1));
  // yy:=-2*y*x4^2*(x*x4-1)*(x^4*x4^2-4*x^3*x4^3+2*x^2*x4^4+x^4-4*x^3*x4+10*x^2*x4^2-4*x*x4^3-4*x*x4+x4^2+1)/((x-x4)^3*(2*x*x4-x4^2-1)^2);

  fp2_Square(x, &t1);             // t1 = x^2
  fp2_Square(&t1, &t2);           // t2 = x^4
  fp2_Square(x4, &t3);            // t3 = x4^2
  fp2_Multiply(&t2, &t3, &t4);    // t4 = x^4*x4^2
  fp2_Add(&t2, &t4, &t2);         // t2 = x^4+x^4*x4^2
  fp2_Multiply(&t1, &t3, &t4);    // t4 = x^2*x4^2
  fp2_Add(&t4, &t4, &t4);         // t4 = 2*x^2*x4^2
  fp2_Add(&t4, &t4, &t5);         // t5 = 4*x^2*x4^2
  fp2_Add(&t5, &t5, &t5);         // t5 = 8*x^2*x4^2
  fp2_Add(&t4, &t5, &t4);         // t4 = 10*x^2*x4^2
  fp2_Add(&t2, &t4, &t2);         // t2 = x^4+x^4*x4^2+10*x^2*x4^2
  fp2_Multiply(&t3, &t3, &t4);    // t4 = x4^4
  fp2_Multiply(&t1, &t4, &t5);    // t5 = x^2*x4^4
  fp2_Add(&t5, &t5, &t5);         // t5 = 2*x^2*x4^4
  fp2_Add(&t2, &t5, &t2);         // t2 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4
  fp2_Multiply(&t1, x, &t1);      // t1 = x^3
  fp2_Multiply(x4, &t3, &t4);     // t4 = x4^3
  fp2_Multiply(&t1, &t4, &t5);    // t5 = x^3*x4^3
  fp2_Add(&t5, &t5, &t5);         // t5 = 2*x^3*x4^3
  fp2_Add(&t5, &t5, &t5);         // t5 = 4*x^3*x4^3
  fp2_Sub(&t2, &t5, &t2);         // t2 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3
  fp2_Multiply(&t1, x4, &t1);     // t1 = x^3*x4
  fp2_Add(&t1, &t1, &t1);         // t1 = 2*x^3*x4
  fp2_Add(&t1, &t1, &t1);         // t1 = 4*x^3*x4
  fp2_Sub(&t2, &t1, &t1);         // t1 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4
  fp2_Multiply(x, &t4, &t2);      // t2 = x*x4^3
  fp2_Add(&t2, &t2, &t2);         // t2 = 2*x*x4^3
  fp2_Add(&t2, &t2, &t2);         // t2 = 4*x*x4^3
  fp2_Sub(&t1, &t2, &t1);         // t1 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3
  fp2_Add(&t1, &t3, &t1);         // t1 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2
  fp2_Add(&t1, &one, &t1);        // t1 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1
  fp2_Multiply(x, x4, &t2);       // t2 = x*x4
  fp2_Sub(&t2, &one, &t4);        // t4 = x*x4-1
  fp2_Add(&t2, &t2, &t2);         // t2 = 2*x*x4
  fp2_Add(&t2, &t2, &t5);         // t5 = 4*x*x4
  fp2_Sub(&t1, &t5, &t1);         // t1 = x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4

  fp2_Multiply(&t4, &t1, &t1);    // t1 = (x*x4-1)*(x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4)
  fp2_Multiply(&t3, &t1, &t1);    // t1 = x4^2*(x*x4-1)*(x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4)
  fp2_Multiply(y, &t1, &t1);      // t1 = x4^2*(x*x4-1)*(x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4)
  fp2_Add(&t1, &t1, &t1);         // t1 = 2*x4^2*(x*x4-1)*(x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4)
  fp2_Negative(&t1, yy);          // yy = -2*x4^2*(x*x4-1)*(x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4)

  fp2_Sub(&t2, &t3, &t2);         // t2 = 2*x*x4-x4^2
  fp2_Sub(&t2, &one, &t1);        // t1 = 2*x*x4-x4^2-1
  fp2_Sub(x, x4, &t2);            // t2 = x-x4
  fp2_Multiply(&t2, &t1, &t1);    // t1 = (x-x4)*(2*x*x4-x4^2-1)
  fp2_Square(&t1, &t5);           // t5 = (x-x4)^2*(2*x*x4-x4^2-1)^2
  fp2_Multiply(&t5, &t2, &t5);    // t5 = (x-x4)^3*(2*x*x4-x4^2-1)^2
  fp2_Invert(&t5, &t5);           // t5 = 1/((x-x4)^3*(2*x*x4-x4^2-1)^2)
  fp2_Multiply(yy, &t5, yy);      // yy = -2*x4^2*(x*x4-1)*(x^4+x^4*x4^2+10*x^2*x4^2+2*x^2*x4^4-4*x^3*x4^3-4*x^3*x4-4*x*x4^3+x4^2+1-4*x*x4)/((x-x4)^3*(2*x*x4-x4^2-1)^2)

  fp2_Multiply(&t1, &t2, &t1);    // t1 = (x-x4)^2*(2*x*x4-x4^2-1)
  fp2_Invert(&t1, &t1);           // t1 = 1/((x-x4)^2*(2*x*x4-x4^2-1))
  fp2_Square(&t4, &t4);           // t4 = (x*x4-1)^2
  fp2_Multiply(&t1, &t4, &t1);    // t1 = (x*x4-1)^2/((x-x4)^2*(2*x*x4-x4^2-1))
  fp2_Multiply(x, &t1, &t1);      // t1 = x*(x*x4-1)^2/((x-x4)^2*(2*x*x4-x4^2-1))
  fp2_Multiply(x, &t3, &t2);      // t2 = x*x4^2
  fp2_Add(&t2, x, &t2);           // t2 = x*x4^2+x
  fp2_Add(x4, x4, &t3);           // t3 = 2*x4
  fp2_Sub(&t2, &t3, &t2);         // t2 = x*x4^2+x-2*x4
  fp2_Negative(&t2, &t2);         // t2 = -(x*x4^2+x-2*x4)
  fp2_Multiply(&t1, &t2, xx);     // xx = -(x*x4^2+x-2*x4)*x*(x*x4-1)^2/((x-x4)^2*(2*x*x4-x4^2-1))

  fp2_Clear(&t1);
  fp2_Clear(&t2);
  fp2_Clear(&t3);
  fp2_Clear(&t4);
  fp2_Clear(&t5);
  fp2_Clear(&one);

}

void curve_4_iso(const mont_pt_t *P4, const mont_curve_int_t *E, mont_curve_int_t *isoE) {

  const fp2 *x4 = &P4->x;
  //const fp2 *a = &E->coeff.a;
  const fp2 *b = &E->b;
  fp2 *aa = &isoE->a;
  fp2 *bb = &isoE->b;

  fp2 t1 = {0}, t2 = {0};
  fp2_Init(&t1);
  fp2_Init(&t2);

  // aa:=4*x4^4-2;
  // bb:=-(1/2)*x4*(x4^2+1)*b = -(1/2)*(x4^3+x4)*b;

  fp2_Square(x4, &t1);         // t1 = x4^2
  fp2_Square(&t1, aa);         // aa = x4^4
  fp2_Add(aa, aa, aa);         // aa = 2*x4^4
  fp2_Add(aa, aa, aa);         // aa = 4*x4^4
  fp2_Set(&t2, 2, 0);          // t2 = 2
  fp2_Sub(aa, &t2, aa);        // aa = 4*x4^4-2

  fp2_Multiply(x4, &t1, &t1);  // t1 = x4^3
  fp2_Add(&t1, x4, &t1);       // t1 = x4^3+x4
  fp2_Multiply(&t1, b, &t1);   // t1 = (x4^3+x4)*b
  fp2_Invert(&t2, &t2);        // t2 = 1/2 -> precompute
  fp2_Negative(&t2, &t2);      // t2 = -(1/2)
  fp2_Multiply(&t2, &t1, bb);

  fp2_Clear(&t1);
  fp2_Clear(&t2);
}

void iso_2_e(int e, const mont_curve_int_t *E, mont_pt_t *S, const mont_pt_t *P1, const mont_pt_t *P2, mont_curve_int_t *isoE, mont_pt_t *isoP1, mont_pt_t *isoP2) {

  int p1Eval = P1 != NULL, p2Eval = P2 != NULL;

  mont_pt_t T = { 0 };
  mont_pt_init(&T);
  mont_curve_copy(E, isoE);

  if (p1Eval)
    mont_pt_copy(P1, isoP1);

  if (p2Eval)
    mont_pt_copy(P2, isoP2);


  if (e % 2) {
    xDBLe(isoE, S, e - 1, &T);
    curve_2_iso(&T, isoE, isoE);
    eval_2_iso(&T, S, S);

    if (p1Eval)
      eval_2_iso(&T, isoP1, isoP1);

    if (p2Eval)
      eval_2_iso(&T, isoP2, isoP2);

    e--;
  }

  for (int i = e - 2; i >= 0; i -= 2) {
    xDBLe(isoE, S, i, &T);
    curve_4_iso(&T, isoE, isoE);

    eval_4_iso(&T, S, S);

    if (p1Eval)
      eval_4_iso(&T, isoP1, isoP1);

    if (p2Eval)
      eval_4_iso(&T, isoP2, isoP2);
  }

  mont_pt_clear(&T);
}

void iso_3_e(int e, const mont_curve_int_t *E, mont_pt_t *S, const mont_pt_t *P1, const mont_pt_t *P2, mont_curve_int_t *isoE, mont_pt_t *isoP1, mont_pt_t *isoP2) {

  int p1Eval = P1 != NULL, p2Eval = P2 != NULL;

  mont_pt_t T = { 0 };
  mont_pt_init(&T);
  mont_curve_copy(E, isoE);

  if (p1Eval)
    mont_pt_copy(P1, isoP1);

  if (p2Eval)
    mont_pt_copy(P2, isoP2);

  for (int i = e - 1; i >= 0; --i) {
    xTPLe(isoE, S, i, &T);
    curve_3_iso(&T, isoE, isoE);

    eval_3_iso(&T, S, S);

    if (p1Eval)
      eval_3_iso(&T, isoP1, isoP1);

    if (p2Eval)
      eval_3_iso(&T, isoP2, isoP2);
  
  }

  mont_pt_clear(&T);
}



// Pretty printing
static void printPoint (const mont_pt_t P) {
    printf("{x = [0x%08x, 0x%08x], y = [0x%08x, 0x%08x]}\n", *P.x.x0, *P.x.x1, *P.y.x0, *P.y.x1);
}

static void printCurve (const mont_curve_int_t C) {
    printf("{a = [0x%08x, 0x%08x], b = [0x%08x, 0x%08x]}\n", *C.a.x0, *C.a.x1, *C.b.x0, *C.b.x1);
}


int main () {
    sidh_params_t * params;
    sidh_setup_params(&sidhRawParams, params);
    
    mont_pt_t R;
    mont_pt_init(&R);
    mont_pt_t iP1;
    mont_pt_init(&iP1);
    mont_pt_t iP2;
    mont_pt_init(&iP2);
    mont_curve_int_t C;
    mont_curve_init(&C);
    mp i;
    fp_Init(&i);
    fp_Constant(4, i);
    mont_double_and_add(&params->startingCurve, i, &params->param_Q2, &R, 31);
    xADD(&params->startingCurve, &R, &params->param_P2, &R);
    iso_2_e(15, &params->startingCurve, &R, &params->param_P3, &params->param_Q3, &C, &iP1, &iP2);
    printCurve(C);
    printPoint(iP1);
    printPoint(iP2);
    sidh_teardown_params(params);

    return 0;
}
/*
sidh32> let S = xADD startingCurve param_P3 (double_and_add startingCurve param_Q3 4)
[warning] at <interactive>:1:38--1:52:
  Defaulting type argument 'n' of 'double_and_add' to 3
sidh32> iso_3_e S (startingCurve, [param_P2, param_Q2])
({A = [0x02240ecd, 0x014eab54], B = [0x099b0a56, 0x0aec096b]},
 [{x = [0x03623611, 0x00d0dae5], y = [0x05b0ece0, 0x05492c58]},
  {x = [0x031f6418, 0x003736ba], y = [0x0cb1647c, 0x07da285b]}])
  */
