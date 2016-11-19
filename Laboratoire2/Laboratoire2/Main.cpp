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

class DisqueDur{
private:

	// Un seul fichier est �crit par le programme en ex�cution (HD.DH).
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
void read(string nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void write(string nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void deleteEOF(string nomFichier, fpos_t position);

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
	Votre programme principal �crit al�atoirement des fichiers nomm�es a.txt, b.txt, etc. qui contiennent des a,b, c, etc. Il efface al�atoirement des fichiers et des fins de fichiers.
	*/
}

void UpdateScreen() {
	/*
	L'ensemble de son fichier HD.DH soit 256 lettres majuscules ou minuscules selon que le bloc est plein ou incomplet. 
	Par exemple, �crire un A (majuscule) pour repr�senter un bloc plein de "a" et un a (minuscule) pour un bloc incomplet de "a".
	La liste des fichiers et des blocs qu'ils occupent.
	*/
}

// FONCTIONS DES INTERACTIONS AVEC LES FICHIERS

void read(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture) {
	CHAR premierBlock;
	// ouvre un fichier (s'il existe) et lit (selon les param�tres) les donn�es pour les mettre dans TampLecture puis le referme.
	premierBlock = FindFichier(nomFichier, 0);
}

CHAR FindFichier(CHAR* nomFichier, CHAR start) {
	CHAR* block = new CHAR[blockSize];
	CHAR* blockFichier = new CHAR[blockSize];
	bool keepgoing = true;

	CHAR nextBlock = start;

	while (keepgoing) {
		dur->readBlock(nextBlock, block);
		for (int i = 0; i < blockSize; i++) {
			if (block[i] == 255) {
				keepgoing = false;
				break;
			} else {
				dur->readBlock(block[i], blockFichier);
				if (blockFichier == nomFichier) return block[i];
			}
		}
		CHAR nextBlock = dur->ReadFAT(nextBlock);
	}
	//throw 
}

void write(string nomFichier, fpos_t position, int nbChar, CHAR* TampLecture) {
	// ouvre un fichier ou le cr�e au besoin et �crit (selon les param�tres) TampEcriture puis le referme.
}

void deleteEOF(string nomFichier, fpos_t position) {
	// ouvre un fichier existant et le coupe � "position" puis le referme. Si position est 0, le fichier est effac�.
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
