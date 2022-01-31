/*
 Name: Raul Gonzalez, Jayvee Novilla
RedID 824258754, 824319685
 Assignment 1 Disassembler
*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <vector>
#include <algorithm>
#define nullptr NULL
#define WORD 32
#define OPCOL 26
#define HEADERROW 18
using namespace std;

/*function prototypes*/
int hexDec(string num);
int sHexDec(string num);
string decHex(int num);
string flagBit(string hex);
bool opCheck(string hex);
int opcodeFormat(string hex);
string HexaSymbol(string hex);
void mnemonic(FILE *filePtr,int val);
string decHex(int val);
bool formatFour(string str);

/*store symbol table*/
struct Sym{
	int decimal;
	string str, val;

	Sym *next;
	Sym(string nam, string val, char flg, Sym *nextSym){
		this->decimal = hexDec(val);
		this->str = nam;
		this->val = val;
		this->next = nextSym;
	}

	int receiveDecimalVal(){
		return this->decimal;
	}

	string receiveName(){
		return this->str;
	}

	string receiveVal(){
		return this->val;
	}

	void setSym(Sym *nextSym){
		this->next = nextSym;
	}

};

/*store literal table*/
struct Lit{
	int decAddr, decLen;
	string str, addr;

	Lit *next;
	Lit(string nam, string addy, string len, Lit *nextLit){
		this->str = nam;
		this->decAddr = hexDec(addy);
		this->decLen = hexDec(len);
		this->next = nextLit;
	}

	string receiveAddr(){
		return this->addr;
	}

	int getDecAddress(){
		return this-> decAddr;
	}

	int getDecLength(){
		return this-> decLen;
	}

	string receiveName(){
		return this->str;
	}
			
	void setLit(Lit *nextLit){
		this->next = nextLit;
	}
};

Sym* addrSym(Sym *symPtr, string addr);
Lit* addrLit(Lit *litPtr, string addr);
Sym* symbol(Sym* symPtr, FILE *filePtr);
Lit* literal(Lit* &litPtr, FILE *filePtr);
vector<string> parse(Lit *literalPtr, FILE *filePtr);
void toFile(Sym *symbolPtr, Lit *literalPtr, vector<string> obj, FILE *filePtr);


/*
main driver
open, read, write to file, and close
*/
int main(int argc, char* argv[]){
    if(argc != 3){	/*error and exit if the program arguments are incorrect*/
		printf("Please enter 3 arguments: ./dissem objectCodeFile.txt symTableleFile.txt\n");
        exit(1);
    }

	string objectCodeFile = argv[1];
	string symTableleFile = argv[2];
	string outfileName = argv[1];
	outfileName.erase(outfileName.size()-4,4);
	string outfile = outfileName;
	outfile.append(".lis.txt");
	
	/*check files*/	
	FILE *objFilePtr;
	if(objFilePtr = fopen(objectCodeFile.c_str(), "r")){
		/*printf("Successfully opened object file.\n");###TESTING PURPOSES ONLY###*/
    }
	else{
		printf("Can't open file: %s\n", objectCodeFile.c_str());
		exit(1);
	}
	
	FILE *symFilePtr;
    if(symFilePtr = fopen(symTableleFile.c_str(), "r")){
		/*printf("Successfully opened sym table file.\n");###TESTING PURPOSES ONLY*###*/
    }
	else{
		printf("Can't open file: %s\n", symTableleFile.c_str());
		exit(1);
	}

    Sym *symPtr = nullptr;
    Lit *litPtr = nullptr;
    symPtr = symbol(symPtr, symFilePtr);
    litPtr = literal(litPtr, symFilePtr);
 
    vector<string> obj = parse(litPtr, objFilePtr);
	FILE *outfilePtr=fopen(outfile.c_str(), "w");
	toFile(symPtr, litPtr, obj, outfilePtr);

	fclose(symFilePtr);
	fclose(objFilePtr);
	fclose(outfilePtr);
    return 0;
}


