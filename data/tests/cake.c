#include <math.h>
int t, N, C,i,k;
long res;
long dp[30000];
int isSquare(int n);
int main(){
    scanf("%d",&t);
    for(k=0;k<t;k++)
    {
        scanf("%d%d",&N,&C);
        res = C;
        dp[0]=C;
        for (i=2;i<=N;i++)
        {
            if (i % 7 == 0)
                dp[i-1] = (i % 11) + 1;
            else if (isSquare(i))
                dp[i-1] = (long)sqrt(i);
             else if (i % 2 == 0)
                dp[i-1]=1+dp[i-2];
              else if (dp[i-2] == 0)
                dp[i-1]=dp[0];
               else
                dp[i-1]=dp[i-2]-1;
            
            res += dp[i-1];
        }
        printf("Data set %d: %ld units of cake are eaten.\n",k+1,res);
    }
    return 0;
}

int isSquare(int n)
{
    int tmp;
    tmp = (int)sqrt(n);
    if(tmp*tmp==n)
        return 1;
    else
         return 0;
}