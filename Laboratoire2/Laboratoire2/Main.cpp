#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
using namespace std;

#define CHAR unsigned char
#define bitMap 251
#define FAT 252
#define debutListeFichier 0
#define blockSize 64
#define nomFichierSize 63
#define BLOCKFAULT 255
#define nomDur "hd.dh"

// -------------------------------------------------------------
//
// DÉFINITION SOUS FORME D'OBJET D'UN DISQUE DUR
//
// -------------------------------------------------------------

class DisqueDur {
private:
	// Un seul fichier est écrit par le programme en exécution (HD.DH).
	fstream hd;
public:
	DisqueDur(const char* fichier) {
		hd.open(fichier, fstream::in | fstream::out | fstream::binary);

		if (!hd.is_open()) {

			ofstream outfile(fichier);
			outfile.close();
			hd.open(fichier, fstream::in | fstream::out | fstream::binary);

			if (!hd.is_open())
				throw((string)"Erreur lors de l'ouverture du fichier.");
		}
	}

	~DisqueDur() {
		hd.close();
	}

	void readBlock(CHAR numBlock, CHAR* tampLecture); // Lire un bloc
	void writeBlock(CHAR numBlock, CHAR* tampLecture); // Écrire un bloc
};

// -------------------------------------------------------------
//
// LISTE DES FONCTIONNALITÉS DE L'OS
//
// -------------------------------------------------------------

void UpdateFiles();
void UpdateScreen();
void write(const char* nomFichier, int position, int nbChar, CHAR* TampLecture);
void deleteEOF(const char* nomFichier, int position);
void read(const char* nomFichier, int position, int nbChar, CHAR* TampLecture);
bool GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock);
int FileLength(CHAR* nomFichier);
const char* CreateRandomFileName(CHAR* alphabet);
int WriteRandomStuff(int nbCharMax, CHAR laLettre, CHAR* & buffer);
const char* FindRandomFileName();
int CountNbFiles();
void AddToString(string & result, CHAR* input, int lenght);
CHAR FindFichier(CHAR* nomFichier);
void DeleteFichier(CHAR* nomFichier);
void ClearBlockLinksFrom(CHAR currentBlock, bool firstBlockIntact = true);
CHAR GetBlockLibre();
bool IsBlockLibre(CHAR numBlock);
void WriteFAT(CHAR numBlock, CHAR value);
CHAR ReadFAT(CHAR numBlock);
CHAR ReadCellFromBlock(CHAR numBlock, CHAR numCell);
void SetBitMap(CHAR numBlock, bool state);
bool Compare(CHAR* a, CHAR* b, int size);
bool GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock);
CHAR CreateFile(CHAR* nomFichier);
CHAR NewLinkedBlock(CHAR from);
CHAR NewBlock();
CHAR NewBlock(CHAR* content);
void FillBlockWith(CHAR block, CHAR character);
CHAR GetCharacter(CHAR block, CHAR character);
CHAR* Convert(const char* text);
bool FileExists(const char*  name);
void FormatHDD();
const char* Convert(CHAR* text, int lenght);

// DISQUE DUR
DisqueDur* dur; 

// -------------------------------------------------------------
//
// FONCTION PRINCIPALE EFFECTUANT DIVERSES OPERATIONS ALÉATOIRES
//
// -------------------------------------------------------------