/*
 reads the object file and transfers the information onto a vector<string>
*/
Lit* addrLit(Lit *litPtr, string str){
    if(litPtr!=nullptr){
        do{
            if(litPtr->receiveAddr() == str){
				return litPtr;
			}
            litPtr = litPtr->next;
        }
        while(litPtr!=nullptr);
    }
    return litPtr;
}

/*
 reads the object file and transfers the information onto a vector<string>
*/
Sym* addrSym(Sym *symPtr, string str){
    if(symPtr!=nullptr){
        do{
            if(symPtr->receiveVal() == str){
				return symPtr;
			}
            symPtr = symPtr->next;
        }
        while(symPtr!=nullptr);
    }

    return symPtr;
}

/*
 transfers info onto linked list from .sym file
*/
Lit* literal(Lit* &litPtr, FILE *filePtr){
	string name, str, length = "";
	char ch;
	int num = 0;
	
	if(feof(filePtr)){
		return litPtr;
	}
	for(int i = 0; i<2; i++){
		num = fgetc(filePtr);
		if(num != 10){
			i--;
		}
	}

	if(!feof(filePtr)){ /*check end of file*/
		do{
			num = fgetc(filePtr);
			if(num == -1){/*if -1 return the start of linked list node*/
				return litPtr;
			}
			if(num == WORD){
				do{
					num = fgetc(filePtr);
				}
				while(num == WORD);
			}
			/* continue until run across a blank space */
			if(num != WORD){
				do{
					ch = static_cast<char>(num);
					name = name + ch;
					num = fgetc(filePtr);
				}while(num != WORD);
			}
			/* continue past spaces until a character appears */
			if(num == WORD){
				do{
					num = fgetc(filePtr);
				}
				while(num == WORD);
			}
			/* continue until run across a blank space */
			if(num != WORD){
				do{
					ch = static_cast<char>(num);
					length = length + ch;
					num = fgetc(filePtr);
				}
				while(num != WORD);
			}
			if(num == WORD){
				do{
					num = fgetc(filePtr);
				}
				while(num == WORD);
			}
			
			/*address is 6 bytes*/
			for(int i = 0; i<6; i++){
				ch = static_cast<char>(num);
				str = str + ch;
				num =fgetc(filePtr);
			}

			/*create a new literal object*/
			Lit *tmpLit = new Lit(name, str, length, nullptr);
			if(litPtr == nullptr) litPtr = tmpLit;
			else{
				Lit *litPtr = litPtr;
				if(litPtr->next !=nullptr){
					do{
						litPtr = litPtr->next;
					}
					while(litPtr->next !=nullptr);
				}
				litPtr->next = tmpLit;
			}
			litPtr = tmpLit;
			name,str,length = "";
			
			if(num==WORD){
				do{
					num = fgetc(filePtr);
				}while(num==WORD);
			}

		}
	while(!feof(filePtr));
    }
    return litPtr;
}

/*
 transfers info onto linked list from .sym file
*/
Sym* symbol(Sym* symPtr, FILE *filePtr){
    int num = 0;
    string name, val = "";
    char ch, symCh;

    if(feof(filePtr)){
		return symPtr;
	}
    for(int i = 0; i<2; i++){//
        num = fgetc(filePtr);
        if(num != 10) i--;
    }

    if(!feof(filePtr)){
        do{/*retrieve first 6 bytes*/
            for(int i = 0; i<6; i++){
                num =fgetc(filePtr);
                if(i==0 && num == 10){/*return start of linked list node. A= ascii 10*/
					return symPtr;
				}
                ch = static_cast<char>(num);
                name = name + ch;
            }
			for(int i = 0; i<2; i++){
				num =fgetc(filePtr);
			}
			/*address is 6 bytes*/
            for(int i = 0; i<6; i++){
                num =fgetc(filePtr);
                ch = static_cast<char>(num);
                val = val + ch;
            }
			for(int i = 0; i<3; i++){
				num =fgetc(filePtr);
			}
			symCh = static_cast<char>(num);
            /*create a new symbol object*/
            Sym *tmpSym = new Sym(name, val, symCh, nullptr);
            if(symPtr == nullptr) symPtr = tmpSym;
            else{
                Sym *ptr = symPtr;
                while(ptr->next !=nullptr) ptr = ptr->next;
                ptr->next = tmpSym;
            }
            name = "";
			val = "";
            num = fgetc(filePtr);
        }
        while(!feof(filePtr));
    }
    return symPtr;
}

