// ACSE-1.cpp�: d�finit les fonctions export�es pour l'application DLL.
//

#include "stdafx.h"
#include "ACSE-1.h"


// Il s'agit d'un exemple de variable export�e
ACSE1_API int nACSE1=0;

// Il s'agit d'un exemple de fonction export�e.
ACSE1_API int fnACSE1(void)
{
	return 42;
}

// Il s'agit du constructeur d'une classe qui a �t� export�e.
// consultez ACSE-1.h pour la d�finition de la classe
CACSE1::CACSE1()
{
	return;
}
