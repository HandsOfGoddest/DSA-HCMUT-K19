//#ifndef FRAGMENT_LINKED_LIST
//#define FRAGMENT_LINKED_LIST
template <class T>
class FragmentLinkedList : IList<T>
{
public:
    class Node;
    class Iterator;

protected:
    Node **fragmentPointers;
    int fragmentMaxSize;
    int count;
    int capacity;
private:
	void ensureCapacity(int capacity);
    void reduceCapacity(int capacity);
	void add_Head(const T& element);
public:
    FragmentLinkedList(int fragmentMaxSize = 5)
    {
    	this->capacity=2;
        this->count = 0;
        this->fragmentMaxSize = fragmentMaxSize;
        this->fragmentPointers = new Node *[2];
        this->fragmentPointers[0] = NULL;
        this->fragmentPointers[1] = NULL;
    }
    virtual ~FragmentLinkedList();
    virtual void add(const T& element);
    virtual void add(int index, const T& element);
    virtual T removeAt(int index);
    virtual bool removeItem(const T& item);
    virtual bool empty();
    virtual int size();
    virtual void clear();
    virtual T get(int index);
    virtual void set(int index, const T& element);
    virtual int indexOf(const T& item);
    virtual bool contains(const T& item);
    virtual string toString();
    Iterator begin(int index = 0);
    Iterator end(int index = -1);

public:
    class Node
    {
    private:
        T data;
        Node *next;
        Node *prev;
        friend class FragmentLinkedList<T>;

    public:
        Node()
        {
            next = 0;
            prev = 0;
        }
        Node(Node *next, Node *prev)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next, Node *prev)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    class Iterator
    {
    private:
        FragmentLinkedList<T> *pList;
        Node *pNode;
        int index;

    public:
        Iterator(FragmentLinkedList<T> *pList = 0, bool begin = true);
        Iterator(int fragmentIndex, FragmentLinkedList<T> *pList = 0,  bool begin = true);
        Iterator &operator=(const Iterator &iterator);
        T &operator*();
        bool operator!=(const Iterator &iterator);
        void remove();
        void set(const T& element);
        Iterator &operator++();
        Iterator operator++(int);
    };
};

template<class T>
void FragmentLinkedList<T>::ensureCapacity(int capacity){
    if(capacity > this->capacity){
        	int n_Capacity= capacity;
       		Node **n_fragmentPointers=new Node*[n_Capacity];// tao ra 1 list moi co suc chua this->capacity+1
        	for(int i=0; i<this->capacity; i++){
        		n_fragmentPointers[i]=this->fragmentPointers[i];//copy lai cac fragment cua list cu vao list moi
        		if(i==this->capacity-1)	n_fragmentPointers[i+1]=n_fragmentPointers[i];// set red_frag cua list moi vao node cuoi cua list cu
			}
        	this->capacity=n_Capacity;//set capacity=capacity+1
        	delete[] this->fragmentPointers;// xoa di list cu
        	this->fragmentPointers=n_fragmentPointers;// tro vao list moi
    }
} 

template<class T>
void FragmentLinkedList<T>::reduceCapacity(int capacity){
    if(capacity < this->capacity){
    	int n_Capacity= capacity;
    	Node **n_fragmentPointers=new Node*[n_Capacity];// tao ra list moi co suc chua this->capacity-1
    	for(int i=0; i<n_Capacity; i++){
			n_fragmentPointers[i]=this->fragmentPointers[i];// bo di red_frag cua list cu
		}
    	this->capacity=n_Capacity;// set capacity=capacity-1
    	delete[] this->fragmentPointers;//xoa di list cu
    	this->fragmentPointers=n_fragmentPointers;// tro vao list moi
    }
}

template<class T>
FragmentLinkedList<T>::~FragmentLinkedList(){
    this->clear();
}

