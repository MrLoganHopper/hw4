#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/



template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(
        const std::pair<const Key, Value>& new_item);

    virtual void remove(const Key& key);

protected:
    virtual void nodeSwap(
        AVLNode<Key, Value>* n1,
        AVLNode<Key, Value>* n2);

    AVLNode<Key, Value>* rotateLeft(
        AVLNode<Key, Value>* node);

    AVLNode<Key, Value>* rotateRight(
        AVLNode<Key, Value>* node);

    void fixAfterInsert(AVLNode<Key, Value>* inserted);

    void fixAfterRemove(
        AVLNode<Key, Value>* parent, int delta);
};


// ============================================
// LEFT ROTATION
// ============================================

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::rotateLeft(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* child = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = child->getLeft();

    child->setParent(parent);

    if (parent == nullptr) {
        this->root_ = child;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    child->setLeft(node);
    node->setParent(child);

    node->setRight(middle);

    if (middle != nullptr) {
        middle->setParent(node);
    }

    return child;
}


// ============================================
// RIGHT ROTATION
// ============================================

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::rotateRight(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* child = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = child->getRight();

    child->setParent(parent);

    if (parent == nullptr) {
        this->root_ = child;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    child->setRight(node);
    node->setParent(child);

    node->setLeft(middle);

    if (middle != nullptr) {
        middle->setParent(node);
    }

    return child;
}


// ============================================
// AVL INSERT
// ============================================

template<class Key, class Value>
void AVLTree<Key, Value>::insert(
    const std::pair<const Key, Value>& new_item)
{
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(
            new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);

    while (current != nullptr) {
        parent = current;

        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else if (current->getKey() < new_item.first) {
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode =
        new AVLNode<Key, Value>(
            new_item.first, new_item.second, parent);

    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    fixAfterInsert(newNode);
}


// ============================================
// REBALANCE AFTER INSERT
// ============================================

template<class Key, class Value>
void AVLTree<Key, Value>::fixAfterInsert(
    AVLNode<Key, Value>* inserted)
{
    AVLNode<Key, Value>* child = inserted;
    AVLNode<Key, Value>* node = inserted->getParent();

    while (node != nullptr) {
        node->updateBalance(
            child == node->getLeft() ? -1 : 1);

        int balance = node->getBalance();

        // Subtree height has not increased.
        if (balance == 0) {
            return;
        }

        // Continue toward the root.
        if (balance == -1 || balance == 1) {
            child = node;
            node = node->getParent();
            continue;
        }

        if (balance == -2) {
            AVLNode<Key, Value>* left = node->getLeft();

            // Left-left case.
            if (left->getBalance() <= 0) {
                rotateRight(node);

                node->setBalance(0);
                left->setBalance(0);
            }
            // Left-right case.
            else {
                AVLNode<Key, Value>* middle =
                    left->getRight();

                int middleBalance = middle->getBalance();

                rotateLeft(left);
                rotateRight(node);

                node->setBalance(
                    middleBalance == -1 ? 1 : 0);

                left->setBalance(
                    middleBalance == 1 ? -1 : 0);

                middle->setBalance(0);
            }
        }
        else {
            AVLNode<Key, Value>* right = node->getRight();

            // Right-right case.
            if (right->getBalance() >= 0) {
                rotateLeft(node);

                node->setBalance(0);
                right->setBalance(0);
            }
            // Right-left case.
            else {
                AVLNode<Key, Value>* middle =
                    right->getLeft();

                int middleBalance = middle->getBalance();

                rotateRight(right);
                rotateLeft(node);

                node->setBalance(
                    middleBalance == 1 ? -1 : 0);

                right->setBalance(
                    middleBalance == -1 ? 1 : 0);

                middle->setBalance(0);
            }
        }

        return;
    }
}


// ============================================
// AVL REMOVE
// ============================================

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* target =
        static_cast<AVLNode<Key, Value>*>(
            this->internalFind(key));

    if (target == nullptr) {
        return;
    }

    // Swap actual nodes with the predecessor.
    if (target->getLeft() != nullptr &&
        target->getRight() != nullptr) {

        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(
                this->predecessor(target));

        nodeSwap(target, pred);
    }

    // Target now has at most one child.
    AVLNode<Key, Value>* parent = target->getParent();

    AVLNode<Key, Value>* child =
        target->getLeft() != nullptr
        ? target->getLeft()
        : target->getRight();

    int delta = 0;

    if (parent != nullptr) {
        if (parent->getLeft() == target) {
            parent->setLeft(child);
            delta = 1;
        }
        else {
            parent->setRight(child);
            delta = -1;
        }
    }
    else {
        this->root_ = child;
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    delete target;

    fixAfterRemove(parent, delta);
}


// ============================================
// REBALANCE AFTER REMOVE
// ============================================

template<class Key, class Value>
void AVLTree<Key, Value>::fixAfterRemove(
    AVLNode<Key, Value>* node, int delta)
{
    while (node != nullptr) {
        node->updateBalance(delta);

        int balance = node->getBalance();

        // The subtree height has not decreased.
        if (balance == -1 || balance == 1) {
            return;
        }

        AVLNode<Key, Value>* top = node;

        if (balance == -2) {
            AVLNode<Key, Value>* left = node->getLeft();
            int leftBalance = left->getBalance();

            // Left-left case.
            if (leftBalance <= 0) {
                top = rotateRight(node);

                if (leftBalance == 0) {
                    node->setBalance(-1);
                    left->setBalance(1);
                    return;
                }

                node->setBalance(0);
                left->setBalance(0);
            }
            // Left-right case.
            else {
                AVLNode<Key, Value>* middle =
                    left->getRight();

                int middleBalance = middle->getBalance();

                rotateLeft(left);
                top = rotateRight(node);

                node->setBalance(
                    middleBalance == -1 ? 1 : 0);

                left->setBalance(
                    middleBalance == 1 ? -1 : 0);

                middle->setBalance(0);
            }
        }
        else if (balance == 2) {
            AVLNode<Key, Value>* right = node->getRight();
            int rightBalance = right->getBalance();

            // Right-right case.
            if (rightBalance >= 0) {
                top = rotateLeft(node);

                if (rightBalance == 0) {
                    node->setBalance(1);
                    right->setBalance(-1);
                    return;
                }

                node->setBalance(0);
                right->setBalance(0);
            }
            // Right-left case.
            else {
                AVLNode<Key, Value>* middle =
                    right->getLeft();

                int middleBalance = middle->getBalance();

                rotateRight(right);
                top = rotateLeft(node);

                node->setBalance(
                    middleBalance == 1 ? -1 : 0);

                right->setBalance(
                    middleBalance == -1 ? 1 : 0);

                middle->setBalance(0);
            }
        }

        // Continue toward the root if height decreased.
        AVLNode<Key, Value>* parent = top->getParent();

        if (parent == nullptr) {
            return;
        }

        delta = parent->getLeft() == top ? 1 : -1;
        node = parent;
    }
}


// ============================================
// NODE SWAP
// ============================================

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(
    AVLNode<Key, Value>* n1,
    AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);

    int8_t tempB = n1->getBalance();

    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
