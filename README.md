# ACL

> Simple Access Control List system done in C++

## CS 465 Introduction to Computer Security:
###Programming Assignment #2

	Due on December 2nd, 2015
	Frederik Berentsen
	Login: xxxxxxxxx
	ID: xxxxxxxxx
	Email: xxxxxxxxx@mix.wvu.edu

The program is written in C++ and assumed that C++11 support is available.

The program contains two (2) files:

- access.h
- access.cpp

In addition, the zip contains TestCase-1, 2 and 3 files.

access.cpp contains the main part of the program (including the main function).

To compile and run the program type:

	g++ -std=c++11 -o access access.cpp
	./access filename.txt

The program runs without errors on the LOUD client.

The program passes all three (3) test cases. The program also passes the extra credit cases without errors.

The file audit.txt contains the result. All other nessesary files are also created.

Note:

- TestCase-1: Audit.txt line 22 says: "The ACL for file2.txt appended by alice to include Users:R". Based on the TestCase-1.txt file and TestCase-1-Line-by-Line.pdf it should be "file2.txt", but in PDF audit part it says "file1.txt". The program runs correctly based on the input file (TestCase-1.txt).

- TestCase-3: In the PDF of TestCase-3 the output of file1.txt line 2 says "write" twice, I assume this is an error and the program only writes it once.
