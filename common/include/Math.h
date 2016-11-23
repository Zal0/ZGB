#ifndef MATH_H
#define MATH_H

#define U_LESS_THAN(A, B) ((A) - (B) & 0x8000u)

#define DISTANCE(A, B) (U_LESS_THAN(A, B) ? (B - A) : (A - B))

#define SET_BIT(N, POS)   N |=   1 << POS
#define UNSET_BIT(N, POS) N &= ~(1 << POS)
#define GET_BIT(N, POS)   (1 & (N >> POS))

#endif