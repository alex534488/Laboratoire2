#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <stdio.h>
#include <time.h>
#include <math.h>
using namespace std;

time_t t = time(NULL);
tm* timePtr = localtime(&t);

#define CHAR unsigned char
const CHAR bitMap = 251;
const CHAR FAT = 252;
const CHAR debutListeFichier = 0;
const int blockSize = 64;

class DisqueDur{
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
	int maxLenght = 100;
	CHAR alphabet[] = "abcdefghijklmnopqrstvwxyz";
	CHAR* buffer = new CHAR[maxLenght];
	
	// Ecriture d'un nouveau fichier
	write(CreateRandomFileName(alphabet), 0, WriteRandomStuff(maxLenght, alphabet, buffer), buffer);
	
	// Supprimer un fichier
	deleteEOF(FindRandomFileName(), 0);

	// Ecriture dans un fichier existant
	write(FindRandomFileName(), 0, WriteRandomStuff(maxLenght, alphabet, buffer),buffer);

	// Effacement de la fin d'un fichier
	CHAR* fileName = FindRandomFileName();
	deleteEOF(fileName, rand() % FileLenght(fileName));
}

int FileLenght(CHAR* nomFichier) {
	// Trouve la longueur d'un fichier en octet
} // A FAIRE

CHAR* CreateRandomFileName(CHAR* alphabet) {
	// Creer un nom de fichier inexistant
	CHAR* nomFichier = new CHAR[blockSize];
	for (int j = 0; j < blockSize; j++) {
		nomFichier[j] = alphabet[rand() % 26];
		if (-1 == FindFichier(nomFichier, debutListeFichier)) break;
	}
	return nomFichier;
}

CHAR WriteRandomStuff(int nbCharMax, CHAR* alphabet, CHAR* & buffer) {
	int textLenght = rand() % nbCharMax;
	for (int i = 0; i < textLenght + 1; i++) {
		buffer[i] = alphabet[rand() % 26];
	}
	return textLenght;
}

CHAR* FindRandomFileName() {
	// Trouver un fichier
	CHAR* nomFichier = new CHAR[blockSize];
	CHAR* numBlock = new CHAR[blockSize];
	int nbFiles = CountNbFiles();
	int nbBlocks = floor(nbFiles / 64);
	CHAR block = rand() % nbBlocks;
	int nbCells = nbFiles - ((nbBlocks - block) * 64);
	CHAR cell = rand() % nbCells;

	dur->readBlock(block, numBlock);
	dur->readBlock(numBlock[cell], nomFichier);

	return nomFichier;
}

int CountNbFiles() {
	bool keepgoing = true;
	int nbFiles = 0;
	CHAR* block = new CHAR[blockSize];
	CHAR nextBlock = 0;

	while (keepgoing) {
		dur->readBlock(nextBlock, block);
		for (int i = 0; i < blockSize; i++) {
			if (block[i] == 255) {
				// Fin de la liste des fichiers
				keepgoing = false;
				break;
			}
			else {
				nbFiles++;
			}
		}
		nextBlock = dur->ReadFAT(nextBlock);
	}
	return nbFiles;
}