/*
Read object file and store into a string vector
*/
vector<string> parse(Lit *literalPtr, FILE *filePtr){
	string str, tmp = "";
	char H = 'H';		/*header record*/
	char T = 'T';		/*text record*/
	char M = 'M';		/*modification record*/
	char E = 'E';		/*end record*/
	int format, addr, tmpInt, tmpInt2 = 0;
	int h = H;		/*casts char H as int */
	int t = T;		/*casts char T as int */	
	int m = M;		/*casts char M as int */
	int e = E;		/*casts char E as int */
    int fileChar = fgetc(filePtr);	
    if(h != fileChar){	/*Check if if Header Row is in object file*/
		printf("Header not found in object file\n");
		exit(1);
	}	
	
	vector<string> parse;
    parse.emplace_back(string(1,H));
    for(int i=0; i<HEADERROW; i++){ /*Stores Header*/
        fileChar= fgetc(filePtr);
        tmp = static_cast<char>(fileChar);
        str = str + tmp;
    }

    addr = hexDec(str.substr(6,6));
    parse.emplace_back(str);/*add element to end of the vector string*/
    str = "";
	if(fileChar!=10){
		do{
			fileChar=fgetc(filePtr);
		}
		while(fileChar!=10);
	}
	
    parse.emplace_back(string(1,T));/*add element to end of the vector string*/
	fileChar=fgetc(filePtr);
	if( t == fileChar ){ /* if we are in a Text record*/
		do{ 
			for(int i=0;i<9;i++){
				fileChar=fgetc(filePtr);
			}
			if(fileChar!=10){/*opcode*/
				do{
					for(int i=0;i<2;i++){
						tmp = static_cast<char>(fileChar);
						str = str + tmp;
						fileChar=fgetc(filePtr);
					}
					format = opcodeFormat(str);
				
					/*Format 2 instruction*/
					if(format == 2){
						for(int i = 0; i < format; i++){
							tmp = static_cast<char>(fileChar);
							str = str + tmp;
							fileChar=fgetc(filePtr);
						}
						fileChar=fgetc(filePtr);

						parse.emplace_back(str);/*add element to end of the vector string*/
						str = "";
						addr= addr + 2;
					}	
					
					/*Format 3 instruction*/
					else if(format == 3){
						tmpInt = format;
						tmp = static_cast<char>(fileChar);
						str = str + tmp;
						fileChar=fgetc(filePtr);

						if(formatFour(tmp) == true){/*format 4*/
							tmpInt = 5;
							addr= addr + 1;
						}

						for(int i=0; i<tmpInt; i++){/*store nibble*/
							tmp = static_cast<char>(fileChar);
							str = str + tmp;
							fileChar=fgetc(filePtr);
						}
						parse.emplace_back(str);/*add element to end of the vector string*/
						str = "";
						addr = addr + 3;
					}
					/*literal*/
					if(literalPtr!=nullptr && literalPtr->getDecAddress()<= addr){
						for(int i=0; i < literalPtr->getDecLength();i++){
							tmp = static_cast<char>(fileChar);
							str = str + tmp;
							fileChar=fgetc(filePtr);
						}

						parse.emplace_back(str);/*add element to end of the vector string*/
						str = "";
						addr = addr + literalPtr->getDecLength()/2;
						literalPtr = literalPtr->next;
						continue;
					}
				}
				while(fileChar!=10); /*end of Do while(fileChar!=10)*/
			}
			fileChar=fgetc(filePtr);
		}
		while( t == fileChar );/*end of Do while ( t == fileChar );*/
	}
    parse.emplace_back(string(1,M));/*add element to end of the vector string*/
	
	if(m == fileChar){/* if we are in a Modification record */
		do{
			fileChar= fgetc(filePtr);
			while(fileChar!=10){
				tmp = static_cast<char>(fileChar);
				str = str + tmp;
				fileChar= fgetc(filePtr);
			}
			parse.emplace_back(str);/*add element to end of the vector string*/
			str = "";
			fileChar= fgetc(filePtr);
		}
		while(m == fileChar);
	}

    /*Check for End record*/
    if(e != fileChar){
		printf("End record not found\n");
		exit(1);
	}
    parse.emplace_back(string(1,E));/*add element to end of the vector string*/
	
    for(int i=0; i<6; i++){/*END record*/
        fileChar= fgetc(filePtr);
        tmp = static_cast<char>(fileChar);
        str = str + tmp;
    }
    parse.emplace_back(str);/*add element to end of the vector string*/
    str = "";
    return parse;
}

