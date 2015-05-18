#ifndef _MINISCHEME_SLIST_H
#define _MINISCHEME_SLIST_H

namespace MiniScheme{

	template<typename T>
	class _list_node{
	public:
		T* ptr_to_data;
		_list_node<T>* next;
		_list_node() :next(nullptr), ptr_to_data(nullptr){}
		_list_node(T* ptr) :ptr_to_data(ptr), next(nullptr){}
		~_list_node(){
			delete ptr_to_data;
		}
	};

	template<class T>
	class slist{
	public:
		typedef _list_node<T> node;
		typedef _list_node<T>* node_ptr;
		typedef slist<T> self;

		bool empty(){ return head == nullptr; }

		void pop_front(){
			if (empty ()){
				std::cerr << "current slist is empty" << std::endl;
				return *this;
			}
			
			node_ptr cur = head;
			head = head->next;
			destroy_node(cur);
			return *this;
		}

		self push_front(T* ptr){
			node_ptr cur = new node();
			cur->ptr_to_data = ptr;
			cur->next = head;
			return cur;
		}

		T* front(){ return head->data; }
		
	public:
		node_ptr head;
		slist() :head(nullptr){};

		self& operator=(const self& others){
			this->head = others.head;
			return *this;
		}

		slist(const self& others){
			this->head = others->head;
		}

		slist(node_ptr cur_node) :head(cur_node){}

		void destroy_node(node_ptr ptr){
			if (ptr){ delete ptr; }
		 }

		node_ptr construct_node(T* ptr){ return new node(ptr); }

		void clearLocal(){
			destroy_node(head);
		}
		~slist(){}
	};
}


#endif	//_MINISCHEME_SLIST_H