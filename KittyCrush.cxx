/**
 *
 * @file   KittyCrush.cxx
 *
 * @author Romain ROUX, Hugo ROS, David SAIGNE, Florian THIBAULT, Jérémy WASNER
 *
 * @date   12/01/2015
 *
**/

#include <iostream>
#include <fstream> // Lecture, écriture de fichiers
#include <sstream> // Utilisation des stringstream : commande de mouvement
#include <string>
#include <vector>
#include <iomanip> // setw ()
#include <cstdlib> // Fonction rand () : génère un nombre aléatoire
#include <ctime> // Nécessaire pour rand ()
#include <cctype> // Fonction tolower ()
#include <unistd.h> // Fonction sleep ()
#ifdef _WIN32
#include <windows.h>
#endif
#include <limits> // cin.ignore ()
#include <math.h>
#include <array>

using namespace std;


namespace KittyCrush
{
    typedef vector <unsigned> CVLine; 				// un type représentant une ligne de la grille
    typedef vector <CVLine> CMat; 					// un type représentant la grille
    typedef pair <unsigned, unsigned> CPosition; 	// une position dans la grille
    typedef vector <string> CVStr ;                 // Un tableau de string pour le tableau de clef


		/* Constantes */
    const unsigned KImpossible (0); // valeur pour laquelle une case est considérée comme vide
    const unsigned RealMax (1023) ; // Les deux lignes suivantes : pour les fonctions de conversion
    const unsigned NbMax (RealMax / 2 + 1) ;
    const unsigned NbZero (ceil ( log2 (RealMax) ) ) ;
    const unsigned NbCandies (10) ;
    const char Separator ('O') ; // Séparateur dans la sauvegarde



    void ClearScreen () // Linux
    {
		#ifdef _WIN32
			system ("cls");
		#else
			cout << "\033[H\033[2J";
		#endif

    } // ClearScreen ()

    const string KReset   ("0");
    const string KNoir    ("30");
    const string KRouge   ("31");
    const string KVert    ("32");
    const string KJaune   ("33");
    const string KBleu    ("34");
    const string KMagenta ("35");
    const string KCyan    ("36");
    const string KNoirBG  ("40");
	const string KJauneBG ("46");

    void Couleur (const string & Couleur)
    {
        cout << "\033[" << Couleur << "m";

    } // Couleur ()



	/* Vide le buffer clavier */
	void ClearBuf ()
	{
		cin.ignore (numeric_limits<streamsize>::max (), '\n');

	} // ClearBuf ()



	/* 	UTILISATION : Affiche le message désigné par Invite puis l'utilisateur entrera quelque chose qui sera stocké dans Saisie

		Si ce que l'utilisateur tape n'est pas conforme au type de Saisie (ex : il tape abc alors que Saisie est un entier) alors
		on affiche Erreur puis on réaffiche Invite et on redemande une Saisie jusqu'à ce qu'elle soit correcte
		--> Inutile pour la saisie en une ligne <-- */

    template <typename Type>
    void SaisieCin (Type & Saisie, string Erreur, string Invite = "")
    {
        cout << Invite;
		cin.clear ();
        cin >> Saisie;
        while (! cin.eof () && cin.fail ())
        {
			cin.clear ();
            ClearBuf (); // Vide le buffer
            cout << Erreur << endl;
            cout << Invite;
            cin >> Saisie;
        }

		/* Vide ce qui reste dans le buffer avant de quitter la fonction */
		ClearBuf ();

    } // SaisieCin ()

	void MenuPrompt (unsigned & Choix, unsigned NbChoixMax, const string & Erreur = "Choix invalide", const string & Message = "Votre choix : ")
	{
		/* L'utilisateur choisit parmi les différentes options du menu */
		SaisieCin (Choix, Erreur, Message);
		while (! (Choix >= 1 && Choix <= NbChoixMax))
		{
			cin.clear ();
			cout << "Choix incorrect, entrez un nombre entre 1 et " << NbChoixMax << endl;
			SaisieCin (Choix, Erreur, Message);
		}

	} // MenuPrompt ()



	bool AllFilesAreValid () // Vérifie si tous les fichiers nécessaires au jeu sont présents
	{
		//ifstream Save
		ifstream Title ("TitleScreen.txt") ;
		ifstream Credit ("Credits.txt") ;
		return (Title && Credit);

	} // AllFilesAreValid ()



	void DisplayTitleScreen () // Affiche l'écran titre
	{
		ClearScreen ();
		ifstream Title ("TitleScreen.txt");

		if (Title) // TitleScreen.txt existe et est accessible
		{
		    string Line;
		    while (getline (Title, Line))
		        cout << Line << endl;
		}
		else
		{
		    cout << '\t' << "Kitty Crush by" << endl
				 << "Hugo ROS" << endl << "David SAIGNE" << endl << "Florian THIBAULT" << endl << "Romain ROUX" << endl << "Jeremy WASNER" << endl;
		}

	} // DisplayTitleScreen ()

