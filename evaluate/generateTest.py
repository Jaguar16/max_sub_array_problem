import sys, os
import random
NB_TEST_CASE = 20

# Function to print contiguous sublist with the largest sum
# in given set of integers
# Techiedelight's code !!
def kadane(A):
    # stores maximum sum sublist found so far
    maxSoFar = 0
    # stores maximum sum of sublist ending at current position
    maxEndingHere = 0
    # stores end-points of maximum sum sublist found so far
    start = end = 0
    # stores starting index of a positive sum sequence
    beg = 0
    # traverse the given list
    for i in range(len(A)):
        # update maximum sum of sublist "ending" at index i
        maxEndingHere = maxEndingHere + A[i]
        # if maximum sum is negative, set it to 0
        if maxEndingHere < 0:
            maxEndingHere = 0        # empty sublist
            beg = i + 1
        # update result if current sublist sum is found to be greater
        if maxSoFar < maxEndingHere:
            maxSoFar = maxEndingHere
            start = beg
            end = i
    f = [maxSoFar] + [el for el in A[start: end + 1]]
    return f

def clean():
    # Delete all test cases in data folder.
    cwd = os.getcwd()
    if "data" not in os.listdir():
        raise NameError(f"le dossier 'data' n'a pas été trouvé. Assurez-vous qu'il soit dans le répertoire courant : {cwd}")
    for filename in os.listdir("data"):
        os.unlink(os.path.join(cwd, "data", filename))
        # Toujours print avant de supprimer au cas où ...
        #print(os.path.join(cwd, "data", filename))

def generateTest(n, borneInf, borneSup, tailleArray):
    d = dict()
    for i in range(n):
        d[i] = random.sample(range(borneInf, borneSup), tailleArray)
        #print(f"Liste de taille : {len(d[i])} \n Liste = {d[i]}")
    return d

def generateResults(dictTest):
    r = dict()
    for k, test in dictTest.items():
        r[k] = kadane(test)
    return r

def writeTestCase(dictTest, test):
    n = len(dictTest)
    for i in range(1, n+1):
        str_result=""
        fd = open(f"./data/{test}{i}", "w+")
        for el in dictTest[i-1]:
            str_result += str(el) + ' '
        str_result = str_result[: -1]
        fd.write(str_result)
        fd.write("\n")
        fd.close()

def isApowerOf2(x):
    return (x != 0) & ((x & (x - 1)) == 0)

if __name__ == '__main__':
    usage = """Utilisation :\n\t$python3 generateTest.py [n] [t]\n\tn = nombre de fichier de test (10 par défaut)
\n\tt = taille des tableaux. Doit être une puissance de 2(32 par défaut)\n Pour supprimer tous les cas de test utilisez l'option clean :\n\t$python3 generateTest.py clean"""
    arg = len(sys.argv)
    n, t = 10, 32

    if sys.argv[1] == "-h":
        print(usage)
        exit(0)
    if sys.argv[1] == "clean":
        print("Suppression de tous les cas de test...")
        clean()
        print("Fait")
        exit(0)
    if arg > 3:
        print("ERREUR : Nombre d'arguments invalide\n")
        exit(-1)
    if arg == 3:
        n, t = int(sys.argv[1]), int(sys.argv[2])
    if arg == 2:
        n = int(sys.argv[1])
    if not isApowerOf2(t):
        print(f"{t} n'est pas une puissance de 2")
        print(usage)
        exit(-1)
    testCases = generateTest(n, -100000, 100000, t)
    results = generateResults(testCases)
    writeTestCase(testCases, "test")
    writeTestCase(results, "result")
    print("Fait")
