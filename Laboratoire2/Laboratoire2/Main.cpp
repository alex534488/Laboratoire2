#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <math.h>
using namespace std;

#define CHAR unsigned char
#define bitMap 251
#define FAT 252
#define debutListeFichier 0
#define blockSize 64
#define nomFichierSize 63
#define BLOCKFAULT 255

class DisqueDur {
private:

	// Un seul fichier est écrit par le programme en exécution (HD.DH).
	fstream hd;
public:
	DisqueDur(string fichier) {
		hd.open(fichier, fstream::in | fstream::out | fstream::binary);
		if (!hd.is_open) {
			throw("Erreur lors de l'ouverture de " + fichier);
		}
	}

	~DisqueDur() {

	}

	void readBlock(CHAR numBlock, CHAR* tampLecture);
	void writeBlock(CHAR numBlock, CHAR* tampLecture);
};

void UpdateFiles();
void UpdateScreen();
void read(string nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void write(string nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void deleteEOF(string nomFichier, fpos_t position);
bool GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock);
int FileLength(CHAR* nomFichier);
CHAR* GetRandomFileName(CHAR* alphabet);
int WriteRandomStuff(int nbCharMax, CHAR laLettre, CHAR* & buffer);
CHAR* FindRandomFileName();
int CountNbFiles();
void AddToString(string & result, CHAR* input, int lenght);
CHAR FindFichier(CHAR* nomFichier);
void DeleteFichier(CHAR* nomFichier);
void ClearBlockLinksFrom(CHAR currentBlock);
CHAR GetBlockLibre();
bool IsBlockLibre(CHAR numBlock);
CHAR WriteFAT(CHAR numBlock, CHAR value);
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

DisqueDur* dur;

int main() {
	dur = new DisqueDur("hd.dh");

	//Initialise la seed du random
	srand(time(NULL));

	int timer = 0;
	string buffer;

	while (true) {
		UpdateFiles();
		if (timer >= 50000) {
			UpdateScreen();
			timer = 0;
		}
		timer++;
	}

	return 0;
}

// FONCTIONS MAIN

void UpdateFiles() {
	int maxLenght = 100;
	CHAR alphabet[] = "abcdefghijklmnopqrstvwxyz";
	CHAR* buffer = new CHAR[maxLenght];

	int action = rand() % 4;

	switch (action)
	{
	default:
	case 0:
	{
		// Ecriture d'un nouveau fichier
		CHAR* nomFichier = GetRandomFileName(alphabet);
		write(nomFichier, 0, WriteRandomStuff(maxLenght, nomFichier[0], buffer), buffer);
		delete[] nomFichier;
	}
	break;
	case 1:
		// Supprimer un fichier
		deleteEOF(FindRandomFileName(), 0);
		break;
	case 2:
	{
		CHAR* nomFichier = FindRandomFileName();
		// Ecriture dans un fichier existant
		write(nomFichier, 0, WriteRandomStuff(maxLenght, nomFichier[0], buffer), buffer);
		delete[] nomFichier;
	}
	break;
	case 3:
	{
		// Effacement de la fin d'un fichier
		CHAR* nomFichier = FindRandomFileName();
		deleteEOF(nomFichier, rand() % FileLength(nomFichier));
		delete[] nomFichier;
	}
	break;
	}

	delete[] buffer;
}

//Exclue la tete de fichier
int FileLength(CHAR* nomFichier) {

	CHAR filePos = FindFichier(nomFichier);

	if (filePos == BLOCKFAULT) throw("Impossible de trouver la grosseur d'un fichier car il est inexistant.");

	//Grosser du dernier block ex: [64,64,64,21]  -> 21
	CHAR grosseurDernierBlock = ReadCellFromBlock(filePos, blockSize - 1);

	int quantitéDeBlockPlein = 0;

	//Trouve la quantité de block plein (INCLUANT LE PREMIER)  ex: [64,64,64,21]  -> 3
	CHAR next = filePos;
	while (true)
	{
		next = ReadFAT(next);
		if (next == 255) break;
		quantitéDeBlockPlein++;
	}

	// Retourne la quantité totale ex: (3 * 64) + 21
	return quantitéDeBlockPlein * blockSize + grosseurDernierBlock;
}

//A noter que si toute les lettre de l'alphabet son prise, plusieur fichier auront le nom z.txt
CHAR* GetRandomFileName(CHAR* alphabet) {
	//Toujours _.txt
	CHAR* nomFichier = new CHAR[nomFichierSize];
	nomFichier[1] = '.';
	nomFichier[2] = 't';
	nomFichier[3] = 'x';
	nomFichier[4] = 't';

	//Trouve une lettre non-utilisé
	for (int i = 0; i < 26; i++) {
		nomFichier[0] = alphabet[i];
		if (FindFichier(nomFichier) == BLOCKFAULT) break;
	}
	return nomFichier;
}

int WriteRandomStuff(int nbCharMax, CHAR laLettre, CHAR* & buffer) {
	int textLenght = rand() % nbCharMax;

	for (int i = 0; i < textLenght; i++) {
		buffer[i] = laLettre;
	}
	return textLenght;
}

// Trouver un fichier
CHAR* FindRandomFileName() {
	//Trouve un bloc aléatoire du dossier racine
	int nbCells = CountNbFiles();
	int nbBlocks = floor(nbCells / 64);
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

	return nomFichier;
}

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

void UpdateScreen() {
	CHAR* block = new CHAR[blockSize];
	CHAR* buffer = new CHAR[blockSize];
	CHAR* blockFichier = new CHAR[blockSize];
	CHAR* listInfo = new CHAR[256];
	CHAR currentBlock = 0;
	CHAR lenghtLastBlock;
	CHAR nextFileBlock;
	string output = "";
	bool keepgoing = true;
	bool endOfFile = false;

	while (keepgoing) {
		dur->readBlock(currentBlock, block);
		for (int i = 0; i < blockSize; i++) {
			if (block[i] == 255) {
				// Fin de la liste des fichiers
				keepgoing = false;
				// On doit quand meme imprimer tout les blocs
				for (int j = 0; j < bitMap; j++) {
					listInfo[i] = '0';
				}
				break;
			}
			else {
				dur->readBlock(block[i], blockFichier);
				listInfo[i] = 'A';

				CHAR length = GetCharacter(currentBlock, 3);
				if (length == BLOCKFAULT)length = blockSize - 1;
				AddToString(output, blockFichier, blockSize - 1);

				lenghtLastBlock = blockFichier[63];
				output += ", ";
				nextFileBlock = ReadFAT(block[i]);
				if (nextFileBlock == BLOCKFAULT) {
					// Fin du fichier deja rencontrer
					output += "|";
					endOfFile = true;
				}
				while (!endOfFile) {
					output += nextFileBlock + "->";
					nextFileBlock = ReadFAT(nextFileBlock);
					if (nextFileBlock == BLOCKFAULT) {
						if (lenghtLastBlock != 0) {
							listInfo[i] = 'a';
						}
						else {
							listInfo[i] = 'A';
						}
						endOfFile = true; // Fin du fichier
					}
				}
				endOfFile = false;
				output += "|";
			}
		}
		currentBlock = ReadFAT(currentBlock);
	}

	cout << "\n\nListe des fichiers : ";

	cout << output << endl;

	for (int i = 0; i < 256; i++) {
		cout << " Bloc " << i << ": " << listInfo[i] << " |";
	}

	delete[] block;
	delete[] buffer;
	delete[] blockFichier;
	delete[] listInfo;
	return;
}

void AddToString(string & result, CHAR* input, int lenght) {
	for (int i = 0; i < lenght; i++) {
		result += input[i];
	}
	return;
}

// FONCTIONS DES INTERACTIONS AVEC LES FICHIERS

// ouvre un fichier (s'il existe) et lit (selon les paramètres) les données pour les mettre dans TampLecture puis le referme.
void read(CHAR* nomFichier, int position, int nbChar, CHAR* & TampLecture) {
	CHAR currentBlock;
	int currentChar = 0;
	bool keepgoing = true;
	CHAR* buffer = new CHAR[blockSize];

	try
	{
		CHAR currentBlock;
		CHAR startCell;
		GetPosIntoFile(nomFichier, position, startCell, currentBlock);

		while (true)
		{
			dur->readBlock(currentBlock, buffer);

			for (int i = startCell; i < blockSize; i++) {
				TampLecture[currentChar] = buffer[i];
				currentChar++;
				if (currentChar >= nbChar) return; //Fini !
			}

			startCell = 0;
			currentBlock = ReadFAT(currentBlock);
			if (currentBlock == BLOCKFAULT) throw("Fichier est trop court.");
		}
	}
	catch (string error) {
		cout << error << endl;
	}
	catch (...) {
		cout << "Erreur inconnue de lecture de fichier." << endl;
	}
}

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
				delete[] block;
				delete[] blockFichier;
				return block[i];
			}
		}

		//Pas dans ce block ? next one!
		CHAR nextBlock = ReadFAT(nextBlock);

		if (nextBlock == BLOCKFAULT) break;
	}

	delete[] block;
	delete[] blockFichier;
	return BLOCKFAULT;
}

