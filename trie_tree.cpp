#include <iostream>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>

using namespace std;

template<class _Kty, class _IKty, class _Ty>
class TrieTree;

template <class node_type>
class IteratorTrieTree
{

public:
	using key_type = typename node_type::key_type;
	using internal_key_type = typename node_type::internal_key_type;
	using mapped_type = typename node_type::mapped_type;
	using InternalNodesType = typename node_type::InternalNodesType;

protected:
	node_type* _ptr{ nullptr };
	node_type* _last_ptr{ nullptr };
	node_type* _not_valid{ nullptr };
	deque<pair<node_type*, key_type>> _buffer_branches;
	key_type last_key;

public:
	IteratorTrieTree(node_type* _ptr);
	IteratorTrieTree(node_type* _ptr, node_type* _not_valid);
	pair<key_type, mapped_type> operator*();
	IteratorTrieTree& operator++();
	IteratorTrieTree operator++(int);

	bool operator==(const IteratorTrieTree< node_type>& r)const;
	bool operator!=(const IteratorTrieTree< node_type>& r)const;


};

template<class node_type>
IteratorTrieTree<node_type>::IteratorTrieTree(node_type* _ptr) :_ptr(_ptr) {}

template<class node_type>
IteratorTrieTree<node_type>::IteratorTrieTree(node_type* type, node_type* _not_valid) : _ptr(type), _not_valid(_not_valid) {}

template<class node_type>
pair<typename IteratorTrieTree<node_type>::key_type, typename IteratorTrieTree<node_type>::mapped_type> IteratorTrieTree<node_type>::operator*()
{
	if (_ptr->isRoot)
		++(*this);
	return { last_key, _ptr->data };
}

template<class node_type>
IteratorTrieTree<node_type>& IteratorTrieTree<node_type>::operator++()
{
	while (true)
	{
		if (!_ptr->nodes.empty())
		{
			for (auto iter = _ptr->nodes.rbegin(); iter != prev(_ptr->nodes.rend(), 1); ++iter)
			{
				_buffer_branches.push_back({ &iter->second, last_key + iter->first }); // реализовать в ключе +
			}
			last_key += _ptr->nodes.begin()->first; // реализовать в ключе +=
			_ptr = &_ptr->nodes.begin()->second;
		}
		else
		{
			if (_buffer_branches.empty())
			{
				_ptr = _not_valid;
				break;
			}
			auto& elem = _buffer_branches.back();
			_ptr = elem.first;
			last_key = elem.second;
			_buffer_branches.pop_back();
		}

		if (_ptr->isNode)
		{
			_last_ptr = _ptr;
			break;
		}
	}

	return *this;
}

template<class node_type>
IteratorTrieTree<node_type> IteratorTrieTree<node_type>::operator++(int)
{
	IteratorTrieTree<node_type> result = *this;
	++(*this);
	return result;
}

template<class node_type>
bool IteratorTrieTree<node_type>::operator==(const IteratorTrieTree< node_type>& r) const
{
	return !(_ptr != r._ptr);
}

template<class node_type>
bool IteratorTrieTree<node_type>::operator!=(const IteratorTrieTree< node_type>& r) const
{
	return _ptr->isNotValid != r._ptr->isNotValid;
}

template<class _Kty, class _IKty, class _Ty>
struct TrieNode
{
	template <class node_type>
	friend class IteratorTrieTree;

	template<class _Kty, class _IKty, class _Ty> 
	friend class TrieTree;

private: 
	bool isNotValid{ false }; // true, если не валидный узел
	bool isNode{ false }; // true, если это окончание ключа с данными
	bool isRoot{ false }; // true, если это корень

public:
	using key_type = _Kty;
	using internal_key_type = _IKty;
	using mapped_type = _Ty;
	using InternalNodesType = map< internal_key_type, TrieNode>;

	mapped_type data;
	InternalNodesType nodes;

};

template<class _Kty, class _IKty, class _Ty>
class TrieTree
{

public:
	using key_type = _Kty;
	using internal_key_type = _IKty;
	using mapped_type = _Ty;

private:
	TrieNode<key_type, internal_key_type, mapped_type> root;
	TrieNode<key_type, internal_key_type, mapped_type> not_valid;

public:
	using iterator = IteratorTrieTree<TrieNode<key_type, internal_key_type, mapped_type>>;
	using reverse_iterator = IteratorTrieTree<TrieNode<key_type, internal_key_type, mapped_type>>;