/*check format four*/
bool formatFour(string str){
	bool flag;
	if(str == "1"){
		flag = true;		
	}
	else if(str == "3"){
		flag = true;		
	}		
	else if(str == "5"){
		flag = true;		
	}
	else if(str == "7"){
		flag = true;		
	}	
	else if(str == "9"){
		flag = true;		
	}	
	else if(str == "B"){
		flag = true;		
	}	
	else if(str == "D"){
		flag = true;		
	}	
	else if(str == "F"){
		flag = true;		
	}
	else{	
		flag = false;
	}
	return flag;
}

/*
write output file
*/
void toFile(Sym *symbolPtr, Lit *literalPtr, vector<string> obj, FILE *filePtr){
	stringstream ss;
	string str, strResb, strResw, base, flagBits = "";
	int num, format2, nibble, resb, resw, bitAddr, addr, addrStart, indexCol = 0; 
	int indexRow = 3;
	bool flag = false;
	
	Sym *symPtr = symbolPtr;
	Lit *litPtr = literalPtr;
	
	/*prepare formatting of output file*/
	fprintf(filePtr, ". ASSEMBLY LISTING CODE FOR ASSIGNMENT #1\n");
    fprintf(filePtr, "%s  ", obj[1].substr(8,4).c_str());
    fprintf(filePtr, "%s", obj[1].substr(0,6).c_str());	
    fprintf(filePtr, "   START   ");
    fprintf(filePtr, "%s", obj[1].substr(6,6).c_str());
    fputc(10, filePtr);
	ss << hex << "0" + obj[1].substr(6,6);
	ss >> addr;
    
	/* if not a modification record*/
	if(obj[indexRow] != "M"){
		do{	
			str = decHex(addr); /****converts decimal addr to hex addr for the lis file****/
			fprintf(filePtr, "%s  ", str.substr(str.length()-4,4).c_str()); 
			/*### For Testing address
			cout << str << endl;
			*/
			if(symPtr != nullptr && symPtr->receiveDecimalVal() == addr){
				fprintf(filePtr, "%s  ", symPtr->receiveName().c_str());
				symPtr = symPtr->next;
			}
			else fprintf(filePtr, "        ");
			indexCol = indexCol + 8;
		
			if(opcodeFormat(obj[indexRow].substr(0,2))==3 && opCheck(obj[indexRow].substr(0,3))){
				flagBits = flagBit(obj[indexRow].substr(0,3));
				if(flagBits.substr(5,1) == "1") fprintf(filePtr, "+");
				else fprintf(filePtr," ");
			}else fprintf(filePtr, " ");
			indexCol = indexCol + 1;
		
			if(litPtr!=nullptr && litPtr->getDecAddress()==addr){
				fprintf(filePtr, "LTORG  ");
				fputc(10, filePtr);
				indexCol = 0;
				if(litPtr!=nullptr && litPtr->getDecAddress()==addr){
					do{
						str = decHex(addr);/*converts decimal addr to hex addr*/
						fprintf(filePtr, "%s  ", str.substr(str.length()-4,4).c_str());
						fprintf(filePtr, "*       %s", litPtr->receiveName().c_str());
						indexCol = indexCol + 7 + litPtr->receiveName().length();

						if(indexCol < OPCOL){
							do{/*converts decimal addr to hex addr*/
								fputc(WORD, filePtr);
								indexCol= indexCol + 1;
							}
							while(indexCol < OPCOL);
						}
						fprintf(filePtr, "%s", obj[indexRow].c_str());	
						fputc(10, filePtr);
						indexCol = 0;
						addr = addr + obj[indexRow++].length()/2;
						litPtr = litPtr->next;
					}
					while(litPtr!=nullptr && litPtr->getDecAddress()==addr);
				}
				continue;
			}
			fprintf(filePtr, "%s ", HexaSymbol(obj[indexRow].substr(0,2)).c_str());
			indexCol = indexCol + HexaSymbol(obj[indexRow].substr(0,2)).length();
		
			if(opcodeFormat(obj[indexRow].substr(0,2))==3){/*if format 3*/
				flagBits = flagBit(obj[indexRow].substr(0,3));
				if(flagBits.substr(0,1)=="0"){
					fputc(35, filePtr);
				}
				else if(flagBits.substr(1,1)=="0"){
					fputc(64, filePtr);
				}
				else fputc(WORD, filePtr);
				indexCol = indexCol + 1;
			}

			if(HexaSymbol(obj[indexRow].substr(0,2))=="LDB   "){/*check register*/
				nibble = 3;
				bitAddr = 0;
				if(flagBits.substr(5,1)=="1"){
					nibble = 5;
				}
				else if(flagBits.substr(4,1)=="1"){
					bitAddr = addr;
				}
				else if(flagBits.substr(3,1)=="1"){
					bitAddr = addrStart;
				}
				if(flagBits.substr(1,1)=="1"){
					addrStart = bitAddr + hexDec(obj[indexRow].substr(3,nibble));
				}
			}
			
			if(obj[indexRow].substr(0,2)=="4F"){
				if(indexCol < OPCOL){
					do{/*converts decimal addr to hex addr*/
						fputc(WORD, filePtr);
						indexCol= indexCol + 1;
					}
					while(indexCol < OPCOL);
				}
				fprintf(filePtr, "%s", obj[indexRow].c_str());

				fputc(10, filePtr);
			}
			else if(opcodeFormat(obj[indexRow].substr(0,2))==2){/*format 2*/
				indexCol = indexCol + 1;
				format2 = hexDec(obj[indexRow].substr(2,1));
				mnemonic(filePtr,format2);
				if(obj[indexRow].substr(0,2)!="B0"&&obj[indexRow].substr(0,2)!="B4"&&obj[indexRow].substr(0,2)!="B8"){
					indexCol = indexCol + 2;
					format2 = hexDec(obj[indexRow].substr(3,1));
					mnemonic(filePtr,format2);
				}
				if(indexCol < OPCOL){
					do{
						fputc(WORD, filePtr);
						indexCol= indexCol + 1;
					}
					while(indexCol < OPCOL);
				}
				fprintf(filePtr, "%s", obj[indexRow].c_str());

				fputc(10, filePtr);
				indexCol = 0;
			}		
			else if(opcodeFormat(obj[indexRow].substr(0,2))==3){ /*format 3 & 4*/
				if(flagBits.substr(5,1)=="0"){
					num = -1;
					if(flagBits.substr(4,1)=="1"){
						num = addr + 3 + sHexDec(obj[indexRow].substr(3,3));
					}
					else if(flagBits.substr(3,1)=="1"){
						num = addrStart + hexDec(obj[indexRow].substr(3,3));
					}
					else{
						num = hexDec(obj[indexRow].substr(3,3));
					}

					Sym *symCheck = addrSym(symbolPtr, decHex(num));
					Lit *litCheck = addrLit(literalPtr, decHex(num-3));

					if(symCheck != nullptr){
						str = symCheck->receiveName();
						while(str.substr(str.length()-1,1) == " "){/*Concatinates trailing spaces*/
							str = str.substr(0, str.length()-1);
						}
						indexCol -= symCheck->receiveName().length() - str.length();
						fprintf(filePtr, "%s", str.c_str());
                        
						indexCol = indexCol + symCheck->receiveName().length();
					}
					else if(litCheck != nullptr){
						str = litCheck->receiveName();
						while(str.substr(str.length()-1,1) == " "){
							str = str.substr(0, str.length()-1);
						}
						indexCol -= litCheck->receiveName().length() - str.length();
						fprintf(filePtr, "%s", str.c_str());
                        
						indexCol = indexCol + litCheck->receiveName().length();
					}
					else{
						fprintf(filePtr, "%s", obj[indexRow].substr(3,3).c_str());
                        
						indexCol = indexCol + obj[indexRow].substr(3,3).length();
					}

				}
				else {
					Sym *symCheck = addrSym(symbolPtr, "0"+obj[indexRow].substr(3,5));
					Lit *litCheck = addrLit(literalPtr, "0"+obj[indexRow].substr(3,5));

					if(symCheck != nullptr){
						str = symCheck->receiveName();
						while(str.substr(str.length()-1,1) == " "){
							str = str.substr(0, str.length()-1);
						}
						indexCol -= symCheck->receiveName().length() - str.length();
						fprintf(filePtr, "%s", str.c_str());
                        
						indexCol = indexCol + symCheck->receiveName().length();
					}
					else if(litCheck != nullptr){
						str = litCheck->receiveName();
						while(str.substr(str.length()-1,1) == " "){
							str = str.substr(0, str.length()-1);
						}
						indexCol -= litCheck->receiveName().length() - str.length();
						fprintf(filePtr, "%s", str.c_str());
                        
						indexCol = indexCol + litCheck->receiveName().length();
					}
					else{
						fprintf(filePtr, "%s", obj[indexRow].substr(3,5).c_str());
                        
						indexCol = indexCol + obj[indexRow].substr(3,5).length();
					}
				}

				if(flagBits.substr(2,1)=="1"){
					fprintf(filePtr, ",X");
					indexCol= indexCol + 2;
				}

				if(indexCol < OPCOL){
					do{/*converts decimal addr to hex addr*/
						fputc(WORD, filePtr);
						indexCol= indexCol + 1;
					}
					while(indexCol < OPCOL);
				}
				fprintf(filePtr, "%s", obj[indexRow].c_str());

				fputc(10, filePtr);
				indexCol = 0;
			}
            
			/* base or pc relative check*/
			if(obj[indexRow + 1] != "M" && opcodeFormat(obj[indexRow+1].substr(0,2))==3 && opcodeFormat(obj[indexRow].substr(0,2))==3){
				if(flagBit(obj[indexRow+1].substr(0,3)).substr(3,1)=="1" && !flag){
					str = decHex(addr);
					fprintf(filePtr, "%s  ", str.substr(str.length()-4,4).c_str());
					base = addrSym(symbolPtr, decHex(addrStart))->receiveName();
					fprintf(filePtr, "         BASE    %s", base.c_str());
					fputc(10, filePtr);
					indexCol = 0;
					flag = true;
				}
				else if(flagBit(obj[indexRow+1].substr(0,3)).substr(4,1)=="1" && flag){
					str = decHex(addr);
					fprintf(filePtr, "%s  ", str.substr(str.length()-4,4).c_str());
					fprintf(filePtr, "         NOBASE");
					fputc(10, filePtr);
					indexCol = 0;
					flag = false;
				}
			}
			addr = addr + obj[indexRow++].length()/2;
		}
		while(obj[indexRow] != "M");
	}
    Sym *sym = symbolPtr;

	/*RESB and RESW*/
	if(sym!=nullptr){
		do{
			if(sym->receiveDecimalVal() == addr){

				resb = 0;
				resw = 0;

				if(sym->next != nullptr){
					resb = sym->next->receiveDecimalVal() - sym->receiveDecimalVal();
				}
				else{
					resb = hexDec(obj[1].substr(12,6)) - sym->receiveDecimalVal();
				}
				if(resb % 3 == 0){
					resw = resb / 3;
				}
				strResb = to_string(resb);/*converts resb to string*/
				strResw = to_string(resw);/*converts resw to string*/
				str = decHex(addr);/*converts decimal addr to hex addr*/
				fprintf(filePtr, "%s  ", str.substr(str.length()-4,4).c_str());
				if((resb) % 3 == 0){
					fprintf(filePtr, "%s   RESW    %s", sym->receiveName().c_str() ,strResw.c_str());
				}
				else{
					fprintf(filePtr, "%s   RESB    %s", sym->receiveName().c_str() ,strResb.c_str());
				}
				fputc(10, filePtr);
				addr = addr + resb;
				
				/*###for testing address output###
				str = decHex(addr);
				cout << str << endl;
				*/
			}
			sym = sym->next;
		}
		while(sym!=nullptr);
	}

    sym = addrSym(symbolPtr, obj[obj.size()-1]);
    fprintf(filePtr, "      ");
    if(sym != nullptr){/*Formatting for END row*/
		fprintf(filePtr, "         END     %s", sym->receiveName().c_str());
	}
    else{
		fprintf(filePtr, "         END   %s", obj[obj.size()-1].c_str());
	}
    fputc(10, filePtr);

}