int main() {

	//Initialise la seed du random
	srand((unsigned int)time(NULL));

	//Check si le hd.dh existait au paravant
	bool mustFormat = !FileExists(nomDur);

	// Creation du Disque Dur (et de son fichier)
	try {
		dur = new DisqueDur(nomDur);
	}
	catch (string erreur) {
		std::cout << erreur << endl;
	}

	//Setup le HDD si necessaire
	if (mustFormat) FormatHDD();

	// Introduction au programme
	cout << "-------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << "\nBienvenue dans le programme du systeme d'exploitation PATOS faissant la gestion du disque de dur." << endl;
	cout << "\nProjet realise le 22 novembre 2016 par Alexandre Blanchet et Frederic Bessette." << endl;
	cout << "\nVoici quelques elements important dont vous devrez faire la connaissance avant votre premiere utilisation:" << endl;
	cout << "\n* A chaque mise a jour du programme, celui-ci affichera l'etat complet du système a moins qu'il n'y ait une erreur" << endl;
	cout << "\n* Le programme attendra une entree de l'utilisateur avant de poursuivre vers sa prochaine mise a jour" << endl;
	cout << "\n* Il suffit d'entree Q pour quitter le programme entre les mises a jour" << endl;
	cout << "\n* La mise a jour peut effectue l'une des 4 actions suivantes: Creation ou suppression d'un fichier et ecriture ou suppression de donnees" << endl;
	cout << "\n* Une action est aleatoirement choisit parmis ceux disponible, la creation et l'ecriture ayant une plus grande priorite" << endl;
	cout << "\n-------------------------------------------------------------------------------------------------------------------------------------------";

	// Boucle principale du programme
	while (true) {
		try
		{
			// Mise a jour du système
			UpdateFiles();
			UpdateScreen();
		}
		// Gestion des erreurs
		catch (string error) { 
			cout << error << endl;
		}
		catch (const std::exception&)
		{
			cout << "Erreur inconnue." << endl;
		}

		// Arrêt du programme jusqu'à la prochaine entrée
		char* temp = new char[30];
		cin >> temp;
		if (temp[0] == 'q')
		{
			delete temp;
			break;
		}
		delete temp;
	}

	delete dur;
	return 0;
}

// -------------------------------------------------------------
//
// FONCTIONS QUI MET A JOUR LE DISQUE DUR ET L'ÉCRAN
//
// -------------------------------------------------------------

// Mise a jour du système de fichier dans le disque dur
void UpdateFiles() {
	int maxLenght = 100;
	CHAR alphabet[] = "abcdefghijklmnopqrstuvwxyz";
	CHAR* buffer = new CHAR[maxLenght];

	int action = rand() % 100;

	cout << "\n\n" << endl;

	// Ecriture d'un nouveau fichier, poids: 30
	if (action < 30) {
		const char* nomFichier = CreateRandomFileName(alphabet);
		int longueur = WriteRandomStuff(maxLenght, nomFichier[0], buffer);
		cout << "Creation du fichier " << nomFichier << " (de longueur " + to_string(longueur) << ")." << endl;
		write(nomFichier, 0, longueur, buffer);
		delete[] nomFichier;
	}

	// Ecriture dans un fichier existant, poids: 40
	else if (action < 70) {
		const char* nomFichier = FindRandomFileName();
		int totalSize = FileLength(Convert(nomFichier));
		int position = (rand() % totalSize*0.2) + totalSize * 0.8f;
		int size = WriteRandomStuff(maxLenght, nomFichier[0], buffer);
		cout << "Ecriture dans " << nomFichier << " (a partir du " + to_string(position + 1) << "e octet, sur un longeur de " << to_string(size) << ")." << endl;
		write(nomFichier, position, size, buffer);
		delete[] nomFichier;
	}

	// Supprimer un fichier, poids: 15
	else if (action < 85)
	{
		const char* nomFichier = FindRandomFileName();
		cout << "Suppression totale de " << nomFichier << endl;
		deleteEOF(nomFichier, 0);
	}

	// Effacement de la fin d'un fichier, poids: 15
	else
	{
		const char* nomFichier = FindRandomFileName();
		int pos = rand() % FileLength(Convert(nomFichier));
		cout << "Suppresion partiel de " << nomFichier << " (du " + to_string(pos + 1) << "e octets jusqu'a la fin)." << endl;
		deleteEOF(nomFichier, pos);
		delete[] nomFichier;
	}

	cout << "\n";

	delete[] buffer;
}

