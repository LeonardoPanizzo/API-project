#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUSER 61

struct user{
    char name[50];
    struct user *after;
    struct user *before;
};

struct entrante{
    char name[50];
    struct entrante *after;
    struct entrante *before;
    struct ricevente *receiver;
};

struct ricevente{
    char name[50];
    int follower;
    struct relazione *relation;
    struct ricevente *after;
    struct ricevente *before;
    struct entrante *seguace;
};

struct relazione{
    char name[50];
    struct relazione *after;
    struct relazione *before;
    struct ricevente *ricevente;
};

struct user* newUser(char *s){
    struct user* nuser=(struct user*)malloc(sizeof(struct user));
    strcpy(nuser->name,s);
    nuser->after=NULL;
    nuser->before=NULL;
    return nuser;
}

struct entrante* newEntrante(char *s){
    struct entrante* nentrante=(struct entrante*)malloc(sizeof(struct entrante));
    strcpy(nentrante->name,s);
    nentrante->after=NULL;
    nentrante->before=NULL;
    nentrante->receiver=NULL;
    return nentrante;
}

struct ricevente* newRicevente(char *s){
    struct ricevente* nricevente=(struct ricevente*)malloc(sizeof(struct ricevente));
    strcpy(nricevente->name,s);
    nricevente->follower=1;
    nricevente->relation=NULL;
    nricevente->after=NULL;
    nricevente->before=NULL;
    nricevente->seguace=NULL;
    return nricevente;
}

struct relazione* newRelazione(char *s){
    struct relazione* nrelazione=(struct relazione*)malloc(sizeof(struct relazione));
    strcpy(nrelazione->name,s);
    nrelazione->after=NULL;
    nrelazione->before=NULL;
    nrelazione->ricevente=NULL;
    return nrelazione;
}

/*
funzione di hash per le relazioni che distribuisce su 5 chiavi
*/
int hashrel(char *s){
    int x=0;
    if(s[1]=='-' || (s[1]>='0' && s[1]<='9')){
        x=0;
    }else if(s[1]>='A' && s[1]<='M'){
        x=1;
    }else if((s[1]>='N' && s[1]<='Z') || s[1]=='_'){
        x=2;
    }else if(s[1]>='a' && s[1]<='m'){
        x=3;
    }else if(s[1]>='n' && s[1]<='z'){
        x=4;
    }
    return x;
}

/*
funzione di hash per gli utenti che distribuisce su NUSER chiavi
*/
int hashuser(char *s){
    if(strlen(s)>=3){
        int a=((s[1]*7)+s[2])%NUSER;
        return a;
    }else{
        int a=(s[1]*7)%NUSER;
        return a;
    }
}

/*
Questo metodo viene usato per aggiungere un utente in ordine alfabetico nella lista
restituisce un puntatore a utente che va a sostituire quello che punta alla lista perchè così
funziona anche nel caso in cui il nome che si vuole inserire sia precedente al primo della lista.
*/
struct user* add(struct user *root,char *s){
    struct user *aa=newUser(s);
    if(root==NULL){     //nel caso in cui la lista sia vuota;
        root=aa;
    }else if(strcmp(root->name,s)>0){     //nel caso in cui vada inserito prima del primo elemento della stringa
        aa->after=root;
        root->before=aa;
        root=aa;
    }else{
        struct user* temp=root;
        int here=0;
        while(temp->after!=NULL && here==0){
            if(strcmp(temp->name,s)<0 && strcmp(temp->after->name,s)<0){  //va inserito dopo
                temp=temp->after;
            }else{
                here=1;
            }
        }
        if(temp->after==NULL && here==0){   //va inserito alla fine
            temp->after=aa;
            aa->before=temp;
        }else if(here=1){   //va inserito fra temp e temp->after
            aa->after=temp->after;
            aa->before=temp;
            temp->after=aa;
            aa->after->before=aa;
        }
    }
    return root;
}

/*
restituisce il puntatore all'utente cercato, utilizzato nella creazione delle relazioni, se l'utente non c'è restituisce NULL
*/
int getUser(struct user* root, char* s){
    struct user* temp=root;
    int found=0;
    int end=0;
    while(temp!=NULL && found==0 && end==0){
        if(strcmp(temp->name,s)==0){
            found=1;
        }else if(strcmp(temp->name,s)<0){
            temp=temp->after;
        }else if(strcmp(temp->name,s)>0){
            end=1;
        }
    }
    return found;
}

