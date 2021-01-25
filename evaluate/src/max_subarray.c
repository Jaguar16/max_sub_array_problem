#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <limits.h>
/*
    BAROUDI AYMEN
*/

struct tablo {
    int * tab;
    int size;
};

typedef enum {PREFIX, SUFFIX} T_SENS;
typedef enum {PLUS, MAX} OPERATEUR;

int max(int a, int b);
int apply(OPERATEUR op, int a, int b);

void initTabloNeutralElement(struct tablo* tableau, OPERATEUR op);
void generateArray(struct tablo * s);
void generateRandomArray(struct tablo * s, int size);
void printArray(struct tablo * tmp);

void montee(struct tablo * source, struct tablo * destination, OPERATEUR op, T_SENS sens);
void final(struct tablo * a, struct tablo * b, OPERATEUR op);
void descente(struct tablo * a, struct tablo * b, OPERATEUR op);

void sumPrefix(struct tablo * source, struct tablo * result);
void maxPrefix(struct tablo *source, struct tablo *result);
void sumSuffix(struct tablo *source, struct tablo *result);
void maxSuffix(struct tablo *source, struct tablo *result);

void maxSubArray(struct tablo * source, struct tablo * M);
struct tablo * maxSubSequence(struct tablo * source, struct tablo * indexes);
void writeResult(struct tablo * source, struct tablo * result, T_SENS sens);
struct tablo * findIndex(struct tablo * source);


/**
 * Fonction: apply
 * ---------------
 * @param a un entier
 * @param b un entier
 * @param OP un operateur (enum OPERATEUR)
 * 
 * Applique l'operateur op a a et b : a op b
 * 
 * @return a OP b
 */
int apply(OPERATEUR op, int a, int b){
    if (op == PLUS){
        return a + b;
    }
    if (op == MAX){
        return fmax(a,b);
    }
    return -1;
}

void printArray(struct tablo * tmp) {
    printf("-------- Array of size %i -------- \n", tmp -> size);
    int size = tmp -> size;
    int i;
    for (i = 0; i < size; ++i) {
        printf("%i ", tmp -> tab[i]);
    }
    printf("\n\n");
}

void printResult(struct tablo * tmp) {
    int size = tmp -> size;
    int i;
    for (i = 0; i < size; ++i) {
        printf("%i ", tmp -> tab[i]);
    }
    printf("\n");
}


struct tablo * allocateTablo(int size) {
    struct tablo * tmp = malloc(sizeof(struct tablo));
    tmp -> size = size;
    tmp -> tab = calloc(size, sizeof(int));
    return tmp;
}

/**
 * Fonction: initTabloNeutralElement
 * ---------------
 * @param tableau un pointeur vers une struct tablo
 * @param OP un operateur (enum OPERATEUR)
 * 
 * Initialise le premier element de l'arbre avec l'element neutre de l'operateur op
 */
void initTabloNeutralElement(struct tablo * tableau, OPERATEUR op){
    // initialise le tableau avec l'élement neutre de l'opérateur op
    if (op == PLUS){
        tableau->tab[1] = 0;
       
    }
    // l'élement neutre de la fonction max (on traite que les int) est la le nombre (int) le plus petit.
    if (op == MAX){
        tableau->tab[1] = INT_MIN;
    }
    
}

/**
 * Fonction: max
 * ---------------
 * @param x un entier
 * @param y un entier
 * cf. geeksforgeeks
 * @return max entre x et y
 */
int max(int x, int y){
    return x ^ ((x ^ y) & -(x < y));  
}

/**
 * Fonction: montee
 * ---------------
 * @param source un pointeur vers une struct tablo representant le tableau source de la phase montee du scan/prefix
 * @param destination un pointeur vers une struct tablo representant le tableau destination de la phase montee du scan/prefix
 * On suppose que la taille de destination est 2 fois la taille du tableau source
 * @param op un operateur (enum OPERATEUR)
 * @param sens un sens de lecture, prefixe ou suffixe
 * 
 * Effectue la phase montee du scan/prefix
 */