	void DisplayMainMenu (const string & ErrorMsg) // Affiche le menu
	{
		ClearScreen ();
		cout << '\t' << "Bienvenue dans Number Crush" << endl << endl
		     << "1 : Nouvelle Partie" << endl
			 << "2 : Charger Partie" << endl
			 << "3 : Options" << endl
		     << "4 : Credits" << endl
		     << "5 : Quitter" << endl << endl;

	    if (ErrorMsg.size () != 0)
	        cout << ErrorMsg << endl;

		if (! AllFilesAreValid ())
		    cerr << endl << "ATTENTION : Il vous manque certains fichiers ! Vérifiez que vous avez bien : TitleScreen.txt, Credits.txt" << endl;

	} // DisplayMainMenu ()

	void DisplayCredits () // Affiche les crédits
	{
		ClearScreen ();
		ifstream Credit ("Credits.txt");

		if (Credit)
		{
		    string Line;
		    while (getline (Credit, Line))
		        cout << Line << endl;
		}
		else
		    cerr << "Erreur : Fichier Credits.txt manquant" << endl;

		cout << endl << "Tapez Entrée pour revenir au menu...";
		string B;
		getline (cin, B);

	} // DisplayCredits ()



	void DisplayChoixSaisie (array <unsigned, 2> & Choices)
	{
		unsigned Choix;

		ClearScreen ();

		cout << "Choix du mode de saisie" << endl << endl
		 << "1 : Saisie détaillée : la ligne, colonne et direction sont demandées séparément" << endl
		 << "2 : Saisie sur une ligne (ex : 2 3 z)" << endl << endl;

		MenuPrompt (Choix, 2);

		Choices [0] = Choix - 1;

	} // DisplayChoixSaisie ()

	void DisplayLineColumnFirst (array <unsigned, 2> & Choices)
	{
		unsigned Choix;

		ClearScreen ();

		cout << "Saisie de la ligne ou de la colonne en premier lors d'un déplacement" << endl << endl
		 << "1 : Ligne en premier" << endl
		 << "2 : Colonne en premier" << endl << endl;

		MenuPrompt (Choix, 2);

		Choices [1] = Choix - 1;

	} // DisplayChoixSaisie ()



		/* Touches par défaut */
	char KeyUp ('z');
	char KeyDown ('s');
	char KeyLeft ('a');
	char KeyRight ('e');

	void ChangeMvtKeysMenu ()
	{
		ClearScreen ();

		cout << "Touches actuellement utilisées :" << endl
			 << "  " << KeyUp << endl
			 << KeyLeft << " " << KeyDown << " " << KeyRight << endl << endl;

		bool KeysAreValid;
		do {
			SaisieCin (KeyUp, "Saisie invalide", "Saisir la touche haut : ");
			SaisieCin (KeyLeft, "Saisie invalide", "Saisir la touche gauche : ");
			SaisieCin (KeyDown, "Saisie invalide", "Saisir la touche bas : ");
			SaisieCin (KeyRight, "Saisie invalide", "Saisir la touche droite : ");

			cout << endl;
			KeysAreValid = ! (KeyUp == KeyDown || KeyUp == KeyLeft || KeyUp == KeyRight || KeyDown == KeyLeft || KeyDown == KeyRight || KeyLeft == KeyRight);
			if (! KeysAreValid)
				cout << "Erreur ! Deux directions sont assignées à la même touche, veuillez recommencer..." << endl;

		} while (! KeysAreValid);

	} // ChangeMvtKeysMenu ()

	void DisplayMovementKeys ()
	{
		cout << KeyUp << " : haut, " << KeyDown << " : bas, " << KeyLeft << " : gauche, " << KeyRight << " : droite" << endl;

	} // DisplayMovementKeys ()


	void DisplayOptions (array <unsigned, 2> & Choices)
	{
		const unsigned ValeurChoixMax (4);
		unsigned Choix;

		do {
			ClearScreen ();

			cout << '\t' << "Options :" << endl << endl
			 << "1 : Changer touches de déplacement" << endl
			 << "2 : Choisir mode de saisie (détaillé ou en une seule ligne)" << endl
			 << "3 : Choix entre saisir ligne puis colonne ou colonne puis ligne lors d'une commande" << endl
			 << "4 : Retour au menu principal" << endl << endl;

			MenuPrompt (Choix, ValeurChoixMax);

			switch (Choix)
			{
				case 1:
					ChangeMvtKeysMenu ();
					break;
				case 2:
					DisplayChoixSaisie (Choices);
					break;
				case 3:
					DisplayLineColumnFirst (Choices);
					break ;
			}

		} while (Choix != ValeurChoixMax); // Si Choix vaut la dernière option, on quitte

	} // DisplayOptions ()



