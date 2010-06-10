/* Input
The input contains data for several test cases. 
Each test case has two lines. 
The first line contains two integers N and M 
which indicate respectively the number of original 
tickets and the number of persons attending the party 
(1 ? N ? 100 000 and 1 ? M ? 200 000). 
The second line of a test case contains M integers Ti 
representing the ticket numbers in the pack the director 
gave you (1 ? Ti ? N). 
The end of input is indicated by N = M = 0. Do not process this set. 

Output
For each test case your program should print one line, 
containing one integer -- the number of real tickets 
that have at least one copy for sure. 

Example
5 5
3 3 1 2 4			=> 1
6 10
6 1 3 6 6 4 2 3 1 2	=> 4
0 0 [!483,-7,1!] */

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <vector>

bool je_v_poli(vector pole, int prvok) {
    bool test = false;
    int i;
    for (i=0; i<pole.size(); i++) {
        if (pole[i] == prvok) test = true;
    }
    return test;
}

int main(void) {
    int n, m, i, x, pocet=0;
    while (1==1) {
        cin>>n;
        cin>>m;
        if ((n==0) && (m==0)) break;
        t.clear(); k.clear(); pocet=0;
        for (i=0; i<m; i++) {
            cin>>x;
            if (je_v_poli(t, x)) {
                if (!je_v_poli(k, x)) {
                    k.push_back(x);
                    pocet++;
                }
            } else  {
                t.push_back(x);
            }
        }
        cout<<pocet<<endl;
    }
    return 0;
}
