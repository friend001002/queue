#include <iostream>

using namespace std;

template<class val_type>
class Double_linked_node
{
  public:

  Double_linked_node() : val_{}, prev_{ nullptr }, next_{ nullptr }
  {}

  Double_linked_node(val_type val) : val_{ val }, prev_{ nullptr }, next_{ nullptr }
  {}

  Double_linked_node(const Double_linked_node<val_type>&) = delete;
  Double_linked_node(Double_linked_node<val_type>&&) = delete;

  Double_linked_node& operator=(const Double_linked_node<val_type>&) = delete;
  Double_linked_node& operator=(Double_linked_node<val_type>&&) = delete;

  virtual ~Double_linked_node()
  {
    cout << this << " double linked node (val = " << val_ << ", prev = " << prev_ << ", next = " << next_ << ") destr\n";

    val_ = {};
    next_ = { nullptr };
    prev_ = { nullptr };
  }

  val_type Get_val()
  {
    return val_;
  }

  Double_linked_node<val_type>* Get_prev()
  {
    return prev_;
  }

  Double_linked_node<val_type>* Get_next()
  {
    return next_;
  }

  template<class val_type> friend class Double_linked_list;

  private:

  val_type val_;

  Double_linked_node<val_type> *prev_;
  Double_linked_node<val_type> *next_;
};

template<class val_type>
class Double_linked_list
{
  public:

  Double_linked_list() : first_{ nullptr }, size_{}
  {}

  virtual ~Double_linked_list()
  {
    while (nullptr != first_)
    {
      void *tmp{ first_->next_ };

      delete first_;

      first_ = { static_cast<Double_linked_node<val_type>*>(tmp) };

      size_--;
    }

    if (0 != size_)
    {
      cerr << "~Double_linked_list: all elements were destroyed, but size = " << size_ << " (" << __FILE__ << ", " << __LINE__ << ")\n";
    }

    cout << this << " double linked list destr\n";
  }

  Double_linked_node<val_type>* Get_first()
  {
    return first_;
  }

