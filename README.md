# [Maximum subarray problem](https://en.wikipedia.org/wiki/Maximum_subarray_problem)
Une solution parallélisée en C avec [OpenMP](https://www.openmp.org/).
Implémentation de l'algorithme page 4, [Parallel Algorithms For Maximum Subsequence And Maximum Subarray (1995) by Kalyan Perumalla and Narsingh Deo](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.24.1291&rep=rep1&type=pdf)

J'ai implémenté la version parallèle du scan/pre(suf)fix sum(max) toujours avec OpenMP.
On considère les tableaux d'une taille qui est une puissance de 2 uniquement.

## Compiler
Il faut compiler avec la commande :
```sh
$gcc -o max_subarray -Wall -std=c99 max_subarray.c -lm -fopenmp
```

## Utilisation

```sh
$max_subarray file
```
- file : fichier contenant le tableau dont on veut trouver le maximum subarray. Les nombres doivent être séparé par un espace.
**Attention, il faut que la taille du tableau soit une puissance de 2**.
Example le fichier `test` contenant : 1 -7 9 18

```sh
$cat test
1 -7 9 18
```


### Générer des test

Pour générer des tests et leur résultats :

```sh
$cd evaluate
$python3 generateTest.py [n] [t]
```
Les fichiers générés seront dans le dossier `./evaluate/data`

| option | description |
| ------ | ------ |
| n | Nombre de fichier de test à génerer (10 par défaut) |
| t | Taille des tableaux. Doit être une puissance de 2 (32 par défaut) |
| -h | Affiche sur la sortie standard l'utilisation du programme |
| clean | Supprime tous les tests genérés par le programme |

### Evaluer un ensemble de test

Dans un premier temps, il faut générer des test : 

```sh
~evaluate/$python3 generateTest.py 20 128
```
Les tests et leur résultat sont dans `~/evaluate/data`
Les résultats ont été calculés avec l'algorhitme de Kadane

On peut évaluer tous les tests avec :
```sh
~evaluate/$python3 evaluate.py
max_subarray;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True;True
```
#### Output de l'évaluation

| Entrées | description |
| ------ | ------ |
| 1 | Le nom du programme, max_subarray. |
| 2 | La compilation : `True` = compilation sans erreur, `Error` = erreur de compilation |
| 3 | Premier cas de test (du dossier data) : `True` = Bon résultat, `False` = mauvais résultat, 'Error' = crash du programme ou temps de réponse supérieur à 5s |
| ... | ...|
| ... | ...|
| ... | ...|
| n | Résultat du n-3 ème cas de test|


Pour le fonctionnement des algorithmes voir la documentation des codes source.