struct relazione* getExistingRel(struct relazione* rel, char* s){
    struct relazione* result=NULL;
    int end=0;
    while(!end){
        if(rel==NULL){
            end=1;
        }else if(strcmp(rel->name,s)>0){
            end=1;
        }else if(strcmp(rel->name,s)==0){
            result=rel;
            end=1;
        }else{
            rel=rel->after;
        }
    }
    return result;
}

/*
usato per la creazione di une relazione fra utenti, se la relazione esiste restituisce il puntatore a quella relazione,
se la relazione non esiste la crea e ne restituisce il puntatore, il tutto mantenendo i nomi delle relazioni in ordine alfabetico
*/
struct relazione* getRel(struct relazione* root, char *s){
    struct relazione* temp=root;
    if(temp==NULL){     //nel caso in cui la lista sia vuota
        temp=newRelazione(s);
        return temp;
    }else if(strcmp(root->name,s)>0){       //nel caso in cui la relazione vada inserita prima del primo elemento della lista in questo caso bisogna modificare la tabella di hash
        struct relazione* aa=newRelazione(s);
        aa->after=root;
        root->before=aa;
        return aa;
    }else{
        while(temp->after!=NULL){
            if(strcmp(temp->name,s)==0){    //temp contiene la relazione cercata
                return temp;
            }else if(strcmp(temp->name,s)<0 && strcmp(temp->after->name,s)<=0){  //se temp è presente è dopo
                temp=temp->after;
            }else if(strcmp(temp->name,s)<0 && strcmp(temp->after->name,s)>0){  //temp non è presente e dovrebbe trovarsi fra temp e temp->after
                struct relazione* tt=newRelazione(s);
                temp->after->before=tt;
                tt->after=temp->after;
                temp->after=tt;
                tt->before=temp;
                return tt;
            }
        }   //finito questo while temp punta all'ultimo elemento della lista quindi l'elemento cercato è l'ultimo o deve essere inserito dopo l'ultimo
        if(strcmp(temp->name,s)==0){
            return temp;
        }else{
            struct relazione* tt=newRelazione(s);
            temp->after=tt;
            tt->before=temp;
            return tt;
        }
    }
}

struct ricevente* getRic(struct ricevente* root, char *s){  //quando viene chiamato questo metodo so che ricevente non è NULL e il ricevente non va prima del primo elemento
    struct ricevente* temp=root;
    while(temp->after!=NULL){
        if(strcmp(temp->name,s)==0){
            return temp;
        }else if(strcmp(temp->name,s)<0 && strcmp(temp->after->name,s)<=0){  //se temp è presente è dopo
            temp=temp->after;
        }else if(strcmp(temp->name,s)<0 && strcmp(temp->after->name,s)>0){  //il ricevente che cerco non è presente e dovrebbe essere fra temp e temp->after
            struct ricevente* tt=newRicevente(s);
            temp->after->before=tt;
            tt->after=temp->after;
            temp->after=tt;
            tt->before=temp;
            return tt;
        }
    }   //se il while è finito temp punta all'ultimo elemento
    if(strcmp(temp->name,s)==0){
        return temp;
    }else{
        struct ricevente* tt=newRicevente(s);
        temp->after=tt;
        tt->before=temp;
        return tt;
    }
    return NULL;
}