/*retrieve mnemonic and writes to file*/
void mnemonic(FILE *filePtr,int val){
	if(val == 0){
		fprintf(filePtr, "A");
		return;
	}
	else if(val == 1){
		fprintf(filePtr, "X");
		return;
		}
	else if(val == 2){
		fprintf(filePtr, "L");
		return;
	}
	else if(val == 3){
		fprintf(filePtr, "B");
		return;
	}
	else if(val == 4){
		fprintf(filePtr, "S");
		return;
	}
	else if(val == 5){
		fprintf(filePtr, "T");
		return;
	}
	else if(val == 6){
		fprintf(filePtr, "F");
		return;
	}
	else if(val == 8){
		fprintf(filePtr, "PC");
		return;
	}
	else if(val == 9){
		fprintf(filePtr, "SW");
		return;
	}
	else
		return;
}


/*
converts decimal to hexadecimal
 */
string decHex(int num){
	stringstream strs;
	strs << hex << num;
	string st (strs.str());

	/*Converts to its uppercase from lowercase letter*/
	for(int i=0;st[i]!='\0';i++)
	{
		if (st[i] >= 'a' && st[i] <= 'z')
			st[i] = st[i] - 32;        
	}

	if(st.length() < 6){
		do {/*pad the beginning with 0s*/
			st = "0" + st;
		}
		while(st.length() < 6);
	}
	else{
		st = st.substr(st.length()-6,6);
	}
	return st;
}