// Mise a jour de l'affichage de l'état du disque dur
void UpdateScreen() {
	CHAR* dossierBlock = new CHAR[blockSize];
	CHAR* buffer = new CHAR[blockSize];
	CHAR* blockTeteFichier = new CHAR[blockSize];
	CHAR* listInfo = new CHAR[256];
	CHAR currentBlock = 0;
	CHAR lenghtLastBlock;
	CHAR nextFileBlock;
	string outputListFile = "";
	bool keepgoing = true;

	//Fill list info avec 0
	for (int i = 0; i < 256; i++) {
		if (i >= bitMap)
			listInfo[i] = 33;
		else
			listInfo[i] = 95;
	}

	//Parcourt Fichier
	while (keepgoing) {
		dur->readBlock(currentBlock, dossierBlock);
		listInfo[currentBlock] = 35;
		for (int i = 0; i < blockSize; i++) {

			// Fin de la liste des fichiers
			if (dossierBlock[i] == 255) {
				keepgoing = false;
				break;
			}

			//Debut du balayage d'un fichier
			else {
				//Load la tete de fichier
				CHAR teteFichier = dossierBlock[i];
				dur->readBlock(teteFichier, blockTeteFichier);
				listInfo[teteFichier] = toupper(blockTeteFichier[0]);;

				//Ajoute le nom du fichier à la liste en output
				CHAR length = GetCharacter(teteFichier, 0);
				if (length == BLOCKFAULT)length = blockSize - 1;
				outputListFile += "\n";
				AddToString(outputListFile, blockTeteFichier, blockSize - 1);
				outputListFile += "[" + to_string(teteFichier) + "]";

				//Prepare la boucle qui suit
				lenghtLastBlock = blockTeteFichier[blockSize - 1];
				nextFileBlock = ReadFAT(teteFichier);
				bool endOfFile = false;

				// Fin du fichier deja rencontrer
				if (nextFileBlock == BLOCKFAULT)
					endOfFile = true;

				// Bondit, block à block, on s'amuse !
				while (!endOfFile) {
					//Ajoute le text
					outputListFile += " -> " + to_string((int)nextFileBlock);  // vide.txt | toto.txt -> 21 -> 40|

					CHAR* content = new CHAR[blockSize];
					dur->readBlock(nextFileBlock, content);

					CHAR pastFileBlock = nextFileBlock;
					nextFileBlock = ReadFAT(nextFileBlock);

					if (nextFileBlock == BLOCKFAULT) {
						if (lenghtLastBlock != 0) {
							listInfo[pastFileBlock] = tolower(content[0]);
						}
						else {
							listInfo[pastFileBlock] = toupper(content[0]);
						}
						endOfFile = true; // Fin du fichier
					}
					else {
						listInfo[pastFileBlock] = toupper(content[0]);
					}
				}
			}
		}
		currentBlock = ReadFAT(currentBlock);
		if (currentBlock == BLOCKFAULT) break;
	}
	std::cout << "Legende : \n# = dossier racine (une liste de tete de fichier)\n! = espace reserve (bitmap et FAT)";

	std::cout << "\n\nListe des fichiers : ";

	std::cout << outputListFile << endl;

	std::cout << "\nDimension du tableau: 16x16";
	for (int i = 0; i < 256; i++) {
		if (i % 16 == 0)
			std::cout << endl;
		std::cout << "|" << listInfo[i] << "|";
	}

	delete[] dossierBlock;
	delete[] buffer;
	delete[] blockTeteFichier;
	delete[] listInfo;
	return;
}

// -------------------------------------------------------------
//
// FONCTIONS ÉCRIVANT DU TEXTE ALÉATOIREMENT
//
// -------------------------------------------------------------

// Écrit du texte aléatoirement
int WriteRandomStuff(int nbCharMax, CHAR laLettre, CHAR* & buffer) {
	int textLenght = rand() % nbCharMax;
	if (textLenght < 5) textLenght = 5;

	for (int i = 0; i < textLenght; i++) {
		buffer[i] = laLettre;
	}
	return textLenght;
}

// Crée un nom de fichier aléatoirement et vérifie s'il existe deja
const char* CreateRandomFileName(CHAR* alphabet) {
	//Toujours _.txt
	CHAR* nomFichier = new CHAR[nomFichierSize];

	//Remplie de 0
	for (int i = 0; i < nomFichierSize; i++)
		nomFichier[i] = 0;

	//Met le .txt
	nomFichier[1] = '.';
	nomFichier[2] = 't';
	nomFichier[3] = 'x';
	nomFichier[4] = 't';

	//Trouve une lettre non-utilisé
	for (int i = 0; i < 26; i++) {
		nomFichier[0] = alphabet[i];
		if (FindFichier(nomFichier) == BLOCKFAULT) break;
	}
	return Convert(nomFichier, 5);
	//A noter que si toute les lettre de l'alphabet son prise, plusieur fichier auront le nom z.txt
}

// -------------------------------------------------------------
//
// FONCTIONS DE BASE DE L'OS POUR LIRE, ÉCRIRE ET SUPPRIMER
//
// -------------------------------------------------------------

