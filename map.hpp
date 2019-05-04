#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP
// only for std::less<T>
#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"
namespace sjtu {
	__inline int max(int x,int y){return x>y?x:y;}
	template<
			class Key,
			class T,
			class Compare = std::less<Key>
	> class map {
	public:typedef pair<const Key, T> value_type;
	private:
		size_t Size;
		Compare comp=Compare();
		bool equal(const Key &A,const Key &B) const{
			return !(comp(A,B)||comp(B,A));
		}
		struct node{
			node *left,*right,*next,*pre;
			value_type *data;
			int depth;
			node(const value_type *d=nullptr,int D=1):depth(D){
				left=right=next=pre=nullptr;
				if(d!=nullptr){
					data=(value_type*)malloc(sizeof(value_type));
					new(data) value_type(*d);
				}
				else{
					data=nullptr;
				}
			}
			~node(){
				if(data!=nullptr){
					data->first.~Key();data->second.~T();
					free(data);
					data=nullptr;
				}
			}
		};
		inline int Depth(node *p) const{
			if(p==nullptr) return 0;
			else return p->depth;
		}
		inline int Depth(node *p){
			if(p==nullptr) return 0;
			else return p->depth;
		}
		node *root;
		node *head;
		node *tail;
	public:
		class const_iterator;
		class iterator;
		class iterator{
			friend const_iterator;
		private:
			map* M;
			node *p;
		public:
			iterator(map* Map=nullptr,node * P=nullptr):M(Map),p(P){}
			iterator(const iterator &other):M(other.M),p(other.p){}
			iterator operator++(int) {
				if(p==M->tail) throw invalid_iterator();
				iterator tmp(*this);
				++(*this);
				return tmp;
			}
			iterator & operator++() {
				if(p==M->tail) throw invalid_iterator();
				p=p->next;
				return *this;
			}
			iterator operator--(int) {
				if(p==M->head->next) throw invalid_iterator();
				iterator tmp(*this);
				--(*this);
				return tmp;
			}
			iterator & operator--(){
				if(p==M->head->next) throw invalid_iterator();
				p=p->pre;
				return *this;
			}
			value_type & operator*() const {
				return *(p->data);
			}
			bool operator==(const iterator &rhs) const {
				return M==rhs.M&&p==rhs.p;
			}
			bool operator==(const const_iterator &rhs) const {
				return M==rhs.M&&p==rhs.p;
			}
			bool operator!=(const iterator &rhs) const {
				return !(*this==rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this==rhs);
			}
			value_type* operator->() const noexcept {
				return (p->data);
			}
			map* pointer(){return M;}
		};
		class const_iterator {
			friend iterator;
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		private:
			const map*  M;
			const node *p;
		public:
			const_iterator(const map *Map=nullptr,node *P=nullptr):M(Map),p(P){}
			const_iterator(const const_iterator &other):M(other.M),p(other.p){}
			const_iterator(const iterator &other):M(other.M),p(other.p){}
			const_iterator operator++(int) {
				if(p==M->tail) throw invalid_iterator();
				const_iterator tmp(*this);
				++(*this);
				return tmp;
			}
			const_iterator & operator++() {
				if(p==M->tail) throw invalid_iterator();
				p=p->next;
				return *this;
			}
			const_iterator operator--(int) {
				if(p==M->head->next) throw invalid_iterator();
				const_iterator tmp(*this);
				--(*this);
				return tmp;
			}
			const_iterator & operator--(){
				if(p==M->head->next) throw invalid_iterator();
				p=p->pre;
				return *this;
			}
			value_type & operator*() const {
				return *(p->data);
			}
			bool operator==(const iterator &rhs) const {
				return M==rhs.M&&p==rhs.p;
			}
			bool operator==(const const_iterator &rhs) const {
				return M==rhs.M&&p==rhs.p;
			}
			bool operator!=(const iterator &rhs) const {
				return !(*this==rhs);
			}
			bool operator!=(const const_iterator &rhs) const {
				return !(*this==rhs);
			}
			value_type* operator->() const noexcept {
				return (p->data);
			}
			map* pointer(){return M;}
		};

		void deletenode(node *& p){
			if(p==nullptr) return;
			deletenode(p->left);
			deletenode(p->right);
			delete p;
			p=nullptr;
		}

		map(){
			Size=0;
			root=nullptr;
			head=new node();
			tail=new node();
			head->next=tail;
			tail->pre=head;
		}
		map(const map &other){
			Size=0;
			root=nullptr;
			head=new node();
			tail=new node();
			head->next=tail;
			tail->pre=head;
			for(auto it=other.cbegin();it!=other.cend();++it){
				insert(*it);
			}
		}
		map & operator=(const map &other){
			if(&other==this) return *this;
			clear();
			for(map::const_iterator it=other.cbegin();it!=other.cend();++it){
				insert(*it);
			}
			return *this;
		}
		~map(){
			clear();
			delete head;
			delete tail;
		}
		T & at(const Key &key){
			node *p = root;
			while (p != nullptr &&!equal(p->data->first,key)) {
				if (comp(key,p->data->first)) {
					p=p->left;
				} else {
					p=p->right;
				}
			}
			if (p==nullptr) throw index_out_of_bound();
			return p->data->second;
		}
		const T & at(const Key &key) const{
			node *p = root;
			while (p != nullptr &&!equal(p->data->first,key)) {
				if (comp(key,p->data->first)) {
					p=p->left;
				} else {
					p=p->right;
				}
			}
			if (p==nullptr) throw index_out_of_bound();
			return p->data->second;
		}

		T & operator[](const Key &key){
			node *p = root;
			if(p==nullptr){
				value_type tmp(key,T());
				pair<iterator,bool> result(insert(tmp));
				return  (*(result.first)).second;
			}
			while (!equal(p->data->first,key)){
				if (comp(key,p->data->first)) {
					if(p->left==nullptr){
						value_type tmp(key,T());
						pair<iterator,bool> result(insert(tmp));
						return (*(result.first)).second;
					}
					p=p->left;
				} else
				{
					if(p->right==nullptr){
						value_type tmp(key,T());
						pair<iterator,bool> result(insert(tmp));
						return (*(result.first)).second;
					}
					p=p->right;
				}
			}
			return p->data->second;
		}
		const T & operator[](const Key &key) const {
			return at(key);
		}
		iterator begin(){return iterator(this,head->next);}
		const_iterator cbegin() const {return const_iterator(this,head->next);}
		iterator end() {return iterator(this,tail);}
		const_iterator cend() const {return const_iterator(this,tail);}
		bool empty() const {return Size==0;}
		size_t size() const {return Size;}
		void clear(){
			deletenode(root);
			tail->pre=head;
			head->next=tail;
			Size=0;
		}
		void adjust(node *&p,int Subtree){
			if(Subtree==0){
				if(Depth(p->left->left)>Depth(p->left->right)){
					LL(p);
				}
				else{
					LR(p);
				}
			}
			else if(Subtree==1){
				if(Depth(p->right->right)>Depth(p->right->left)){
					RR(p);
				}
				else{
					RL(p);
				}
			}
		}
		pair<node*,bool> insert(node *&p,const value_type &value){
			if(equal(value.first,p->data->first)){
				return pair<node*,bool>(p,false);
			}
			if(comp(value.first,p->data->first)){
				if(p->left!=nullptr){
					pair<node*,bool> tmp(insert(p->left,value));
					if(Depth(p->left)==Depth(p->right)+2){
						adjust(p,0);
					}
					else{
						p->depth=max(Depth(p->left),Depth(p->right))+1;
					}
					return tmp;
				}
				else{
					p->left=new node(&value);
					p->left->next=p;
					p->left->pre=p->pre;
					p->pre=p->left;
					p->left->pre->next=p->left;
					p->depth=max(Depth(p->left),Depth(p->right))+1;
					++Size;
					return pair<node*,bool>(p->left,true);
				}
			}
			else{
				if(p->right!=nullptr){
					pair<node*,bool> tmp(insert(p->right,value));
					if(Depth(p->right)==Depth(p->left)+2){
						adjust(p,1);
					}
					else{
						p->depth=max(Depth(p->left),Depth(p->right))+1;
					}
					return tmp;
				}
				else{
					p->right=new node(&value);
					p->right->next=p->next;
					p->right->pre=p;
					p->next->pre=p->right;
					p->next=p->right;
					p->depth=max(Depth(p->left),Depth(p->right))+1;
					++Size;
					return pair<node*,bool>(p->right,true);
				}
			}
		}
		pair<iterator, bool> insert(const value_type &value){
			if(root==nullptr){
				root=new node(&value);
				root->pre=head;root->next=tail;
				head->next=root;tail->pre=root;
				++Size;
				return pair<iterator,bool>(iterator(this,root),true);
			}
			pair<node*,bool> tmp(insert(root,value));
			return pair<iterator,bool>(iterator(this,tmp.first),tmp.second);
		}
		/**
         * Swap element p with element k.
         */
		void Swap(node *&p,node *k,node *fa){
			 //std::cout<<"Swapping"<<std::endl;
			 node *p1,*p2;
			 if(fa==p){
			 	p1=p->right;
			 	p->left=k->left;p->right=k->right;
			 	k->left=p;k->right=p1;
			 }
			 else{
			 	 p1=p->left;p2=p->right;
				 p->left=k->left;p->right=k->right;
				 k->left=p1;k->right=p2;
				 fa->right=p;
			 }

			 p1=k->pre;p2=p->next;
			 k->pre=p;
			 p->pre=p1;
			 p->next=k;
			 k->next=p2;
			 p2->pre=k;
			 p1->next=p;
			 int D=k->depth;
			 k->depth=p->depth;p->depth=D;
			 p=k;

		}
		void erase(const Key &value,node *&p){
			if(p== nullptr) return;
			if(equal(value,p->data->first)){
				if(p->left==nullptr&&p->right==nullptr){
					--Size;
					p->next->pre=p->pre;
					p->pre->next=p->next;
					delete p;p=nullptr;
					return;
				}
				if(p->left!=nullptr&&p->right!= nullptr){
					node *tmp=p->left,*fa=p;
					while(tmp->right!= nullptr){
						  fa=tmp;
						  tmp=tmp->right;
					}
					Swap(p,tmp,fa);
					erase(value,p->left);
					if(Depth(p->right)==Depth(p->left)+2){
						adjust(p,1);
					}
					else{
						p->depth=max(Depth(p->left),Depth(p->right))+1;
					}
					return;
				}
				node *tmp=p;
				p=p->left!=nullptr?p->left:p->right;
				tmp->next->pre=tmp->pre;
				tmp->pre->next=tmp->next;
				--Size;
				delete tmp;
				return;
			}
			if(comp(value,p->data->first)){
				erase(value,p->left);
				if(Depth(p->right)==Depth(p->left)+2){
					adjust(p,1);
				}
				else{
					p->depth=max(Depth(p->left),Depth(p->right))+1;
				}
			}
			else{
				erase(value,p->right);
				if(Depth(p->left)==Depth(p->right)+2){
					adjust(p,0);
				}
				else{
					p->depth=max(Depth(p->left),Depth(p->right))+1;
				}
			};
		}
		void erase(iterator pos) {
			if(pos.pointer()!=this||pos==end())  throw invalid_iterator();
			size_t  tmp=count(pos->first);
			if(tmp==0) throw invalid_iterator();
			erase(pos->first,root);
		}
		size_t count(const Key &key) const {
			const_iterator tmp=find(key);
			if(tmp!=cend())
				return 1;
			else
				return 0;
		}
		iterator find(const Key &key){
			node *tmp=root;
			while(tmp!=nullptr&&!equal(tmp->data->first,key)){
				if(comp(key,tmp->data->first)){
					tmp=tmp->left;
				}
				else
				{
					tmp=tmp->right;
				}
			}
			if(tmp==nullptr)
				return end();
			return iterator(this,tmp);
		}
		const_iterator find(const Key &key) const{
			node *tmp=root;
			while(tmp!=nullptr&&!equal(tmp->data->first,key))
			{
				if(comp(key,tmp->data->first)){
					tmp=tmp->left;
				}
				else
				{
					tmp=tmp->right;
				}
			}
			if(tmp==nullptr)
				return cend();
			return const_iterator(this,tmp);
		}


		void RR(node *& p){
			node* tmp=p->right;
			p->right=p->right->left;
			p->depth=max(Depth(p->left),Depth(p->right))+1;
			tmp->left=p;
			p=tmp;
			p->depth=max(Depth(p->left),Depth(p->right))+1;
		}


		void LL(node *&p){
			node* tmp=p->left;
			p->left=p->left->right;
			p->depth=max(Depth(p->left),Depth(p->right))+1;
			tmp->right=p;
			p=tmp;
			p->depth=max(Depth(p->left),Depth(p->right))+1;
		}

		void RL(node *& p){
			LL(p->right);
			RR(p);
		}


		void LR(node *& p){
			RR(p->left);
			LL(p);
		}
	};
}

#endif
