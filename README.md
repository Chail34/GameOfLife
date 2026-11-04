# GameOfLife

Ma version du jeu de la vie (2D et 3D) avec OpenGL.

## Compilation et lancement

Dépendances : `g++`, freeglut, OpenGL/GLU et GSL.

```bash
make          # compile le programme
./main        # lance le jeu
make clean    # supprime l'exécutable et les fichiers .o
```

## Commandes

Au lancement, le jeu est en mode 2D et le temps est en pause.

### Simulation

| Touche | Action |
|--------|--------|
| `p` | Lancer / mettre en pause le temps |
| `n` | Passer à la génération suivante (pas à pas) |
| `s` | Générer une soupe aléatoire de cellules |
| `+` / `-` | Augmenter / diminuer la taille de la soupe (50 par défaut) |
| `r` | Tout effacer (reset) |
| `/` | Retirer une partie des cellules (environ une sur deux) |
| `0` | Afficher le nombre de cellules vivantes dans le terminal |

### Affichage

| Touche | Action |
|--------|--------|
| `2` | Afficher / masquer le jeu 2D |
| `3` | Afficher / masquer le jeu 3D |
| `*` | Afficher / masquer le repère (axes X, Y, Z) |
| `f` | Plein écran / fenêtré |

### Ajouter des cellules à la main

| Touche | Action |
|--------|--------|
| `d` | Afficher / masquer le curseur (cube rouge) |
| Flèches | Déplacer le curseur en X et Y |
| `v` / `b` | Déplacer le curseur en Z (avant / arrière, utile en 3D) |
| `Espace` | Créer une cellule à la position du curseur |

### Caméra (souris)

| Bouton | Action |
|--------|--------|
| Clic gauche + glisser | Tourner |
| Clic droit + glisser | Déplacer |
| Clic molette + glisser | Zoomer |

> Les actions `n`, `s`, `/`, `0` et `Espace` s'appliquent au(x) jeu(x) actuellement affiché(s).