template<class T>
void FragmentLinkedList<T>::add(const T& element){
    this->ensureCapacity(count/fragmentMaxSize + 2);
	// (count/fragmentMaxSize) dung de xac dinh xem so node hien tai co vuot qua max_size hay ko
    // co thi lay (count/fragmentMaxSize) + 2 vi capacity ban dau la 2 va neu (count/fragmentMaxSize) =1 thi tuc la so node hien tai da vuot qua max_size cua frag dg quan ly 
	Node* pNew=new Node;
	pNew->data=element;
	if(count==0){// add vao tail neu list dang rong
		this->fragmentPointers[0]=pNew;
		this->fragmentPointers[1]=pNew;
		pNew->next=NULL;
		pNew->prev=NULL;
	}
	else{
		int red_frag= this->capacity-1;// lay index cua red fragment
		if(count%fragmentMaxSize==0){// kiem tra xem node dg can add co phai la node dau cua 1 fragment hay ko
			int FragmentIndex= count/fragmentMaxSize;//lay index fragment dg quan ly node add vao
			this->fragmentPointers[FragmentIndex]->next=pNew;
			pNew->prev=this->fragmentPointers[FragmentIndex];
			pNew->next=NULL;
			this->fragmentPointers[FragmentIndex]=pNew;// white_frag quan ly node moi dc add vao
			this->fragmentPointers[red_frag]=pNew;//red_frag tro vao node cuoi cua list
		}
		else{
			this->fragmentPointers[red_frag]->next=pNew;
			pNew->prev=this->fragmentPointers[red_frag];
			pNew->next=NULL;
			this->fragmentPointers[red_frag]=pNew;
		}
	}
	count++;
}

template<class T>
void FragmentLinkedList<T>::add_Head(const T& element){
	Node* pNew= new Node;
	pNew->data=element;
	if(count==0){
		this->fragmentPointers[0]=pNew;
		this->fragmentPointers[1]=pNew;
		pNew->next=NULL;
		pNew->prev=NULL;
	}
	else{
		pNew->prev=NULL;
		pNew->next=this->fragmentPointers[0];
		this->fragmentPointers[0]->prev=pNew;
		this->fragmentPointers[0]=pNew;
		if(count>this->fragmentMaxSize)
			for(int i=1; i<this->capacity-1; i++){
				this->fragmentPointers[i]=this->fragmentPointers[i]->prev;
			}
		this->ensureCapacity(count/fragmentMaxSize + 2);
	}
	count++;
}

