#pragma once
#include <memory>
#include <cassert>
#include <functional>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iostream>

namespace ds
{
	namespace bin_tree
	{
		enum class node_type { k_left_child, k_right_child, k_root };

		template<typename T>
		class node
		{
		public:
			explicit node(T data, const node_type type) : type(type), ancestor(nullptr), left_child(nullptr),
				right_child(nullptr), height(1), data(data)
			{
			}

			bool is_leaf() { return left_child == nullptr && right_child == nullptr; }
			void copy_data_to(node<T> *other) { other->data = this->data; }
			static void remove(node<T> *&n);

			node_type type;
			node<T> *ancestor;
			node<T> *left_child;
			node<T> *right_child;
			long long height;
			T data;
		};

		template <typename T>
		void node<T>::remove(node<T> *&n)
		{
			n->left_child = n->right_child = nullptr;
			auto &ancestor = n->ancestor;
			switch (n->type)
			{
			case node_type::k_left_child:
				ancestor->left_child = nullptr;
				break;

			case node_type::k_right_child:
				ancestor->right_child = nullptr;
				break;

			case node_type::k_root:
				break;

			default:
				// Unhandled node type
				assert(false);
			}

			delete n;
			n = nullptr;
		}
	}
}

namespace ds
{
	namespace bin_tree
	{
		template<class T>
		struct node_info
		{
			T *key;
			node<T> *ancestor;
			node_type type;
		};

		template<class T>
		class binary_tree
		{
			using big_int = unsigned long long;

		public:
			explicit binary_tree() : root(nullptr) {}

			// Traversal methods with visit_action as a lambda
			void preorder(std::function<void(node<T> *n)> visit_action = [](node<T> *n)->void {std::cout << n->data << '\n'; });
			void postorder(std::function<void(node<T> *n)> visit_action = [](node<T> *n)->void {std::cout << n->data << '\n'; });
			void inorder(std::function<void(node<T> *n)> visit_action = [](node<T> *n)->void {std::cout << n->data << '\n'; });

			// C++ doesn't allow a member to be used as the default value (for node as root)
			// Thus, we need expose traversal methods as wrappers of these functions
			void preorder(node<T> *n, std::function<void(node<T> *)> &&visit_action);
			void postorder(node<T> *n, std::function<void(node<T> *)> &&visit_action);
			void inorder(node<T> *n, std::function<void(node<T> *)> &&visit_action);
			long long height();
			static long long height(const node<T> *n);

		protected:
			// A utility function to get the height of the node
			static long long get_height(node<T> *n) { return n == nullptr ? 0 : n->height; }
			void replace(node<T> *&current, node<T> *other);

		public:
			node<T> *root;
		};

		template <class T>
		void binary_tree<T>::preorder(std::function<void(node<T> *n)> visit_action)
		{
			preorder(root, std::move(visit_action));
		}

		template <class T>
		void binary_tree<T>::postorder(std::function<void(node<T> *n)> visit_action)
		{
			postorder(root, std::move(visit_action));
		}

		template <class T>
		void binary_tree<T>::inorder(std::function<void(node<T> *n)> visit_action)
		{
			inorder(root, std::move(visit_action));
		}

		template <class T>
		void binary_tree<T>::preorder(node<T> *n, std::function<void(node<T> *)>&& visit_action)
		{
			if (n == nullptr)
			{
				return;
			}

			visit_action(n);
			preorder(n->left_child, std::move(visit_action));
			preorder(n->right_child, std::move(visit_action));
		}

		template <class T>
		void binary_tree<T>::postorder(node<T> *n, std::function<void(node<T> *)>&& visit_action)
		{
			if (n == nullptr)
			{
				return;
			}

			postorder(n->left_child, std::move(visit_action));
			postorder(n->right_child, std::move(visit_action));
			visit_action(n);
		}

		template <class T>
		void binary_tree<T>::inorder(node<T> *n, std::function<void(node<T> *)>&& visit_action)
		{
			if (n == nullptr)
			{
				return;
			}

			inorder(n->left_child, std::move(visit_action));
			visit_action(n);
			inorder(n->right_child, std::move(visit_action));
		}

		template <class T>
		long long binary_tree<T>::height()
		{
			return binary_tree<T>::height(this->root);
		}

		template <class T>
		long long binary_tree<T>::height(const node<T> *n)
		{
			if (n == nullptr)
			{
				return 0;
			}

			auto left_height = binary_tree<T>::height(n->left_child);
			auto right_height = binary_tree<T>::height(n->right_child);

			return std::max(left_height, right_height) + 1;
		}

		// Replaces 'current' node with 'other'
		// This doesn't handle childrens' links which is the responsibility of the caller
		template <typename T>
		void binary_tree<T>::replace(node<T> *&current, node<T> *other)
		{
			auto ancestor = current->ancestor;
			switch (current->type)
			{
			case node_type::k_right_child:
				ancestor->right_child = other;
				break;

			case node_type::k_left_child:
				ancestor->left_child = other;
				break;

			case node_type::k_root:
				root = other;
				break;

			default:
				// Not handled for this type
				assert(false);
			}

			other->ancestor = ancestor;
			other->type = current->type;

			delete current;
			current = other;
		}
	}
}

