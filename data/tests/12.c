/*
Andrej Fogelton
skupina 10
projekt DSA #3
zadanie #12 - hash tabulka databaza studentov

*/
#define VELKOST_MENA 20// definovana velkost mena a includnute potrebne kniznice
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<conio.h>


typedef struct student{
 //definovana datova struktura
 
    int id;// polozky
    char meno[VELKOST_MENA];
    char priezvisko[VELKOST_MENA];
    int rocnik;
    float priemer;
    int dalsi;
 
 }STUDENT;

int hash(int velkost,int id){
// funkcia na hashovanie pouzivam jednoduchu funkciu modulo

    while(id>=velkost)
        id=id-velkost;
    
    return id;

}
STUDENT create(STUDENT student){
//funkcia na priradenie defaultnych hodnot

    student.dalsi=-1;
    strcpy(student.meno,"");
    strcpy(student.priezvisko,"");
    student.id=-1;
    student.priemer=-1;
    student.rocnik=-1;
    
    return student;//vrati default studenta
}
//funkcia priradi zadane hodnoty danemu studentovi
STUDENT insert(STUDENT student,int id,char* meno,char* priezvisko,int rocnik,float priemer){

    student.dalsi=-1;
    strcpy(student.meno,meno);
    strcpy(student.priezvisko,priezvisko);
    student.id=id;
    student.priemer=priemer;
    student.rocnik=rocnik;
    
    return student;//vrati upraveneho studenta
}
STUDENT *delete_student(STUDENT *databaza,int id,int velkost,int *hash_table){
//vymaze studenta s danym id

    int kde;// pomocne premenne
    int rob=1;
    int odkial;
    int pom;
    
    odkial=hash(velkost,id);
    kde=hash_table[odkial];
    
    if(kde == -1){
    //ak vyslo ze v hash tabulke je -1 znamena to ze dany ziak neexistuje
        printf("ERROR neexistujuce id!\n");
        return databaza;
    }
    
    if(databaza[kde].id==id){
    //ak hned prvy prvok je ten hladany
    
        if(databaza[kde].dalsi!=-1)
        //a existuje dalsi tak ho da namiesto toho do tabulky
            hash_table[odkial]=databaza[kde].dalsi;
        else
            hash_table[odkial]=-1;//ak nie tak -1
        
        databaza[kde]=create(databaza[kde]);//vymaze ho
    }
    else{
    //ak ten prvy nie je ten hladany
        do{
            if(databaza[kde].id == -1){
            //ak je tam -1 tak neexistuje
                printf("ERROR neexistujuce id!\n");
                return databaza;
            }
            pom=kde;//ulozim si predchadzajucu hodnotu
            kde=databaza[kde].dalsi;
            
            if(kde == -1){
            //ak je tam -1 tak neexistuje
                printf("ERROR neexistujuce id!\n");
                return databaza;
            }
            if(databaza[kde].id==id){
            //ak to je hladany prvok
            
                databaza[pom].dalsi=databaza[kde].dalsi;//preskocim ho v zozname
                databaza[kde]=create(databaza[kde]);//a vymazem
                rob=0;//ukoncim cyklus
            }
        }while(rob);
    }
    return databaza;//vratim upravenu databazu
}
void print(int *hash_table,int velkost,int id, STUDENT *databaza){
//vytlacenie udajov o studentovi

    int kde;//pomocne premenne
    int rob=1;
    int odkial;
    int pom;
    
    odkial=hash(velkost,id);// ulozim si vypocty adries aby som nemusel ich vzdy vypocitavat
    kde=hash_table[odkial];
    
    if(kde ==-1){
    //ak je na tom mieste -1 tak neexistuje
        printf("ERROR neexistujuce id!\n");
        return ;
    }
    
    do{
    //hladam prvok s danym id
        if(databaza[kde].id==id){
        //ak je to on vytlacim udaje
            printf("\n%d %s %s %d %3.2f\n",databaza[kde].id,databaza[kde].meno,databaza[kde].priezvisko,databaza[kde].rocnik,databaza[kde].priemer);
            rob=0;
        }
        else{
        //ak to nie je on tak posunem sa v zozname
            kde=databaza[kde].dalsi;
            
            if(kde ==-1){
            // ak je tam -1 tak neexistuje
                printf("ERROR neexistujuce id!\n");
                return ;
            }
        }
    }while(rob);//cykli dokym nenajde daneho studenta alebo zisti ze neexistuje
}
void vypis(int *hash_table, int velkost, STUDENT *databaza,int velkost_d){
//vypis hash tabulky
    int i=0;
    printf("\n-----------\n");
    for(i=0;i<velkost;i++)
        printf("| %2d | %2d |\n",i,hash_table[i]);
    printf("-----------\n");
    //vypis databazy
    printf("----------------------------------------------------------\n");
    for(i=0;i<velkost_d;i++)
        printf("| %2d | %5d | %10s | %10s | %2d | %4.2f | %2d |\n",i,databaza[i].id,databaza[i].meno,databaza[i].priezvisko,databaza[i].rocnik,databaza[i].priemer,databaza[i].dalsi);
    
    printf("----------------------------------------------------------\n");

}
int najdi_volne(STUDENT *databaza,int velkost_d){
//funkcia na najdenie prveho volneho miesta v databaze

    for(i=0;i<velkost_d;i++)
        if(databaza[i].id==-1)
            return i;//vrati jeho hodnotu
    
    return -1;//ak nie je volne miesto vrati -1

}
int main(int argc, char **argv){

    STUDENT *databaza=NULL;//potrebne premenne
    int *hash_table=NULL;
    int rocnik=0,mom;
    float priemer=0;
    char meno[VELKOST_MENA],volba,priezvisko[VELKOST_MENA],a;
    int id=0,velkost=0,rob=1,i=0;
    int odkial,ok=1,kde=0,velkost_d=0,stara_velkost=0;
    //urci sa velkost hash tabulky
    printf("Zadaj velkost hash tabulky: ");
    scanf("%d",&velkost);
    //alokacia
    if((hash_table=(int *)malloc(sizeof(int)*velkost))==NULL){
        printf("ERROR pri alokacii pamti");
        getchar();
        exit(1);
    }
    //vynulovanie
    for(i=0;i<velkost;i++)
        hash_table[i]=-1;
    
    fflush(stdin);
    //urci sa velkost databazy
    printf("Zadaj velkost databazy studentov: ");
    scanf("%d",&velkost_d);
    //alokuje sa miesto
    if((databaza=(STUDENT *)malloc(sizeof(STUDENT)*velkost_d))==NULL){
        printf("ERROR pri alokacii pamti");
        getchar();
        exit(1);
    }
    
    for(i=0;i<velkost_d;i++)
    //vynuluje sa
        databaza[i]=create(databaza[i]);
        //menu programu
    printf("i - vlozit studenta do databazy\n");
    printf("d - vymaze studenta z databazy\n");
    printf("p - vytlacit udaje o studentovi\n");
    printf("v - vypis tabulky\n");
    printf("k - koniec programu\n");
    
    do{
    //vynulovanie buffra
        fflush(stdin);
        volba=tolower(getch());//volba sa da na male pismeno
        switch(volba){
            case'i':
            //vlozi studenta
                printf("Zadaj id, meno, priezvisko, rocnik a priemer studenta:\n");
                scanf("%d %s %s %d %f",&id,meno,priezvisko,&rocnik,&priemer);
                kde=najdi_volne(databaza,velkost_d);
                
                if(kde==-1){
                //ak nie je volne miesto
                    printf("Plna pamat!");
                    printf("Mam zvacsit databazu? a/n: ");
                    a=getchar();
                    fflush(stdin);
                    
                    if(a=='a'){
                    //ak ma zvacsit pamat
                        stara_velkost=velkost_d;
                        printf("Zadaj novu velkost: ");
                        scanf("%d",&velkost_d);
                        //zvacsi sa na danu velkost
                        if((databaza=(STUDENT *)realloc(databaza,sizeof(STUDENT)*velkost_d))==NULL){
                            printf("ERROR pri alokacii pamti");
                            getchar();
                            exit(1);
                        }
                        
                        for(i=stara_velkost;i<velkost_d;i++)
                            databaza[i]=create(databaza[i]);
                            //vynuluju sa nove prvky
                    }
                    else{
                        putchar('\n');
                        break;
                    }
                }
                odkial=hash(velkost,id);
                databaza[kde]=insert(databaza[kde],id,meno,priezvisko,rocnik,priemer);
                // ak je na danom miesto volne vlozi index
                if(hash_table[odkial]==-1){
                    hash_table[odkial]=kde;
                }
                else{
                //ak nie je miesto hlada sa posledny prvok v zozname
                    mom=hash_table[odkial];
                    while(ok){
                    
                        if(databaza[mom].dalsi==-1){
                            ok=0;//vlozi sa na koniec zoznamu
                            databaza[mom].dalsi=kde;
                        }
                        else
                            mom=databaza[mom].dalsi;
                    
                    }
                }
                break;
            case'p':
            //vytlact daneho studenta
                printf("Zadaj id na zobrazenie udajov o studentovi: ");
                scanf("%d",&id);
                print(hash_table,velkost,id,databaza);
                break;
            case'd':
            //vymazat
                printf("Zadaj id studenta, ktoreho chces vymazat: ");
                scanf("%d",&id);
                databaza =delete_student(databaza,id,velkost,hash_table);
                break;
            case'v':
            //vypis tabuliek
                vypis(hash_table,velkost,databaza,velkost_d);
                break;
            case'k':
            //koniec programu
                rob=0;
                break;
        }
        ok=1;
    }while(rob);
    
    //vymazanie alokovanej pamati
    free((void*)databaza);
    databaza=NULL;
    free((void*)hash_table);
    hash_table=NULL;
    
    return 0;
}