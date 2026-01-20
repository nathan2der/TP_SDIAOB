# TP_SDIAOB

Version du 18 janvier

Ajout de kernels-opt1.c (et changement makefile) 

    _pour suppression de l'appel à pow -> +10 (40) fps en enlevant de la fonction collision

    _ et ajout de static inline : pas de gros gain de performance
    
    _ajout de scale vectors : baisse de fps.



Ameliorations possibles : ajout de check_position, enlever l'appel à sqrt


version 20 janvier

Ajout de kernels-opt2.c et de checkposition


    _suppresion de scale vectors
    
    _je sors les calucls constants de la boucle i (dans compute acceleration) et je calcule qu'une seule difference de vecteurs de position car le module d'un vecteur (i vers j) egale au module d'un vecteur (j,i), ils ont la meme distance euclidienne. => petit gain de 15 fps ( environ 57 fps).

    _j'evite l'appel à sqrt dans compute acceleration, gain de 5fps (63fps).
