#include <iostream>
#include <algorithm>
#include <cstddef>

template<typename T>
class Set {
public:
    struct Node {
        T key;
        int height;
        Node* left;
        Node* right;
        Node* parent;
        size_t size;
        Node(const T& key) : key(key), height(1), left(nullptr), right(nullptr), parent(nullptr), size(1) {
        }
    };
    int get_height(Node* x) const {
        if (x == nullptr) {
            return 0;
        }
        return x->height;
    }
    int get_balance(Node* x) const {
        if (x == nullptr) {
            return 0;
        }
        return get_height(x->right) - get_height(x->left);
    }

    size_t get_size(Node *x) const {
        if (x == nullptr) {
            return 0;
        }
        return x->size;
    }

    void update_node(Node* x) {
        x->size = get_size(x->left) + get_size(x->right) + 1;
        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;
    }

    Node* small_left_rotate(Node* q) {
        Node* x = q->right;
        q->right = x->left;
        if (x->left) {
            x->left->parent = q;
        }
        x->left = q;
        q->parent = x;
        update_node(q);
        update_node(x);
        return x;
    }

    Node* small_right_rotate(Node* x) {
        Node* q = x->left;
        x->left = q->right;
        if (q->right) {
            q->right->parent = x;
        }
        q->right = x;
        x->parent = q;
        update_node(x);
        update_node(q);
        return q;
    }

    Node* big_right_rotate(Node *x) {
        if (get_balance(x->left) > 0) {
            x->left = small_left_rotate(x->left);
            if (x->left) {
                x->left->parent = x;
            }
        }
        x = small_right_rotate(x);
        return x;
    }

    Node* big_left_rotate(Node *x) {
        if (get_balance(x->right) < 0) {
            x->right = small_right_rotate(x->right);
            if (x->right) {
                x->right->parent = x;
            }
        }
        x = small_left_rotate(x);
        return x;
    }

    Node* make_balanced(Node* x) {
        update_node(x);
        if (get_balance(x) == -2) {
            return big_right_rotate(x);
        }
        if (get_balance(x) == 2) {
            return big_left_rotate(x);
        }
        return x;
    }

    Node* Insert(Node* x, const T& k) {
        if (!x) {
            return new Node(k);
        }
        if (k < x->key) {
            x->left = Insert(x->left, k);
            if (x->left) {
                x->left->parent = x;
            }
        }
        else if (x->key < k){
            x->right = Insert(x->right, k);
            if (x->right) {
                x->right->parent = x;
            }
        }
        x = make_balanced(x);
        return x;
    }

    Node* deep_left(Node* x) const {
        if (x->left) {
            return deep_left(x->left);
        }
        else {
            return x;
        }
    }

    Node* delete_min(Node* x) {
        if (!x->left) {
            return x->right;
        }
        x->left = delete_min(x->left);
        if (x->left) {
            x->left->parent = x;
        }
        x = make_balanced(x);
        return x;
    }

    Node *lower_bound(Node *x, const T& k) const {
        if (x == nullptr) {
            return nullptr;
        }
        if (x->key < k) {
            return lower_bound(x->right, k);
        }
        if (k < x->key) {
            auto l = lower_bound(x->left, k);
            if (l == nullptr || l->key < k) {
                return x;
            }
            else {
                return l;
            }
        }
        return x;

    }

    void print(Node *x) {
        if (!x) {
            return;
        }
        print(x->left);
        std::cout << "(" << x->left << ' ' << x->key << ' ' << x->right << ") ";
        print(x->right);
    }

    Node* delete_key(Node* x, const T& k) {
        if (!x)
            return nullptr;
        if (k < x->key) {
            x->left = delete_key(x->left, k);
            if (x->left) {
                x->left->parent = x;
            }
            x = make_balanced(x);
            return x;
        }
        else if (x->key < k) {
            x->right = delete_key(x->right, k);
            if (x->right) {
                x->right->parent = x;
            }
            x = make_balanced(x);
            return x;
        }
        else {
            Node* xleft = x->left;
            Node* xright = x->right;
            delete x;
            if (!xright) {
                return xleft;
            }
            Node* new_root = deep_left(xright);
            new_root->right = delete_min(xright);
            if (new_root->right) {
                new_root->right->parent = new_root;
            }
            new_root->left = xleft;
            if (new_root->left) {
                new_root->left->parent = new_root;
            }
            new_root = make_balanced(new_root);
            return new_root;
        }
    }