/*
la ricerca/inserimento della relazione è già stato gestito da getrel
*/
void addrel(struct relazione* rel, char* ricevente, char* entrante){
    struct ricevente* ric;
    struct entrante* entr;
    if(rel->ricevente==NULL){  //caso in cui non ci sia nessun ricevente
        ric=newRicevente(ricevente);
        rel->ricevente=ric;
        ric->relation=rel;
    }else if(strcmp(rel->ricevente->name,ricevente)>0){     //caso in cui il ricevente deve andare prima del primo ricevente
        ric=newRicevente(ricevente);
        ric->after=rel->ricevente;
        rel->ricevente->before=ric;
        ric->relation=rel;
        rel->ricevente=ric;
    }else{          //si trova in prima posizione o dopo
        ric=getRic(rel->ricevente,ricevente);
        ric->relation=rel;
    }
    //ora so che il ricevente è stato creato/trovato e ric punta a lui
    if(ric->seguace==NULL){     //ricevente non ha seguaci, quindi non faccio controlli su chi lo segue
        entr=newEntrante(entrante);
        ric->seguace=entr;
        entr->receiver=ric;
    }else{
        struct entrante* entran=ric->seguace;
        int donothing=0;
        while(entran->after!=NULL && (donothing==0)){     //controllo tutti gli entranti per vedere se entrante c'è già e per andare alla fine
            if(strcmp(entrante,entran->name)==0){
                donothing=1;
            }
            entran=entran->after;
        }
        if(donothing==0){   //se fra i seguaci non ho trovato il potenziale seguace controllo l'ultimo
            if(strcmp(entrante,entran->name)==0){
                donothing=1;
            }
        }
        if(donothing==0){
            entran->after=newEntrante(entrante);
            entran->after->before=entran;
            entran=entran->after;
            entran->receiver=ric;
            ric->follower=ric->follower+1;
        }
    }
}

/*
il motivo per cui viene restituito il puntatore è sempre il discorso del primo elemento
poi bisogna controllare anche le relazioni per controllare che la prima non sia nulla
*/
struct user* deleteuser(struct user* root, char *s){
    if(root!=NULL){
        struct user* temp=root;
        int found=0;
        int end=0;
        while(temp!=NULL && end==0 && found==0){
            if(strcmp(temp->name,s)==0){
                found=1;
            }else if(strcmp(temp->name,s)<0){
                temp=temp->after;
            }else{
                end=1;
            }
        }
        if(found==1){
            if(temp->before==NULL){ //questo utente è il primo
                if(temp->after==NULL){  //temp è unico
                    free(temp);
                    return NULL;
                }else{
                    temp->after->before=NULL;
                    root=temp->after;
                    free(temp);
                    return root;
                }
            }else if(temp->after==NULL){    //temp è ulitmo e c'è un altro user prima
                temp->before->after=NULL;
                free(temp);
            }else{  //temp è in mezzo
                temp->after->before=temp->before;
                temp->before->after=temp->after;
                free(temp);
                return root;
            }
        }
    }
    return root;
}

struct ricevente* cancRic(struct ricevente* ric){
    struct ricevente* sup;
    struct entrante* ent=ric->seguace;
    while(ent!=NULL){
        if(ent->after==NULL){
            free(ent);
            ent=NULL;
        }else{
            ent=ent->after;
            free(ent->before);
        }
    }
    if(ric->before==NULL){
        if(ric->after==NULL){
            ric->relation->ricevente=NULL;
            free(ric);
            return NULL;
        }else{
            ric->relation->ricevente=ric->after;
            ric->after->before=NULL;
            sup=ric->after;
            free(ric);
            return sup;
        }
    }else if(ric->after==NULL){
        ric->before->after=NULL;
        free(ric);
        return NULL;
    }else{
        ric->after->before=ric->before;
        ric->before->after=ric->after;
        sup=ric->after;
        free(ric);
        return sup;
    }
}

struct entrante* cancEnt(struct entrante* ent){
    struct entrante* sup;
    ent->receiver->follower-=1;
    if(ent->before==NULL){
        if(ent->after==NULL){
            ent->receiver->seguace=NULL;
            free(ent);
            return NULL;
        }else{
            ent->receiver->seguace=ent->after;
            ent->after->before=NULL;
            sup=ent->after;
            free(ent);
            return sup;
        }
    }else if(ent->after==NULL){
        ent->before->after=NULL;
        free(ent);
        return NULL;
    }else{
        ent->after->before=ent->before;
        ent->before->after=ent->after;
        sup=ent->after;
        free(ent);
        return sup;
    }
}

