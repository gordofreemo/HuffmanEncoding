## Huffman Encoding 
This project implements the concept of huffman coding into 
a program that is able to compress and decompress files. 
<br>
<br>
## Using the Program 
To compile the program, use the Makefile with "make all" in the terminal. Two executables will be generated, huffencode.exe and huffdecode.exe. 
<br>
As can be gathered, huffencode.exe is used to compress a file 
and huffdecode.exe is used to decompress a compressed file. 
<br>
Both of the files follow the same format for command line arguments: 
<br>
<br>
huffencode inputFile outputFile - compress the given inputFile and put the results into outputFile.
<br>
Running this program will also print the corresponding huffman code used for each character in the inputFile to the terminal.  
<br>
huffdecode inputFile outFile - decompress the given inputFile and put the results into outFile. 
<br>
<br>
There are some files to play around with in the "inputs" folder, where you can experiment with compressing and decompressing the files and seeing the results. 