#include "red_black_tree_file.h"
#include <iostream>
#include <sys/stat.h>

#define RED 1
#define BLACK 0
#define NULL_POSITION -1
#define BRANCH_SIZE 24		// bytes
#define DAD_DISPLACEMENT 8 
#define DAD_SIZE 4

struct branch {
	int left = NULL;	// children - use (int) instead (std::streampos)
	int right = NULL;	// children
	int dad = NULL;
	char color;
	int index;			// position in file
	int content;

	friend std::ostream& operator<<(std::ostream& os, const Branch& b) {
		os << "\nindex: " << b.index;

		if (b.dad == NULL_POSITION) os << "\ndad: NULL"; else os << "\ndad: " << b.dad;
		if (b.left == NULL_POSITION) os << "\nleft: NULL"; else os << "\nleft: " << b.left;
		if (b.right == NULL_POSITION) os << "\nright: NULL"; else os << "\nright: " << b.right;
		
		os << "\ncontent: " << b.content << "\ncolor: " << (b.color ? 'R' : 'B') << "\n\n";

		return os;
	}
};

file::red_black_tree::red_black_tree(char* directory) {
	struct stat results;
	root = NULL;
	this->directory = directory;

	if (stat(this->directory, &results)) {
		tree_file.open(directory, std::ios::out | std::ios::binary);
		tree_file.close();
		//results.st_size - if want to known the lenght of tree, devide the lenght per branch size
	} else {
		root = new Branch;
		tree_file.open(directory, std::ios::in | std::ios::binary);
		tree_file.read((char*) root, BRANCH_SIZE);
		tree_file.close();
	}
}

void file::red_black_tree::add(int value) {
	tree_file.open(directory, std::ios::out | std::ios::in | std::ios::binary);

	if (!tree_file.is_open())
		std::cout << "probrema!";

	if (!root) {
		root = new Branch;
		root->content = value;
		root->color = BLACK;
		root->right = root->left = root->dad = NULL_POSITION;
		root->index = 0;

		tree_file.write((char*) root, BRANCH_SIZE);
		tree_file.close();
		return;
	}

	Branch* temp_branch = root;
	int dad_position = 0;
	char flag = 0;

	while (!flag)
		if (value > temp_branch->content) {
			flag = temp_branch->right != NULL_POSITION ? 0 : 1;

			if (!flag) {
				dad_position = temp_branch->right;
				tree_file.seekg(temp_branch->right);
				tree_file.read((char*) temp_branch, BRANCH_SIZE);
			}
		} else {
			flag = temp_branch->left != NULL_POSITION ? 0 : -1;
			
			if (!flag) {
				dad_position = temp_branch->left;
				tree_file.seekg(temp_branch->left);
				tree_file.read((char*) temp_branch, BRANCH_SIZE);
			}
		}
	
	Branch* new_branch = new Branch;

	new_branch->content = value;
	new_branch->dad = dad_position;
	new_branch->left = NULL_POSITION;
	new_branch->right = NULL_POSITION;
	new_branch->color = RED;

	tree_file.seekp(0, std::ios::end);
	new_branch->index = (int) tree_file.tellp();
	tree_file.write((char*) new_branch, BRANCH_SIZE);

	if (flag == -1)
		temp_branch->left = new_branch->index;
	else
		temp_branch->right = new_branch->index;

	// balancing(temp_branch->left); || balancing(temp_branch->right); (MEMORY VERSION)
	// balancing(new_branch); (STORAGE VERSION)

	tree_file.seekp(dad_position);
	tree_file.write((char*) temp_branch, BRANCH_SIZE);

	balancing(new_branch);

	tree_file.seekg(0, std::ios::beg);
	tree_file.read((char*) root, BRANCH_SIZE);	// back root to memory
	tree_file.close();
	
	delete new_branch;
}

