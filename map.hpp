/**
 * implement a container like std::map
 */
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
    Compare comp;
    bool compare(const Key &A,const Key &B)const{
        return comp(A,B);
    }
    bool equal(const Key &A,const Key &B) const{
        return !(compare(A,B)||compare(B,A));
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
    inline node* ls(node *p){
        if(p== nullptr) return p;
        else return p->left;
    }
    inline node* rs(node *p){
        if(p== nullptr) return p;
        else return p->right;
    }
    inline node* ls(node *p) const{
        if(p== nullptr) return p;
        else return p->left;
    }
    inline node* rs(node *p) const{
        if(p== nullptr) return p;
        else return p->right;
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
        friend void erase(iterator pos);
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
        for(map::const_iterator it=other.cbegin();it!=other.cend();++it){
            insert(*it);
        }
	}
	map & operator=(const map &other){
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
        if(p== nullptr){
            value_type tmp(key,T());
            pair<iterator,bool> result(insert(tmp));
            return  (*(result.first)).second;
        }
        while (p != nullptr && !equal(p->data->first,key)) {
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
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<node*,bool> insert(node *&p,const value_type &value,node *Pre,node * Next){
	    if(p==nullptr){
	        p=new node(&value);
	        p->pre=Pre;
	        p->next=Next;
	        Pre->next=p;
	        Next->pre=p;
	        ++Size;
	        return pair<node*,bool>(p,true);
	    }
	    if(equal((p->data->first),value.first)){
	        return pair<node*,bool>(p,false);
	    }
	    if(compare(value.first,p->data->first)){
	        pair<node*,bool> tmp(insert(p->left,value,p->pre,p));
            if(Depth(p->left)==Depth(p->right)+2){
                if(compare(value.first,p->left->data->first)){
                    LL(p);
                }
                else{
                    LR(p);
                }
            }
	        p->depth=max(Depth(p->left),Depth(p->right))+1;
            return tmp;
	    }
	    else{
            pair<node*,bool> tmp(insert(p->right,value,p,p->next));
            if(Depth(p->right)==Depth(p->left)+2){
                if(compare(p->right->data->first,value.first)){
                    RR(p);
                }
                else{
                    RL(p);
                }
            }
            p->depth=max(Depth(p->left),Depth(p->right))+1;
            return tmp;
	    }
	}
	pair<iterator, bool> insert(const value_type &value){
        pair<node*,bool> tmp(insert(root,value,head,tail));
        return pair<iterator,bool>(iterator(this,tmp.first),tmp.second);
	}
	/**
	 * Erase value in node p, return true if the depth of p changes,false if it does not change
	 */
	bool erase(const Key &value,node *&p){
	    if(p==nullptr) return true;
	    if(equal(p->data->first,value)){
	        if(p->left==nullptr||p->right==nullptr){
	            node *tmp=p;
	            p->next->pre=p->pre;
	            p->pre->next=p->next;
	            p=(p->left!=nullptr)?p->left:p->right;
	            --Size;
	            delete tmp;
	            return false;
	        }
	        else{
	            node *tmp=p->right;
	            while(tmp->left!=nullptr) tmp=tmp->left;
                p->data->first.~Key();p->data->second.~T();
                free(p->data);
                p->data=(value_type*)malloc(sizeof(value_type));
                new(p->data) value_type(*(tmp->data));
	            if(erase(tmp->data->first,p->right)) return true;
	            return adjust(p,1);
	        }
	    }
	    if(compare(value,p->data->first)){
	        if(erase(value,p->left)) return true;
	        return adjust(p,0);
	    }
	    else{
	        if(erase(value,p->right)) return true;
	        return adjust(p,1);
	    }
	}
	/**
	 * erase the element at pos.
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
	    if(pos.pointer()!=this||pos==end()||pos==cend()) throw invalid_iterator();
	    size_t tmp=count(pos->first);
	    if(tmp==0) throw invalid_iterator();
	    erase(pos->first,root);
	}
	/**
	 * Adjust the node p after deleting an element in its subtree
	 * subTree=1 denotes the right son
	 * subTree=0 denotes the left son
	 * @param p
	 * @param subTree
	 * @return
	 */
	bool adjust(node *& p,int subTree){
	    if(subTree){
            if(Depth(p->left)-Depth(p->right)==1) return true;
            if(Depth(p->left)==Depth(p->right)){
                --p->depth;return false;
            }
            if(Depth(rs(ls(p)))>Depth(ls(ls(p)))){
                LR(p);
                return false;
            }
            LL(p);
            return !(Depth(p->right)==Depth(p->left));
	    }
	    else{
	        if(Depth(p->right)-Depth(p->left)==1) return true;
	        if(Depth(p->right)==Depth(p->left)){
	            --p->depth;return false;
	        }
	        if(Depth(ls(rs(p)))>Depth(rs(rs(p)))){
	            RL(p);
	            return false;
	        }
	        RR(p);
	        return !(Depth(p->right)==Depth(p->left));
	    }
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
	    const_iterator tmp=find(key);
	    if(tmp!=cend())
	        return 1;
	    else
	        return 0;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */


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
	    if(p->right== nullptr||p->right->left== nullptr) return;
        node *tmp1=ls(rs(p));
        p->right->left=p;
        node *tmp2=p->right;
        p->right=tmp1;
        p->depth=max(Depth(p->left),Depth(p->right))+1;
        p=tmp2;
	}

    void LL(node *&p){
        if(p->left== nullptr||p->left->right== nullptr) return;
        node *tmp1=rs(ls(p));
        p->left->right=p;
        node *tmp2=p->left;
        p->left=tmp1;
        p->depth=max(Depth(p->left),Depth(p->right))+1;
        p=tmp2;
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
