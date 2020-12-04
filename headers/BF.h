#ifndef BF_H_
#define BF_H_

/* Error codes for BF layer */
#define BFE_OK                       0
#define BFE_NOMEM                   -1
#define BFE_CANNOTOPENFILE          -2
#define BFE_CANNOTCLOSEFILE         -3
#define BFE_CANNOTCREATEFILE        -4
#define BFE_INCOMPLETEREAD          -5
#define BFE_INCOMPLETEWRITE         -6
#define BFE_FILEEXISTS              -7
#define BFE_NOBUF                   -8
#define BFE_LISTERROR               -9
#define BFE_FILEOPEN                -10
#define BFE_FD                      -11
#define BFE_FILENOTEXISTS           -12
#define BFE_FTABFULL                -13
#define BFE_HEADOVERFLOW            -14
#define BFE_BLOCKFIXED              -15
#define BFE_BLOCKUNFIXED            -16
#define BFE_EOF                     -17
#define BFE_FILEHASFIXEDBLOCKS     -18
#define BFE_BLOCKFREE               -19
#define BFE_BLOCKINBUF              -20
#define BFE_BLOCKNOTINBUF           -21
#define BFE_INVALIDBLOCK            -22
#define BFE_CANNOTDESTROYFILE		-23


/* H metavlhth opou kataxwreitai o kwdikos tou teleftaiou sfalmatos */
int BF_Errno;

/* Kathorizei to mege8os enos block apo to arxeio  */
#define BLOCK_SIZE 512


/* Arxikopoiei tin eswterikh plhroforia tin opoia krataei to epipedo block arxeiwn (BF) */
void BF_Init();


/* Dimiourgei ena neo arxeio epipedou block. An to arxeio yparxei hdh grafetai ek neou apo panw.
 * filename	to onoma tou arxeiou pros dimiourgia
 * Epistrefei:
 * 		0 se periptwsi epityxias,
 * 		Mia arnhtikh timh se periptwsh pou symvei kapoio sfalma.
 * Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh
*/
int BF_CreateFile(const char* filename);


/* Anoigei ena yparxon arxeio epipedou block.
 *
 * filename:	To onoma tou arxeiou pros anoigma
 *
 * Epistrefei:
 * 		Enan mh arnhtiko akeraio se periptwsh epityxias, pou einai o anagnwristikos ari8mos tou arxeiou.
 * 		Enan arnhtiko ari8mo se periptwsh sfalmatos.
 * Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh.
*/
int BF_OpenFile(const char* filename);


/* Kleinei ena anoixto arxeio me anagnwristiko ari8mo anoixtou arxeiou.
 *
 * fileDesc:	O anagnwristikos ari8mos tou anoigmatos tou arxeiou.
 *
 * Epistrefei:
 * 		0 se periptwsh epityxias.
 * 		Enan arnhtiko ari8mo se periptwsh sfalmatos.
 * Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh.
*/
int BF_CloseFile(const int fileDesc);


/* Epistrefei to pli8os twn dia8esimwn block pou yparxoun sto arxeio me anagnwristiko ari8mo fileDesc.
 *
 * fileDesc:	O anagnwristikos ari8mos tou anoigmatos arxeiou typou block
 *
 * Epistrefei:
 * 		To synoliko pli8os apo blocks pou yparxoun sto arxeio me anagnwristiko ari8mo fileDesc, se periptwsh epityxias.
 * 		Enan arnhtiko ari8mo se periptwsh sfalmatos.
 * Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh.
*/
int BF_GetBlockCounter(const int fileDesc);


/* Desmevei ena neo block sto anoixto arxeio epipedou block, me anagnwristiko ari8mo fileDesc.
 * To neo block exei mege8os BLOCK_SIZE kai ola ta bytes tou einai arxikopoihmena se 0.
 * To block afto topo8eteitai sto telos tou trexontos arxeiou, epomenws o ari8mos tou einai
 * BF_getBlockCounter(fileDesc) - 1.
 *
 * fileDesc:	Anagnwristikos ari8mos anoigmatos arxeiou epipedou block
 *
 * Epistrefei:
 * 		0 se periptwsh epityxous ektelesis.
 * 		Enan arnhtiko ari8mo se periptwsh sfalmatos.
 * Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh.
*/
int BF_AllocateBlock(const int fileDesc);


/* Diavazei ena sygkekrimeno block apo to arxeio epipedou block, me anagnwristiko ari8mo fileDesc.
 * To block pou diavazetai antistoixei sto blockNumber-osto block tou arxeiou,
 * me tin ari8misi na xekinaei apo to 0. Prosvash sto neo block parexetai mesw tou teleftaiou orismatos,
 * block, me klisi me anafora (call by reference).
 *
 * fileDesc:	Anangwristikos ari8mos anoigmatos arxeiou epipedou block
 * blockNumber:	O ari8mos tou block pou prokeitai na diavastei. H ari8misi twn blocks xekinaei apo to 0.
 * block:		Deiktis pros to neo block pou diavastike apo to arxeio.
 *
 * Epistrefei:
 * 		0 se periptwsi epityxias
 * 		Enan arnhtiko ari8mo se periptwsh sfalmatos.
 * Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh.
*/
int BF_ReadBlock(const int fileDesc, const int blockNumber, void** block);


/* Grafei sto arxeio epipedou block ta dedomena pou yparxoun sto block yp' ari8mon blockNumber,
 * opws afto ziti8ike apo tin BF_ReadBlock, apo to arxeio me anagnwristiko ari8mo anoigmatos fileDesc.
 *
 * fileDesc:	Anangwristikos ari8mos anoigmatos arxeiou epipedou block
 * blockNumber:	O ari8mos tou block pou prokeitai na graftei
 *
 * Epistrefei:
 * 		0 se periptwsi epityxias.
 *		Enan arnhtiko ari8mo se periptwsh sfalmatos.
 *Mporeite na kalesete thn BF_PrintError() gia na deite to sfalma pou synevh.
*/
int BF_WriteBlock(const int fileDesc, const int blockNumber);


/* Typwnei to mhnyma message sto standard error, akolou8oumeno apo mia perigrafh tou teleftaiou sfalmatos
 * pou prokli8ike sto BF epipedo.
 *
 * message:		To mhnyma pros ektypwsh
*/
void BF_PrintError(const char* message);


#endif /* BF_H_ */
