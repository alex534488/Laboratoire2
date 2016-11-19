#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
using namespace std;

#define CHAR unsigned char
const CHAR bitMap = 251;
const CHAR FAT = 252;
const int blockSize = 64;

const CHAR bitMap = 251;
const CHAR FAT = 252;

class DisqueDur{
private:

	// Un seul fichier est écrit par le programme en exécution (HD.DH).
	fstream hd;
public:
	DisqueDur(string fichier) {
		hd.open(fichier, fstream::in | fstream::out | fstream::binary);
		if (!hd.is_open) {
			string erreur = "Erreur lors de l'ouverture de " + fichier;
			cout << erreur;
		}
	}

	~DisqueDur() {

	}

	void readBlock(CHAR numBlock, CHAR* tampLecture);
	void writeBlock(CHAR numBlock, CHAR* tampLecture);
	CHAR GetBlockLibre();
	bool IsBlockLibre(CHAR numBlock);
	CHAR ReadFAT(CHAR numBlock);
	CHAR ReadCellFromBlock(CHAR numBlock, CHAR numCell);
};

void UpdateFiles();
void UpdateScreen();
void read(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void write(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void deleteEOF(CHAR* nomFichier, fpos_t position);

DisqueDur* dur;

int main() {
	dur = new DisqueDur("hd.dh");

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
	/*
	Votre programme principal écrit aléatoirement des fichiers nommées a.txt, b.txt, etc. qui contiennent des a,b, c, etc. Il efface aléatoirement des fichiers et des fins de fichiers.
	*/
}

void UpdateScreen() {
	/*
	L'ensemble de son fichier HD.DH soit 256 lettres majuscules ou minuscules selon que le bloc est plein ou incomplet. 
	Par exemple, écrire un A (majuscule) pour représenter un bloc plein de "a" et un a (minuscule) pour un bloc incomplet de "a".
	La liste des fichiers et des blocs qu'ils occupent.
	*/
}

// FONCTIONS DES INTERACTIONS AVEC LES FICHIERS

void read(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture) {
	CHAR premierBlock;
	// ouvre un fichier (s'il existe) et lit (selon les paramètres) les données pour les mettre dans TampLecture puis le referme.
	premierBlock = FindFichier(nomFichier);
}

CHAR FindFichier(CHAR* nomFichier) {
	CHAR* blockBuffer1;
	CHAR* blockBuffer2;
	for (CHAR i = FAT; i < 256; i++) {
		dur->readBlock(i, blockBuffer1);
		for (int j = 0; j < blockSize; j++) {
			dur->readBlock(blockBuffer1[j], blockBuffer2);
			if(nomFichier == blockBuffer2) {

			}
		}
	}
}

void write(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture) {
	// ouvre un fichier ou le crée au besoin et écrit (selon les paramètres) TampEcriture puis le referme.
}

void deleteEOF(CHAR* nomFichier, fpos_t position) {
	// ouvre un fichier existant et le coupe à "position" puis le referme. Si position est 0, le fichier est effacé.
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

}

<<<<<<< HEAD
// ouput seekp
// input seekg
=======
CHAR DisqueDur::GetBlockLibre()
{
	CHAR* map = (CHAR*)malloc(blockSize);
	readBlock(bitMap, map);

	return CHAR();
}

bool DisqueDur::IsBlockLibre(CHAR numBlock)
{
	CHAR cell = numBlock / 8;
	CHAR bit = numBlock % 8;

	CHAR result = ReadCellFromBlock(bitMap, cell);

	return ((result >> bit) & 0x01) == 0;
}

CHAR DisqueDur::ReadFAT(CHAR numBlock)
{
	CHAR block = (numBlock / blockSize) + FAT;
	CHAR index = numBlock % blockSize;
	CHAR result = ReadCellFromBlock(block, index);
	return result;
}

CHAR DisqueDur::ReadCellFromBlock(CHAR numBlock, CHAR numCell)
{
	CHAR* row = new CHAR[blockSize];

	readBlock(numBlock, row);

	CHAR result = row[numCell];

	delete row;
	return result;
}
>>>>>>> 60c7a0179b03766b6c7bc4111b054dc651fb52d5
