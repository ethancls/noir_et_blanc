#include "lodepng.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* COMMANDES DE COMPILATION :

    gcc noir_et_blanc.c lodepng.c -o noir_et_blanc
    ./noir_et_blanc entree.png sortie.png

*/

/* CONSTANTES */

/* STRUCTURES DE DONNÉES */

typedef struct myimage_t
{
    unsigned hauteur;
    unsigned largeur;
    unsigned char *rouge;
    unsigned char *vert;
    unsigned char *bleu;
    unsigned char *alpha;

} myimage;

/* DÉCLARATIONS DES FONCTIONS */

// Initialiser la structure
void init_myimage(myimage *image);

// Lire une image et la stocker dans une structure myimage
myimage lireImage(const char *filename);

// Écrire l'image à partir des composantes rouge, vert, bleu et alpha
void ecrireImage(myimage img, const char *filename);

// Conversion Noir et Blanc
void NoirEtBlanc(myimage *img);

/* DÉFINITIONS DES FONCTIONS */

void init_myimage(myimage *image)
{
    image->hauteur = 0;
    image->largeur = 0;
    image->rouge = NULL;
    image->vert = NULL;
    image->bleu = NULL;
    image->alpha = NULL;
}

myimage lireImage(const char *filename)
{
    myimage img;
    init_myimage(&img);
    unsigned erreur;
    unsigned char *temp;

    erreur = lodepng_decode32_file(&temp, &img.largeur, &img.hauteur, filename);
    if (erreur)
        printf("Erreur %u : %s\n", erreur, lodepng_error_text(erreur));

    printf("Données image : %u x %u pixels | %u MP", img.largeur, img.hauteur, (img.largeur * img.hauteur) / 1000000);

    img.rouge = (unsigned char *)malloc(img.largeur * img.hauteur);
    img.vert = (unsigned char *)malloc(img.largeur * img.hauteur);
    img.bleu = (unsigned char *)malloc(img.largeur * img.hauteur);
    img.alpha = (unsigned char *)malloc(img.largeur * img.hauteur);

    for (unsigned int i = 0; i < img.largeur * img.hauteur; i++)
    {
        img.rouge[i] = temp[4 * i];
        img.vert[i] = temp[4 * i + 1];
        img.bleu[i] = temp[4 * i + 2];
        img.alpha[i] = temp[4 * i + 3];
    }

    free(temp);
    return img;
}

void ecrireImage(myimage img, const char *filename)
{
    unsigned erreur;
    unsigned char *temp = (unsigned char *)malloc((4 * img.largeur * img.hauteur));

    int j = 0;
    for (unsigned int i = 0; i < 4 * img.largeur * img.hauteur; i += 4)
    {
        temp[i] = img.rouge[j];
        temp[i + 1] = img.vert[j];
        temp[i + 2] = img.bleu[j];
        temp[i + 3] = img.alpha[j];
        // printf("Pixel n°%d : R %u V %u B %u a %u\n", j, temp[i], temp[i + 1], temp[i + 2], temp[i + 3]);
        j++;
    }

    erreur = lodepng_encode32_file(filename, temp, img.largeur, img.hauteur);
    if (erreur)
        printf("Erreur %u : %s\n", erreur, lodepng_error_text(erreur));

    free(temp);
}

void NoirEtBlanc(myimage *img)
{
    for (unsigned int i = 0; i < img->largeur * img->hauteur; i++)
    {
        unsigned char moyenne = (unsigned char)((img->rouge[i] / 3) + (img->vert[i] / 3) + (img->bleu[i] / 3));
        img->rouge[i] = moyenne;
        img->vert[i] = moyenne;
        img->bleu[i] = moyenne;
    }
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Paramètres incorrects, réessayez !\n");
        return EXIT_FAILURE;
    }
    const char *in = argv[1];
    const char *out = argv[2];
    if (in == NULL)
    {
        printf("Erreur d'ouverture de fichier !\n");
        return EXIT_FAILURE;
    }
    myimage img;
    init_myimage(&img);
    img = lireImage(in);
    NoirEtBlanc(&img);
    ecrireImage(img, out);
    free(img.rouge);
    free(img.vert);
    free(img.bleu);
    free(img.alpha);
    return EXIT_SUCCESS;
}