void file::red_black_tree::get(int value) {
	Branch* node = root;
	int temp_val;
	bool find, flag = 0;

	tree_file.open(directory, std::ios::in | std::ios::binary);

	if (node)
		temp_val = node->content;

	while ((find = (value != temp_val)) && !flag) {
		if (value > temp_val) {
			flag = node->right != NULL_POSITION ? 0 : 1;

			if (!flag) {
				tree_file.seekg(node->right);
				tree_file.read((char*)node, BRANCH_SIZE);
			}
		} else {
			flag = node->left != NULL_POSITION ? 0 : 1;

			if (!flag) {
				tree_file.seekg(node->left);
				tree_file.read((char*)node, BRANCH_SIZE);
			}
		}

		temp_val = node->content;
	}

	tree_file.seekg(0, std::ios::beg);
	tree_file.read((char*) root, BRANCH_SIZE);
	tree_file.close();

	if (find)
		std::cout << "not find\n";
	else
		std::cout << "encontrado: " << temp_val << '\n';
}

int file::red_black_tree::get_element(int value) {
	Branch* node = root;
	int temp_val;
	bool find, flag = 0;
	int jumps = 0;

	tree_file.open(directory, std::ios::in | std::ios::binary);

	if (node)
		temp_val = node->content;

	while ((find = (value != temp_val)) && !flag) {
		if (value > temp_val) {
			flag = node->right != NULL_POSITION ? 0 : 1;

			if (!flag) {
				tree_file.seekg(node->right);
				tree_file.read((char*)node, BRANCH_SIZE);
			}
		} else {
			flag = node->left != NULL_POSITION ? 0 : 1;

			if (!flag) {
				tree_file.seekg(node->left);
				tree_file.read((char*)node, BRANCH_SIZE);
			}
		}

		temp_val = node->content;
		jumps++;
	}

	tree_file.seekg(0, std::ios::beg);
	tree_file.read((char*)root, BRANCH_SIZE);
	tree_file.close();

	if (find)
		std::cout << "not found - num de saltos: " << jumps << '\n';
	else
		std::cout << "found - num de saltos : " << jumps << '\n';
		//std::cout << "encontrado: " << temp_val << " - num de saltos : " << jumps << '\n';

	return jumps;
}

void file::red_black_tree::print() {
	tree_file.open(directory, std::ios::in | std::ios::binary);
	tree_file.seekg(0, std::ios::end);
	int len = (int) tree_file.tellg();
	tree_file.seekg(0, std::ios::beg);
	Branch temp;

	for (int i = 0; i < len; i += BRANCH_SIZE) {
		tree_file.read((char*) &temp, BRANCH_SIZE);
		std::cout << temp;
	}
	
	tree_file.close();
	std::cout << '\n';
}

Branch * file::red_black_tree::grandfather(Branch * node) {
	Branch* grandpa = new Branch;

	if (node && node->dad != NULL_POSITION) {
		tree_file.seekg(node->dad);
		tree_file.read((char*) grandpa, BRANCH_SIZE);
		
		if (grandpa->dad != NULL_POSITION) {
			tree_file.seekg(grandpa->dad);
			tree_file.read((char*) grandpa, BRANCH_SIZE);
			return grandpa;
		}
	}

	delete grandpa;
	return NULL;
}

Branch * file::red_black_tree::uncle(Branch * node) {
	Branch* grandpa = grandfather(node);
	bool uncle_flag = 0;

	if (!grandpa)
		return NULL;

	Branch* out = new Branch;

	if (grandpa->left == node->dad) {
		if ((uncle_flag = (grandpa->right != NULL_POSITION)))
			tree_file.seekg(grandpa->right);
	} else {
		if ((uncle_flag = (grandpa->left != NULL_POSITION)))
			tree_file.seekg(grandpa->left);
	}

	if (uncle_flag) {
		tree_file.read((char*) out, BRANCH_SIZE);
		return out;
	}

	delete out;
	return NULL;
}