void write(CHAR* nomFichier, int position, int nbChar, CHAR* TampLecture) {

	CHAR teteFichier = FindFichier(nomFichier);
	try
	{
		// Creer la tete du fichier
		if (teteFichier == BLOCKFAULT) {
			teteFichier = CreateFile(nomFichier);
		}

		//Ajuste la grosseur présente dans le 64e octet
		int oldTotalSize = FileLength(nomFichier);
		int newTotalSize = position + nbChar;

		if (newTotalSize > oldTotalSize)
		{
			CHAR* teteContent = new CHAR[blockSize];
			dur->readBlock(teteFichier, teteContent);
			teteContent[63] = newTotalSize % blockSize;
			dur->writeBlock(teteFichier, teteContent);
		}
	}
	catch (string error) {
		cout << error << endl;
		return;
	}
	catch (...)
	{
		cout << "Erreur de creation de tete de fichier" << endl;
		return;
	}

	try
	{
		int currentChar = 0;
		CHAR* buffer = new CHAR[blockSize];
		CHAR currentBlock;
		CHAR startCell;

		//Reserve l'espace necessaire, tant que GetPosIntoFile retourne 'false'
		while (!GetPosIntoFile(nomFichier, position + nbChar, startCell, currentBlock))
			NewLinkedBlock(currentBlock);

		//Marque le dernier block comme etant la fin (dans la FAT)
		WriteFAT(currentBlock, BLOCKFAULT);

		//Replace les compteur aux bonne place
		GetPosIntoFile(nomFichier, position, startCell, currentBlock);

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
				{
					delete[] buffer;
					return;
				}
			}

			//Écrase l'ancien block avec la nouvelle version
			dur->writeBlock(currentBlock, buffer);

			//Passe au prochain block
			startCell = 0;
			currentBlock = ReadFAT(currentBlock);

			//Erreur ?
			if (currentBlock == BLOCKFAULT) throw("Erreur d'ecriture. L'espace reserve n'est pas suffisant.");
		}
	}
	catch (string error) {
		cout << error << endl;
		return;
	}
	catch (...)
	{
		cout << "Erreur d'ecriture dans le fichier" << endl;
		return;
	}
}