    void DisplayGrid (const CMat & Grid, const string & Message = "")
    {
        ClearScreen ();

		/* 	Affiche les numéros des colonnes avec fond noir et police cyan au dessus de la grille
			(on commence à 1 au lieu de 0 pour que ce soit plus intuitif pour le joueur) */

        cout << "    ";
        #ifdef __linux__
            Couleur (KJauneBG);
            Couleur (KNoir);
        #endif
        for (unsigned i (0); i < Grid [0].size (); ++i)
            cout << setw (2) << i + 1;
        cout << " ";
        #ifdef __linux__
            Couleur (KReset);
        #endif
        cout << endl << endl;


        for (unsigned i (0); i < Grid.size (); ++i) // Parcourt les lignes de la grille
        {
			/* Affiche le numéro de la ligne toujours en fond noir */
			#ifdef __linux__
                Couleur (KJauneBG);
                Couleur (KNoir);
            #endif
            cout << setw (2) << i + 1 << " ";
            #ifdef __linux__
                Couleur (KReset);
			#endif
			cout << " ";

            for (unsigned j (0); j < Grid [i].size (); ++j)	// Parcourt tous les éléments d'une ligne
            {
                cout << setw (2);
                (Grid [i][j] != 0) ? cout << Grid [i][j] : cout << " "; // Affiche l'élément
            }
            cout << endl;
        }
		cout << endl;

		if (Message.size () != 0)
			cout << Message << endl << endl;

    } // DisplayGrid ()



	bool IsValidDirection (char Direction)
	{
		return (Direction == KeyUp || Direction == KeyDown || Direction == KeyLeft || Direction == KeyRight);

	} // IsValidDirection ()

	/*  Retourne vrai si la case désignée par Pos est comprise dans la grille et si sa valeur n'est pas 0 */
	bool IsValidPosition (const CMat & Grid, const CPosition & Pos)
	{
		return (Pos.first >= 0 && Pos.first < Grid.size () && Pos.second >= 0 && Pos.second < Grid [0].size ()
				&& Grid [Pos.first][Pos.second] != 0);

	} // IsValidPosition ()

	bool IsValidMove (const CMat & Grid, const CPosition & Pos, char Direction)
	{
		CPosition PosTest;

		if (Direction == KeyUp)
			PosTest = make_pair (Pos.first - 1, Pos.second);

		else if (Direction == KeyDown)
			PosTest = make_pair (Pos.first + 1, Pos.second);

		else if (Direction == KeyLeft)
			PosTest = make_pair (Pos.first, Pos.second - 1);

		else if (Direction == KeyRight)
			PosTest = make_pair (Pos.first, Pos.second + 1);


		return IsValidPosition (Grid, PosTest);

	} // IsValidMove ()



	/* S'assurer que le mouvement est possible avec IsValidMove avant d'executer cette fonction */
    void MakeAMove (CMat & Grid, const CPosition & Pos, char Direction)
    {
		if (Direction == KeyUp)
			swap (Grid [Pos.first][Pos.second], Grid [Pos.first - 1][Pos.second]);

		else if (Direction == KeyDown)
			swap (Grid [Pos.first][Pos.second], Grid [Pos.first + 1][Pos.second]);

		else if (Direction == KeyLeft)
			swap (Grid [Pos.first][Pos.second], Grid [Pos.first][Pos.second - 1]);

		else if (Direction == KeyRight)
			swap (Grid [Pos.first][Pos.second], Grid [Pos.first][Pos.second + 1]);

    } // MakeAMove ()



    bool AtLeastThreeInAColumn (const CMat & Grid, CPosition & Pos, unsigned & HowMany)
    {
		/*	On se place sur la première case de la colonne et on stocke sa valeur dans NbPrec

			Ensuite on va sur la case en dessous. Si elle est identique à la précédente, on ajoute +1 à HowMany
			et on laisse NbPrec où il est. On passe à la case suivante.

			Sinon si elle est différente, 2 options :
				- HowMany vaut moins de 3, pas de suite détectée, on met la nouvelle case dans NbPrec et on reset HowMany

				- HowMany vaut 3 ou plus, c'est qu'il y a eu 3 ou plus cases identiques précédemment. La valeur de la première case étant dans NbPrec
				  puisqu'il n'a pas été changé.
				  Il suffit donc de stocker la position NbPrec dans Pos et d'arrêter la fonction.

			Si aucune suite n'est trouvée dans une colonne, on passe à la suivante et ainsi de suite. */


        unsigned NbPrec;
        for (unsigned j (0); j < Grid [0].size (); ++j) // Parcourt toutes les colonnes
        {
            NbPrec = Grid [0][j]; 	// Première case dans NbPrec
			Pos = make_pair (0, j);	// Pour l'instant Pos vaut la position de la première case
            HowMany = 1;

            for (unsigned i (1); i < Grid.size (); ++i)
            {
                if (Grid [i][j] == NbPrec && Grid [i][j] != 0) // Il ne faut pas détecter les suites de 0 (cases vides) !
                    ++HowMany;

                else
                {
                    if (HowMany >= 3) return true;
                    HowMany = 1;
                    Pos = make_pair (i, j);
                    NbPrec = Grid [i][j];
                }
            }
            if (HowMany >= 3) return true;
        }
        return false;

    } // AtLeastThreeInAColumn ()

