#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <queue>

using std::queue;
using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::pair;
using std::make_pair;
using std::unordered_set;

struct Node{
	int id;
	string read;
	vector<pair<Node*,int>> all_children;
	vector<pair<Node*,int>> big_children;
	
	Node(int id, string read) :
	id(id), read(read) {}
};

int find_overlap(const string &str_suf, const string &str_pre, const int &min_length) {
	size_t start = 0;
	while (true) {
		//check if the str_suf contains the first min_length characters of str_pre
		start = str_suf.find(str_pre.substr(0, min_length), start);
		if (start == string::npos) {
			return 0;
		}
		string suf_overlap = str_suf.substr(start, str_suf.size() - start);
		string pre_overlap = str_pre.substr(0, str_suf.size() - start);
		if (pre_overlap.compare(suf_overlap) == 0) {
			return str_suf.size() - start;
		}
		++start;
	}
}

void traverse_path(const int &start, const int &current, 
				   const vector<pair<int, int> > &path_k, 
				   const unordered_set<int> &set_path, 
				   const int &distance,
				   const vector<Node*> &nodes,				   
				   vector<pair<int, vector<pair<int, int>>>> &all_paths) {
	
	if (!all_paths.empty()) {
		return;
	}
		
	if (nodes[current]->all_children.size() > 0) {
		int i = 1;
		int overlap_with_start;
		
		//check if children i is already in the set_path
		while (i <= nodes[current]->all_children.size() && 
			   set_path.find(nodes[current]->all_children[i-1].first->id) != set_path.end()) {
			i++;
		}
		
		//if all children are already in the set_path
		if (i > nodes[current]->all_children.size()) {
			if (set_path.size() == nodes.size()) {
				//find children of the last node in the path that is equal to the starting node
				auto it = std::find_if(nodes[current]->all_children.begin(), nodes[current]->all_children.end(), [&start](auto &element) {
					return element.first->id == start;
				});
				
				if (it != nodes[current]->all_children.end()) {
					int new_distance = distance + it->second;
					vector<pair<int, int>> new_path = path_k;
					new_path.emplace_back(make_pair(start, it->second));
					all_paths.emplace_back(make_pair(new_distance, new_path));
				}
			}
			return;
		}
		
		int local_biggest_overlap = nodes[current]->all_children[i - 1].second;
		
		int j_start = i - 1;
		//if the next biggest overlap is not only found in 1 children
		while (i < nodes[current]->all_children.size() && nodes[current]->all_children[i - 1].second == nodes[current]->all_children[i].second) {
			i++;
		}
		
		int j_end = i - 1;
		
		
		//consider all children with the overlap = local biggest overlap;
		for (int j = j_start; j <= j_end; ++j) {
			int curr_node_id = nodes[current]->all_children[j].first->id;
			// if id of the children isn't found;
			if (set_path.find(curr_node_id) == set_path.end()) {
				vector<pair<int,int>> new_path = path_k;
				unordered_set<int> new_set = set_path;
				int new_distance = distance + local_biggest_overlap;
				
				new_path.emplace_back(make_pair(curr_node_id, local_biggest_overlap));
				new_set.insert(curr_node_id);
				traverse_path(start, curr_node_id, new_path, new_set, 
							  new_distance, nodes, all_paths);
			}
		}
	}	
}

string traverse_path_single(const int &start,
				   unordered_set<int> &set_path, 
				   const vector<Node*> &nodes) {

	int current = start;
	int next = -1;
	string minscs = "";
	int noverlap = 0;
	
	bool path_blocked = false;
	
	//cout << "for start = " << start << "\n";
	while (next != start && !path_blocked) {		
		set_path.insert(current);
		minscs = minscs + nodes[current]->read.substr(noverlap);
		
		//cout << "current = " << current << ", minscs = " << minscs << "\n";
		
		if (nodes[current]->all_children.size() > 0) {			
			int k = 0;
			while (k < nodes[current]->all_children.size() &&
				   set_path.find(nodes[current]->all_children[k].first->id) != set_path.end()
				   ) {
				k++;
			}
			
			if (k < nodes[current]->all_children.size()) {
				next = nodes[current]->all_children[k].first->id;
				noverlap = nodes[current]->all_children[k].second;
				current = next;
				//cout << "  k = " << k << ", next_id = " << next << ", reads = " << nodes[current]->read << ", noverlap = " << noverlap << "\n";

			} else {
				
				auto it = std::find_if(nodes[current]->all_children.begin(), nodes[current]->all_children.end(), [&start](auto &element) {
					return element.first->id == start;
				});
				
				if (it != nodes[current]->all_children.end()) {
					next = start;
					minscs = minscs.substr(0, minscs.length() - it->second);
					
					//cout << "  no available child, but start is found\n";
					//cout << "  minscs = " << minscs << "\n";

				} else {
					path_blocked = true;					
				}

			}
		} else {
			path_blocked = true;
		}
	}
	
	if (set_path.size() != nodes.size() || path_blocked) {
		return "";
	}
	
	//cout << "  end of traverse, minscs = " << minscs << "\n";	
	return minscs;
}