void montee(struct tablo * source, struct tablo * destination, OPERATEUR op, T_SENS sens) {
    /*Phase montee */
    // on suppose que le malloc de destination a ete fait avant
    // Si isPref = 0 la montee est suffix
    int m = log2(source->size);
    //Remplissage des 0
    initTabloNeutralElement(destination, op);
    
    //Remplissage du tableau prefix
    if (sens == PREFIX){
        #pragma omp parallel for num_threads(omp_get_num_procs()/2)
        for (int i = 0; i < source->size; i++){
        destination->tab[(i+source->size)] = source->tab[i];
        }
    }
    
    //Remplissage tableau suffix
    else if (sens == SUFFIX){
        #pragma omp parallel for num_threads(omp_get_num_procs()/2)
        for (int i = 0; i < source->size; i++){
        destination->tab[(i+source->size)] = source->tab[source->size - i - 1];
        }
    }

    //Montee de l'arbre prefix
    //Ajout de la borne max de la 2eme boucle pour respecter l'omp "Canonical loop form" de la doc d'OpenMP
    int borneMax = 0;
    for (int l = (m-1); l >= 0; l--){
        borneMax = pow(2,(l+1));
        #pragma omp parallel for num_threads(omp_get_num_procs()/2)
        for (int j = pow(2,l); j < borneMax; j++){
            destination->tab[j] = apply(op, destination->tab[2*j], destination->tab[(2*j)+1]);
        }
    }
    
}

/**
 * Fonction: descente
 * ---------------
 * @param a un pointeur vers une struct tablo representant le tableau intermediaire apres la phase montee du scan/prefix
 * @param b un pointeur vers une struct tablo representant le tableau destination de la phase descente du scan/prefix
 * @param op un operateur (enum OPERATEUR)
 * On suppose que la taille des deux tablo sont egaux
 * 
 * Effectue la phase descente du scan/prefix
 */
void descente(struct tablo * a, struct tablo * b, OPERATEUR op) {
    //Implementation de la phase de descente
    int m = log2((a->size)/2);
    //Ajout de la borne max de la 2eme boucle pour respecter l'omp "Canonical loop form" de la doc d'OpenMP
    int borneMax = 0;
    // Prefix
    for (int l = 1; l <= m; l++){
        borneMax = (pow(2,(l+1)))-1;
        #pragma omp parallel for num_threads(omp_get_num_procs()/2)
        for (int j = pow(2,l); j <= borneMax; j++){
            if (j & 1){ // si c'est impair
                b->tab[j] = apply(op, b->tab[(j-1)/2], a->tab[j-1]);
            }
            else{
                b->tab[j] = b->tab[j/2];
            }   
        }
    }
    
    
}

/**
 * Fonction: final
 * ---------------
 * @param a un pointeur vers une struct tablo representant le tableau intermediaire apres la phase descente du scan/prefix
 * @param b un pointeur vers une struct tablo representant le tableau final de la phase final du scan/prefix
 * Avec les resultat du scan/prefix dans la deuxieme moitiee du tablo b.
 * @param op un operateur (enum OPERATEUR)
 * 
 * Effectue la phase final du scan/prefix
 */
void final(struct tablo * a, struct tablo * b, OPERATEUR op) {
    //phase finale
    int m = log2((a->size)/2);
    //Ajout de la borne max de la boucle pour respecter l'omp "Canonical loop form" de la doc d'OpenMP
    int borneMax = (pow(2,(m+1)))-1;
    #pragma omp parallel for num_threads(omp_get_num_procs()/2)
    for (int j = pow(2,m); j <= borneMax; j++){
        b->tab[j] = apply(op, b->tab[j], a->tab[j]);
    }
}