    bool AtLeastThreeInARow (const CMat & Grid, CPosition & Pos, unsigned & HowMany)
    {
		/*	Même procédé que pour les colonnes.. */

        unsigned NbPrec;
        for (unsigned i (0); i < Grid.size (); ++i)
        {
            NbPrec = Grid [i][0];
            HowMany = 1;
            Pos = make_pair (i, 0);
            for (unsigned j (1); j < Grid [i].size (); ++j)
            {
                if (Grid [i][j] == NbPrec && Grid [i][j] != 0)
                    ++HowMany;
                else
                {
                    if (HowMany >= 3) return true;
                    HowMany = 1;
                    Pos = make_pair (i, j);
                    NbPrec = Grid [i][j];
                }
            }
            if (HowMany >= 3) return true;
        }
        return false;

    } // AtLeastThreeInARow ()



	/* Pos désigne la position du début de la suite et HowMany la longueur de cette suite */
    void RemovalInColumn (CMat & Grid, const CPosition & Pos, unsigned HowMany)
    {
		/* On remplace les cases de la suite par des cases vides (cases qui ont pour valeur KimPossible) */
        for (unsigned i (Pos.first); i < Pos.first + HowMany; ++i)
        {
            Grid [i][Pos.second] = KImpossible;
        }

		/* 	On parcourt le reste de la colonne et on décale les cases restantes de HowMany indices
			(ex : Si c'est une suite de 4 on décale ce qu'il y a au dessus de 4 cases vers le bas) */
        for (unsigned i (0); i < Pos.first; ++i)
        {
            Grid [i + HowMany][Pos.second] = Grid [i][Pos.second];
            Grid [i][Pos.second] = KImpossible;
        }

    } // RemovalInColumn ()

    void RemovalInRow (CMat & Grid, const CPosition & Pos, unsigned HowMany)
    {
        for (unsigned j (Pos.second); j < Pos.second + HowMany; ++j)
        {
            Grid [Pos.first][j] = KImpossible;
        }

		/* Cette fois ci on décale toutes les cases au dessus de la ligne supprimée d'un indice vers le bas */
        for (unsigned j (Pos.second); j < Pos.second + HowMany; ++j)
        {
            for (unsigned i (Pos.first); i > 0; --i)
                Grid [i][j] = Grid [i - 1][j];

			Grid [0][j] = KImpossible; // On complète la ligne du haut par des cases vides
        }

    } // RemovalInRow ()



	void InitGrid (CMat & Grid, unsigned Size, unsigned NbCandies)
    {
        srand (time (NULL)); 	// Initialise la génération aléatoire (en fonction du temps système)
		Grid = CMat (Size);		// Dimensionne la grille pour qu'elle contienne "Size" lignes

		CPosition Pos;
		unsigned HowMany; // osef, c'est juste pour utiliser la fonction de test

		/* Génère la grille jusqu'à ce qu'il n'y ait plus de suites dès le début */
		do {
			for (CMat::iterator GridIter (Grid.begin ()); GridIter < Grid.end (); ++GridIter)
			{
				*GridIter = CVLine (Size); // Dimensionne les lignes (même procédé que pour la grille)

				/* Chaque case prend une valeur aléatoire entre 1 et NbCandies */
				for (CVLine::iterator LineIter (GridIter -> begin ()); LineIter < GridIter -> end (); ++LineIter)
					*LineIter = (rand () % NbCandies) + 1;
			}

		} while (AtLeastThreeInAColumn (Grid, Pos, HowMany) || AtLeastThreeInARow (Grid, Pos, HowMany));

    } // InitGrid ()

	/*	Détecte et supprime les suites de nombres identiques */
    void ArrangeGrid (CMat & Grid, unsigned & Score)
    {
        CPosition Pos;
        unsigned HowMany;
		unsigned PointsEarned (0);
		unsigned ComboMeter (0);

		while (AtLeastThreeInARow (Grid, Pos, HowMany))
		{
			RemovalInRow (Grid, Pos, HowMany);
			++ComboMeter;
			for (unsigned i (1); i <= HowMany; ++i)
				PointsEarned += i;
		}

		while (AtLeastThreeInAColumn (Grid, Pos, HowMany))
		{
			RemovalInColumn (Grid, Pos, HowMany);
			++ComboMeter;
			for (unsigned i (1); i <= HowMany; ++i)
				PointsEarned += i;
		}

		Score += PointsEarned * ComboMeter;

	} // ArrangeGrid ()



