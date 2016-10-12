#include <iostream>
#include <fstream>
#include <string>

//#include "libs\red_black_tree.h"
#include "libs\red_black_tree_file.h"

int main(int argc, char * argv[]) {
	file::red_black_tree file_t(argv[1]);
	
	std::ifstream myFile(argv[2]);
	std::string line;

	if (myFile.is_open())
		while (getline(myFile, line))
			file_t.add(std::stoi(line));
	else
		std::cout << "unable to open input file\n";

	myFile.close();
	//file_t.print();
	myFile.open(argv[3]);
	
	if (myFile.is_open())
		while (getline(myFile, line)) {
			std::cout << "elemento de busca: " << line << ' ';
			file_t.get_element(std::stoi(line));
		}
	else
		std::cout << "unable to open search file\n";

	//system("pause");
}