/**
 * Fonction: sumPrefix
 * ---------------
 * @param source un pointeur vers une struct tablo representant le tableau dont il faut effectuer la somme prefix
 * @param result un pointeur vers une struct tablo representant le tableau final resultat de la somme prefix
 * @param op un operateur (enum OPERATEUR)
 * 
 * Effectue la somme prefix du tablo source et ecrit le resultat dans le tablo result
 */
void sumPrefix(struct tablo * source, struct tablo * result){ 
    struct tablo * a = allocateTablo(source->size * 2);
    struct tablo * b = allocateTablo(source->size * 2);
    
    montee(source, a, PLUS, PREFIX);
    //printArray(a);
    initTabloNeutralElement(b, PLUS);
    descente(a, b, PLUS);
    //printArray(b);

    final(a, b, PLUS);
    //printArray(b);

    writeResult(b, result, PREFIX);
    //printArray(result);
    free(a->tab);
    free(b->tab);
    free(a);
    free(b);
}

/**
 * Fonction: sumSuffix
 * ---------------
 * @param source un pointeur vers une struct tablo representant le tableau dont il faut effectuer la somme suffix
 * @param result un pointeur vers une struct tablo representant le tableau final resultat de la somme suffix
 * @param op un operateur (enum OPERATEUR)
 * 
 * Effectue la somme suffix du tablo source et ecrit le resultat dans le tablo result
 */
void sumSuffix(struct tablo * source, struct tablo * result){
    
    struct tablo * a = allocateTablo(source->size * 2);
    struct tablo * b = allocateTablo(source->size * 2);

    montee(  source, a, PLUS, SUFFIX);
    //printArray(a);
    initTabloNeutralElement(b, PLUS);
    descente(a, b, PLUS);
    //printArray(b);

    final(a, b, PLUS);
    //printArray(b);

    writeResult(b, result, SUFFIX);
    //printArray(result);    
    free(a->tab);
    free(b->tab);
    free(a);
    free(b);
}


/**
 * Fonction: maxPrefix
 * ---------------
 * @param source un pointeur vers une struct tablo representant le tableau dont il faut effectuer le max prefix
 * @param result un pointeur vers une struct tablo representant le tableau final resultat du max prefix
 * @param op un operateur (enum OPERATEUR)
 * 
 * Effectue le max prefix du tablo source et ecrit le resultat dans le tablo result
 */
void maxPrefix(struct tablo * source, struct tablo * result){
    
    struct tablo * a = allocateTablo(source->size * 2);
    struct tablo * b = allocateTablo(source->size * 2);

    montee(source, a, MAX, PREFIX);
    //printArray(a);
    initTabloNeutralElement(b, MAX);
    descente(a, b, MAX);
    //printArray(b);

    final(a, b, MAX);
    //printArray(b);

    writeResult(b, result, PREFIX);
    //printArray(result);
    free(a->tab);
    free(b->tab);
    free(a);
    free(b);
}

/**
 * Fonction: maxSuffix
 * ---------------
 * @param source un pointeur vers une struct tablo representant le tableau dont il faut effectuer le max suffix
 * @param result un pointeur vers une struct tablo representant le tableau final resultat du max suffix
 * @param op un operateur (enum OPERATEUR)
 * 
 * Effectue le max suffix du tablo source et ecrit le resultat dans le tablo result
 */
void maxSuffix(struct tablo * source, struct tablo * result){
    struct tablo * a = allocateTablo(source->size * 2);
    struct tablo * b = allocateTablo(source->size * 2);

    montee(source, a, MAX, SUFFIX);
    //printArray(a);
    initTabloNeutralElement(b, MAX);
    descente(a, b, MAX);
    //printArray(b);

    final(a, b, MAX);
    //printArray(b);

    writeResult(b, result, SUFFIX);
    //printArray(result);
    free(a->tab);
    free(b->tab);
    free(a);
    free(b);
}

