#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib> // Pour atoi
//#include <Zydis/Zydis.h>
//#include "Dossier-git-zydis/include/Zydis/Zydis.h"
#include "../zydis/include/Zydis/Zydis.h"

using namespace std;

// Éventuellement tu pourras tester en déclarant la variable instruction en dehors,
// et en la donnant en argument (par référence ou pointeur). Pour éviter d'en créer une à
// chaque fois : peut-être que ça ira plus vite (mais je suis dubitatif).
inline bool isInstructionValid(unsigned char* bytes, int size, ZydisDecoder *decoder)
{
    ZydisDecodedInstruction instruction;
    ZyanStatus status = ZydisDecoderDecodeBuffer(decoder, bytes, size, &instruction);

    if (ZYAN_SUCCESS(status))
        return true;
    else
        return false;
}

int list(int n, int size, ZydisDecoder *decoder, unsigned char* instr)
{
	for(unsigned int i=0; i<256; ++i)
	{
		instr[size-n] = (unsigned char) i;
		if (n>1 && isInstructionValid(instr, 15, decoder)) {
			list(n-1, size, decoder, instr); // La récurence
		}
		else {
			if (isInstructionValid(instr, size, decoder))
				printf("Valid ");
			else if (isInstructionValid(instr, 15, decoder))
				printf("Too short ");
			else printf("Hopeless ");
			for(int j=0; j<size; j++){
				printf("%X ", (unsigned int)instr[j]);
			}
			printf("\n");
		}
	}
	return 0;
} 

int main(int argc, char* argv[])
{
    bool zydisMinimalMode = false;

    cin.tie(NULL); // ne flushe pas cout avant un cin: rend les I/O plus rapides
    ios::sync_with_stdio(false); // désynchronine cin de stdin et cout de stdout, idem

    ZydisDecoder decoder;
    if (!ZYAN_SUCCESS(ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32)))
    {
        cout << "ERROR initialising the decoder" << endl;
        exit(1);
    }
    //if (!ZYAN_SUCCESS(ZydisDecoderEnableMode(&decoder, ZYDIS_DECODER_MODE_MINIMAL, zydisMinimalMode)))

	
    /* Petit code pour détecter les préfixes, pour plus tard
     *
       vector<unsigned char> prefixes[5] = {{0xF0}, {0xF2, 0xF3}, {0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65}, {0x66}, {0x67}};
       bool isPrefix[256];
       int prefixGroup[256];
       for (int i = 0; i < 5; i++)
       for (int x : prefixes[i])
       prefixGroup[x] = i;

       for (int i = 0; i < 256; i++)
       isPrefix[i] = false;
       for (auto &x : prefixes)
       for (unsigned char y : x)
       isPrefix[y] = true;
    */

    if (argc < 2)
    {
        cout << "Error: give the length of the word to enumerate (number of bytes)" << endl;
        exit(1);
    }

    if (argc == 2)
    {
	printf("i am here\n");
        int size = atoi(argv[1]);

	unsigned char* instr = (unsigned char*) malloc(size*sizeof(unsigned char));
	list(size, size, &decoder, instr);
	free(instr);
    }


    /* Exemple de comment utiliser les arguments. Ici on aura besoin par exemple de la fonction C atoi pour obtenir l'entier
    if (argc > 2)
    {
        switch (argv[1][1])
        {
            case 'c':
                runCapstone = true;
                break;
            case 'z':
                runZydis = true;
                break;
            default:
                runAutomaton = true;
        }
    }
    */

    return 0;
}