// Ouvre un fichier (s'il existe) et lit (selon les paramètres) les données pour les mettre dans TampLecture puis le referme
void read(const char* nomFichier, int position, int nbChar, CHAR* TampLecture) {
	CHAR* newNomFichier = Convert(nomFichier);
	int currentChar = 0;
	CHAR* buffer = new CHAR[blockSize];

	CHAR currentBlock;
	CHAR startCell;
	GetPosIntoFile(newNomFichier, position, startCell, currentBlock);

	while (true)
	{
		dur->readBlock(currentBlock, buffer);

		for (int i = startCell; i < blockSize; i++) {
			TampLecture[currentChar] = buffer[i];
			currentChar++;
			if (currentChar >= nbChar) {
				delete[] buffer;
				delete[] newNomFichier;
				return; //Fini !
			}
		}

		startCell = 0;
		currentBlock = ReadFAT(currentBlock);
		if (currentBlock == BLOCKFAULT) throw((string)"Fichier est trop court.");
	}
	delete[] buffer;
	delete[] newNomFichier;
}

// Ouvre un fichier (s'il existe) et écrit (selon les paramètres) les données dans TampLecture puis le referme
void write(const char* nomFichier, int position, int nbChar, CHAR* TampLecture) {
	CHAR* newNomFichier = Convert(nomFichier);

	CHAR teteFichier = FindFichier(newNomFichier);

	// Creer la tete du fichier
	if (teteFichier == BLOCKFAULT) {
		teteFichier = CreateFile(newNomFichier);
	}

	//Ajuste la grosseur présente dans le 64e octet
	int oldTotalSize = FileLength(newNomFichier);
	int newTotalSize = position + nbChar;

	if (newTotalSize > oldTotalSize)
	{
		CHAR* teteContent = new CHAR[blockSize];
		dur->readBlock(teteFichier, teteContent);
		teteContent[nomFichierSize] = newTotalSize % blockSize;
		dur->writeBlock(teteFichier, teteContent);
	}

	int currentChar = 0;
	CHAR* buffer = new CHAR[blockSize];
	CHAR currentBlock;
	CHAR startCell;

	//Reserve l'espace necessaire, tant que GetPosIntoFile retourne 'false'
	while (!GetPosIntoFile(newNomFichier, position + nbChar - 1, startCell, currentBlock))
		NewLinkedBlock(currentBlock);

	//Marque le dernier block comme etant la fin (dans la FAT)
	WriteFAT(currentBlock, BLOCKFAULT);

	//Replace les compteur aux bonne place
	GetPosIntoFile(newNomFichier, position, startCell, currentBlock);

	//Ecriture !
	while (true)
	{
		//Lit ce qui etait present
		dur->readBlock(currentBlock, buffer);

		//Remplie le text avec les nouvelles valeur
		for (int i = startCell; i < blockSize; i++) {
			buffer[i] = TampLecture[currentChar];
			currentChar++;
			if (currentChar >= nbChar) //Fini ! 
				break;
		}

		//Écrase l'ancien block avec la nouvelle version
		dur->writeBlock(currentBlock, buffer);

		//Passe au prochain block
		startCell = 0;
		currentBlock = ReadFAT(currentBlock);

		//Erreur ?
		if (currentBlock == BLOCKFAULT) break;
	}
	delete[] buffer;
	delete[] newNomFichier;
}

// Ouvre un fichier (s'il existe) et efface (selon les paramètres) les données du fichier puis le referme
void deleteEOF(const char* nomFichier, int position) {
	CHAR* newNomFichier = Convert(nomFichier);
	CHAR* buffer = new CHAR[blockSize];

	CHAR currentBlock;
	CHAR startCell;
	int previousPos = position - 1;

	if (position <= 0) {
		DeleteFichier(newNomFichier);
	}
	else {
		//Ajuste le dernier bit de la tete de fichier (indiquant la grosseur du dernier block)
		int newLastBlockSize = position % blockSize;
		CHAR teteFichier = FindFichier(newNomFichier);

		dur->readBlock(teteFichier, buffer);
		buffer[blockSize - 1] = newLastBlockSize;
		dur->writeBlock(teteFichier, buffer);


		//Efface les données.	 ex: pos=2   ->  [5,12,DEL,DEL,DEL...]
		GetPosIntoFile(newNomFichier, previousPos, startCell, currentBlock);

		dur->readBlock(currentBlock, buffer);
		ClearBlockLinksFrom(currentBlock);
		for (int i = startCell+1; i < blockSize; i++) {
			buffer[i] = 0;
		}
		dur->writeBlock(currentBlock, buffer);
	}

	delete[] newNomFichier;
}