/*
converts hexadecimal to decimal
*/
int hexDec(string str){
	int num;
	stringstream strs;
	strs << hex << "0" + str;
	strs >> num;
	return num;
}

/*
converts signed hexadecimal to decimal
*/
int sHexDec(string num){
	int tmp;
	int length = num.length();
	string z = "";
	string ch = num.substr(0,1);

	if(ch == "8" || ch == "9" || ch == "A" || ch == "B" || ch == "C" || ch == "D" || ch == "E" || ch == "F"){
		for(int i = length; i < 9; i++){
			z= z + "F";
		}
	}
	z= z + num;
	tmp = (int)strtol(z.c_str(), 0, 16);
	return tmp;
}

/*
finds flag bits of nixbpe
*/
string flagBit(string s){
    int i = hexDec(s.substr(0,2));
    int j = hexDec(s.substr(2,1));
    string str = "";
    i = i % 4;
    if(i==3){
		str = str + "11";
	}
    if(i==2){
		str = str + "10";
	}
    if(i==1){
		str = str + "01";
	}
    if(j>=8){
        j= j-8;
        str = str + "1";
    }
    else{
		str = str + "0";
	}
    if(j>=4){
        j= j-4;
        str = str + "1";
    }
    else{
		str = str + "0";
	}
    if(j>=2){
        j= j-2;
        str = str + "1";
    }
    else{
		str = str + "0";
	}
    if(j>=1){
        j= j-1;
        str = str + "1";
    }
    else{
		str = str + "0";
	}
    return str;
}