namespace ds
{
	namespace bin_tree
	{
		// Represents start and end of a segment
		struct range
		{
			std::size_t lower_bound;
			std::size_t upper_bound;
		};

		// Response of a query call
		template<class T>
		struct response
		{
			bool is_valid;
			T data;
		};

		template<class C, class T, class U>
		class seg_tree : public binary_tree<T>
		{
		public:
			// container is a void pointer to the container
			// access_data - function pointer which returns the data from the container for the given index
			// merge_nodes - function pointer which performs the operation on 2 data points (min, max, sum etc.) and returns the result
			explicit seg_tree(const C& container, std::size_t size,
				T(*access_data)(const C&, std::size_t),
				T(*merge_nodes)(T, T),
				T(*update_data)(const range& segment, const T& node_data, const U& data));

			// Returns the result of the operation on the specified segment
			response<T> query(const range& query_segment);

			// Updates the values in the specified segment
			void update_range(const range& update_segment, const U& data);

			C container;
			std::size_t size;
			T(*merge_nodes)(T, T);
			T(*access_data)(const C&, std::size_t);
			T(*update_data)(const range& segment, const T& node_data, const U& data);

		private:
			node<T>* build_tree(node_type type, const range& segment) const;
			response<T> query(node<T>* n, const range& segment, const range& query_segment);
			void update_range(node<T>* n, const range& segment, const range& update_segment, const U& data);
			void clear_laziness(const range& segment, node<T>* n);
			void propagate_laziness(const range& segment, node<T>* n, const U& data);

			// Stores the data which will needs to get updated in the next query/update in each node
			std::unordered_map<node<T>*, U> lazy_store_;
		};

		template<class C, class T, class U>
		seg_tree<C, T, U>::seg_tree(const C& container, const std::size_t size,
			T(*access_data)(const C&, const std::size_t),
			T(*merge_nodes)(T, T),
			T(*update_data)(const range& segment, const T& node_data, const U& data)) :
			container(container), size(size), merge_nodes(merge_nodes), access_data(access_data), update_data(update_data)
		{
			const range segment{ 0, size - 1 };
			this->root = build_tree(node_type::k_root, segment);
		}

		template<class C, class T, class U>
		response<T> seg_tree<C, T, U>::query(const range& query_segment)
		{
			const range segment{ 0, size - 1 };
			return query(this->root, segment, query_segment);
		}

		template<class C, class T, class U>
		void seg_tree<C, T, U>::update_range(const range& update_segment, const U& data)
		{
			const range segment{ 0, size - 1 };
			return update_range(this->root, segment, update_segment, data);
		}

		template<class C, class T, class U>
		node<T>* seg_tree<C, T, U>::build_tree(const node_type type, const range& segment) const
		{
			// Leaf node
			if (segment.lower_bound == segment.upper_bound)
			{
				// Store the c[i] value in the leaf node
				return new node<T>(access_data(container, segment.lower_bound), type);
			}

			range new_segment;

			// Recurse left
			new_segment.lower_bound = segment.lower_bound;
			new_segment.upper_bound = (segment.lower_bound + segment.upper_bound) >> 1;
			const auto left_child = build_tree(node_type::k_left_child, new_segment);

			// Recurse right
			new_segment.lower_bound = new_segment.upper_bound + 1;
			new_segment.upper_bound = segment.upper_bound;
			const auto right_child = build_tree(node_type::k_right_child, new_segment);

			// Perform operation on the 2 nodes and store its result in the parent node
			const auto new_node = new node<T>(merge_nodes(left_child->data, right_child->data), type);
			new_node->left_child = left_child;
			new_node->right_child = right_child;
			left_child->ancestor = right_child->ancestor = new_node;
			return new_node;
		}


		template<class C, class T, class U>
		response<T> seg_tree<C, T, U>::query(node<T>* n, const range& segment, const range& query_segment)
		{
			// Outside query range
			if (query_segment.lower_bound > segment.upper_bound || query_segment.upper_bound < segment.lower_bound)
			{
				return response<T>{ false };
			}

			clear_laziness(segment, n);

			// Completely within the query range
			if (segment.lower_bound >= query_segment.lower_bound && segment.upper_bound <= query_segment.upper_bound)
			{
				return response<T>{ true, n->data };
			}

			range new_segment;

			new_segment.lower_bound = segment.lower_bound;
			new_segment.upper_bound = (segment.lower_bound + segment.upper_bound) >> 1;
			const auto left_response = query(n->left_child, new_segment, query_segment);

			new_segment.lower_bound = new_segment.upper_bound + 1;
			new_segment.upper_bound = segment.upper_bound;
			const auto right_response = query(n->right_child, new_segment, query_segment);

			if (!left_response.is_valid)
			{
				return right_response;
			}

			if (!right_response.is_valid)
			{
				return left_response;
			}

			return response<T>{ true, merge_nodes(left_response.data, right_response.data) };
		}