template<class T>
void FragmentLinkedList<T>::add(int index, const T& element){
	if(index<0 || index>this->count)	throw std::out_of_range("The index is out of range!");//kiem tra index co hop le ko (index>=0 va index<=this->count --> hop le)
	else{
		if(index==(this->count))	this->add(element);// index=this->count	--> add vao tail
		else {
			if(index==0)	this->add_Head(element);// index=0 --->add vao head
			else{
				Node* pNew= new Node;
				pNew->data=element;
				this->ensureCapacity(count/fragmentMaxSize + 2);// kiem tra capacity
				Node*temp_H=new Node;
				int FragmentIndex= index/this->fragmentMaxSize;//lay index cua fragment dg quan ly node co index can add vao
				int First_node_of_frag= FragmentIndex*this->fragmentMaxSize;// lay index dau tien cua node dc quan ly boi fragment dang duoc truy cap
				temp_H=this->fragmentPointers[FragmentIndex];
				for(int i=0; i<this->fragmentMaxSize; i++){
					if(First_node_of_frag+i==index){
						pNew->next=temp_H;
						pNew->prev=temp_H->prev;
						temp_H->prev->next=pNew;
						temp_H->prev=pNew;
						int jump=(index%this->fragmentMaxSize==0)?0:1;
						/* neu Node moi duoc add vao first_node duoc quan ly boi fragment thi jump=0 do ta phai thuc hien dich cac fragment bat dau tu fragment dg truy cap qua trai 1 node
				    		neu Node moi duoc add ben trong vung quan ly cua fragment( ko phai node dau) thi ta thuc hien dich cac fragment bat dau tu fragment dg truy cap + jump (1) sang trai 1 node
						*/
						if(this->count%this->fragmentMaxSize!=0 ){
							if(this->capacity-2!=FragmentIndex || jump==0)	this->fragmentPointers[this->capacity-2]=this->fragmentPointers[this->capacity-2]->prev;
						}
							/*
							if(this->count%this->fragmentMaxSize!=0)
								truoc khi add:
								Frag_0		Frag_1		Frag_2			Frag_3		Frag4
								0-1-2-3-4___5-6-7-8-9___10-11-12-13-14__15-16-17-18-19
								this->count=20
								sau khi add:
								Frag_0		Frag_1		Frag_2			Frag_3					Frag_4==Frag_5(sau khi check capacity)
								0-1-2-3-4___5-6-7-8-9___10-11-12-13-14__15-16-17-(index)-18_____19
								this->count=20
								Do 20%5 == 0	--> ko the dich Frag_4(this->capacity-2) do neu dich thi se gay ra loi quan ly node cua fragment
								
							if(this->capacity-2 !=FragmentIndex)
								truoc khi add:
								Frag_0		Frag_1
								0 - 1 - 2 - 3
								sau khi add:
								Frag_0->NULL(Error)
								0 - (index) - 1 - 2 - 3
								---> this->capacity-2 != FragmentIndex
								
							if(jump==0)
								truoc khi add:
								Frag_0		Frag_1		Frag_2			Frag_3	 Frag4
								0-1-2-3-4___5-6-7-8-9___10-11-12-13-14__15-16-17-18
								sau khi add:
								Frag_0		Frag_1		Frag_2			Frag_3	 		 Frag4
								0-1-2-3-4___5-6-7-8-9___10-11-12-13-14__(index)-15-16-17-18
								Do neu chi co dk: this->capacity-2 !=FragmentIndex thi khi add Node moi vao node dau cua Fragment[this->capacity-2] dg quan ly
								thi Fragment[this->capacity-3] se gap loi bo nho khi quan ly so node=this->fragmentMaxsize+1 ---> xet them dk OR jump==0
							*/
						for(int i=FragmentIndex+jump; i<(this->capacity-2); i++){
							this->fragmentPointers[i]=this->fragmentPointers[i]->prev;
						}
						count++;
						break;
					}
					else	temp_H=temp_H->next;
				}	
			}
		}
	}
}

template<class T>
T FragmentLinkedList<T>::removeAt(int index){
    if(index<this->count && index>=0){
		T data;
		Node* temp=new Node;
		int FragmentIndex= index/this->fragmentMaxSize; // lay index cua Fragment dang quan ly node can remove
		int First_node_of_frag= FragmentIndex*this->fragmentMaxSize; // lay index cua node dau tien duoc quan ly boi fragment dg duoc truy cap
		int red_frag= this->capacity-1; // lay index cua red fragment
		if(index==0){ // truong hop remove Node dau tien cua list
			data = this->fragmentPointers[0]->data;
			temp=this->fragmentPointers[0];
			if(this->fragmentPointers[0]->next!=NULL){
				this->fragmentPointers[0]=this->fragmentPointers[0]->next;
				this->fragmentPointers[0]->prev=NULL;
			}	
			else{
				this->fragmentPointers[0]=NULL;
				this->fragmentPointers[1]=NULL;
			} 
		}
		else{// cac truong hop con lai
			Node* temp=new Node;
			if(index==this->count-1){//remove node cuoi cung cua list
				int Is_same=(index%this->fragmentMaxSize==0)?(FragmentIndex):(red_frag); // kiem tra red_frag va white_frag co tro chung 1 node ko ? neu co thi Is_same=whhite_index else thi Is_same=red_index
				temp=this->fragmentPointers[Is_same];
				this->fragmentPointers[Is_same]=this->fragmentPointers[Is_same]->prev;
				this->fragmentPointers[Is_same]->next=NULL;
				data=temp->data;
			}
			else{
				//Node* prev=new Node;
				temp=this->fragmentPointers[FragmentIndex];
				//prev=this->fragmentPointers[FragmentIndex]->prev;
				for(int i=0; i<this->fragmentMaxSize; i++){
					if(First_node_of_frag+i==index){
						data = temp->data;
						temp->prev->next=temp->next;
						temp->next->prev=temp->prev;	
						break;
					}
					else{
						//prev=temp;
				    	temp=temp->next;
					}
				}
			}
		}
		int Is_0=(index==0)?1:(FragmentIndex+((index%this->fragmentMaxSize==0)?0:1));// lay index
		for(int i=Is_0; i<(this->capacity)-(((this->count-1)%this->fragmentMaxSize==0)?2:1); i++)
			this->fragmentPointers[i]=this->fragmentPointers[i]->next;	
		count--;
		this->reduceCapacity((count-1)/fragmentMaxSize + 2);
		delete temp;
		temp=NULL;
		return data;
	}
	else{
		throw std::out_of_range("The index is out of range!");	
	}  
}