void deleteEOF(CHAR* nomFichier, int position) {
	CHAR* buffer = new CHAR[blockSize];
	try
	{
		CHAR currentBlock;
		CHAR startCell;
		int pos = position - 1;

		if (position <= 0) {
			DeleteFichier(nomFichier);
		}
		else {
			GetPosIntoFile(nomFichier, pos, startCell, currentBlock);

			dur->readBlock(currentBlock, buffer);
			ClearBlockLinksFrom(currentBlock);
			for (int i = startCell + 1; i < blockSize; i++) {
				buffer[i] = 0;
			}
			dur->writeBlock(currentBlock, buffer);
			WriteFAT(currentBlock, 0);
		}
	}
	catch (string error) {
		cout << error << endl;
	}
	catch (...) {
		cout << "Erreur inconnue d'effaceage de fichier." << endl;
	}
}

void DeleteFichier(CHAR* nomFichier) {
	CHAR firstFileBloc = FindFichier(nomFichier);
	CHAR* buffer = new CHAR[blockSize];
	CHAR* oldRacine = new CHAR[blockSize];
	bool keepgoing = false;
	bool getout = false;

	// On rend libre les blocs du fichier
	ClearBlockLinksFrom(firstFileBloc);
	WriteFAT(firstFileBloc, 0);

	// On retire le fichier du bloc racine contenant la liste des fichiers
	for (int j = 0; !getout; j++) {
		dur->readBlock(j, oldRacine);
		buffer = oldRacine;
		for (int i = 0; i < blockSize; i++) {
			if (keepgoing) {
				buffer[i] = buffer[i + 1];
			}
			else if (oldRacine[i] == firstFileBloc) {
				// on a trouve la cellule du fichier!
				keepgoing = true;
				buffer[i] = buffer[i + 1];
			}
			else if (oldRacine[i] == 255) {
				getout = true;
				break;
			}
		}
		dur->writeBlock(j, buffer);
	}
	return;
}

void ClearBlockLinksFrom(CHAR currentBlock) {

	while (true)
	{
		CHAR pastBlock = currentBlock;
		currentBlock = ReadFAT(currentBlock);
		WriteFAT(pastBlock, BLOCKFAULT);

		if (currentBlock == BLOCKFAULT) break;

		SetBitMap(currentBlock, false);
	}
}

// FONCTIONS DU DISQUE DUR