		template<class C, class T, class U>
		void seg_tree<C, T, U>::update_range(node<T>* n, const range& segment, const range& update_segment, const U& data)
		{
			clear_laziness(segment, n);

			// Completely outside query range
			if (update_segment.lower_bound > segment.upper_bound || update_segment.upper_bound < segment.lower_bound)
			{
				return;
			}

			// Completely within query range
			if (segment.lower_bound >= update_segment.lower_bound && segment.upper_bound <= update_segment.upper_bound)
			{
				n->data = update_data(segment, n->data, data);

				range new_segment;
				new_segment.lower_bound = segment.lower_bound;
				new_segment.upper_bound = (segment.lower_bound + segment.upper_bound) >> 1;
				propagate_laziness(new_segment, n->left_child, data);

				new_segment.lower_bound = new_segment.upper_bound + 1;
				new_segment.upper_bound = segment.upper_bound;
				propagate_laziness(new_segment, n->right_child, data);
				return;
			}

			range new_segment;
			new_segment.lower_bound = segment.lower_bound;
			new_segment.upper_bound = (segment.lower_bound + segment.upper_bound) >> 1;
			update_range(n->left_child, new_segment, update_segment, data);

			new_segment.lower_bound = new_segment.upper_bound + 1;
			new_segment.upper_bound = segment.upper_bound;
			update_range(n->right_child, new_segment, update_segment, data);

			n->data = merge_nodes(n->left_child->data, n->right_child->data);
		}

		template <class C, class T, class U>
		void seg_tree<C, T, U>::clear_laziness(const range& segment, node<T>* n)
		{
			auto find = lazy_store_.find(n);
			if (find != lazy_store_.end())
			{
				range new_segment;
				new_segment.lower_bound = segment.lower_bound;
				new_segment.upper_bound = (segment.lower_bound + segment.upper_bound) >> 1;
				propagate_laziness(new_segment, n->left_child, find->second);

				new_segment.lower_bound = new_segment.upper_bound + 1;
				new_segment.upper_bound = segment.upper_bound;
				propagate_laziness(new_segment, n->right_child, find->second);

				n->data = update_data(segment, n->data, find->second);
				lazy_store_.erase(n);
			}
		}

		// Induces/updates the laziness associated with the current node to its children
		template<class C, class T, class U>
		void seg_tree<C, T, U>::propagate_laziness(const range& segment, node<T>* n, const U& data)
		{
			if (n == nullptr)
			{
				return;
			}

			auto find = lazy_store_.find(n);
			if (find == lazy_store_.end())
			{
				lazy_store_[n] = data;
				return;
			}

			const auto parent_lazy_data = find->second;
			n->data = update_data(segment, n->data, find->second);
			lazy_store_[n] = data;

			if (n->left_child == nullptr || n->right_child == nullptr)
			{
				return;
			}

			range new_segment;
			new_segment.lower_bound = segment.lower_bound;
			new_segment.upper_bound = (segment.lower_bound + segment.upper_bound) >> 1;
			propagate_laziness(new_segment, n->left_child, parent_lazy_data);

			new_segment.lower_bound = new_segment.upper_bound + 1;
			new_segment.upper_bound = segment.upper_bound;
			propagate_laziness(new_segment, n->right_child, parent_lazy_data);
		}
	}
}

template<class T>
T merge_nodes(T d1, T d2)
{
	return d1 + d2;
}

template<class C, class T>
T access_data(const C& container, const std::size_t index)
{
	return container[index] == '1' ? 1 : 0;
}

template<class T, class U>
T update_data(const ds::bin_tree::range& segment, const T& node_data, const U& data)
{
	switch (data)
	{
	case 'F':
		return segment.upper_bound - segment.lower_bound + 1;

	case 'E':
		return 0;

	case 'I':
		return segment.upper_bound - segment.lower_bound + 1 - node_data;

	default:
		assert(false);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	std::string pirates = "", pat = "";
	std::size_t t, n, m, a, b, q;
	char q_type;
	std::cin >> t;
	for (std::size_t c = 0; c < t; ++c)
	{
		pirates = pat = "";
		std::cin >> m;
		while (m--)
		{
			std::cin >> n;
			std::cin >> pat;
			while (n--)
			{
				pirates += pat;
			}
		}

		ds::bin_tree::seg_tree<std::string, std::size_t, char> seg_tree(pirates, pirates.length(),
			access_data<std::string, std::size_t>,
			merge_nodes<std::size_t>,
			update_data<std::size_t, char>);

		std::cout << "Case " << c + 1 << ":" << std::endl;
		std::cin >> q;
		std::size_t i = 0;
		ds::bin_tree::range segment;
		while (q--)
		{
			std::cin >> q_type >> a >> b;
			segment.lower_bound = a;
			segment.upper_bound = b;

			if (q_type == 'S')
			{
				std::cout << "Q" << ++i << ": " << seg_tree.query(segment).data << std::endl;
			}
			else
			{
				seg_tree.update_range(segment, q_type);
			}
		}
	}

	return 0;
}
