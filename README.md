
Name: Raul Gonzalez, Jayvee Novilla
RedID 824258754, 824319685
 Assignment 1


# Assignment 1 Disassembler
    the disassembler program will read in SIC/XE object and symbol files then output the accompanying assembly list file. 
    The program works for format 2,3,4 as format 1 was not required for this project. Providing the .sic file was also not a requirement of this project.
    
    
# Files
**README.MD**
**dissem.cpp**
**makefile**
    input files must also be included
    ex:::**sample-obj.txt**
    ex:::**sample.sym.txt**

# Usage

    "make clean" //removes output files from "make"
    "make" //creates executable file
    "./dissem sample-obj.txt sample.sym.txt" // using the input files, creates the output sample-obj.lis.txt file
    
**NOTE** for final step, what comes after "./dissem" should be name of input files (must be obj and sym file) and willl output objfilename.lis.txt
    
# Approach 
    to divide the work more evenly between the pair of us, we felt that the method of using several functions made the most sense. Our program implements throughout the different functions we created. It was also imperative that we understand how object codes turns into assembly before implementing a solution so that we were capable of coding it into C++. 
    
    list of primary functions in code 
    - code begins with storing literals and symbols into structs
    
    - code created with several functions
    - (addrLit addrSym) these functions store literal and symbol information onto linked list
    - (parse) function implementation of reading object file
    - (formatFour) function that can check for format 4 used in parse
    - (toFile) largest function that writes Lis file with formatting
    - (decHex) we created a function that could convert decimal to hexadecimal
    - (hexDec sHexDec)we created 2 other functions that could convert signed hexadecimal and hexadecimal to decimal that could be called on throughout the code
    - (flagBit) then we had to make it so that the program could find nixbpe bits so we implemented code that could find it by converting hex to decimal then modulus 4 then the remainder became variations of 1 and 0
    - (opcodeFormat) the next function implemented finds what the format is between 2-4, using the opcode
    - (opCheck) checks if opcode is valid using boolean
    - (HexaSymbol)function converts hexadecimals to their appropriate symbol


  
 

# Deficiencies 
    program produces out of range error when literals are not at the bottom possibly due to an assumption that literals would be at the bottom 
    


