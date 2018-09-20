#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <vector>

using namespace std;
//char test[] = "12 15 76 23 99";
//char test[] = "9 5 3 7 2 4 6 13";
//char test[] = "19 15 13 17 12 14 16 23";
//int count = 0;

struct Node{
	int content;
	bool visited;
	int y;
};

vector<int> createTreeArray(char*);
vector<Node> doPreorder(vector<Node>, int);
vector<Node> doInorder(vector<Node>, int);
void outputParen(fstream &, vector<Node>);
void outputGraph(fstream &, vector<Node>, int);

int main(int argc, char *argv[]){

	/*** Read File ***/
	char *file_path;
	file_path = argv[1];
	char line[128]; // as buffer
	fstream fp_r;
	fp_r.open(file_path, ios::in);
	if(!fp_r){
		cout << "File Error!" << endl;
		return 0;
	}
	/*** Write File ***/
	fstream fp_w1, fp_w2, fp_w3;
	fp_w1.open("parenthesis_representation.txt", ios::out);
	fp_w2.open("textual_printing.txt", ios::out);
	fp_w3.open("left_boundary.txt", ios::out);
	if(!fp_w1 || !fp_w2 || !fp_w3){
		cout << "File Error!" <<endl;
		return 0;
	}

	while(fp_r.getline(line, sizeof(line), '\n')){

		vector<int> tree = createTreeArray(line);

		int max_depth = 0;
		for(int i = 1; i < tree.size(); i ++){
			if(tree[i] != 0){
				max_depth = i;
			}
		}
		max_depth = floor(log2(max_depth));
		tree.resize(pow(2, max_depth + 1)); // adjust array size, to the max_depth

		/*
		for(int i = 0; i < tree.size(); i ++){
			cout << i << " : " << tree.at(i) << endl;
		}
		*/

		/*** 1. Convert to Parenthesis ***/
		vector<Node> Tree; // preorder(contain 0)
		vector<Node> Tree_path; // preordeer(not contain 0)
		Tree.resize(tree.size());
		// Tree init
		for(int i = 0; i < Tree.size(); i ++){
			Tree[i].content = tree[i];
			Tree[i].visited = false;
			Tree[i].y = floor(log2(i));
		}
		// Preorder Traversal
		Tree_path = doPreorder(Tree, max_depth);

		// Output Parenthesis
		outputParen(fp_w1, Tree_path);

		/*** 2. Inorder Graph ***/
		//vector<Node> Tree; // Inorder
		vector<Node> Tree_g; // Inorder(not contain 0)
		//Tree.resize(tree.size());
		// Tree init
		for(int i = 0; i < Tree.size(); i ++){
			//Tree[i].content = tree[i];
			Tree[i].visited = false;
			//Tree[i].y = floor(log2(i));
		}

		// Inorder Traversal
		Tree_g = doInorder(Tree, max_depth);

		// Draw the graph
		outputGraph(fp_w2, Tree_g, max_depth);
		fp_w2 << '\n';

		/*** 3. Left Boundary ***/
		for(int i = 0; i <= max_depth; i ++){ // Notice: depth from 0
			for(int j = 1; j < Tree.size(); j ++){
				int test_node = Tree[j].content;
				int test_node_y = Tree[j].y;
				if(test_node == 0)
					continue;
				if(test_node_y == i){
					fp_w3 << test_node << ' ';
					break;
				}
			}
			if(i == max_depth)
				fp_w3 << '\n';
		}

	}

	fp_r.close();
	fp_w1.close();
	fp_w2.close();
	fp_w3.close();
	return 0;
}


vector<int> createTreeArray(char *line){
	vector<int> temp_container;
	vector<int> tree;

	char *div = strtok(line, " ");
	while(div){
		temp_container.push_back(atoi(div));// char to int
		div = strtok(NULL, " ");
	}
	tree.resize(pow(2, temp_container.size()));

	/* Construct Tree  */
	tree[1] = temp_container[0]; // root
	for(int i = 1; i < temp_container.size(); i ++){
		for(int j = 1; j < tree.size(); j ++){
			int insert_pos = 0;
			if(tree[j] == 0)
				continue;
			if(temp_container[i] > tree[j]){
				insert_pos = 2 * j + 1;
				if(tree[insert_pos] == 0){
					tree[insert_pos] = temp_container[i];
					break;
				}else{
					j = insert_pos - 1;
				}
			}else{
				insert_pos = 2 * j;
				if(tree[insert_pos] == 0){
					tree[insert_pos] = temp_container[i];
					break;
				}else{
					j = insert_pos - 1;
				}
			}
		}
	}

	return tree;
}