	TrieTree()
	{
		root.isRoot = true;
		not_valid.isNotValid = true;
	}
	void insert(const pair<key_type, mapped_type>& pair)
	{
		const key_type& key = pair.first;
		const mapped_type& value = pair.second;
		TrieNode<key_type, internal_key_type, mapped_type>* tmp = &root;
		auto _begin_key = std::begin(key);
		auto _end_key = std::end(key);

		while (true)
		{
			if (_begin_key == _end_key)
			{
				tmp->data = value;
				tmp->isNode = true;
				break;
			}
			else
			{
				bool isFind{ false };
				auto iter = tmp->nodes.find(*_begin_key);
				if (iter == tmp->nodes.end())
				{
					TrieNode<key_type, internal_key_type, mapped_type> node;
					node.data = value;
					iter = tmp->nodes.insert({ *_begin_key , node }).first;
				}
				tmp = &iter->second;
				++_begin_key;
			}
		}
	}

	pair<mapped_type, bool> find(const _Kty& key)
	{
		TrieNode<key_type, internal_key_type, mapped_type>* tmp = &root;
		auto _begin_key = std::begin(key);
		auto _end_key = std::end(key);

		while (true)
		{
			if (_begin_key == _end_key)
			{
				if(tmp->isNode)
					return { tmp->data , true };
				else
					return { mapped_type() , false };
			}
			else
			{
				bool isFind{ false };
				auto iter = tmp->nodes.find(*_begin_key);
				if (iter == tmp->nodes.end())
				{
					return { mapped_type() , false };
				}
				tmp = &iter->second;
				++_begin_key;
			}
		}
	}

	bool erase(const _Kty& key)
	{
		TrieNode<key_type, internal_key_type, mapped_type>* tmp = &root;
		auto _begin_key = std::begin(key);
		auto _end_key = std::end(key);

		while (true)
		{
			if (_begin_key == _end_key)
			{
				if (tmp->isNode)
				{
					tmp->isNode = false;
					return true;
				}
				else
					return false;
			}
			else
			{
				bool isFind{ false };
				auto iter = tmp->nodes.find(*_begin_key);
				if (iter == tmp->nodes.end())
				{
					return false;
				}
				tmp = &iter->second;
				++_begin_key;
			}
		}
	}

	iterator begin()
	{
		return iterator(&root, &not_valid);
	}

	iterator end()
	{
		return iterator(&not_valid);
	}


};

template <class Container>
class CustomInserterIterator
{
public: // usings
	using value_type = void;
	using difference_type = std::ptrdiff_t;
	using reference = void;
	using pointer = void;
	using iterator_category = std::output_iterator_tag;
	using container_type = Container;

public: // methods
	CustomInserterIterator() :_ptr(nullptr)
	{

	}
	CustomInserterIterator(container_type& vtp) :_ptr(std::addressof(vtp))
	{

	}
	template <class T>
	CustomInserterIterator& operator=(const T& value)
	{
		_ptr->insert(value);
		return *this;
	}
	template <class T>
	CustomInserterIterator& operator=(T&& value)
	{
		_ptr->insert(std::move(value));
		return *this;
	}
	CustomInserterIterator& operator* ()
	{
		return *this;
	}
	CustomInserterIterator& operator++()
	{
		return *this;
	}
	CustomInserterIterator& operator++(int)
	{
		return *this;
	}

private: // members
	container_type* _ptr;
};

template< class Container >
CustomInserterIterator<Container> CustomInserter(Container& c)
{
	return CustomInserterIterator<Container>(c);
}


int main()
{
	std::string alphabet = "bcdfghjklmnpqrstvwxyz";
	TrieTree<string, char, int> tree;

	vector<pair<string, int>> keys_datas = { {"c", 1}, {"cap", 2}, {"car", 3}, {"cdr", 4}, {"go", 5}, {"if", 6}, {"is", 7}, {"it", 8}, {"at", 9}, {"io", 10}};

	std::copy(keys_datas.cbegin(), keys_datas.cend(), CustomInserter(tree));

	for (int i = 0; i < keys_datas.size(); ++i)
		cout << boolalpha << keys_datas[i].first << " find is " << tree.find(keys_datas[i].first).second << endl;
	cout << endl;

	for (TrieTree<string, char, int>::iterator iter = tree.begin(); iter != tree.end(); ++iter)
	{
		for (int i = 0; i < alphabet.size(); ++i)
		{
			std::string key = (*iter).first;
			if (key.find_last_of(alphabet[i]) == key.length() - 1)
			{
				cout << key << endl;
				break;
			}
		}
	}
	cout << endl;

	for (int i = 0; i < keys_datas.size(); i += 2)
		cout << boolalpha << keys_datas[i].first << " erase is " << tree.erase(keys_datas[i].first) << endl;
	cout << endl;

	for (int i = 0; i < keys_datas.size(); ++i)
		cout << boolalpha << keys_datas[i].first << " find is " << tree.find(keys_datas[i].first).second << endl;
	cout << endl;
	
}