struct relazione* deleterel1(struct relazione* rel, char* s){
    struct relazione* temp=rel;
    struct ricevente* tempr;
    struct entrante* tempe;
    struct entrante* sup;
    struct ricevente* supr;
    struct relazione* suprel;
    while(temp!=NULL){
        tempr=temp->ricevente;
        while(tempr!=NULL){
            tempe=tempr->seguace;
            while(tempe!=NULL){
                if(strcmp(tempe->name,s)==0){
                    tempe->receiver->follower-=1;
                    if(tempe->before==NULL){
                        if(tempe->after==NULL){
                            tempr->seguace=NULL;
                            free(tempe);
                            tempe=NULL;
                        }else{
                            tempr->seguace=tempe->after;
                            tempe->after->before=NULL;
                            free(tempe);
                            tempe=tempr->seguace;
                        }
                    }else if(tempe->after==NULL){
                        tempe->before->after=NULL;
                        free(tempe);
                        tempe=NULL;
                    }else{
                        tempe->after->before=tempe->before;
                        tempe->before->after=tempe->after;
                        sup=tempe;
                        tempe=tempe->after;
                        free(sup);
                    }
                }else{
                    tempe=tempe->after;
                }
            }
            if(tempr->seguace==NULL){
                if(tempr->before==NULL){
                    if(tempr->after==NULL){
                        tempr->relation->ricevente=NULL;
                        free(tempr);
                        tempr=NULL;
                    }else{
                        tempr->relation->ricevente=tempr->after;
                        tempr->after->before=NULL;
                        free(tempr);
                        tempr=temp->ricevente;
                    }
                }else if(tempr->after==NULL){
                    tempr->before->after=NULL;
                    free(tempr);
                    tempr=NULL;
                }else{
                    tempr->after->before=tempr->before;
                    tempr->before->after=tempr->after;
                    supr=tempr;
                    tempr=tempr->after;
                    free(supr);
                }
            }else{
                tempr=tempr->after;
            }
        }
        if(temp->ricevente==NULL){
            if(temp->before==NULL){
                if(temp->after==NULL){
                    rel=NULL;
                    free(temp);
                    temp=NULL;
                }else{
                    rel=temp->after;
                    temp->after->before=NULL;
                    free(temp);
                    temp=rel;
                }
            }else if(temp->after==NULL){
                temp->before->after=NULL;
                free(temp);
                temp=NULL;
            }else{
                temp->after->before=temp->before;
                temp->before->after=temp->after;
                suprel=temp;
                temp=temp->after;
                free(suprel);
            }
        }else{
            temp=temp->after;
        }
    }
    return rel;
}

struct relazione* deleterel(struct relazione* rel, char *s){
    struct relazione* temp=rel;
    struct relazione* suprel;
    struct ricevente* tempr;
    while(temp!=NULL){
        tempr=temp->ricevente;
        while(tempr!=NULL){
            if(strcmp(tempr->name,s)==0){
                tempr=cancRic(tempr);
            }else if(strcmp(tempr->name,s)>0){
                tempr=NULL;
            }else{
                tempr=tempr->after;
            }
        }
        if(temp->ricevente==NULL){
            if(temp->before==NULL){
                if(temp->after==NULL){
                    rel=NULL;
                    free(temp);
                    temp=NULL;
                }else{
                    rel=temp->after;
                    temp->after->before=NULL;
                    free(temp);
                    temp=rel;
                }
            }else if(temp->after==NULL){
                temp->before->after=NULL;
                free(temp);
                temp=NULL;
            }else{
                temp->after->before=temp->before;
                temp->before->after=temp->after;
                suprel=temp;
                temp=temp->after;
                free(suprel);
            }
        }else{
            temp=temp->after;
        }
    }
    return rel;
}

