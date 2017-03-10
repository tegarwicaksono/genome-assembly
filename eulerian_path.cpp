#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::sort;
using std::unordered_map;

class Vertex{
	private:
		int index;
		vector<Vertex> out;
		vector<Vertex> in;

	public:
		Vertex(const int &index) : index(index) {}

		void add_outgoing_neighbour(const Vertex &to) {
			out.emplace_back(to);
		}

		void add_incoming_neighbour(const Vertex &from) {
			in.emplace_back(from);
		}

		int get_number_of_outgoing_neighbours() {
			return out.size();
		}

		const int id() const {
			return this->index;
		}

		int get_number_of_incoming_neighbours() {
			return in.size();
		}

		vector<Vertex> get_outgoing_neighbours() {
			vector<Vertex> results{out};
			return results;
		}

		vector<Vertex> get_incoming_neighbours() {
			vector<Vertex> results{in};
			return results;
		}

		bool in_equal_out() {
			return out.size() == in.size();
		}

		bool operator==(const Vertex &v) const {
			return ( this->index == v.id());
		}
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(const Vertex& v) const
        {
            return ((hash<int>()(v.id())));
        }
    };
}


class Graph{
	private:
		int n_vertices, n_edges;
		vector<Vertex> vertices;

	public:
		Graph(const int &n_vertices, const int &n_edges) : 
			n_vertices(n_vertices), n_edges(n_edges) {

			for (int i = 0; i < n_vertices; ++i) {
				vertices.emplace_back(Vertex(i));
			}
		}

		void add_edge(Vertex &from, Vertex &to) {
			from.add_outgoing_neighbour(to);
			to.add_incoming_neighbour(from);
		}

		void add_edges() {
			for (int i = 0; i < n_edges; ++i) {
				int id_in, id_out;
				cin >> id_in >> id_out;

				add_edge(vertices[--id_in], vertices[--id_out]);
			}	
		}

		bool eulerian_cycle_exists() {
			for (Vertex v : vertices) {

				if (v.get_number_of_outgoing_neighbours() == 0 &&
					v.get_number_of_incoming_neighbours() == 0) {
					return false;
				}
				if (!v.in_equal_out()) {
					return false;
				}
			}
			return true;
		}

		void print_outgoing_edges(Vertex v) {
			vector<Vertex> out = v.get_outgoing_neighbours();
			cout << v.id() << " outgoing: ";

			for (Vertex nn : out) {
				cout << " " << nn.id() << " ";
			}
			cout << "\n";
		}

		void print_incoming_edges(Vertex v) {
			vector<Vertex> out = v.get_incoming_neighbours();
			cout << v.id() << " incoming: ";

			for (Vertex nn : out) {
				cout << " " << nn.id() << " ";
			}		
			cout << "\n";
		}

		void print_edges() {
			for (Vertex v : vertices) {
				print_outgoing_edges(v);
				print_incoming_edges(v);
			}
		}

		vector<Vertex> find_eulerian_cycle() {
			vector<Vertex> empty;

			if (eulerian_cycle_exists()) {
				vector<vector<Vertex>> neighbours(n_vertices);

				for (int i = 0; i < n_vertices; ++i) {
					neighbours[i] = vertices[i].get_outgoing_neighbours();

					std::sort(neighbours[i].begin(), neighbours[i].end(), [](auto &left, auto &right){
						return left.get_number_of_outgoing_neighbours() > right.get_number_of_outgoing_neighbours();
					});
				}

				vector<Vertex> cycle;

				auto max_it = std::max_element(vertices.begin(), vertices.end(), [](auto &left, auto &right) {
					return left.get_number_of_outgoing_neighbours() < right.get_number_of_outgoing_neighbours();
				});

				Vertex start = *max_it;
				Vertex next = start, prev = start;
				int count_edge = 0;
				bool expand_possible = true;

				while (expand_possible) {
					bool cycle_found = false;
					vector<Vertex> current_cycle;
					while (!cycle_found) {
						auto cand = neighbours[prev.id()].begin();
						while (neighbours[prev.id()].size() > 1 &&
							   neighbours[cand->id()].size() == 0) {
							++cand;
						}

						next = *cand;
						neighbours[prev.id()].erase(cand);
						prev = next;
						current_cycle.emplace_back(prev);
						++count_edge;


						if (prev.id() == start.id()) {
							cycle_found = true;
						}
					}


					//find a way to expand the cycle
					bool at_least_one_vertex_unexplored = false;
					auto itr = current_cycle.begin();

					while (itr != current_cycle.end() &&
						  !at_least_one_vertex_unexplored) {

						if (neighbours[itr->id()].size() > 0) {
							at_least_one_vertex_unexplored = true;
							start = *itr;
							prev  = start;
						} else {
							++itr;
						}
					}

					//prepare for expanding;

					if (at_least_one_vertex_unexplored) {				
						std::rotate(current_cycle.begin(), itr + 1, current_cycle.end());
					} else {
						expand_possible = false;
					}

					cycle.reserve(cycle.size() + current_cycle.size());
					cycle.insert(cycle.end(), current_cycle.begin(), current_cycle.end());
			
				}

				if (count_edge == n_edges) {
					return cycle;
				} 

				return empty;					
			}
			return empty;
		}

		void print_eulerian_steps(vector<Vertex> cycle) {
			for (Vertex v : cycle) {
				cout << v.id() + 1 << " ";
			}
			cout << "\n";
		}

		void print_eulerian_cycle() {
			vector<Vertex> cycle = find_eulerian_cycle();
			if (cycle.size() > 0) {
				cout << "1\n";
				print_eulerian_steps(cycle);
			} else {
				cout << "0\n";
			}
		}

};


int main() {
	int n_vertices, n_edges;
	cin >> n_vertices >> n_edges;

	Graph graph(n_vertices, n_edges);
	graph.add_edges();
	//graph.print_edges();
	graph.print_eulerian_cycle();

}