vector<Node*> generate_readnodes(const vector<string> &reads, 
								 const int &n_overlap) {
	
	vector<Node*> readnodes;
	int k = 0;
	
	for (string r : reads) {
		readnodes.emplace_back(new Node(k++,r));
	}
		
	for (int i = 0; i < readnodes.size() - 1; ++i) {
		for (int j = i + 1; j < readnodes.size(); ++j) {
			int overlap_forw = find_overlap(reads[i], reads[j], n_overlap);
			int overlap_back = find_overlap(reads[j], reads[i], n_overlap);

			
			if (overlap_forw > 0) {
				readnodes[i]->all_children.emplace_back(make_pair(readnodes[j],overlap_forw));
			}
			
			if (overlap_back > 0) {
				readnodes[j]->all_children.emplace_back(make_pair(readnodes[i],overlap_back));
			}			
		}
	}
	
	for (int i = 0; i < readnodes.size(); ++i) {
		std::sort(readnodes[i]->all_children.begin(), readnodes[i]->all_children.end(), [](auto &left, auto &right){
			return left.second > right.second;
		});
		
		/*
		if (readnodes[i]->all_children.size() > 0) {
			int j = 1;
			while (j < readnodes[i]->all_children.size() && 
				   readnodes[i]->all_children[j-1].second == readnodes[i]->all_children[j].second) {
				readnodes[i]->big_children.emplace_back(readnodes[i]->all_children[j-1]);
			}
		}
		*/
	}	
	
	return readnodes;
}

pair<int, vector<pair<int, int>>> genome_error_free(const vector<string> &reads,
													const int &n_overlap) {	

	vector<Node*> readnodes = generate_readnodes(reads, n_overlap);
	
	vector<pair<int, vector<pair<int, int>>>> all_paths;
	
	int k = 0;
	while (all_paths.empty() && k < readnodes.size()) {
	//for (int k = 0; k < readnodes.size(); ++k) {
		unordered_set<int> set_path{k};
		int distance = 0;
		vector<pair<int, int>> path_k;
		traverse_path(k, k, path_k, set_path, distance, readnodes, all_paths);
		k++;
	}
		
	
	if (all_paths.size() > 0) {
		std::sort(all_paths.begin(), all_paths.end(), [](auto &left, auto &right){
				return left.first > right.first;
		});	
		return all_paths[0];
	}
	
	pair<int, vector<pair<int, int>>> result(make_pair(0,vector<pair<int,int>>(0)));
	
	return result;
	
}

string genome_error_free_single_traverse(const vector<string> &reads,
										 const int &n_overlap) {	

	vector<Node*> readnodes = generate_readnodes(reads, n_overlap);
		
	int k = 0;
	int minscs_length = reads.size()*reads[0].size();
	string minscs = "";
	
	while (k < readnodes.size()) {
		unordered_set<int> set_path{k};
		string possible_scs = traverse_path_single(k, set_path, readnodes);
		if (possible_scs.size() > 0 && minscs_length > possible_scs.size()) {
			minscs = possible_scs;
			minscs_length = possible_scs.size();
		}
		k++;
	}
	
	
	/*
	k = 0;
	while (k < readnodes.size() && minscs.size() > 0) {
		unordered_set<int> set_path{k};
		string possible_scs = traverse_path_single(k, set_path, readnodes);
		minscs = possible_scs;
		k++;
	}
	*/
	
	return minscs;
}

string assemble_genome(const vector<string> &reads, 
					   const vector<pair<int, int>> &path) {	
	string minscs = reads[path[path.size() - 1].first];
	int last_overlap = path[path.size() - 1].second;

	for (int k = 0; k < path.size() - 1; ++k) {
		int node_id = path[k].first;
		int noverlap = path[k].second;
			
		minscs = minscs + reads[node_id].substr(noverlap, reads[node_id].size() - noverlap);
	}
		
	minscs = minscs.substr(0, minscs.size() - last_overlap);
	
	return minscs;	
}

string find_genome_version1(const vector<string> &reads, 
							const int &min_overlap, 
							const int &max_overlap,
							const int &increment) {
	int maxdistance = 0;
	
	vector<pair<int, int>> maxpath;		
	
	for (int noverlap = min_overlap; noverlap <= max_overlap; noverlap += increment) {
		auto possible_path = genome_error_free(reads, noverlap);
		
		//cout << ", distance found = " << possible_path.first << "\n";
		
		if (possible_path.first > maxdistance) {
			maxdistance = possible_path.first;
			maxpath = possible_path.second;
		}
	}
	
	return assemble_genome(reads, maxpath);
}

string find_genome_version2(const vector<string> &reads, 
							const int &min_overlap, 
							const int &max_overlap,
							const int &increment) {
								
	int min_genome_length = reads[0].size()*reads.size();
	string min_genome_string;
	
	for (int noverlap = min_overlap; noverlap <= max_overlap; noverlap += increment) {
		string possible_genome = genome_error_free_single_traverse(reads, noverlap);
				
		if (min_genome_length > possible_genome.size()) {
			min_genome_string = possible_genome;
			min_genome_length = possible_genome.size();
		}
	}
	
	return min_genome_string;
}


int main() {
	int n = 1618;
	//cin >> n;
	int min_overlap = 12, max_overlap = 12, increment = 1;
	//min_overlap = 1; max_overlap = 1; increment = 1;
	//cin >> min_overlap >> max_overlap >> increment;
	
	std::unordered_set<string> set_reads;
	string read;	
	
	vector<string>sample1{"ACAGC","GCCCC","CAGCC","CAGTA","GCAAC","GGCAA","CAACA","AGTAG","GTAGG","AACAG","GGGCA","CCCCT","CCCTT","AGGGC","TAGGG","AGCCC"};
	//vector<string>sample1{"ABCD", "CDBC", "BCDA"};
	//cout << "sample1.size = " << sample1.size() << "\n";
	
	
	for (int i = 0; i < n; ++i) {
		cin >> read;
		set_reads.insert(read);
	}
	

	vector<string> reads(set_reads.begin(), set_reads.end());
	//string genome = find_genome_version1(sample1,min_overlap, max_overlap, increment);
	//string genome = find_genome_version2(sample1,min_overlap, max_overlap, increment);
	
	string genome = find_genome_version2(reads,min_overlap, max_overlap, increment);

	cout << genome << "\n";
	//cout << genome.size() << "\n";
	return 0;
}