/*
controlla tutte le relazioni e cancella quelle in cui compare l'utente passato
deprecated
*/
struct relazione* deleterelazione(struct relazione* rel, char *s){
    struct relazione* temp=rel;
    struct ricevente* ric;
    struct ricevente* supr;
    struct entrante* ent;
    struct entrante* supe;
    while(temp!=NULL){
        ric=temp->ricevente;
        while(ric!=NULL){
            if(strcmp(ric->name,s)==0){
                ric=cancRic(ric);
            }else{
                ent=ric->seguace;
                while(ent!=NULL){
                    if(strcmp(ent->name,s)==0){
                        ent=cancEnt(ent);;
                    }else{;
                        ent=ent->after;
                    }
                    /*if(ent==NULL){
                        printf("è null1\n");
                    }else{
                        printf("non è null1\n");
                    }*/
                }
                if(ric->seguace==NULL){
                    if(ric->before==NULL){
                        if(ric->after==NULL){
                            ric->relation->ricevente=NULL;
                            free(ric);
                            ric=NULL;
                        }else{
                            ric->relation->ricevente=ric->after;
                            ric->after->before=NULL;
                            supr=ric->after;
                            free(ric);
                            ric=supr;
                        }
                    }else if(ric->after==NULL){
                        ric->before->after=NULL;
                        free(ric);
                        ric=NULL;
                    }else{
                        ric->before->after=ric->after;
                        ric->after->before=ric->before;
                        supr=ric->after;
                        free(ric);
                        ric=supr;
                    }
                }
            }
        }
        if(temp->ricevente==NULL){
            if(temp->before==NULL){
                if(temp->after==NULL){
                    free(temp);
                    temp=NULL;
                    rel=NULL;
                }else{
                    rel=temp->after;
                    rel->before=NULL;
                    free(temp);
                    temp=rel;
                }
            }else if(temp->after==NULL){
                temp->before->after=NULL;
                free(temp);
                temp=NULL;
            }else{
                temp->before->after=temp->after;
                temp->after->before=temp->before;
                temp=temp->after;
                free(temp->before);
            }
        }else{
            temp=temp->after;
        }
    }
    return rel;
}

void cancRel(struct relazione* relaz,char* ric, char* entr){
    struct relazione* sup=relaz;
    struct ricevente* temp=relaz->ricevente;
    struct entrante* tempe;
    int found=0;
    int efound=0;
    int end=0;
    while(temp!=NULL && found==0 && end==0){
        if(strcmp(temp->name,ric)==0){
            found=1;
        }else if(strcmp(temp->name,ric)<0){
            temp=temp->after;
        }else{
            end=1;
        }
    }
    if(found==1){   //ho trovato il ricevente
        tempe=temp->seguace;
        while(tempe!=NULL && efound==0){
            if(strcmp(tempe->name,entr)==0){
                efound=1;
            }else{
                tempe=tempe->after;
            }
        }
        if(efound==1){  //qui dentro ho la relazione relaz, il ricevente temp e l'entrante tempe
            if(tempe->before==NULL){
                if(tempe->after==NULL){
                    tempe->receiver->seguace=NULL;
                }else{
                    tempe->after->before=NULL;
                    tempe->receiver->seguace=tempe->after;
                }
            }else if(tempe->after==NULL){
                tempe->before->after=NULL;
            }else{
                tempe->after->before=tempe->before;
                tempe->before->after=tempe->after;
            }
            tempe->receiver->follower-=1;
            free(tempe);
            if(temp->seguace==NULL){
                if(temp->before==NULL){
                    if(temp->after==NULL){
                        temp->relation->ricevente=NULL;
                    }else{
                        temp->after->before=NULL;
                        temp->relation->ricevente=temp->after;
                    }
                }else if(temp->after==NULL){
                    temp->before->after=NULL;
                }else{
                    temp->after->before=temp->before;
                    temp->before->after=temp->after;
                }
                free(temp);
            }
            if(relaz->ricevente==NULL){
                if(relaz->before!=NULL){
                    if(relaz->after==NULL){
                        relaz->before->after=NULL;
                        free(relaz);
                    }else{
                        relaz->after->before=relaz->before;
                        relaz->before->after=relaz->after;
                        free(relaz);
                    }
                }
            }
        }
    }
}

int report(struct relazione* rel, int spa){
    int spazio=spa;
    char out[10000];
    out[0]='\0';
    struct ricevente* ric;
    int max=0;
    while(rel!=NULL){
        ric=rel->ricevente;
        max=ric->follower;
        if(spazio==1){
            printf(" ");
        }else if(spazio==0){
            spazio=1;
        }
        printf("%s ",rel->name);
        while(ric!=NULL){
            if(ric->follower>max){
                max=ric->follower;
                out[0]='\0';
                strcpy(out, ric->name);
                strcat(out," ");
            }else if(max==ric->follower){
                strcat(out,ric->name);
                strcat(out," ");
            }
            ric=ric->after;
        }
        printf("%s",out);
        printf("%d;",max);
        out[0]='\0';
        rel=rel->after;
    }
    return spazio;
}