/**
 * Fonction: writeResult
 * ---------------
 * @param source un pointeur vers une struct tablo representant un tableau resultat d'un scan/(suf)prefix
 * @param result un pointeur vers une struct tablo representant le tableau final resultat de la du scan/(suf)prefix
 * @param sens un sens de lecture (prefix ou suffix)
 * 
 * Parcours le tablo source dans le sens sens et ecrit dans result le resultat final du scan/(suf)prefix 
 */
void writeResult(struct tablo * source, struct tablo * result, T_SENS sens){
    // Lis le résultat de source et l'ecrit dans le tablo result
    // si on fait un préfix on prends les elements un à un à partir de la moitiee du tablo source
    int mid = source->size/2;
    if (sens == PREFIX){
        #pragma omp parallel for num_threads(omp_get_num_procs()/2)
        for (int i = 0; i < mid; i++){
            result->tab[i] = source->tab[i+mid];
        }
    }
    else if (sens == SUFFIX){
        #pragma omp parallel for num_threads(omp_get_num_procs()/2)
        for (int i = 0; i < mid; i++){
            result->tab[i] = source->tab[source->size - i - 1];
        }
    }
}

/**
 * Fonction: findIndex
 * ---------------
 * @param source un pointeur vers une struct tablo representant un tableau M resultal de l'implementation parallele du maximum subarray
 * Parcours le tablo source, stock dans un tablo l'index de debut et de fin de l'apparition du maximum du tablo source et le maximum
 * @return un pointeur de struct tablo de taille 3 contenant : index de debut, index de fin
 */
struct tablo * findIndex(struct tablo * source){
    int * maximum = NULL; // le maximum courant
    int * actual = NULL; // pour stocker et comparer l'entier courant du tablo source et le max courant
    struct tablo * bornes = allocateTablo(3); // pour stocker les bornes
    maximum = &source->tab[0]; // premier maximum
    for (int i = 1; i < source->size; i++){
        actual = &source->tab[i]; // l'entier courant du tablo
        if ((*actual) > (*maximum)){ // si l'entier courant et plus grand que l'actuel maximum
            bornes->tab[0] = i; // le index de debut est donc i
            bornes->tab[1] = bornes->tab[0]; // on reinitialise l'indice de fin
            maximum = actual;
        }    
        else if ((*actual) == (*maximum)){
        bornes->tab[1]++;
        }
        //printf("actual = %d, maximum = %d");
    }
    bornes->tab[2] = *maximum;
    return bornes;
}

/**
 * Fonction: maxSubSequence
 * ---------------
 * @param source un pointeur vers une struct tablo representant un tableau dont on veut calculer le maximum subarray
 * @param indexes un pointeur vers une struct tablo representant les indexes du debut et de fin qui contient la sequence du max subarray 
 * Parcours le tablo source et ecrit dans un tablo la sequence du maximum subarray du tablo source
 * @return un pointeur de struct tablo contenant la sequence du max subarray
 */
struct tablo * maxSubSequence(struct tablo * source, struct tablo * indexes){
    int size = indexes->tab[1] - indexes->tab[0] + 2;
    struct tablo * result = allocateTablo(size);
    result->tab[0] = indexes->tab[2];
    #pragma omp parallel for num_threads(omp_get_num_procs()/2)
    for (int i = 1; i < size; i++){
        //printf("%d \n", source->tab[indexes->tab[0] + i-2]);
        result->tab[i] = source->tab[indexes->tab[0] + i-1];
    }
    return result;
}

/**
 * Fonction: maxSubSequence
 * ---------------
 * @param source un pointeur vers une struct tablo representant un tableau dont on veut calculer le maximum subarray
 * @param M un pointeur vers une struct tablo representant ou sera ecris le resultat de l'algo parallele du maximum subarray
 * implemente l'algo decrit dans :
 * "Parallel Algorithms For Maximum Subsequence And Maximum Subarray (1995) by Kalyan Perumalla and Narsingh Deo"
 */