// -------------------------------------------------------------
//
// FONCTIONS D'INTERACTIONS AVEC LES BLOCS
//
// -------------------------------------------------------------

// Lire un block selon son numéro et l'insère dans TampLecture
void DisqueDur::readBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekg(pos);
	hd.read((char*)tampLecture, blockSize);
}

// Écrit TampLecture un block selon son numéro
void DisqueDur::writeBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekp(pos);
	hd.write((char*)tampLecture, blockSize);
}

// Retourne un numéro de block libre
CHAR GetBlockLibre()
{
	CHAR* map = (CHAR*)malloc(blockSize);
	dur->readBlock(bitMap, map);

	int cell = 0;
	//Trouve la cell avec au moins un bit a 0
	for (cell = 0; cell < 32; cell++) {
		if (map[cell] != 255) break;
	}

	if (cell == 32)
	{
		delete[] map;
		throw((string)"Disque dur plein !");
	}

	CHAR result = 255;
	for (int bit = 0; bit < 8; bit++) {
		if ((map[cell] >> bit & 0x01) == 0) { //00011001
			result = (cell * 8) + bit;
			break;
		}
	}

	delete[] map;

	if (result >= bitMap)
	{
		throw((string)"Disque dur plein !");
	}
	return result;
}

// Vérification de la disponibilité d'un bloc
bool IsBlockLibre(CHAR numBlock)
{
	CHAR cell = numBlock / 8;
	CHAR bit = numBlock % 8;

	CHAR result = ReadCellFromBlock(bitMap, cell);

	return ((result >> bit) & 0x01) == 0;
}

// Crée un nouveau bloc lié à son précédent 
CHAR NewLinkedBlock(CHAR from)
{
	CHAR nouveauBlock = GetBlockLibre();
	if (nouveauBlock == BLOCKFAULT) throw((string)"Echec de creation de nouveau block, espace insuffisant sur le disque.");
	WriteFAT(from, nouveauBlock); //Cree le lien dans la FAT
	SetBitMap(nouveauBlock, true); //Marque le nouveau block comme etant utilisee

	return nouveauBlock;
}

// Crée un nouveau bloc vide
CHAR NewBlock()
{
	CHAR nouveauBlock = GetBlockLibre();
	if (nouveauBlock == BLOCKFAULT) throw((string)"Echec de creation de nouveau block, espace insuffisant sur le disque.");
	SetBitMap(nouveauBlock, true); //Marque le nouveau block comme etant utilisee

	return nouveauBlock;
}

// Crée un nouveau bloc et y insère du contenu
CHAR NewBlock(CHAR* content)
{
	CHAR nouveauBlock = GetBlockLibre();
	if (nouveauBlock == BLOCKFAULT) throw((string)"Echec de creation de nouveau block, espace insuffisant sur le disque.");
	SetBitMap(nouveauBlock, true); //Marque le nouveau block comme etant utilisee

	dur->writeBlock(nouveauBlock, content);

	return nouveauBlock;
}

// Remplir un bloc
void FillBlockWith(CHAR block, CHAR character)
{
	CHAR* content = new CHAR[blockSize];

	for (int i = 0; i < blockSize; i++) {
		content[i] = character;
	}
	dur->writeBlock(block, content);
	delete content;
}

// Lire à partir d'une position dans le bloc
CHAR ReadCellFromBlock(CHAR numBlock, CHAR numCell)
{
	CHAR* row = new CHAR[blockSize];

	dur->readBlock(numBlock, row);

	CHAR result = row[numCell];

	delete[] row;
	return result;
}

// Libérer les liens du bloc et de ses suivants
void ClearBlockLinksFrom(CHAR currentBlock, bool firstBlockIntact) {

	while (true)
	{
		CHAR pastBlock = currentBlock;
		currentBlock = ReadFAT(currentBlock);
		WriteFAT(pastBlock, BLOCKFAULT);

		if (!firstBlockIntact)
			SetBitMap(pastBlock, false);

		if (currentBlock == BLOCKFAULT) break;

		if (firstBlockIntact)
			SetBitMap(currentBlock, false);
	}
}