void stampaUtenti(struct user* root){
    struct user* temp=root;
    if(temp!=NULL){
        while(temp!=NULL){
            printf("%s, ",temp->name);
            temp=temp->after;
        }
        printf("\n");
    }
}

void stampaRelazioni(struct relazione* root){
    struct ricevente* rr;
    struct entrante* ee;
    while(root!=NULL){
        printf("%s\n",root->name);  //nome relazione
        rr=root->ricevente;
        while(rr!=NULL){
            printf("%s, ",rr->name);    //nome ricevente
            printf("%d: ",rr->follower); //num riceventi
            ee=rr->seguace;
            while(ee!=NULL){
                printf("%s, ",ee->name);
                ee=ee->after;
            }
            printf("\n");
            rr=rr->after;
        }
        root=root->after;
    }
}

int main()
{
    struct user* hashtable[NUSER];         //tabella degli utenti, decidere sistema di hash
    for(int i=0; i<NUSER; i++){
        hashtable[i]=NULL;
    }
    struct relazione* reltable[5];     //tabella hash delle relazioni, funziona semplicemente mettendo in ordine lafabetico
    for(int i=0; i<5; i++){
        reltable[i]=NULL;
    }
    char st[50];
    char rel[20];
    char st2[50];
    char ist[10];
    int v1;
    int v2;
    struct relazione* relaz=NULL;
    int k;
    do{
        k=scanf("%s",ist);
        if(strcmp(ist,"addent")==0){ //inserisco un utente  FATTO
            v1=scanf("%s",st);
            k=hashuser(st);
            v1=getUser(hashtable[k],st);
            if(v1==0){
                hashtable[k]=add(hashtable[k],st);
            }
        }else if(strcmp(ist,"delent")==0){  //cancello un utente
            v1=scanf("%s",st);
            k=hashuser(st);
            hashtable[k]=deleteuser(hashtable[k],st);
            for(k=0; k<5; k++){
                reltable[k]=deleterel(reltable[k],st);
                reltable[k]=deleterel1(reltable[k],st);
            }
        }else if(strcmp(ist,"addrel")==0){ //inserisco una relazione FATTO
            v1=scanf("%s",st2);
            v2=scanf("%s",st);
            k=scanf("%s",rel);
            k=hashuser(st);
            v2=getUser(hashtable[k],st);
            k=hashuser(st2);
            v1=getUser(hashtable[k],st2);
            if(v1==1 && v2==1){
                k=hashrel(rel);
                relaz=getRel(reltable[k],rel);
                if(reltable[k]==NULL){
                    reltable[k]=relaz;
                }else if(strcmp(reltable[k]->name,rel)>0){
                    reltable[k]=reltable[k]->before;
                }
                addrel(relaz,st,st2);
            }
        }else if(strcmp(ist,"delrel")==0){  //cancello una relazione FATTO
            v2=scanf("%s",st2);
            v1=scanf("%s",st);
            k=scanf("%s",rel);
            k=hashrel(rel);
            relaz=getExistingRel(reltable[k],rel);
            if(relaz!=NULL){
                cancRel(relaz,st,st2);
                if(reltable[k]->ricevente==NULL){
                    relaz=reltable[k];
                    if(reltable[k]->after=NULL){
                        reltable[k]=NULL;
                    }else{
                        reltable[k]=reltable[k]->after;
                    }
                    free(relaz);
                }
            }
        }else if(strcmp(ist,"report")==0){  //FATTO
            k=0;
            for(int i=0; i<5; i++){
                k=report(reltable[i],k);
            }
            if(k==0){
                printf("none");
            }
            printf("\n");
        }else if(strcmp(ist,"users")==0){
            for(k=0; k<NUSER; k++){
                stampaUtenti(hashtable[k]);
            }
        }else if(strcmp(ist,"rela")==0){
        for(k=0; k<5; k++){
            stampaRelazioni(reltable[k]);
        }
        }
    }while(strcmp(ist,"end")!=0);
    return 0;
}