	bool IsBinary(string Str)
	/* On teste si une string est binaire, si oui renvoie true */
    {
        for (unsigned i (0) ; i < Str.size() ; ++ i)
        {
           if ( ! (Str [i] == '0' || Str [i] == '1') )
               return false ;
        }
        return true ;

    } // IsBinary ()

	string DecToBin (unsigned Nbr)
	/* Conversion du décimal vers binaire, on rentre un nombre entier
       positif, et on le convertis en une string de NbZero caractères */
    {
            string Binary ( NbZero, '0') ; // Le nombre de zeros, initialisé à 0
            unsigned Pos (0), Local (Nbr) ;
            if (Nbr > RealMax) return "Size_Error" ; // On ne peut pas dépasser le max
            for (unsigned i (NbMax) ; i >= 1 ; i /= 2 )
            {
                if ( Local >= i )
                {
                    Binary[Pos] = '1' ;
                    Local -= i ;
                }
                Pos += 1 ;
            }
            return Binary ;

    } // DecToBin ()

    unsigned BinToDec (string Str) // Pour le décryptage
    /* On prend une string binaire et on la transforme en un unsigned */
    {
        unsigned NbDec (0), Squared (1) ; // Squared (au carré en anglais) va être multiplié pour faire 1,2,4,8,16
        if (IsBinary (Str))
        {
            for (unsigned i (Str.size() ) ; i-- > 0;)
            {
                if (Str[i] == '1')
                    NbDec += Squared ;
                if (i != Str.size() )
                    Squared *=2 ;
            }
            return NbDec ;
        }
        else
            return 0 ; // Renvoie 0 si la string qu'on demande n'est pas binaire.

    } // BinToDec ()

    bool ExistingSave () // Le dossier de sauvegarde existe-t-il ?
    {
        ifstream SaveFile ("./save.txt") ;
        return SaveFile.good ();

    } // ExistingSave ()



	const CVStr KeyTab {"0001101101","0101100010", "1000010101", "0000000110" }; // Le tableau des clés qui serviront a coder les données
    string CodeXor (string Bin, string Key) // Un ou exclusif sur les string
    {
        if ( (Bin.size() == Key.size()) && IsBinary(Key) )
        {
            string Code (Key.size(), '0') ;
            for (unsigned i(0) ; i < Code.size() ; ++i)
            {
                if ( ( ( Bin[i] == '0' ) && ( Key[i] == '1' ) ) || ( ( Bin[i] == '1' ) && ( Key[i] == '0' ) ) )
                Code[i] = '1' ;
            }
            return Code ;
        }
        else
            return "Error_XOR" ;

    } // CodeXor ()

    string Crypt (unsigned Data, unsigned Key, const CVStr & KeyTab )
    /* Un compilé des fonctions précédentes pour crypter de mannière simple
       Data la donnée a crypter, Key la clé actuelle qui va changer à chaque tour
       et KeyTab le tableau de clés déterminé préalablement */
    {
        if (Key > KeyTab.size() - 1)
            return "Error:Bad_Key" ;
        string Code ;
        string BinData ( DecToBin(Data) ) ;
        string KeyBin ;
        if ( BinData == "Size_Error")
            return "Size_Error" ;
        Code = CodeXor(BinData, KeyTab [Key]) ;
        return Code ;

    } // Crypt ()

    unsigned Decrypt (string StrData, unsigned Key, const CVStr & KeyTab) // Décrypte une string binaire en unsigned, le processus inverse du cryptage.
    {
        if (IsBinary(StrData) )
        {
            string CodeCrypt (CodeXor (StrData, KeyTab[Key]) ) ;
            unsigned CodeDecrypt (BinToDec (CodeCrypt) ) ;
            return CodeDecrypt ;
        }
        else return 1024 ; // Valeur qu'on utilisera jamais, forcément supérieure à NbCandies, elle renverra forcément erreur dans le programme de chargement

    } // Decrypt ()