// -------------------------------------------------------------
//
// FONCTIONS EFFECTUANT DES OPERATIONS DANS LA SECTION RÉSERVÉE
//
// -------------------------------------------------------------

// Écrire dans la FAT
void WriteFAT(CHAR numBlock, CHAR value)
{
	CHAR block = (numBlock / blockSize) + FAT;
	CHAR cell = numBlock % blockSize;

	CHAR* row = new CHAR[blockSize];
	dur->readBlock(block, row);

	row[cell] = value;

	dur->writeBlock(block, row);

	delete[] row;
}

// Lire la FAT
CHAR ReadFAT(CHAR numBlock)
{
	CHAR block = (numBlock / blockSize) + FAT;
	CHAR index = numBlock % blockSize;
	CHAR result = ReadCellFromBlock(block, index);
	return result;
}

// Changer l'état d'un bloc dans la bitmap
void SetBitMap(CHAR numBlock, bool state)
{
	if (numBlock >= bitMap)
	{
		throw((string)"Erreur, ne peut pas set un bit dans la bitMap > 250");
		return;
	}
	CHAR* map = (CHAR*)malloc(blockSize);
	dur->readBlock(bitMap, map);

	CHAR cell = numBlock / 8;
	CHAR bit = numBlock % 8;

	if (state)
		map[cell] |= 1 << bit; //Met le bit a 1
	else
		map[cell] &= ~(1 << bit); //Met le bit a 0

	dur->writeBlock(bitMap, map);
	delete[] map;
}

// Formatage de la bitmap
void FormatHDD() {
	//Fill bitmap de 0
	FillBlockWith(bitMap, 0);
	//FAT
	for (int pos = FAT; pos <= BLOCKFAULT; pos++)
		FillBlockWith(pos, BLOCKFAULT);
	//Dossier racine
	FillBlockWith(0, BLOCKFAULT);
	//Bitmap
	SetBitMap(0, true);
}

// -------------------------------------------------------------
//
// FONCTIONS D'INTERACTIONS AVEC LES FICHIERS
//
// -------------------------------------------------------------

// Compte le nombre de fichier dans le système
int CountNbFiles() {
	bool keepgoing = true;
	int nbFiles = 0;
	CHAR* block = new CHAR[blockSize];

	CHAR nextBlock = 0;

	while (keepgoing) {
		keepgoing = false;

		dur->readBlock(nextBlock, block);

		//Compte tous les fichier dans le block en cours
		for (int i = 0; i < blockSize; i++) {
			if (block[i] == BLOCKFAULT) break; // Si on arrive a un BLOCKFAULT (255), alors on stop
			nbFiles++;
		}

		// Si le prochain block n'est pas = BLOCKFAULT, alors il faut continuer
		nextBlock = ReadFAT(nextBlock);
		keepgoing = nextBlock != BLOCKFAULT;
	}

	delete[] block;
	return nbFiles;
}

// Supprime un fichier
void DeleteFichier(CHAR* nomFichier) {
	CHAR firstFileBloc = FindFichier(nomFichier);
	if (firstFileBloc == BLOCKFAULT) throw ((string) "Fichier inexistant");
	CHAR* buffer = new CHAR[blockSize];
	bool keepgoing = false;
	bool getout = false;

	// On rend libre les blocs du fichier
	ClearBlockLinksFrom(firstFileBloc, false);

	CHAR currentBlock = debutListeFichier;
	CHAR pastBlock = debutListeFichier;
	// On retire le fichier du bloc racine contenant la liste des fichiers
	while (!getout)
	{
		dur->readBlock(currentBlock, buffer);
		for (int i = 0; i < blockSize; i++) {
			//Si on a terminé (on est arrivé aux 255)
			if (buffer[i] == BLOCKFAULT) {
				getout = true;
				break;
			}
			//Si on a dépassé le fichier à enlever (et qu'on doit tasser le reste)
			if (keepgoing) {
				//Si le i+1 doit acceder au prochain block
				if (i + 1 == blockSize) {
					CHAR nextBlock = ReadFAT(currentBlock);
					if (nextBlock != BLOCKFAULT) {
						CHAR* tempBuffer = new CHAR[64];
						dur->readBlock(nextBlock, tempBuffer);
						buffer[i] = tempBuffer[0];
						delete[] tempBuffer;
					}
				}
				else
				{
					buffer[i] = buffer[i + 1];
					//si on vient tout juste de vidé le dernier byte du block, on supprime le block
					if (i == 0 && buffer[i] == BLOCKFAULT && pastBlock != debutListeFichier)
						ClearBlockLinksFrom(pastBlock);
				}
			}
			//Quand on arrive à la position du fichier
			else if (buffer[i] == firstFileBloc) {
				// on a trouve la cellule du fichier!
				keepgoing = true;
				buffer[i] = buffer[i + 1];
				if (i == 0 && buffer[i] == BLOCKFAULT && pastBlock != debutListeFichier)
					ClearBlockLinksFrom(pastBlock);
			}
		}
		dur->writeBlock(currentBlock, buffer);
		pastBlock = currentBlock;
		currentBlock = ReadFAT(currentBlock);
		if (currentBlock == BLOCKFAULT) break;
	}
	delete[] buffer;
	return;
}

