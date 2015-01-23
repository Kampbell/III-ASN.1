// Le bloc ifdef suivant est la fa�on standard de cr�er des macros qui facilitent l'exportation 
// � partir d'une DLL. Tous les fichiers contenus dans cette DLL sont compil�s avec le symbole ACSE1_EXPORTS
// d�fini sur la ligne de commande. Ce symbole ne doit pas �tre d�fini pour un projet
// qui utilisent cette DLL. De cette mani�re, les autres projets dont les fichiers sources comprennent ce fichier consid�rent les fonctions 
// ACSE1_API comme �tant import�es � partir d'une DLL, tandis que cette DLL consid�re les symboles
// d�finis avec cette macro comme �tant export�s.
#ifdef ACSE1_EXPORTS
#define ACSE1_API __declspec(dllexport)
#else
#define ACSE1_API __declspec(dllimport)
#endif

// Cette classe est export�e de ACSE-1.dll
class ACSE1_API CACSE1 {
public:
	CACSE1(void);
	// TODO: ajoutez ici vos m�thodes.
};

extern ACSE1_API int nACSE1;

ACSE1_API int fnACSE1(void);