void UpdateScreen() {
	CHAR* block = new CHAR[blockSize];
	CHAR* buffer = new CHAR[blockSize];
	CHAR* blockFichier = new CHAR[blockSize];
	CHAR nextBlock = 0;
	CHAR nextFileBlock;
	bool keepgoing = true;
	bool endOfFile = false;
	
	/*
	L'ensemble de son fichier HD.DH soit 256 lettres majuscules ou minuscules selon que le bloc est plein ou incomplet.
	Par exemple, écrire un A (majuscule) pour représenter un bloc plein de "a" et un a (minuscule) pour un bloc incomplet de "a".
	*/
	for (int i = 0; i < 251; i++) {
		dur->readBlock((CHAR)i, buffer);
		// es ce que buffer est plein ou incomplet?
		//plein
			cout << " Bloc " << i << ": A |";
		// incomplet
			cout << " Bloc " << i << ": a |";
		// vide
			cout << " Bloc " << i << ": 0 |";
	}

	//La liste des fichiers et des blocs qu'ils occupent.
	cout << "\n\nListe des fichiers : ";

	while (keepgoing) {
		dur->readBlock(nextBlock, block);
		for (int i = 0; i < blockSize; i++) {
			if (block[i] == 255) {
				// Fin de la liste des fichiers
				keepgoing = false;
				break;
			}
			else {
				dur->readBlock(block[i], blockFichier);
				cout << blockFichier << ", ";
				nextFileBlock = dur->ReadFAT(block[i]);
				if (nextFileBlock == 0) {
					cout << nextFileBlock << "|";
					endOfFile = true;
				}
				while (!endOfFile) {
					cout << nextFileBlock << "->";
					nextFileBlock = dur->ReadFAT(nextFileBlock);
					if (nextFileBlock == 0) endOfFile = true;
				}
				endOfFile = false;
				cout << "|";
			}
		}
		nextBlock = dur->ReadFAT(nextBlock);
	}
	return;
}

// FONCTIONS DES INTERACTIONS AVEC LES FICHIERS

// ouvre un fichier (s'il existe) et lit (selon les paramètres) les données pour les mettre dans TampLecture puis le referme.
void read(CHAR* nomFichier, CHAR position, int nbChar, CHAR* & TampLecture) {
	CHAR currentBlock;
	int currentChar = 0;
	bool keepgoing = true;
	CHAR currentPos = position;
	CHAR* buffer = new CHAR[blockSize];

	try 
	{
		currentBlock = FindFichier(nomFichier, debutListeFichier);
	
		for (int j = 0; keepgoing; j++) {

			dur->readBlock(currentBlock, buffer);

			for (int i = 0; i < 64; i++) {
				if (((j * 64 + i) >= currentPos) && nbChar <= ((j * 64 + i) - position)) {
					// on a trouve la position!
					TampLecture[currentChar] = buffer[i];
					currentChar++;
				}
				else if (currentChar > nbChar) {
					return;
				}
			}

			currentBlock = dur->ReadFAT(currentBlock);
		}
	
		throw("Impossible de trouve la position");

	}
	catch (string error) {
		cout << error << endl;
	}
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

	throw("Aucun fichier trouvé !");

	return -1;
}

void write(CHAR* nomFichier, CHAR position, int nbChar, CHAR* TampLecture) {
	// ouvre un fichier ou le crée au besoin et écrit (selon les paramètres) TampEcriture puis le referme.

	// Si le fichier n'existe pas deja, creer sa referance avec son nom 

	// position 0 correspond a apres la reference
}

void deleteEOF(CHAR* nomFichier, CHAR position) {
	// ouvre un fichier existant et le coupe à "position" puis le referme. Si position est 0, le fichier est effacé.
}

// FONCTIONS DU DISQUE DUR

void readBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekg(pos);
	hd.read((char*)tampLecture, blockSize);
}

void writeBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekp(pos);
	hd.write((char*)tampLecture, blockSize);

	SetBitMap(numBlock, true);
}

CHAR GetBlockLibre()
{
	CHAR* map = (CHAR*)malloc(blockSize);
	readBlock(bitMap, map);
	
	int cell = 0;
	//Trouve la cell avec au moins un bit a 0
	for ( cell = 0; cell < 32; cell++) {
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

	readBlock(numBlock, row);

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
	readBlock(bitMap, map);

	CHAR cell = numBlock / 8;
	CHAR bit = numBlock % 8;

	if(state)
		map[cell] |= 1 << bit; //Met le bit a 1
	else
		map[cell] &= ~(1 << bit); //Met le bit a 0
	
	writeBlock(bitMap, map);
	delete map;
}