// Trouve un fichier
CHAR FindFichier(CHAR* nomFichier) {
	CHAR* block = new CHAR[blockSize];
	CHAR* blockFichier = new CHAR[blockSize];
	bool keepgoing = true;

	CHAR nextBlock = debutListeFichier;

	while (keepgoing) {
		dur->readBlock(nextBlock, block);

		//Check tous les fichier du block courent
		for (int i = 0; i < blockSize; i++) {
			if (block[i] == BLOCKFAULT) {
				delete[] block;
				delete[] blockFichier;
				return BLOCKFAULT;
			}
			dur->readBlock(block[i], blockFichier);
			if (Compare(blockFichier, nomFichier, nomFichierSize)) {
				CHAR result = block[i];
				delete[] block;
				delete[] blockFichier;
				return result;
			}
		}

		//Pas dans ce block ? next one!
		nextBlock = ReadFAT(nextBlock);

		if (nextBlock == BLOCKFAULT) break;
	}

	delete[] block;
	delete[] blockFichier;
	return BLOCKFAULT;
}

// Trouve une position dans un fichier
bool GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock) {
	CHAR currentBlock = FindFichier(nomFichier);

	if (currentBlock == BLOCKFAULT) throw((string)"Fichier inexistant");
	outBlock = currentBlock;
	outPos = blockSize - 1;
	currentBlock = ReadFAT(currentBlock);
	if (currentBlock == BLOCKFAULT) return false;

	CHAR* buffer = new CHAR[blockSize];

	for (int j = 0; true; j++) {

		//Set les donnees
		outBlock = currentBlock;

		for (int i = 0; i < blockSize; i++) {
			int currentTotalPos = j * blockSize + i;

			//Set les donnees
			outPos = i;

			if (currentTotalPos >= position) {
				delete[] buffer;
				return true;
			}
		}
		currentBlock = ReadFAT(currentBlock);
		if (currentBlock == BLOCKFAULT) {
			delete[] buffer;
			return false;
		}
	}
	delete[] buffer;
	return false;
}

// Crée un nouveau fichier
CHAR CreateFile(CHAR* nomFichier)
{
	//Vérifie si le fichier existe déjà
	if (FindFichier(nomFichier) != BLOCKFAULT) throw((string)"Erreur de création de fichier, nom déjà existant.");

	//Cree le fichier
	nomFichier[nomFichierSize] = 0; //Dernier octet -> grosseur du dernier block
	CHAR teteFichier = NewBlock();
	FillBlockWith(teteFichier, 0);
	dur->writeBlock(teteFichier, nomFichier);

	// -------Fait le lien dans le dossier racine------- //

	// Trouve ou faire le lien
	int index = CountNbFiles() % blockSize;
	CHAR nbBlock = CountNbFiles() / blockSize; //Arondit deja au plus bas

	CHAR* buffer = new CHAR[blockSize];
	CHAR currentBlock = debutListeFichier;

	//Va au dernier block du dosser Racine
	for (int i = 0; i < nbBlock; i++) {
		CHAR pastBlock = currentBlock;
		currentBlock = ReadFAT(currentBlock);

		if (currentBlock == BLOCKFAULT)
			currentBlock = NewLinkedBlock(pastBlock);
	}
	//Ecrit le lien dans le dossier Racine
	dur->readBlock(currentBlock, buffer);
	buffer[index] = teteFichier;
	dur->writeBlock(currentBlock, buffer);

	delete[] buffer;
	return teteFichier;
}

