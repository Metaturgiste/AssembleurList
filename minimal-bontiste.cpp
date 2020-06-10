#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib> // Pour atoi
//#include <Zydis/Zydis.h>
//#include "Dossier-git-zydis/include/Zydis/Zydis.h"
#include "../zydis/include/Zydis/Zydis.h"
#include "../zydis/include/Zydis/Utils.h"

using namespace std;

// Éventuellement tu pourras tester en déclarant la variable instruction en dehors,
// et en la donnant en argument (par référence ou pointeur). Pour éviter d'en créer une à
// chaque fois : peut-être que ça ira plus vite (mais je suis dubitatif).

inline bool isMemoryMessed(ZydisDecodedInstruction instr)
{
    int nbOp = instr.operand_count;
    for(int i=0; i<nbOp; i++) {
        const ZydisDecodedOperand& op = instr.operands[i];
        if (op.type == ZYDIS_OPERAND_TYPE_MEMORY) return true; // ça implique la mémoire
        else return false;
    }
}

inline bool isInstructionValid(unsigned char* bytes, int size, ZydisDecoder *decoder)
{
    ZydisDecodedInstruction instruction;
    ZyanStatus status = ZydisDecoderDecodeBuffer(decoder, bytes, size, &instruction);
    
    if (ZYAN_SUCCESS(status)) {
        return true;
    }
    else
        return false;
}

int instrPrint(unsigned char* instr, int n)
{
	for(int j=0; j<n; j++){
		printf("%02X ", (unsigned int)instr[j]);
	}
	printf("\n");
	return 0;
}

inline bool isInstructionWithImmediate(unsigned char* bytes, int n, ZydisDecoder *decoder)
{
    ZydisDecodedInstruction instruction;
    ZydisInstructionSegments segments;
    ZyanStatus statusVal = ZydisDecoderDecodeBuffer(decoder, bytes, n, &instruction);
    ZyanStatus statusSeg = ZydisGetInstructionSegments(&instruction, &segments);

    for (ZyanU8 i = 0; i < segments.count; i++)
    {
        ZyanU8 type = segments.segments[i].type;
        if (type != ZYDIS_INSTR_SEGMENT_OPCODE and type != ZYDIS_INSTR_SEGMENT_MODRM and type != ZYDIS_INSTR_SEGMENT_SIB)
        {
            if (i == 0) return true;
            ZyanStatus status = ZydisDecoderDecodeBuffer(decoder, bytes, 15, &instruction);
            ZyanU8 len = instruction.length;
            ZyanU8 pos = segments.segments[i].offset;
            if (not isMemoryMessed(instruction)) 
                instrPrint(bytes, len);
            return true;
        }
    }
    return false;  
}

int list(int size, ZydisDecoder *decoder, unsigned char* instr, int n)
{
    for(unsigned int i=0; i<256; ++i)
    {
        instr[n-1] = (unsigned char) i; // On agrandi l'instruction
        if (n>1 and isInstructionWithImmediate(instr, n-1, decoder)) {
            return 0;
        }
        else if (isInstructionValid(instr, n, decoder)) {
            ZydisDecodedInstruction instruction;
            ZyanStatus status = ZydisDecoderDecodeBuffer(decoder, instr, n, &instruction);
            if (not isMemoryMessed(instruction))
                instrPrint(instr, n);
        }
        else if (isInstructionValid(instr, 15, decoder)) {
            if(n<size) {
                list(size, decoder, instr, n+1); // La récurence
            }
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
        int size = atoi(argv[1]);

	unsigned char* instr = (unsigned char*) malloc(size*sizeof(unsigned char));
	list(size, &decoder, instr, 1);
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