  bool Emplace_element(val_type val, Double_linked_node<val_type> *after, Double_linked_node<val_type> **result)
  {
    if (nullptr == result)
    {
      cerr << "Emplace_element: Result node wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { new Double_linked_node<val_type>(val) };

      *result = { first_ };
      size_++;
      return true;
    }

    // Add as a first node
    if (nullptr == after)
    {
      *result = { new Double_linked_node<val_type>(val) };

      if (nullptr == *result)
      {
        cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";
        return false;
      }

      Double_linked_node<val_type> *tmp { first_ };

      first_ = { *result };

      (*result)->next_ = { tmp };
      tmp->prev_ = (*result);

      size_++;

      return true;
    }

    if (false == Find_node(after))
    {
      cerr << "Emplace_element: Failed to find node " << after << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    *result = { new Double_linked_node<val_type>(val) };

    if (nullptr == *result)
    {
      cerr << "Emplace_element: Failed to create result node (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    Double_linked_node<val_type> *tmp{ after->next_ };

    after->next_ = { *result };

    (*result)->next_ = { tmp };
    (*result)->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { *result };
    }
    size_++;

    return true;
  }

  bool Add_element(Double_linked_node<val_type> **el, Double_linked_node<val_type> *after)
  {
    if (nullptr == el || nullptr == *el)
    {
      cerr << "Add_element: Element to add wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    if (nullptr != after && false == Find_node(after))
    {
      cerr << "Add_element: Failed to find node " << after << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    // There were no nodes before
    if (nullptr == first_)
    {
      first_ = { *el };
      size_++;

      return true;
    }

    // Add as a first node
    if (nullptr == after)
    {
      Double_linked_node<val_type> *tmp { first_ };

      first_ = { *el };

      (*el)->next_ = { tmp };
      tmp->prev_ = (*el);

      size_++;

      return true;
    }

    Double_linked_node<val_type> *tmp { after->next_ };

    after->next_ = { *el };

    (*el)->next_ = { tmp };
    (*el)->prev_ = { after };

    if (nullptr != tmp)
    {
      tmp->prev_ = { *el };
    }
    size_++;

    return true;
  }

  bool Delete_element(Double_linked_node<val_type> *el)
  {
    if (nullptr == el)
    {
      cerr << "Delete_element: Node to delete wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (false == Find_node(el))
    {
      cerr << "Delete_element: Failed to find node " << el << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (el == first_)
    {
      first_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return true;
    }

    Double_linked_node<val_type> *parent{ el->prev_ };

    if (nullptr == parent)
    {
      cerr << "Delete_element: node " << el << " isn't the first node, but doesn't have parents (" << __FILE__ << ' ' << __LINE__ << ")\n";
      return false;
    }

    if (nullptr == el->next_)
    {
      // No need to connect the el's child with a new parent

      parent->next_ = { nullptr };
      el->~Double_linked_node();
      size_--;

      return true;
    }
    else
    {
      // el's child will need a new parent -- el's parent

      parent->next_ = { el->next_ };
      el->~Double_linked_node();
      size_--;

      return true;
    }

    // Unreacheable for now
    //cout << "Delete_element: Failed to delete node " << el << " from single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    //return false;
  }

  bool Find_value_first_occur(val_type val, Double_linked_node<val_type> **result)
  {
    if (nullptr == result || nullptr == *result)
    {
      cerr << "Find_value_first_occur: Result node wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (nullptr == first_)
    {
      cerr << "Find_value_first_occur: Single linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr->val_ == val)
      {
        *result = { curr };
        return true;
      }

      curr = curr->next_;
    }

    *result = { nullptr };
    cout << "Find_value_first_occur: Failed to find node vith val " << val << " in single linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    return false;
  }

  bool Find_node(Double_linked_node<val_type> *node)
  {
    if (nullptr == node)
    {
      cerr << "Find_node: Node to find wasn't provided (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    if (nullptr == first_)
    {
      cerr << "Find_node: Single linked list " << this << "doesn't have any nodes (" << __FILE__ << ' ' << __LINE__ << ")\n";

      return false;
    }

    Double_linked_node<val_type> *curr{ first_ };

    while (nullptr != curr)
    {
      if (curr == node)
      {
        return true;
      }

      curr = { curr->next_ };
    }

    cout << "Find_node: Node " << node << " is not a part of linked list " << this << " (" << __FILE__ << ' ' << __LINE__ << ")\n";
    return false;
  }

  Double_linked_node<val_type>* operator[](size_t index) const
  {
    if (nullptr == first_)
    {
      cerr << "Double_linked_node operator[]: no elements (" << __FILE__ << ", " << __LINE__ << ")\n";
      return nullptr;
    }

    if (index >= size_)
    {
      cerr << "Double_linked_node operator[]: index " << index << " is out of bounds (" << __FILE__ << ", " << __LINE__ << ")\n";
      return nullptr;
    }

    Double_linked_node<val_type> *curr{ first_ };
    val_type ret{};

    for (size_t i{}; i < index; ++i)
    {
      if (nullptr != curr)
      {
        curr = { curr->next_ };
      }
      else
      {
        cerr << "Double_linked_node operator[]: Can't access element with index" << index << " (" << __FILE__ << ", " << __LINE__ << ")\n";
        return nullptr;
      }
    }

    return curr;
  }

  size_t Get_size() const noexcept
  {
    return size_;
  }

  bool Move_after(size_t what, size_t after)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_after: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (after >= size_)
    {
      cerr << "Double_linked_node Move_after: after (" << after << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == after)
    {
      return true;
    }

    if (abs(what - after) == 1 && what > after)
    {
      return true;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };
    Double_linked_node<val_type> *after_ptr{ operator[](after) };

    return Move(what_ptr, after_ptr);
  }

  bool Move_before(size_t what, size_t before)
  {
    if (what >= size_)
    {
      cerr << "Double_linked_node Move_before: what (" << what << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (before >= size_)
    {
      cerr << "Double_linked_node Move_before: before (" << before << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == before)
    {
      return true;
    }

    if (abs(what - before) == 1 && what < before)
    {
      return true;
    }

    Double_linked_node<val_type> *what_ptr{ operator[](what) };

    if (before > 0)
    {
      Double_linked_node<val_type> *after_ptr{ operator[](before - 1) };

      return Move(what_ptr, after_ptr);
    }
    else
    {
      return Move(what_ptr, nullptr);
    }
  }

  bool Swap_vals(size_t a, size_t b)
  {
    if (a >= size_)
    {
      cerr << "Double_linked_node Swap: a (" << a << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (b >= size_)
    {
      cerr << "Double_linked_node Swap: b (" << b << ") must be < size (" << size_ << ") (" << " (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (a == b)
    {
      return true;
    }

    Double_linked_node<val_type> *a_ptr{ operator[](a) };
    Double_linked_node<val_type> *b_ptr{ operator[](b) };

    return Swap_vals(a_ptr, b_ptr);
  }

  bool Swap_vals(Double_linked_node<val_type>* a, Double_linked_node<val_type>* b)
  {
    if (nullptr == a)
    {
      cerr << "Double_linked_node Swap: a (" << a << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (nullptr == b)
    {
      cerr << "Double_linked_node Swap: b (" << b << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (a == b)
    {
      return true;
    }

    if (false == Find_node(a))
    {
      cerr << "Double_linked_node Swap: node a (" << a << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (false == Find_node(b))
    {
      cerr << "Double_linked_node Swap: node b (" << b << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val_type tmp{ a->val_ };
    a->val_ = { b->val_ };
    b->val_ = { tmp };

    return true;
  }

  private:

  bool Move(Double_linked_node<val_type>* what, Double_linked_node<val_type>* after)
  {
    if (nullptr == what)
    {
      cerr << "Double_linked_node Move: what (" << what << ") must not be nullptr (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (false == Find_node(what))
    {
      cerr << "Double_linked_node Move: node what (" << what << ") must be a part of linked list (" << this << ") (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (what == after)
    {
      return true;
    }

    Double_linked_node<val_type> *what_prev_old{ what->prev_ };
    Double_linked_node<val_type> *what_next_old{ what->next_ };
    Double_linked_node<val_type> *after_next_old{};

    if (nullptr != after)
    {
      after_next_old = { after->next_ };
      after->next_ = { nullptr };
    }

    what->prev_ = what->next_ = { nullptr };

    if (nullptr != what_prev_old)
    {
      what_prev_old->next_ = { what_next_old };
    }

    if (nullptr != what_next_old)
    {
      what_next_old->prev_ = { what_prev_old };
    }

    if (nullptr != after)
    {
      after->next_ = { what };
    }

    what->prev_ = { after };
    if (nullptr != after_next_old)
    {
      what->next_ = { after_next_old };
    }
    else if (nullptr == after)
    {
      what->next_ = { first_ };
    }

    if (nullptr != after_next_old)
    {
      after_next_old->prev_ = { what };
    }

    if (what == first_)
    {
      first_ = { what_next_old };
    }

    if (nullptr == after)
    {
      first_->prev_ = { what };
      first_ = { what };
    }

    return true;
  }

  size_t size_;
  Double_linked_node<val_type> *first_;
};

template<class val_type>
class Queue
{
  public:

  Queue() : data_{ new Double_linked_list<val_type> }
  {}

  virtual ~Queue()
  {
    delete data_;

    data_ = { nullptr };
  }

  bool Enqueue(val_type val)
  {
    if (nullptr == data_)
    {
      cerr << "Queue Enqueue: data_ is nullptr! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    size_t size { data_->Get_size() };

    Double_linked_node<val_type> *node  { new Double_linked_node<val_type>(val) };
    Double_linked_node<val_type> *after { nullptr };

    if (nullptr == node)
    {
      cerr << "Queue Enqueue: failed to create new node! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (0 != size)
    {
      after = { data_->operator[](size - 1) };

      if (nullptr == after)
      {
        cerr << "Queue Enqueue: failed to access the target node! (" << __FILE__ << ", " << __LINE__ << ")\n";
        return false;
      }
    }

    if (data_->Add_element(&node, after))
    {
      return true;
    }
    else
    {
      cerr << "Queue Enqueue: failed to add new node! (" << __FILE__ << ", " << __LINE__ << ")\n";
    }

    return false;
  }

  bool Dequeue(val_type& val)
  {
    if (nullptr == data_)
    {
      cerr << "Queue Dequeue: data_ is nullptr! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (0 == data_->Get_size())
    {
      cout << "Queue Dequeue: queue is empty\n";
      return false;
    }

    Double_linked_node<val_type> *node { data_->operator[](data_->Get_size() - 1) };

    if (nullptr == node)
    {
      cerr << "Queue Dequeue: failed to get the last element of double-linked list (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val = { node->Get_val() };

    data_->Delete_element(node);

    return true;
  }

  bool Peek_front(val_type& val)
  {
    if (nullptr == data_)
    {
      cerr << "Queue Peek_front: data_ is nullptr! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (0 == data_->Get_size())
    {
      cout << "Queue Peek_front: queue is empty\n";
      return false;
    }
    
    Double_linked_node<val_type> *node { data_->Get_first() };

    if (nullptr == node)
    {
      cerr << "Queue Peek_front: failed to get the first element of double-linked list (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val = { node->Get_val() };

    return true;
  }

  bool Peek_back(val_type& val)
  {
    if (nullptr == data_)
    {
      cerr << "Queue Peek_back: data_ is nullptr! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (0 == data_->Get_size())
    {
      cout << "Queue Peek_back: queue is empty\n";
      return false;
    }

    Double_linked_node<val_type> *node { data_->operator[](data_->Get_size() - 1) };

    if (nullptr == node)
    {
      cerr << "Queue Peek_back: failed to get the last element of double-linked list (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val = { node->Get_val() };

    return true;
  }

  protected:

  Double_linked_list<val_type> *data_;
};

template<class val_type>
class Dequeue : protected Queue<val_type>
{
  public:

  Dequeue() : Queue<val_type>()
  {}

  virtual ~Dequeue()
  {}

  bool Add_front(val_type val)
  {
    if (nullptr == Queue<val_type>::data_)
    {
      cerr << "Dequeue Add_front: data_ is nullptr! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    size_t size { Queue<val_type>::data_->Get_size() };

    Double_linked_node<val_type> *node  { new Double_linked_node<val_type>(val) };

    if (nullptr == node)
    {
      cerr << "Dequeue Add_front: failed to create new node! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }
    
    if (Queue<val_type>::data_->Add_element(&node, nullptr))
    {
      return true;
    }
    else
    {
      cerr << "Dequeue Add_front: failed to add new node! (" << __FILE__ << ", " << __LINE__ << ")\n";
    }

    return false;
  }

  bool Add_back(val_type val)
  {
    return Queue<val_type>::Enqueue(val);
  }

  bool Peek_front(val_type& val)
  {
    return Queue<val_type>::Peek_front(val);
  }

  bool Peek_back(val_type& val)
  {
    return Queue<val_type>::Peek_back(val);
  }

  bool Dequeue_front(val_type& val)
  {
    if (nullptr == Queue<val_type>::data_)
    {
      cerr << "Dequeue Dequeue_front: data_ is nullptr! (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    if (0 == Queue<val_type>::data_->Get_size())
    {
      cout << "Dequeue Dequeue_front: queue is empty\n";
      return false;
    }

    Double_linked_node<val_type> *node{ Queue<val_type>::data_->Get_first() };

    if (nullptr == node)
    {
      cerr << "Dequeue Dequeue_front: failed to get the first element of double-linked list (" << __FILE__ << ", " << __LINE__ << ")\n";
      return false;
    }

    val = { node->Get_val() };

    Queue<val_type>::data_->Delete_element(node);

    return true;
  }

  bool Dequeue_back(val_type& val)
  {
    return Queue<val_type>::Dequeue(val);
  }
};

int main()
{
  do
  {
    Queue<double> q;

    cout << "Enqueue 1\n";

    if (false == q.Enqueue(1.0))
    {
      break;
    }

    double val{};

    if (false == q.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == q.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    cout << "Enqueue 2\n";

    if (false == q.Enqueue(2.0))
    {
      break;
    }
    
    if (false == q.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == q.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    if (false == q.Dequeue(val))
    {
      break;
    }

    cout << "Dequeue: " << val << endl;

    if (false == q.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == q.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    if (false == q.Dequeue(val))
    {
      break;
    }

    cout << "Dequeue: " << val << endl;

    cout << "\n##############################\n";
    cout << "Dequeue\n";
    cout << "##############################\n\n";

    Dequeue<double> dq;

    cout << "Add_front 1\n";

    if (false == dq.Add_front(1.0))
    {
      break;
    }

    if (false == dq.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == dq.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    cout << "Add_front 2\n";

    if (false == dq.Add_front(2.0))
    {
      break;
    }

    if (false == dq.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == dq.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    cout << "Add_back 3\n";

    if (false == dq.Add_back(3.0))
    {
      break;
    }

    if (false == dq.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == dq.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    cout << "Add_back 4\n";

    if (false == dq.Add_back(4.0))
    {
      break;
    }

    if (false == dq.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == dq.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    if (false == dq.Dequeue_front(val))
    {
      break;
    }

    cout << "Dequeue_front: " << val << endl;

    if (false == dq.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == dq.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;

    if (false == dq.Dequeue_back(val))
    {
      break;
    }

    cout << "Dequeue_back: " << val << endl;

    if (false == dq.Peek_back(val))
    {
      break;
    }

    cout << "back: " << val << ", ";

    if (false == dq.Peek_front(val))
    {
      break;
    }

    cout << "front: " << val << endl;
  }
  while (false);

  cin.get();

  return 0;
}