// Trouve un nom de fichier aléatoire
const char* FindRandomFileName() {
	//Trouve un bloc aléatoire du dossier racine
	int nbCells = CountNbFiles();
	if (nbCells == 0) throw((string) "Aucun fichier existant.");
	int nbBlocks = (nbCells / 64) + 1;
	CHAR block = rand() % nbBlocks;

	//Trouve la quantité de cellule dans le block choisi
	int nbDeCellsDansBlock = blockSize;
	if (block == nbBlocks - 1) // Si le block est le dernier...
		nbDeCellsDansBlock = nbCells % blockSize;

	//Trouve une cellule aléatoire dans le block choisi
	CHAR cell = rand() % nbDeCellsDansBlock;

	//Lit le premier block du fichier, aka son nom
	CHAR* nomFichier = new CHAR[blockSize];
	dur->readBlock(ReadCellFromBlock(block, cell), nomFichier);

	return Convert(nomFichier, blockSize);
}

// Vérifie si un fichier existe
bool FileExists(const char*  name) {
	ifstream f(name);
	return f.good();
}

// Détermine la longueur d'un fichier
int FileLength(CHAR* nomFichier) { //Exclue la tete de fichier
	
	CHAR filePos = FindFichier(nomFichier);

	if (filePos == BLOCKFAULT) throw((string)"Impossible de trouver la grosseur d'un fichier car il est inexistant.");

	//Grosser du dernier block ex: [64,64,64,21]  -> 21
	CHAR grosseurDernierBlock = ReadCellFromBlock(filePos, blockSize - 1);

	int quantitéDeBlockPlein = 0;

	//Trouve la quantité de block plein (EXCLUANT LE PREMIER)  ex: [64,64,64,21]  -> 3
	CHAR current = filePos;
	while (true)
	{
		current = ReadFAT(current);
		if (current == BLOCKFAULT) break;
		quantitéDeBlockPlein++;
	}
	if (grosseurDernierBlock != 0) quantitéDeBlockPlein--;

	// Retourne la quantité totale ex: (3 * 64) + 21
	return quantitéDeBlockPlein * blockSize + grosseurDernierBlock;
}

// -------------------------------------------------------------
//
// AUTRES FONCTIONS D'UTILITÉES DIVERSES
//
// -------------------------------------------------------------

// Compare deux blocs
bool Compare(CHAR* a, CHAR* b, int size) {
	for (int i = 0; i < size; i++) {
		if (a[i] != b[i]) return false;
	}
	return true;
}

// Cherche la position de la première apparition d'un charactère
CHAR GetCharacter(CHAR block, CHAR character) {
	CHAR* content = new CHAR[blockSize];
	dur->readBlock(block, content);
	for (CHAR i = 0; i < blockSize; i++) {
		if (content[i] == character) return i;
	}
	return BLOCKFAULT;
}

// Convertir une chaine de charactère en un bloc
CHAR* Convert(const char* text)
{
	CHAR* content = new CHAR[blockSize];
	size_t i = 0;
	//Remplie le content du text: "bonjour" -> "bonjour"
	for (i = 0; i < strlen(text); i++)
	{
		content[i] = text[i];
	}

	//Remplie le content de \0: "bonjour" -> "bonjour\0\0\0..."
	for (; i < blockSize; i++)
	{
		content[i] = 0;
	}
	return content;
}

// Convertir un bloc en chaine de charactère
const char* Convert(CHAR* text, int lenght)
{
	char* content = new char[blockSize];
	size_t i = 0;
	//Remplie le content du text: "bonjour" -> "bonjour"
	for (i = 0; i < lenght; i++)
	{
		content[i] = text[i];
	}

	//Remplie le content de \0: "bonjour" -> "bonjour\0\0\0..."
	for (; i < blockSize; i++)
	{
		content[i] = 0;
	}
	return content;
}

// Ajoute un bloc à un string
void AddToString(string & result, CHAR* input, int lenght) {
	for (int i = 0; i < lenght && input[i] != 0; i++) {
		result += input[i];
	}
	return;
}