template<class T>
bool FragmentLinkedList<T>::removeItem(const T& item){
    Node* t_head=this->fragmentPointers[0];
	int index=0;
	while(t_head!=NULL){
		if(t_head->data==item){
			this->removeAt(index);
			return 1;
		}	
		else{
			t_head=t_head->next;
			index++;
		}
	}
	return 0;
}

template<class T>
bool FragmentLinkedList<T>::empty(){
    if(this->count==0)  return 1;
    return 0;
}

template<class T>
int FragmentLinkedList<T>::size(){
    return this->count;
}

template<class T>
void FragmentLinkedList<T>::clear(){
    int i=0;
	while(this->count!=0){
		this->removeAt(i);
	}
}

template<class T>
T FragmentLinkedList<T>::get(int index){
    if(index<this->count && index>=0){
		int start=index/this->fragmentMaxSize;
		T data;
		Node* temp=this->fragmentPointers[start];
		for(int i=0; i<this->fragmentMaxSize; i++){
			if((start*this->fragmentMaxSize+i)==index){
				data=temp->data;
				break;
			}
			else	temp=temp->next;
		}
		return data;
	}
	else throw std::out_of_range("The index is out of range!");
}

template<class T>
void FragmentLinkedList<T>::set(int index, const T& element){
	if(index>=0 && index<this->count){
		int FragmentIndex= index/this->fragmentMaxSize;
		int First_Node_of_Frag= FragmentIndex*this->fragmentMaxSize;
		Node* t_Frag= this->fragmentPointers[FragmentIndex];
		for(int i=0; i<this->fragmentMaxSize; i++){
			if((First_Node_of_Frag+i) == index){
				t_Frag->data= element;
				break;
			}	
			else	t_Frag=t_Frag->next;
		}	
	}
	else	throw std::out_of_range("The index is out of range!");
}

template<class T>
int FragmentLinkedList<T>::indexOf(const T& item){
    Node* t_head=this->fragmentPointers[0];
	int index=0;
	while(t_head!=NULL){
		if(t_head->data==item)	return index;
		else{
			t_head=t_head->next;
			index++;
		}
	}
	return -1;
}

template<class T>
bool FragmentLinkedList<T>::contains(const T& item){
    if(this->indexOf(item)>=0)    return 1;
    return 0;
}

template <class T>
string FragmentLinkedList<T>::toString()
{
    stringstream ss;
    ss << "[";
    Node *ptr = this->fragmentPointers[0];

    if (this->count == 0)
        ss << "]";

    while(ptr!=NULL){
    	if(ptr->next!=NULL){
    		ss<<ptr->data;
			ss<<", ";
		}
		else	ss<<ptr->data<<"]";
		ptr=ptr->next;   		
	}

    return ss.str();
}