void file::red_black_tree::balancing(Branch * node) {	// called inside add function, the file is already open
	if (node->dad == NULL_POSITION) {
		node->color = BLACK;
		tree_file.seekp(0, std::ios::beg);
		tree_file.write((char*) node, BRANCH_SIZE);
	} else {
		Branch dad;
		tree_file.seekg(node->dad);
		tree_file.read((char*) &dad, BRANCH_SIZE);

		if (dad.color) {
			Branch* uncle_node = uncle(node);
			Branch* grandpa_node = grandfather(node);

			if (uncle_node && uncle_node->color) {
				dad.color = BLACK;
				uncle_node->color = BLACK;
				grandpa_node->color = RED;

				tree_file.seekp(node->dad);
				tree_file.write((char*) &dad, BRANCH_SIZE);
				tree_file.seekp(uncle_node->index);
				tree_file.write((char*) uncle_node, BRANCH_SIZE);
				tree_file.seekp(grandpa_node->index);
				tree_file.write((char*) grandpa_node, BRANCH_SIZE);

				balancing(grandpa_node);
			} else {
				if ((node->index == dad.right) && (node->dad == grandpa_node->left)) {
					rotate(&dad, LEFT);
					tree_file.seekg(node->left);
					tree_file.read((char*) node, BRANCH_SIZE);
				} else if ((node->index == dad.left) && (node->dad == grandpa_node->right)) {
					rotate(&dad, RIGHT);
					tree_file.seekg(node->right);
					tree_file.read((char*)node, BRANCH_SIZE);
				}

				dad.color = BLACK;
				grandpa_node->color = RED;

				tree_file.seekp(dad.index);
				tree_file.write((char*) &dad, BRANCH_SIZE);

				if ((node->index == dad.left) && (node->dad == grandpa_node->left))
					rotate(grandpa_node, RIGHT);
				else
					rotate(grandpa_node, LEFT);
			}

			delete uncle_node;
			delete grandpa_node;
		}
	}
}

inline void file::red_black_tree::rotate(Branch * node, bool direction) {	// work with inline instruction
	Branch* temp_node = new Branch;
	
	if (direction) {
		tree_file.seekg(node->left);
		tree_file.read((char*) temp_node, BRANCH_SIZE);
		node->left = temp_node->right;
		
		if (node->left != NULL_POSITION)
			if (node->dad == NULL_POSITION) {
				tree_file.seekp(node->left + DAD_DISPLACEMENT);
				tree_file.write((char*) &temp_node->index, DAD_SIZE);
			} else {
				tree_file.seekp(node->left + DAD_DISPLACEMENT);
				tree_file.write((char*) &node->index, DAD_SIZE);
			}
	} else {
		tree_file.seekg(node->right);
		tree_file.read((char*) temp_node, BRANCH_SIZE);
		node->right = temp_node->left;

		if (node->right != NULL_POSITION)
			if (node->dad == NULL_POSITION) {
				tree_file.seekp(node->right + DAD_DISPLACEMENT);
				tree_file.write((char*) &temp_node->index, DAD_SIZE);
			} else {
				tree_file.seekp(node->right + DAD_DISPLACEMENT);
				tree_file.write((char*) &node->index, DAD_SIZE);
			}
	}

	if (node->dad != NULL_POSITION) {					// if true, no need to change the index file
		Branch dad;
		tree_file.seekg(node->dad);
		tree_file.read((char*) &dad, BRANCH_SIZE);
		if (dad.left == node->index) dad.left = temp_node->index; else dad.right = temp_node->index;
		tree_file.seekp(dad.index);
		tree_file.write((char*) &dad, BRANCH_SIZE);
	} else {											// special case, when rotate is on root branch
		int temp_idx = node->index;
		node->index = temp_node->index;
		temp_node->index = temp_idx;

		if (direction) {
			if (temp_node->left != NULL_POSITION) {
				tree_file.seekp(temp_node->left + DAD_DISPLACEMENT);
				tree_file.write((char*) &temp_idx, DAD_SIZE);
			}

			if (node->right != NULL_POSITION) {
				tree_file.seekp(node->right + DAD_DISPLACEMENT);
				tree_file.write((char*) &node->index, DAD_SIZE);
			}
		} else {
			if (temp_node->right != NULL_POSITION) {
				tree_file.seekp(temp_node->right + DAD_DISPLACEMENT);
				tree_file.write((char*) &temp_idx, DAD_SIZE);
			}

			if (node->left != NULL_POSITION) {
				tree_file.seekp(node->left + DAD_DISPLACEMENT);
				tree_file.write((char*) &node->index, DAD_SIZE);
			}
		}
	}

	if (direction) temp_node->right = node->index; else temp_node->left = node->index;

	temp_node->dad = node->dad;
	node->dad = temp_node->index;

	tree_file.seekp(node->index);
	tree_file.write((char*) node, BRANCH_SIZE);
	tree_file.seekp(temp_node->index);
	tree_file.write((char*) temp_node, BRANCH_SIZE);

	delete temp_node;
}