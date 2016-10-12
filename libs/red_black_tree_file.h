#include <fstream>
#pragma once

#ifndef RB_TREE_FILE_H
#define RB_TREE_FILE_H

#define LEFT 0
#define RIGHT 1

struct branch typedef Branch;

namespace file {
	class red_black_tree {
		Branch* root;
		std::fstream tree_file;
		int size;
		char* directory;

	public:
		red_black_tree(char* directory);
		void add(int value);
		void get(int value);
		int get_element(int value);
		void print();

	private:
		Branch* grandfather(Branch* node);
		Branch* uncle(Branch* node);
		void balancing(Branch* node);
		inline void rotate(Branch* node, bool direction);
	};
}

#endif