    void Save (const CMat & Grid, unsigned Turn, unsigned Score, unsigned BestScore, unsigned Size, const CVStr & KeyTab, unsigned MaxTimes )
    {
        /* La sauvegarde, elle crée un fichier save.txt contenant toutes les données nécéssaires au jeu, et les crypte */
        ofstream SaveFile ("./save.txt", ios::trunc) ;
        unsigned Key ((Turn % KeyTab.size() + 1) - 1 ) ;
        SaveFile << DecToBin(Key) << Separator ; // CRYPTAGE Key (1)
        string CryptScore = Crypt(Score, Key, KeyTab) ;
        string CryptBestScore = Crypt(BestScore, Key, KeyTab) ;
        SaveFile << CryptScore << Separator << CryptBestScore << Separator ; // CRYPTAGE Score (2) BestScore(3)
        string CryptSize = Crypt(Size, Key, KeyTab) ; // CRYPTAGE Taille (4)
        SaveFile << CryptSize << Separator ;
		string CryptTurn = Crypt(Turn, Key, KeyTab) ; // CRYPTAGE Tour (5)
		SaveFile << CryptTurn << Separator ;
		string CryptMaxTimes = Crypt (MaxTimes, Key, KeyTab) ; // CRYPTAGE Niveau (6)
		SaveFile << CryptMaxTimes << Separator ;
        for (unsigned i (0); i < Grid.size (); ++i)
        {
            for (unsigned j (0); j < Grid [i].size (); ++j)
            {
                SaveFile << Crypt(Grid[i][j], Key, KeyTab ) << Separator ;
            }
        }

    } // Save ()



    bool LoadSave (CMat & Grid,const CVStr & KeyTab, unsigned & Score, unsigned & BestScore, unsigned & Turn, unsigned & Size, unsigned & MaxTimes)
    /* Le chargement de sauvegarde, il agit comme un InitGrid, sauf qu'il prend les données de la sauvegarde existante et s'en sert */
    {
        ifstream SaveFile ("./save.txt") ;
        if (SaveFile)
        {
            unsigned PreviousPos(0), WhereSeparator, WhatSubStr (1) ;
            unsigned Key ; // La clé de cryptage (1ere ligne du code)
            unsigned DecryptCurData ; // La sous-str décryptée
            unsigned PosX (0), PosY (0) ; // La position dans la grille
            string CryptedSave ;
            string CurrentData ; // La sous-str sur laquelle on travaille dans la boucle
            getline(SaveFile,CryptedSave) ; // La sauvegarde complete, illisible  genre (0001010O0010)
            for (unsigned Pos (0) ; Pos < CryptedSave.size() ; ++Pos)
            {
                if (CryptedSave [Pos] == Separator)
                {
                    WhereSeparator = CryptedSave.find(Separator, Pos) - PreviousPos ; // Le séparateur est déplacé à la case ou il a trouvé - La case du spéarateur précédent
                    CurrentData = CryptedSave.substr(Pos - NbZero, NbZero) ; // Le segment de sauvegarde sur lequel nous allons travailler
                    PreviousPos = WhereSeparator + 1 ; // Pour éviter un problème sur la fonction find
                    if ( ( CurrentData.size() == NbZero ) && (IsBinary(CurrentData) ) ) // On teste si le segment actuel est binaire
                    {
                        if (WhatSubStr == 1)
                        {
                            Key = BinToDec(CurrentData) ;

                        } // La premiere donnée n'est pas cryptée en réalité, c'est juste son code binnaire.
                        else
                        {
                            DecryptCurData = Decrypt (CurrentData, Key, KeyTab) ;
                            //cout << "Decrypte : " << DecryptCurData << endl ;
                            if (WhatSubStr == 2) // DEUXIEME DONNEE : Le score
                                Score = DecryptCurData ;
                            else if (WhatSubStr == 3) // TROISIEME DONNEE : Le meilleur score
                                BestScore = DecryptCurData ;
                            else if (WhatSubStr == 4) // QUATRIEME DONNEE : La taille du tableau
                            {
                                Size = DecryptCurData ;
                                Grid = CMat (Size) ;
                                for (CMat::iterator GridIter (Grid.begin ()); GridIter < Grid.end (); ++GridIter)
                                    *GridIter = CVLine (Size) ;
                            }
                            else if (WhatSubStr == 5 ) // CINQUIEME DONNEE : Le tour
                                Turn = DecryptCurData ;
                            else if (WhatSubStr == 6) // SIXIEME DONNEE : Le niveau
                                MaxTimes = DecryptCurData ;
                            else if (WhatSubStr > 6 ) // SEPTIEME DONNEE : Le tableau
                            {
                                if (DecryptCurData > NbCandies) // Si la donnée est supérieure à la difficulté, renvoie erreur
                                    return true;
                                Grid[PosX][PosY] = DecryptCurData ;
                                PosY += 1 ;
                                if (PosY >= Size)
                                {
                                    PosY = 0 ;
                                    PosX += 1 ;
                                }
                            }
                        }
                        WhatSubStr += 1 ;
                    }
                    else
                        return true ;
                }
            }
        }
        else
        {
            cout << "Sauvegarde Inexistante" << endl ;
            // S'il n'y a pas de sauvegarde
            return true;
        }

        return false;

    } // LoadSave ()


