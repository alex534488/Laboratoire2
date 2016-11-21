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
			string erreur = "Erreur lors de l'ouverture de " + fichier;
			cout << erreur << endl;
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
void GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock);

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
		delete nomFichier;
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
		delete nomFichier;
	}
	break;
	case 3:
	{
		// Effacement de la fin d'un fichier
		CHAR* nomFichier = FindRandomFileName();
		deleteEOF(nomFichier, rand() % FileLenght(nomFichier));
		delete nomFichier;
	}
	break;
	}

	delete buffer;
}

int FileLenght(CHAR* nomFichier) {

	CHAR filePos = FindFichier(nomFichier);

	if (filePos == BLOCKFAULT) return -1;

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

	delete block;
	return nbFiles;
}

void UpdateScreen() {
	CHAR* block = new CHAR[blockSize];
	CHAR* buffer = new CHAR[blockSize];
	CHAR* blockFichier = new CHAR[blockSize];
	CHAR* listInfo = new CHAR[256];
	CHAR nextBlock = 0;
	CHAR lenghtLastBlock;
	CHAR nextFileBlock;
	string output = "";
	bool keepgoing = true;
	bool endOfFile = false;

	while (keepgoing) {
		dur->readBlock(nextBlock, block);
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
		nextBlock = ReadFAT(nextBlock);
	}

	cout << "\n\nListe des fichiers : ";

	cout << output << endl;

	for (int i = 0; i < 256; i++) {
		cout << " Bloc " << i << ": " << listInfo[i] << " |";
	}

	delete block;
	delete buffer;
	delete blockFichier;
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
			if (currentBlock == BLOCKFAULT) throw("Fichier est trop court."); //Fini !
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
				delete block;
				delete blockFichier;
				return BLOCKFAULT;
			}
			dur->readBlock(block[i], blockFichier);
			if (Compare(blockFichier, nomFichier, nomFichierSize)) {
				delete block;
				delete blockFichier;
				return block[i];
			}
		}

		//Pas dans ce block ? next one!
		CHAR nextBlock = ReadFAT(nextBlock);

		if (nextBlock == BLOCKFAULT) break;
	}

	delete block;
	delete blockFichier;
	return BLOCKFAULT;
}

void write(CHAR* nomFichier, int position, int nbChar, CHAR* TampLecture) {
	CHAR teteFichier = FindFichier(nomFichier);
	
	// Creer la tete du fichier
	if (teteFichier == BLOCKFAULT) {
		//Creer le fichier
		teteFichier = GetBlockLibre();
		nomFichier[63] = 0; //Dernier octet -> grosseur du dernier block
		dur->writeBlock(teteFichier, nomFichier);
		SetBitMap(teteFichier, true);
	}

	while (true)
	{

	}


	// Si le fichier n'existe pas deja, creer sa reference avec son nom 

	// position 0 correspond a apres la reference
}

void deleteEOF(CHAR* nomFichier, int position) {
	// ouvre un fichier existant et le coupe à "position" puis le referme. Si position est 0, le fichier est effacé.
	CHAR* buffer = new CHAR[blockSize];

	try
	{
		CHAR currentBlock;
		CHAR startCell;
		GetPosIntoFile(nomFichier, position, startCell, currentBlock);
		
		if (position == 0) {
			DeleteFichier(nomFichier);
		}
		else {
			dur->readBlock(currentBlock, buffer);
			ResetBitMaps(currentBlock);
			for (int i = startCell; i < blockSize; i++) {
				buffer[i] = 0;
			}
			dur->writeBlock(currentBlock,buffer);
			WriteFAT(currentBlock, 0);
		}
	}
	catch (string error) {
		cout << error << endl;
	}
	catch (...) {
		cout << "Erreur inconnue de lecture de fichier." << endl;
	}
}

void DeleteFichier(CHAR* nomFichier) {

}

void ResetBitMaps(CHAR currentBlock) {

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
		cout << "Disque dur plein !" << endl;
		delete map;
		throw("Disque dur plein !");
	}

	CHAR result = 255;
	for (int bit = 0; bit < 8; bit++) {
		if ((map[cell] >> bit & 0x01) == 0) {
			result = (cell * 8) + bit;
		}
	}

	delete map;

	if (result >= bitMap)
	{
		cout << "Disque dur plein !" << endl;
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

CHAR WriteFAT(CHAR numBlock, CHAR newNumBlock) {

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

	delete row;
	return result;
}

void SetBitMap(CHAR numBlock, bool state)
{
	if (numBlock >= bitMap)
	{
		cout << "Erreur, ne peut pas set un bit dans la bitMap > 250" << endl;
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
	delete map;
}

bool Compare(CHAR* a, CHAR* b, int size) {
	for (int i = 0; i < size; i++) {
		if (a[i] != b[i]) return false;
	}
	return true;
}

void GetPosIntoFile(CHAR* nomFichier, int position, CHAR& outPos, CHAR& outBlock) {
	CHAR currentBlock = FindFichier(nomFichier);

	if (currentBlock == BLOCKFAULT) throw("Fichier inexistant");
	currentBlock = ReadFAT(currentBlock);
	if (currentBlock == BLOCKFAULT) throw("Fichier vide");

	CHAR* buffer = new CHAR[blockSize];

	for (int j = 0; true; j++) {
		for (int i = 0; i < blockSize; i++) {
			int currentTotalPos = j * blockSize + i;

			if (currentTotalPos >= position) {
				outPos = i;
				outBlock = currentBlock;
				return;
			}
		}
		currentBlock = ReadFAT(currentBlock);
		if (currentBlock == BLOCKFAULT) throw("Fichier est plus court que la position donnee");
	}
}