template<class T>
typename FragmentLinkedList<T>::Iterator FragmentLinkedList<T>::begin(int index){
	Iterator it(index, this, true);
	return it;
}

template<class T>
typename FragmentLinkedList<T>::Iterator FragmentLinkedList<T>::end(int index){
	Iterator it(index, this, false);
	return it;
}

template<class T>
FragmentLinkedList<T>::Iterator::Iterator(int fragmentIndex, FragmentLinkedList<T> *pList,  bool begin){
	if( (fragmentIndex<0 && fragmentIndex!=-1) || fragmentIndex >= pList->capacity)	throw std::out_of_range("The index is out of range!");
	else{
		this->pList=pList;
		if(begin){
			if(fragmentIndex != -1){
				this->pNode=pList->fragmentPointers[fragmentIndex];
				this->index=fragmentIndex*(pList->fragmentMaxSize);
			}
			else throw std::out_of_range("The index is out of range!");
		}
		else{
			if(fragmentIndex == -1){
				this->pNode=NULL;
				this->index=pList->count;
			}
			else{
				this->pNode=pList->fragmentPointers[fragmentIndex];
				this->index=fragmentIndex*pList->fragmentMaxSize;
				for(int i=0; i<pList->fragmentMaxSize; i++){
					this->pNode=this->pNode->next;
					if(this->pNode==NULL)	break;
					this->index++;
				}	
			}
		}
	}
}

template<class T>
FragmentLinkedList<T>::Iterator::Iterator(FragmentLinkedList<T> *pList,  bool begin){
	this->pList=pList;
	if(begin){
		if(pList!=NULL){
			this->pNode=pList->fragmentPointers[0];
			this->index=0;
		}
		else{
			this->pNode=NULL;
			this->index=-1;
		}
	}
	else{
		this->pNode=NULL;
		this->index=(pList!=NULL)?pList->size():0;
	}
}

template<class T>
bool FragmentLinkedList<T>::Iterator::operator!=(const Iterator &iterator){
	if(this->pNode!=iterator.pNode && this->index!=iterator.index)	return 1;
	return 0;
}

template<class T>
typename FragmentLinkedList<T>::Iterator& FragmentLinkedList<T>::Iterator::operator=(const Iterator &iterator){
	this->pList=iterator.pList;
	this->pNode=iterator.pNode;
	this->index=iterator.index;
	return *this;
}

template<class T>
T& FragmentLinkedList<T>::Iterator::operator*(){
	if(this->pNode!=NULL)	return this->pNode->data;
	else	throw std::out_of_range("segmentation fault!");
}

template<class T>
void FragmentLinkedList<T>::Iterator::remove(){
	if(this->pNode !=NULL){
		this->pNode=this->pNode->prev;
		this->pList->removeAt(this->index);
		this->index--;
	}
	else throw std::out_of_range("segmentation fault");
}

template<class T>
void FragmentLinkedList<T>::Iterator::set(const T& element){
	if(this->pNode!=NULL)	this->pNode->data=element;
	else throw std::out_of_range("segmentation fault");
}

template<class T>
typename FragmentLinkedList<T>::Iterator& FragmentLinkedList<T>::Iterator::operator++(){
	if(this->pList->count==0 && this->index!=-1)	throw std::out_of_range("segmentation fault");
	else{
		if(this->index==-1){
			this->pNode=this->pList->fragmentPointers[0];
			this->index=0;
		}
		else{
			if(this->index==this->pList->count)	throw std::out_of_range("segmentation fault");
			else{
				this->pNode=this->pNode->next;
				this->index++;
			}
		}
		return *this;
	}
}

template<class T>
typename FragmentLinkedList<T>::Iterator FragmentLinkedList<T>::Iterator::operator++(int){
	if(this->pList->count==0 && this->index!=-1)	throw std::out_of_range("segmentation fault");
	else{
		Iterator temp=*this;
		++*this;
		return temp;
	}
}

//#endif