	void SaisieLigneCol (unsigned & Result, const CMat & Grid, const string & Invite)
	{
		SaisieCin (Result, "Erreur de saisie", Invite);
		while (! cin.eof () && (Result < 1 || Result >= Grid.size () + 1)) // Vérifie que le numéro de ligne ou colonne se trouve bien dans la grille
		{
			cout << "Numéro invalide" << endl;
			SaisieCin (Result, "Erreur de saisie", Invite);
		}

	} // SaisieLigneCol ()



	/* Fonction principale contenant le jeu */
	void Game (const array <unsigned, 2> & OptionsChoices, bool LoadExistingSave)
	{
		/* Il est possible de les modifier pour faire des niveaux de difficulté */
		unsigned Size (6);				// Taille de la grille (à la fois ligne et colonnes) !!
		unsigned MaxTimes (6); 		// Nombre maximal de coups autorisés
		unsigned NbCandies (4); 		// Combien de numéros différents sont présents dans la grille
		unsigned BestScore (0);        	// Variable à modifier avec une fonction, variable ARBITRAIRE
		unsigned Score (0);
		unsigned NbMoves (0);

		/* Options choisies */
		unsigned InputMode (OptionsChoices [0]);	// 0 : saisie "à l'ancienne" où on demande les ligne colonne etc une à la fois, 1 : saisie sur 1 ligne

		bool InputLineFirst; 	// true : la commande est du type "ligne colonne direction", false : "colonne ligne direction"
		InputLineFirst = OptionsChoices [1] == 0 ? true : false; // 0 : ligne en premier, 1 : colonne en premier


		CMat Grid;
		if (! LoadExistingSave)
		{
			ClearScreen ();
			cout << '\t' << "Choisir un niveau de difficulté : " << endl << endl
				 << "1 - Facile    : 6x6, 6 coups maximum, 4 numéros différents" << endl
				 << "2 - Moyen     : 8x8, 9 coups maximum, 4 numéros différents" << endl
				 << "3 - Difficile : 6x6, 8 coups maximum, 5 numéros différents" << endl
				 << "4 - Expert    : 8x8, 11 coups maximum, 5 numéros différents" << endl << endl;

			unsigned ChoixLevel;
			MenuPrompt (ChoixLevel, 4);

			switch (ChoixLevel)
			{
				case 1:
					Size = 6;
					MaxTimes = 6;
					NbCandies = 4;
					break;
				case 2:
					Size = 8;
					MaxTimes = 9;
					NbCandies = 4;
					break;
				case 3:
					Size = 6;
					MaxTimes = 8;
					NbCandies = 5;
					break;
				case 4:
					Size = 8;
					MaxTimes = 11;
					NbCandies = 5;
					break;
			}

			InitGrid (Grid, Size, NbCandies);
		}
		else
		{
			if (LoadSave (Grid, KeyTab, Score, BestScore, NbMoves, Size, MaxTimes)) // true si sauvegarde corrompue
			{
			    cout << "Sauvegarde corrompue" << endl
			         << "Lancement d'une nouvelle partie..." << endl;

				#ifdef _WIN32
					Sleep (3000);
				#else
					sleep (3);
				#endif

				Size = 6;
				MaxTimes = 6;
				NbCandies = 4;
			    InitGrid (Grid, Size, NbCandies);
			    Score = 0;
			    BestScore = 0;
			    NbMoves = 0;
		    }
	    }

		ClearScreen ();
		while (NbMoves < MaxTimes)
		{
			DisplayGrid (Grid);
			cout << "Il vous reste " << MaxTimes - NbMoves
				 << (MaxTimes - NbMoves > 1 ? " coups" : " coup") << " à jouer." << endl // Affiche coup au singulier quand il n'en reste qu'un...
				 << "Votre score est de " << Score << endl << endl;

			CPosition CaseSelect;	// Position de la case sélectionnée par le joueur
			char Direction;

			if (InputMode == 0)
			{
				cout << "Saisir les coordonnées de la case à déplacer puis la case adjacente avec laquelle l'échanger" << endl;
				#ifdef _WIN32
					cout << "ou bien saisir CTRL+Z puis Entrée pour abandonner et revenir au menu" << endl;
				#else
					cout << "ou bien saisir CTRL+D pour abandonner et revenir au menu" << endl;
				#endif
				DisplayMovementKeys ();

				do {
					do {

						string Message;
						unsigned NumFirst;
						unsigned NumSecond;

						Message = InputLineFirst ? "Ligne : " : "Colonne : ";
						SaisieLigneCol (NumFirst, Grid, Message);
						if (cin.eof ()) break;

						Message = InputLineFirst ? "Colonne : " : "Ligne : ";
						SaisieLigneCol (NumSecond, Grid, Message);
						if (cin.eof ()) break;

						if (InputLineFirst)
						{
							CaseSelect.first = NumFirst - 1;  // L'utilisateur entre un nombre entre 1 et taille de la grille, il faut donc lui enlever 1
							CaseSelect.second = NumSecond - 1;
						}
						else
						{
							CaseSelect.second = NumFirst - 1;
							CaseSelect.first = NumSecond - 1;
						}

						if (! IsValidPosition (Grid, CaseSelect))
							cout << endl << "Sélectionnez une case non vide" << endl;

					} while (! IsValidPosition (Grid, CaseSelect));
					if (cin.eof ()) break;

					SaisieCin (Direction, "Erreur de saisie", "Direction : ");
					if (cin.eof ()) break;
					while (! IsValidDirection (Direction))
					{
						cout << "Direction invalide" << endl;
						SaisieCin (Direction, "Erreur de saisie", "Direction : ");
					}

					if (! IsValidMove (Grid, CaseSelect, Direction))
						cout << "Mouvement impossible" << endl << endl;

				} while (! IsValidMove (Grid, CaseSelect, Direction));

			} // InputMode == 0

			else
			{
				cout << "Saisir, séparées d'au moins un espace, "
					 << (InputLineFirst ? "la ligne et la colonne" : "la colonne et la ligne")
					 << " de la case sélectionnée et la direction vers laquelle l'échanger" << endl;
				#ifdef _WIN32
					cout << "ou bien saisir CTRL+Z puis Entrée pour abandonner et revenir au menu" << endl;
				#else
					cout << "ou bien saisir CTRL+D pour abandonner et revenir au menu" << endl;
				#endif

				DisplayMovementKeys ();

				if (NbMoves == 0)
					cout << "ex : 2 3 z --> la case en colonne 2 et ligne 3 échange avec celle du haut" << endl; // Mini tuto au début du jeu


				string Input;
				istringstream iss;
				bool InputMovement (true);

				while (InputMovement)
				{
					getline (cin, Input);  // On récupère la ligne saisie dans Input et on la charge dans iss
					if (cin.eof ()) break;
					iss.str (Input);

					InputLineFirst ? iss >> CaseSelect.first >> CaseSelect.second :
									 iss >> CaseSelect.second >> CaseSelect.first;

					--CaseSelect.first; // Les indices commencent à 1 pour l'utilisateur donc il faut enlever 1 au nombre tapé
					--CaseSelect.second;

					iss >> Direction;


					if (iss.fail ()) // Si l'injection d'une des variables n'a pas pu se faire, càd la commande n'est pas valide, iss passe en fail
					{
						cout << "Erreur de saisie" << endl;
						iss.clear ();
					}

					/* Si la commande est valide, on teste les valeurs saisies */
					else if (! IsValidPosition (Grid, CaseSelect))
						cout << "Case sélectionnée invalide" << endl;

					else if (! IsValidDirection (Direction))
						cout << "Direction invalide" << endl;

					else if (! IsValidMove (Grid, CaseSelect, Direction))
						cout << "Mouvement impossible" << endl;


					else  // Si toutes les conditions sont remplies, on met InputMovement à false ce qui fait sortir de la boucle
						InputMovement = false;
				}

			} // InputMode == 1

			if (cin.eof ()) break;
			MakeAMove (Grid, CaseSelect, Direction);  // On a vérifié précédemment que le mouvement était valide
			ArrangeGrid (Grid, Score);
			++NbMoves;
			Save (Grid, NbMoves, Score, BestScore, Size, KeyTab, MaxTimes);
		}

		if (! cin.eof ())
		{
			DisplayGrid (Grid);
			cout << "Jeu terminé ! Votre score final est de " << Score << endl
				 << "Tapez Entrée pour revenir au menu";
			string B;
			getline (cin, B);
		}

		cin.clear ();

	} // Game ()

} // namespace


using namespace KittyCrush;

int main ()
{
	DisplayTitleScreen ();

	/* Met le jeu en pause pour x secondes */
	#ifdef _WIN32
		Sleep (3000);
	#else
		sleep (3);
	#endif

	array <unsigned, 2> OptionsChoices {0, 0}; // Dans l'ordre : mode de saisie, ordre de saisie ligne/colonne
    string ErrorMsg ("");
	const unsigned ValeurChoixMax (5);
	unsigned Choix (0);

	do {
		DisplayMainMenu (ErrorMsg);
		ErrorMsg = "";

		MenuPrompt (Choix, ValeurChoixMax);

		switch (Choix)
		{
			case 1:
				Game (OptionsChoices, false); // Lance le jeu
				break;
			case 2:
			    if (ExistingSave ())
				    Game (OptionsChoices, true);
			    else
			        ErrorMsg = "Pas de sauvegarde disponible";
				break;
			case 3:
				DisplayOptions (OptionsChoices);
				break;
			case 4:
				DisplayCredits ();
				break ;
		}

	} while (Choix != ValeurChoixMax); // Si Choix vaut la dernière option, on quitte

	cout << endl << "A bientôt !" << endl;

    return 0;

} // main ()