/*
using opcode finds format
*/
int opcodeFormat(string str){
    int x = hexDec(str);/*convert string to hex then divide by 4 to find opcode*/
    x = x / 4;
    
    if (1<=x<=34 || 52<=x<=56 || 60<=x<=62){
        return 3;
	}

    else if (36<=x<=46){
		return 2;
	}
	
    else if (x==48 || x==49 || x==50){ /*Format 1 not needed but just in case*/
		return 1;
	}

    else{
        return 0;
	}
}

/*
opcode validity check
*/
bool opCheck(string str){
	string n = flagBit(str);
	string a,b,c,d,e,f,g;
	a = n.substr(0,1);
	b = n.substr(0,2);
	c = n.substr(1,1);
	d = n.substr(2,1);
	e = n.substr(3,1);
	f = n.substr(4,1);
	g = n.substr(5,1);

    if(f=="1" && g=="1"){
		return false;
	}
    if( a=="0" || c=="0"){
        if(d=="1"){
			return false;
		}
	}
    if(e=="1" && (f=="1" || g=="1")){
		return false;
	}
	if(b=="00"){
		return false;
	}
    return true;
}


/*
 converts hexadecimal opcode
*/
string HexaSymbol(string str){
    int x = hexDec(str);
    x = x/4;
	if (x == 0)
		return "LDA   ";
		
	else if (x==1)
			return "LDX   ";
		
	else if (x==2)
			return "LDL   ";

	else if (x==3)
			return "STA   ";
		
	else if (x==4)
			return "STX   ";
		
	else if (x==5)
			return "STL   ";
		
	else if (x==6)
			return "ADD   ";
		
	else if (x==7)
			return "SUB   ";
		
	else if (x==8)
			return "MUL   ";

	else if (x==9)
			return "DIV   ";
		
	else if (x==10)
			return "COMP   ";
		
	else if (x==11)
			return "TIX   ";
		
	else if (x==12)
			return "JEQ   ";
		
	else if (x==13)
			return "JGT   ";
		
	else if (x==14)
			return "JLT   ";
		
	else if (x==15)
			return "J     ";

	else if (x==16)
			return "AND   ";
		
	else if (x==17)
			return "OR    ";
		
	else if (x==18)
			return "JSUB   ";
		
	else if (x==19)
			return "RSUB   ";

	else if (x==20)
			return "LDCH   ";
		
	else if (x==21)
			return "STCH   ";
		
	else if (x==22)
			return "ADDF   ";

	else if (x==23)
			return "SUBF   ";
		
	else if (x==24)
			return "MULF   ";
		
	else if (x==25)
			return "DIVF   ";
		
	else if (x==26)
			return "LDB   ";

	else if (x==27)
			return "LDS   ";
		
	else if (x==28)
			return "LDF   ";
		
	else if (x==29)
			return "LDT   ";

	else if (x==30)
			return "STB   ";
		
	else if (x==31)
			return "STS   ";
		
	else if (x==32)
			return "STF   ";
		
	else if (x==33)
			return "STT   ";

	else if (x==34)
			return "COMPF   ";
		
	else if (x==35)
			return "ERROR   ";
		
	else if (x==36)
			return "ADDR   ";

	else if (x==37)
			return "SUBR   ";
		
	else if (x==38)
			return "MULR   ";
		
	else if (x==39)
			return "DIVR   ";
		
	else if (x==40)
			return "COMPR   ";

	else if (x==41)
			return "SHIFTL   ";
		
	else if (x==42)
			return "SHIFTR   ";
		
	else if (x==43)
			return "RMO   ";

	else if (x==44)
			return "SVC   ";
		
	else if (x==45)
			return "CLEAR   ";
		
	else if (x==46)
			return "TIXR   ";
		
	else if (x==47)
			return "ERROR   ";

	else if (x==48)
			return "FLOAT   ";
		
	else if (x==49)
			return "FIX   ";
		
	else if (x==50)
			return "NORM   ";

	else if (x==51)
			return "ERROR   ";
		
	else if (x==52)
			return "LPS   ";
		
	else if (x==53)
			return "STI   ";
		
	else if (x==54)
			return "RD   ";

	else if (x==55)
			return "WD   ";
		
	else if (x==56)
			return "TD   ";
		
	else if (x==57)
			return "ERROR   ";

	else if (x==58)
			return "STSW   ";
		
	else if (x==59)
			return "SSK   ";
		
	else if (x==60)
			return "SIO   ";
		
	else if (x==61)
			return "HIO   ";
	else if (x==62)
			return "TIO   ";
	else
		return "ERROR";
}
