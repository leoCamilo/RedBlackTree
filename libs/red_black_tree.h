#pragma once

#ifndef RB_TREE_H
#define RB_TREE_H

#define LEFT 0
#define RIGHT 1

struct branch typedef Branch;

namespace tpa {
	class red_black_tree {
		Branch* root;
		int size;

	public:
		red_black_tree();
		void add(int value);
		void get(int value);
		void print(Branch* node);
		void print();

	private:
		Branch* grandfather(Branch* node);
		Branch* uncle(Branch* node);
		void balancing(Branch* node);
		void rotate(Branch* node, bool direction);
	};
}

#endif