#include<stdio.h>
#include<math.h>
float detrm( float[][], float );
void cofact( float[][], float );
void trans( float[][], float[][], float );
int main()
{
    float a[ 25 ][ 25 ], k, d;
    int i, j;
    printf( "ENTER THE ORDER OF THE MATRIX:\n" );
    scanf( "%f", &k );
    printf( "ENTER THE ELEMENTS OF THE MATRIX:\n" );
 
    for ( i = 0;i < k;i++ )
        {
            for ( j = 0;j < k;j++ )
                {
                    scanf( "%f", &a[ i ][ j ] );
                }
        }
 
    d = detrm( a, k );
    printf( "THE DETERMINANT IS=%f", d );
 
    if ( d == 0 )
        printf( "\nMATRIX IS NOT INVERSIBLE\n" );
    else
        cofact( a, k );
}
 
/******************FUNCTION TO FIND THE DETERMINANT OF THE MATRIX************************/
 
float detrm( float a[ 25 ][ 25 ], float k )
{
    float s = 1, det = 0, b[ 25 ][ 25 ];
    int i, j, m, n, c;
 
    if ( k == 1 )
        {
            return ( a[ 0 ][ 0 ] );
        }
    else
        {
            det = 0;
 
            for ( c = 0;c < k;c++ )
                {
                    m = 0;
                    n = 0;
 
                    for ( i = 0;i < k;i++ )
                        {
                            for ( j = 0;j < k;j++ )
                                {
                                    b[ i ][ j ] = 0;
 
                                    if ( i != 0 && j != c )
                                        {
                                            b[ m ][ n ] = a[ i ][ j ];
 
                                            if ( n < ( k - 2 ) )
                                                n++;
                                            else
                                                {
                                                    n = 0;
                                                    m++;
                                                }
                                        }
                                }
                        }
 
                    det = det + s * ( a[ 0 ][ c ] * detrm( b, k - 1 ) );
                    s = -1 * s;
                }
        }
 
    return ( det );
}
 
/*******************FUNCTION TO FIND COFACTOR*********************************/
 
void cofact( float num[ 25 ][ 25 ], float f )
{
    float b[ 25 ][ 25 ], fac[ 25 ][ 25 ];
    int p, q, m, n, i, j;
 
    for ( q = 0;q < f;q++ )
        {
            for ( p = 0;p < f;p++ )
                {
                    m = 0;
                    n = 0;
 
                    for ( i = 0;i < f;i++ )
                        {
                            for ( j = 0;j < f;j++ )
                                {
                                    b[ i ][ j ] = 0;
 
                                    if ( i != q && j != p )
                                        {
                                            b[ m ][ n ] = num[ i ][ j ];
 
                                            if ( n < ( f - 2 ) )
                                                n++;
                                            else
                                                {
                                                    n = 0;
                                                    m++;
                                                }
                                        }
                                }
                        }
 
                    fac[ q ][ p ] = pow( -1, q + p ) * detrm( b, f - 1 );
                }
        }
 
    trans( num, fac, f );
}
 
/*************FUNCTION TO FIND TRANSPOSE AND INVERSE OF A MATRIX**************************/
 
void trans( float num[ 25 ][ 25 ], float fac[ 25 ][ 25 ], float r )
 
{
    int i, j;
    float b[ 25 ][ 25 ], inv[ 25 ][ 25 ], d;
 
    for ( i = 0;i < r;i++ )
        {
            for ( j = 0;j < r;j++ )
                {
                    b[ i ][ j ] = fac[ j ][ i ];
                }
        }
 
    d = detrm( num, r );
    inv[ i ][ j ] = 0;
 
    for ( i = 0;i < r;i++ )
        {
            for ( j = 0;j < r;j++ )
                {
                    inv[ i ][ j ] = b[ i ][ j ] / d;
                }
        }
 
    printf( "\nTHE INVERSE OF THE MATRIX:\n" );
 
    for ( i = 0;i < r;i++ )
        {
            for ( j = 0;j < r;j++ )
                {
                    printf( "\t%f", inv[ i ][ j ] );
                }
 
            printf( "\n" );
        }
}