void DisqueDur::readBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekg(pos);
	hd.read((char*)tampLecture, blockSize);
}

void DisqueDur::writeBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekp(pos);
	hd.write((char*)tampLecture, blockSize);

	SetBitMap(numBlock, true);
}

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
		throw("Disque dur plein !");
	}

	CHAR result = 255;
	for (int bit = 0; bit < 8; bit++) {
		if ((map[cell] >> bit & 0x01) == 0) {
			result = (cell * 8) + bit;
		}
	}

	delete[] map;

	if (result >= bitMap)
	{
		throw("Disque dur plein !");
	}
	return result;
}

bool IsBlockLibre(CHAR numBlock)
{
	CHAR cell = numBlock / 8;
	CHAR bit = numBlock % 8;

	CHAR result = ReadCellFromBlock(bitMap, cell);

	return ((result >> bit) & 0x01) == 0;
}

CHAR WriteFAT(CHAR numBlock, CHAR value)
{
	CHAR block = (numBlock / blockSize) + FAT;
	CHAR cell = numBlock % blockSize;

	CHAR* row = new CHAR[blockSize];
	dur->readBlock(block, row);

	row[cell] = value;

	dur->writeBlock(block, row);

	delete[] row;
}

CHAR ReadFAT(CHAR numBlock)
{
	CHAR block = (numBlock / blockSize) + FAT;
	CHAR index = numBlock % blockSize;
	CHAR result = ReadCellFromBlock(block, index);
	return result;
}

CHAR ReadCellFromBlock(CHAR numBlock, CHAR numCell)
{
	CHAR* row = new CHAR[blockSize];

	dur->readBlock(numBlock, row);

	CHAR result = row[numCell];

	delete[] row;
	return result;
}

void SetBitMap(CHAR numBlock, bool state)
{
	if (numBlock >= bitMap)
	{
		throw("Erreur, ne peut pas set un bit dans la bitMap > 250");
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

bool Compare(CHAR* a, CHAR* b, int size) {
	for (int i = 0; i < size; i++) {
		if (a[i] != b[i]) return false;
	}
	return true;
}

bool GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock) {
	CHAR currentBlock = FindFichier(nomFichier);

	if (currentBlock == BLOCKFAULT) throw("Fichier inexistant");
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
				return;
			}
		}
		currentBlock = ReadFAT(currentBlock);
		if (currentBlock == BLOCKFAULT) false;
	}
}

CHAR CreateFile(CHAR* nomFichier)
{
	//Vérifie si le fichier existe déjà
	if (FindFichier(nomFichier) != BLOCKFAULT) throw("Erreur de création de fichier, nom déjà existant.");

	//Cree le fichier
	nomFichier[63] = 0; //Dernier octet -> grosseur du dernier block
	CHAR teteFichier = NewBlock();
	FillBlockWith(teteFichier, 3);

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

CHAR NewLinkedBlock(CHAR from)
{
	CHAR nouveauBlock = GetBlockLibre();
	if (nouveauBlock == BLOCKFAULT) throw("Echec de creation de nouveau block, espace insuffisant sur le disque.");
	WriteFAT(from, nouveauBlock); //Cree le lien dans la FAT
	SetBitMap(nouveauBlock, true); //Marque le nouveau block comme etant utilisee

	return nouveauBlock;
}

CHAR NewBlock()
{
	CHAR nouveauBlock = GetBlockLibre();
	if (nouveauBlock == BLOCKFAULT) throw("Echec de creation de nouveau block, espace insuffisant sur le disque.");
	SetBitMap(nouveauBlock, true); //Marque le nouveau block comme etant utilisee

	return nouveauBlock;
}

CHAR NewBlock(CHAR* content)
{
	CHAR nouveauBlock = GetBlockLibre();
	if (nouveauBlock == BLOCKFAULT) throw("Echec de creation de nouveau block, espace insuffisant sur le disque.");
	SetBitMap(nouveauBlock, true); //Marque le nouveau block comme etant utilisee

	dur->writeBlock(nouveauBlock, content);

	return nouveauBlock;
}

void FillBlockWith(CHAR block, CHAR character)
{
	CHAR* content = new CHAR[blockSize];

	for (int i = 0; i < blockSize; i++) {
		content[i] = character;
	}
	dur->writeBlock(block, content);
	delete content;
}

CHAR GetCharacter(CHAR block, CHAR character) {
	CHAR* content = new CHAR[blockSize];
	dur->readBlock(block, content);
	for (CHAR i = 0; i < blockSize; i++) {
		if (content[i] == character) return i;
	}
	return BLOCKFAULT;
}