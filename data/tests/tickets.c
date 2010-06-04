// program
#include <stdio.h>

#include <stdlib.h>
#include <cmath>

#include <iostream>
#include <vector> 

/*
Input
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
0 0

Explanation
In the second case there have to be some fake copies 
of 4 tickets: #1, #2, #3 and #6, thus the answer is 4. 
(There are at least 5 fake tickets, as at least 2 copies 
of #6 are fake. Note that the ticket #4 may be fake, 
but we can't be sure and thus we assume #4 is real.) 
*/

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

		cin>>n; // pocet povodnych listkov
		cin>>m; // pocet ludi na party == pocet pouzitych listkov

		if ((n==0) && (m==0)) break;

		t.clear(); k.clear(); pocet=0;

		// ake cisla listkov mali ludia na party
		for (i=0; i<m; i++) {
			cin>>x;
			if (je_v_poli(t, x)) {
			//	cout<<endl<<"DEBUG "<<x<<" uz je v poli listkov";
				if (!je_v_poli(k, x)) {
				//	cout<<endl<<"DEBUG "<<x<<" este nie je v poli kopirovanych";
					k.push_back(x);
					pocet++;
				}
			} else  {
				t.push_back(x);
			}
		}

		// vypis vysledku
		cout<<pocet<<endl;

	} // end while

	return 0;

}