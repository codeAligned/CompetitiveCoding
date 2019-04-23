// WA

#include <vector>
#include <queue>
#include <iostream>

class BipartiteCheck
{
public:
	BipartiteCheck(int num_nodes, const std::vector<std::pair<int, int>>& edge_list);

	int CountGuards() const;

private:
	int CountGuards(int start_node, std::vector<int>& node_colour) const;

	const int num_nodes_;
	std::vector<std::vector<int>> adj_list_;
};

BipartiteCheck::BipartiteCheck(const int num_nodes,
	const std::vector<std::pair<int, int>>& edge_list) :num_nodes_(num_nodes)
{
	adj_list_.resize(num_nodes_);
	for (const auto& edge : edge_list)
	{
		adj_list_[edge.first].emplace_back(edge.second);
	}
}

int BipartiteCheck::CountGuards() const
{
	std::vector<int> node_colour(num_nodes_, -1);
	auto total_count = 0;

	for (auto i = 0; i < num_nodes_; ++i)
	{
		if (node_colour[i] != -1)
		{
			continue;
		}

		const auto count = CountGuards(i, node_colour);
		if (count == -1)
		{
			return -1;
		}
		total_count += count;
	}
	return total_count;
}

int BipartiteCheck::CountGuards(const int start_node,
	std::vector<int>& node_colour) const
{
	std::queue<int> order;
	order.push(start_node);
	node_colour[start_node] = 0;
	auto num_black = 0;

	while (!order.empty())
	{
		const auto node = order.front();
		order.pop();
		num_black = num_black + (node_colour[node] == 0 ? 1 : 0);

		for (const auto adj_node : adj_list_[node])
		{
			if (node_colour[adj_node] == -1)
			{
				node_colour[adj_node] = 1 - node_colour[node];
				order.push(adj_node);
			}
			else if (node_colour[node] == node_colour[adj_node])
			{
				return -1;
			}
		}
	}
	return num_black;
}

int main(int argc, char* argv[])
{
	int t;
	int n;
	int m;
	std::vector<std::pair<int, int>> edge_list;

	std::cin >> t;
	while (t--)
	{
		std::cin >> n >> m;
		edge_list.resize(m);
		for (auto i = 0; i < m; ++i)
		{
			std::cin >> edge_list[i].first >> edge_list[i].second;
		}

		BipartiteCheck bipartite(n, edge_list);
		std::cout << bipartite.CountGuards() << std::endl;
	}
	return 0;
}