    Node* deep_copy(Node *t) {
        if (t == nullptr) {
            return nullptr;
        }
        auto x = new Node(t->key);
        x->left = deep_copy(t->left);
        if (x->left) {
            x->left->parent = x;
        }
        x->right = deep_copy(t->right);
        if (x->right) {
            x->right->parent = x;
        }
        return x;
    }

    Node *find(Node *x, const T& k) const {
        if (x == nullptr) {
            return nullptr;
        }
        if (x->key < k) {
            return find(x->right, k);
        }
        if (k < x->key) {
            return find(x->left, k);
        }
        return x;
    }

    struct iterator {
        Node *a;
        Node *root;
        iterator() : a(nullptr) {
        }
        iterator(Node *a, Node *root) : a(a), root(root) {
        }
        iterator(const iterator& b) {
            a = b.a;
        }
        void operator=(const iterator& b) {
            a = b.a;
        }
        const T& operator*() const {
            return a->key;
        }
        const T* operator->() const {
            return &(a->key);
        }
        bool operator==(const iterator &b) const {
            return b.a == a;
        }
        bool operator!=(const iterator &b) const {
            return b.a != a;
        }
        iterator operator++() {
            if (a->right) {
                a = a->right;
                while (a->left) {
                    a = a->left;
                }
                return *this;
            }
            while (a->parent && a->parent->right == a) {
                a = a->parent;
            }
            a = a->parent;
            return *this;
        }
        iterator operator++(int) {
            auto last = *this;
            if (a->right) {
                a = a->right;
                while (a->left) {
                    a = a->left;
                }
                return last;
            }
            while (a->parent && a->parent->right == a) {
                a = a->parent;
            }
            a = a->parent;
            return last;
        }
        iterator operator--() {
            if (a == nullptr) {
                a = root;
                while (a->right) {
                    a = a->right;
                }
                return *this;
            }
            if (a->left) {
                a = a->left;
                while (a->right) {
                    a = a->right;
                }
                return *this;
            }
            while (a->parent && a->parent->left == a) {
                a = a->parent;
            }
            a = a->parent;
            return *this;
        }
        iterator operator--(int) {
            auto last = *this;
            if (a == nullptr) {
                a = root;
                while (a->right) {
                    a = a->right;
                }
                return last;
            }
            if (a->left) {
                a = a->left;
                while (a->right) {
                    a = a->right;
                }
                return last;
            }
            while (a->parent && a->parent->left == a) {
                a = a->parent;
            }
            a = a->parent;
            return last;
        }
    };

    Set() : data_(nullptr) {
    }



    template<typename Terator>
    Set(Terator first, Terator last) {
        while (first != last) {
            insert(*first);
            first++;
        }
    }

    Set(std::initializer_list<T> elems) {
        for (const T& elem : elems) {
            insert(elem);
        }
    }

    size_t size() const {
        return get_size(data_);
    }

    bool empty() const {
        return data_ == nullptr;
    }

    void insert(const T& elem) {
        data_ = Insert(data_, elem);
        data_->parent = nullptr;
    }

    void erase(const T& elem) {
        data_ = delete_key(data_, elem);
        if (data_) {
            data_->parent = nullptr;
        }
    }

    void clear_tree(Node *p) {
        if (p == nullptr) {
            return;
        }
        clear_tree(p->left);
        clear_tree(p->right);
        delete p;
    }

    iterator begin() const {
        if (data_ == nullptr) {
            return iterator(nullptr, nullptr);
        }
        return iterator(deep_left(data_), data_);
    }

    iterator end() const {
        return iterator(nullptr, data_);
    }

    iterator find(const T& elem) const {
        return iterator(find(data_, elem), data_);
    }

    iterator lower_bound(const T& elem) const {
        return iterator(lower_bound(data_, elem), data_);
    }

    Set(const Set& other) {
        data_ = nullptr;
        if (this != &other) {
            clear_tree(data_);
            data_ = deep_copy(other.data_);
            if (data_)
                data_->parent = nullptr;
        }
    }

    Set& operator=(const Set& other) {
        if (this == &other) {
            return *this;
        }
        clear_tree(data_);
        data_ = deep_copy(other.data_);
        if (data_) {
            data_->parent = nullptr;
        }
        return *this;
    }

    ~Set() {
        clear_tree(data_);
    }

private:
    Node *data_ = nullptr;
};