vector<Node> doPreorder(vector<Node> Tree, int max_depth){
	int cur_index = 1; // root
	vector<Node> Tree_path; // preordeer(not contain 0)
	while(true){
		if((2 * cur_index) < Tree.size()){
			if(!Tree[2 * cur_index].visited){ // to left
				if(!Tree[cur_index].visited){
					//cout << Tree[cur_index].content << " ";
					if((Tree[cur_index].content != 0) || 
							((cur_index % 2) && (Tree[cur_index - 1].content != 0)) ||
							((!(cur_index % 2)) && (Tree[cur_index + 1].content != 0)))
						Tree_path.push_back(Tree[cur_index]);
					Tree[cur_index].visited = true;
				}
				cur_index = 2 * cur_index;
			}else if(Tree[2 * cur_index].visited && !Tree[2 * cur_index + 1].visited){ // to right
				if(!Tree[cur_index].visited){
					//cout << Tree[cur_index].content << " ";
					if((Tree[cur_index].content != 0) || 
							((cur_index % 2) && (Tree[cur_index - 1].content != 0)) ||
							((!(cur_index % 2)) && (Tree[cur_index + 1].content != 0)))
						Tree_path.push_back(Tree[cur_index]);
					Tree[cur_index].visited = true;
				}
				cur_index = 2 * cur_index + 1;
			}else if(Tree[2 * cur_index].visited && Tree[2 * cur_index + 1].visited){ // to up
				if(cur_index % 2){ // odd
					cur_index = (cur_index - 1) / 2;
				}else{ // even
					cur_index = cur_index / 2;
				}
			}

		}else{ // last layer
			//cout << Tree[cur_index].content << " ";
			if((Tree[cur_index].content != 0) || 
					((cur_index % 2) && (Tree[cur_index - 1].content != 0)) ||
					((!(cur_index % 2)) && (Tree[cur_index + 1].content != 0)))
				Tree_path.push_back(Tree[cur_index]);
			Tree[cur_index].visited = true;
			if(cur_index % 2){
				if(cur_index == (pow(2, max_depth + 1) - 1)){ // last right, terminate
					break;
				}
				cur_index = (cur_index - 1) / 2;
			}else{
				cur_index = cur_index / 2;
			}
		}
	}

	return Tree_path;
}


void outputParen(fstream &fp_w1, vector<Node> Tree_path){
	int cur_depth = 0;
	int mae_depth = 0;
	for(int i = 0; i < Tree_path.size(); i ++){
		cur_depth = Tree_path[i].y;
		int depth_diff = abs(cur_depth - mae_depth);
		if(cur_depth > mae_depth){
			for(int l = 0; l < depth_diff; l ++)
				fp_w1 << "(";
		}else if(cur_depth < mae_depth){
			for(int l = 0; l < depth_diff; l ++)
				fp_w1 << ")";
		}else if(cur_depth == mae_depth && cur_depth != 0){
			fp_w1 << " ";
		}

		if(Tree_path[i].content == 0)
			fp_w1 << "-";
		else
			fp_w1 << Tree_path[i].content;

		mae_depth = Tree_path[i].y;
		if(i == (Tree_path.size() - 1)){
			for(int l = 0; l < Tree_path[i].y; l ++) // last to root
				fp_w1 << ")";
			fp_w1 << '\n';
		}
	}
}

vector<Node> doInorder(vector<Node> Tree, int max_depth){
	int cur_index = 1; // root
	vector<Node> Tree_g; // Inorder(not contain 0)
	while(true){
		if((2 * cur_index) < Tree.size()){
			if(!Tree[2 * cur_index].visited){ // to left
				cur_index = 2 * cur_index;
			}else if(Tree[2 * cur_index].visited && !Tree[2 * cur_index + 1].visited){ // to right
				if(!Tree[cur_index].visited){
					//cout << Tree[cur_index].content << " ";
					if(Tree[cur_index].content != 0)
						Tree_g.push_back(Tree[cur_index]);
					Tree[cur_index].visited = true;
				}
				cur_index = 2 * cur_index + 1;
			}else if(Tree[2 * cur_index].visited && Tree[2 * cur_index + 1].visited){ // to up
				if(cur_index % 2){ // odd
					cur_index = (cur_index - 1) / 2;
				}else{ // even
					cur_index = cur_index / 2;
				}
			}

		}else{ // last layer
			//cout << Tree[cur_index].content << " ";
			if(Tree[cur_index].content != 0)
				Tree_g.push_back(Tree[cur_index]);
			Tree[cur_index].visited = true;
			if(cur_index % 2){
				if(cur_index == (pow(2, max_depth + 1) - 1)){ // last right, terminate
					break;
				}
				cur_index = (cur_index - 1) / 2;
			}else{
				cur_index = cur_index / 2;
			}
		}
	}

	return Tree_g;
}


void outputGraph(fstream &fp_w2, vector<Node> Tree_g, int max_depth){
	for(int l = 0; l <= max_depth; l ++){
		int j = 0;
		for(int i = 0; i < Tree_g.size(); i ++){
			if(Tree_g[i].y == l){
				for(j; j < i; j ++){
					fp_w2 << "  ";
				}
				fp_w2 << Tree_g[i].content;
				j ++; // this output, using a space
				//if(i == Tree_g.size() - 1) // last
				//	fp_w2 << " ";
			}
		}
		/*
		for(j; j < Tree_g.size(); j ++){
			fp_w2 << "  ";
		}
		*/
		fp_w2 << " \n";
	}


}