void maxSubArray(struct tablo * source, struct tablo * M){
    struct tablo * PSUM = allocateTablo(source->size);
    struct tablo * SSUM = allocateTablo(source->size);
    struct tablo * SMAX = allocateTablo(source->size);
    struct tablo * PMAX = allocateTablo(source->size);
    
    #pragma omp parallel num_threads(omp_get_num_procs()/2)
    {
        sumPrefix(source, PSUM);
        sumSuffix(source, SSUM);
    }

    #pragma omp parallel num_threads(omp_get_num_procs()/2)
    {
        maxPrefix(SSUM, PMAX);
        maxSuffix(PSUM, SMAX);
    }

    #pragma omp parallel for num_threads(omp_get_num_procs()/2)
    for (int i = 0; i < M->size; i++){
        M->tab[i] = (PMAX->tab[i] - SSUM->tab[i] + source->tab[i]) +
                    (SMAX->tab[i] - PSUM->tab[i] + source->tab[i]) - source->tab[i]; 
    }
    free(PSUM->tab);
    free(PSUM);
    free(SSUM->tab);
    free(SSUM);
    free(SMAX->tab);
    free(SMAX);
    free(PMAX->tab);
    free(PMAX);
}

void generateArray(struct tablo * s) {
    //construction d'un tableau pour tester
    s -> size = 16;
    s -> tab = malloc(s -> size * sizeof(int));
    s -> tab[0] = 3;
    s -> tab[1] = 2;
    s -> tab[2] = -7;
    s -> tab[3] = 11;
    s -> tab[4] = 10;
    s -> tab[5] = -6;
    s -> tab[6] = 4;
    s -> tab[7] = 9;
    s -> tab[8] = -6;
    s -> tab[9] = 1;
    s -> tab[10] = -2;
    s -> tab[11] = -3;
    s -> tab[12] = 4;
    s -> tab[13] = -3;
    s -> tab[14] = 0;
    s -> tab[15] = 2;
}


void generateRandomArray(struct tablo * s, int size) {
    s -> size = size;
    s -> tab = malloc(size * sizeof(int));
    int i;
    for (i = 0; i < size; i++) {
        s -> tab[i] = rand()%90001;
    }

}

/**
 * Fonction: readFile
 * ---------------
 * @param s un pointeur vers une struct tablo representant un tableau ou sera ecrit les entier du fichier fichier
 * @param fichier nom d'un fichier contenant un tableau d'une taille puissance de 2
 * lit le fichier et ecrit dans s les entiers dans le fichier 
 */
void readFile(struct tablo * s, char * fichier){
    int n = 0;
    int i = 0;
    FILE * file = fopen(fichier, "r");
    if (file == NULL){
        printf("Erreur lors de l'ouverture du fichier");
        exit(-1);
    }
    fseek(file, 0L, SEEK_END);
    s->size = ftell(file);
    rewind(file);
    s->tab = malloc(s->size * sizeof(int));
    while(fscanf(file, "%d", &n) == 1){
        s->tab[i] = n;
        i++;
    }
    s->size = i;
    fclose(file);

    
}


int main(int argc, char ** argv) {
    srand(time(NULL));
    struct tablo source;
    if (argc > 1) {
        //tailleArray = atoi(argv[1]);
        //generateRandomArray( & source, tailleArray);
        readFile(&source, argv[1]);
    }
    else{
    generateArray(&source);
    }
    
    struct tablo * M = allocateTablo(source.size);
    struct tablo * resultIndex = NULL;
    struct tablo * result = NULL;
    
    maxSubArray(&source, M);
    resultIndex = findIndex(M);
    //printArray(M);
    result = maxSubSequence(&source, resultIndex);
    printResult(result);

    // pour faire plaisir à valgrind
    free(source.tab);
    free(M->tab);
    free(M);
    free(resultIndex->tab);
    free(resultIndex);
    free(result->tab);
    free(result);
    return 0;
}
