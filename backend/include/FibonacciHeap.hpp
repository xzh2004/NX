// FibonacciHeap.hpp
#ifndef FIBONACCI_HEAP_HPP
#define FIBONACCI_HEAP_HPP

#include <cmath>
#include <cstddef>
#include <limits>
#include <unordered_map>

// 斐波那契堆节点结构
template <typename T, typename Compare = std::less<T>>
struct FibNode {
    T key;
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;
    int degree;
    bool mark;

    FibNode(const T& key_) 
        : key(key_), parent(nullptr), child(nullptr), 
          left(this), right(this), degree(0), mark(false) {}
};

// 斐波那契堆类
template <typename T, typename Compare = std::less<T>>
class FibonacciHeap {
public:
    FibonacciHeap() : min_node(nullptr), total_nodes(0) {}

    ~FibonacciHeap() {
        // 析构函数需要释放所有节点
        // 简化处理，这里未实现
    }

    bool empty() const {
        return min_node == nullptr;
    }

    // 插入新节点
    void insert(FibNode<T, Compare>* node) {
        if (!min_node) {
            min_node = node;
            node->left = node->right = node;
        } else {
            // 将节点插入根节点列表
            node->left = min_node;
            node->right = min_node->right;
            min_node->right->left = node;
            min_node->right = node;
            if (Compare()(node->key, min_node->key)) {
                min_node = node;
            }
        }
        total_nodes++;
    }

    // 查找最小节点
    FibNode<T, Compare>* find_min() const {
        return min_node;
    }

    // 提取最小节点
    FibNode<T, Compare>* extract_min() {
        FibNode<T, Compare>* z = min_node;
        if (z != nullptr) {
            // 将 z 的所有子节点加入根节点列表
            if (z->child != nullptr) {
                FibNode<T, Compare>* child = z->child;
                do {
                    FibNode<T, Compare>* next = child->right;
                    // 从子节点列表中移除 child
                    child->left->right = child->right;
                    child->right->left = child->left;

                    // 插入到根节点列表
                    child->left = min_node;
                    child->right = min_node->right;
                    min_node->right->left = child;
                    min_node->right = child;

                    child->parent = nullptr;
                    child = next;
                } while (child != z->child);
            }

            // 从根节点列表中移除 z
            z->left->right = z->right;
            z->right->left = z->left;

            if (z->right == z) {
                min_node = nullptr;
            } else {
                min_node = z->right;
                consolidate();
            }
            total_nodes--;
        }
        return z;
    }

    // 减少节点的键值
    void decrease_key(FibNode<T, Compare>* x, const T& new_key) {
        if (Compare()(x->key, new_key)) {
            // 新键值不小于当前键值，无需操作
            return;
        }
        x->key = new_key;
        FibNode<T, Compare>* y = x->parent;
        if (y != nullptr && Compare()(x->key, y->key)) {
            cut(x, y);
            cascading_cut(y);
        }
        if (Compare()(x->key, min_node->key)) {
            min_node = x;
        }
    }

private:
    FibNode<T, Compare>* min_node;
    int total_nodes;

    // 合并树以维护堆的结构
    void consolidate() {
        int D = static_cast<int>(std::floor(std::log2(total_nodes))) + 1;
        std::vector<FibNode<T, Compare>*> A(D, nullptr);

        // 遍历根节点列表并合并同阶数的树
        std::vector<FibNode<T, Compare>*> roots;
        FibNode<T, Compare>* x = min_node;
        if (x != nullptr) {
            do {
                roots.push_back(x);
                x = x->right;
            } while (x != min_node);
        }

        for (FibNode<T, Compare>* w : roots) {
            x = w;
            int d = x->degree;
            while (d >= A.size()) {
                A.resize(d + 1, nullptr);
            }
            while (A[d] != nullptr) {
                FibNode<T, Compare>* y = A[d];
                if (Compare()(y->key, x->key)) {
                    std::swap(x, y);
                }
                link(y, x);
                A[d] = nullptr;
                d++;
                if (d >= A.size()) {
                    A.resize(d + 1, nullptr);
                }
            }
            A[d] = x;
        }

        // 更新最小节点
        min_node = nullptr;
        for (FibNode<T, Compare>* y : A) {
            if (y != nullptr) {
                if (min_node == nullptr) {
                    min_node = y;
                    y->left = y->right = y;
                } else {
                    // 插入到根节点列表
                    y->left = min_node;
                    y->right = min_node->right;
                    min_node->right->left = y;
                    min_node->right = y;
                    if (Compare()(y->key, min_node->key)) {
                        min_node = y;
                    }
                }
            }
        }
    }

    // 将 y 作为 x 的子节点
    void link(FibNode<T, Compare>* y, FibNode<T, Compare>* x) {
        // 从根节点列表中移除 y
        y->left->right = y->right;
        y->right->left = y->left;

        // 将 y 插入到 x 的子节点列表
        y->parent = x;
        if (x->child == nullptr) {
            x->child = y;
            y->left = y->right = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }
        x->degree++;
        y->mark = false;
    }

    // 从 y 的子节点列表中移除 x，并将 x 插入到根节点列表
    void cut(FibNode<T, Compare>* x, FibNode<T, Compare>* y) {
        // 从 y 的子节点列表中移除 x
        if (x->right == x) {
            y->child = nullptr;
        } else {
            x->left->right = x->right;
            x->right->left = x->left;
            if (y->child == x) {
                y->child = x->right;
            }
        }
        y->degree--;

        // 将 x 插入到根节点列表
        x->left = min_node;
        x->right = min_node->right;
        min_node->right->left = x;
        min_node->right = x;
        x->parent = nullptr;
        x->mark = false;
    }

    // 级联切割
    void cascading_cut(FibNode<T, Compare>* y) {
        FibNode<T, Compare>* z = y->parent;
        if (z != nullptr) {
            if (!y->mark) {
                y->mark = true;
            } else {
                cut(y, z);
                cascading_cut(z);
            }
        }
    }
};

#endif // FIBONACCI_HEAP_HPP
