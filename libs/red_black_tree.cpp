#include "red_black_tree.h"
#include <iostream>

#define RED 1
#define BLACK 0

struct branch {
	Branch* left = NULL;	// children
	Branch* right = NULL;	// children
	Branch* dad = NULL;
	int color;
	int index;
	int content;
};

tpa::red_black_tree::red_black_tree() {
	root = NULL;
}

void tpa::red_black_tree::add(int value) {		// iteractive insertion - no balancing yet
	if (!root) {
		root = new Branch;
		root->content = value;
		root->color = BLACK;
		return;
	}

	Branch* temp_branch = root;
	char flag = 0;

	while (!flag)
		if (value > temp_branch->content) {
			flag = temp_branch->right ? 0 : 1;
			temp_branch = temp_branch->right ? temp_branch->right : temp_branch;
		} else {
			flag = temp_branch->left ? 0 : -1;
			temp_branch = temp_branch->left ? temp_branch->left : temp_branch;
		}

	if (flag == -1) {
		temp_branch->left = new Branch;
		temp_branch->left->content = value;
		temp_branch->left->color = RED;
		temp_branch->left->dad = temp_branch;
		balancing(temp_branch->left);
	} else {
		temp_branch->right = new Branch;
		temp_branch->right->content = value;
		temp_branch->right->color = RED;
		temp_branch->right->dad = temp_branch;
		balancing(temp_branch->right);
	}
}

void tpa::red_black_tree::get(int value) {
	Branch* node = root;
	int temp_val;
	bool find;

	if (node)
		temp_val = node->content;

	while ((find = (value != temp_val)) && node) {
		node = value > node->content ? node->right : node->left;
		
		if (node)
			temp_val = node->content;	
	}

	if (find)
		std::cout << "not find\n";
	else
		std::cout << "encontrado: " << temp_val << '\n';
}

void tpa::red_black_tree::print(Branch* node) {		// recursive printing
	if (node->left != NULL)
		print(node->left);

	std::cout << node->content << '|' << (node->color ? 'V' : 'B') << '|' << (node->dad ? node->dad->content : 'R') << " - ";
	
	if (node->right != NULL)
		print(node->right);
}

void tpa::red_black_tree::print() {
	print(root);
	std::cout << '\n';
}

Branch * tpa::red_black_tree::grandfather(Branch * node) {
	if (node && node->dad)
		return node->dad->dad;

	return NULL;
}

Branch * tpa::red_black_tree::uncle(Branch * node) {
	Branch* grandpa = grandfather(node);

	if (!grandpa)
		return NULL;

	if (grandpa->left == node->dad)
		return grandpa->right;
	else
		return grandpa->left;
}

void tpa::red_black_tree::balancing(Branch * node) {
	if (!node->dad)
		node->color = BLACK;
	else
		if (node->dad->color) {
			Branch* uncle_node = uncle(node);
			Branch* grandpa_node = grandfather(node);

			if (uncle_node && uncle_node->color) {
				node->dad->color = BLACK;
				uncle_node->color = BLACK;
				grandpa_node->color = RED;
				balancing(grandpa_node);
			} else {
				if ((node == node->dad->right) && (node->dad == grandpa_node->left)) {
					rotate(node->dad, LEFT);
					node = node->left;
				} else if ((node == node->dad->left) && (node->dad == grandpa_node->right)) {
					rotate(node->dad, RIGHT);
					node = node->right;
				}

				node->dad->color = BLACK;
				grandpa_node->color = RED;

				if ((node == node->dad->left) && (node->dad == grandpa_node->left))
					rotate(grandpa_node, RIGHT);
				else
					rotate(grandpa_node, LEFT);
			}
		}
}

//https://pt.wikipedia.org/wiki/%C3%81rvore_rubro-negra

void tpa::red_black_tree::rotate(Branch * node, bool direction) {
	Branch* temp_node;
	
	if (direction) {
		temp_node = node->left;
		node->left = temp_node->right;
		if (node->left) node->left->dad = node;
		temp_node->right = node;
	} else {
		temp_node = node->right;
		node->right = temp_node->left;
		if (node->right) node->right->dad = node;
		temp_node->left = node;
	}

	if (node->dad)
		if (node->dad->left == node)
			node->dad->left = temp_node;
		else
			node->dad->right = temp_node;
	else
		root = temp_node;

	temp_node->dad = node->dad;
	node->dad = temp_node;
}