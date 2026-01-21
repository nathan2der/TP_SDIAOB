# TP_SDIAOB

Version du 18 janvier

Ajout de kernels-opt1.c (et changement makefile) 

    _ suppression de l'appel à pow -> +10 (40) fps en enlevant de la fonction collision

    _ et ajout de static inline : pas de gros gain de performance
    
    _ajout de scale vectors : baisse de fps.



Ameliorations possibles : ajout de check_position, enlever l'appel à sqrt


version 20 janvier

Ajout de kernels-opt2.c et de checkposition 


    _suppresion de scale vectors (pas utile,au contraire on veut limiter les appels de fonctions)
    
    _je sors les calucls constants de la boucle i (dans compute acceleration) et je calcule qu'une seule difference de vecteurs de position car le module d'un vecteur (i vers j) egale au module d'un vecteur (j,i), ils ont la meme distance euclidienne. => petit gain de 15 fps ( environ 57 fps).

    _j'evite l'appel à sqrt dans compute acceleration, gain de 5fps (63fps).


version 21 janvier 

    _enlever tous les appels aux fonctions. On les remplace directement par le code. gain significatif de 50 fps environ.
    
    _enlever les sqrt (si possible) et les appels à mod. 
    
    _optimisation de lookup table ne marche pas avec ce code car il garde en cache les calcules repetes sauf qu'on pas des calcules de ce genre; resultats : un code plus long parce que le calcule de la vitesse n'est pas previsible et sert a riens de le garde dans le cache:

    - Distances imprévisibles : Chaque paire de particules génère une distance unique et aléatoire, empêchant tout pattern d'accès mémoire prévisible
    - Cache miss élevés : Les accès aléatoires dans la table causent des défauts de cache constants
    - Coût supérieur : L'interpolation + accès mémoire prend plus de cycles que l'instruction SIMD native 'sqrtsd'
    Étapes :

        1-Crée une lookup table : Pré-calcule 10,000 valeurs de sqrt et les stocke en mémoire
        2-Initialise 500 particules : Positions et masses aléatoires
        3-Cherche sqrt dans la table pré-calculée    

    _resolve_collision a rajouter une grande portion de complexite dans le calcule a cause de faite que le seuil pour faire le calcule de collision etait trop grand (DIM WINDOW * DIM WONDOW). Donc j'ai reduit ce seuil a 100 correspondant a COLLISION_THRESHOLD = 10.0 * COLLISION_THRESHOLD Resultat : 70 fps et un affiche plus fluide.

    _Ajout de l'optimisation OpenMP :
        Pas de dépendance entre itérations (chaque corps indépendant) => parallélisation facile
        Utilisation de `schedule(static)` pour une répartition statique des itérations entre threads
        Gestion des sections non parallélisables (initialisation avec `rand()` et résolution des collisions) avec des solutions adaptées
        Chaque thread écrit dans `accelerations[i]` différent = pas de race condition
        #pragma omp critical` => perte de performance mais nécessaire pour éviter les race conditions lors de la résolution des collisions
        Au final, on se rend compte que l'usage d'OpenMP améliore significativement les performances, mais les fps ne sont pas aussi fluides que souhaité , on obtient des grosses chutes (on passe de 3200 fps à environ  60 fps).De plus OpenMP n'est pas attendu pour ce type d'exercice, on va maintenant essayer de faire du code assembleur.

    _Ajout des intrinsèques SIMD (AVX et SSE) pour vectoriser les calculs :
    
    Au lieu de calculer particule par particule, on traite plusieurs particules en même temps grâce aux registres vectoriels du processeur.
    
    compute_accelerations : On utilise AVX (registres ymm de 256 bits)
        - On traite maintenant 4 particules j à la fois au lieu d'une seule
        - Ça veut dire qu'on fait 4 calculs de distance, 4 divisions, 4 multiplications en parallèle dans une seule instruction
        - On utilise aussi FMA (Fused Multiply-Add) qui fait multiplication + addition en un seul cycle CPU
        - À la fin, on fait une "réduction horizontale" pour additionner les 4 accumulateurs
    
    compute_velocities et compute_positions : On utilise SSE (registres xmm de 128 bits)
        - On traite 2 particules à la fois (2 vecteurs = 4 doubles)
        - Les calculs v += a et p += v + 0.5*a sont faits en parallèle sur 2 particules

    Pourquoi ça marche bien ici ?
        - Les calculs sont identiques pour chaque particule (SIMD = Single Instruction Multiple Data)
        - Pas de dépendances entre les particules qu'on traite ensemble
        - Les données sont bien rangées en mémoire (contigus)
    
    Flags de compilation ajoutés : `-mavx2 -mfma`
        - `-mavx2` : Active les instructions AVX2 (256 bits)
        - `-mfma` : Active les instructions FMA pour multiplication-addition fusionnée
    
    Benchmark SIMD vs Sans SIMD :
    
    Pour mesurer l'impact réel des optimisations SIMD, on a fait un benchmark comparatif :
    
    - Sans SIMD (code scalaire classique) : ~827 fps en moyenne
    - Avec SIMD (AVX2 + FMA) : ~2050 fps en moyenne
    - Speed-up : 2.48x (presque 2.5 fois plus rapide )
    
    Le gain est significatif : on passe de 827 fps à 2050 fps, ce qui donne un facteur d'accélération de 2.5x.
    C'est proche du gain théorique attendu (x2-x4) car on traite 4 particules en parallèle avec AVX.
    
    Le script `benchmark_simd.sh` compile les deux versions et compare automatiquement les performances sur 100 frames.
    
    Résultat : Le programme tourne maintenant entre 2000 et 3000 fps de manière plus stable. Les optimisations SIMD donnent de très bonnes performances !!!!!

