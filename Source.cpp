#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>

struct Element {
    // key
    uint_fast64_t X;
    // priority
    uint_fast64_t Y;
    uint_fast64_t sum;
    Element* left;
    Element* right;
    Element* parent;

    Element(uint_fast64_t key) : X(key), Y(rand()), left(nullptr), right(nullptr) {
    }
    Element(uint_fast64_t key, uint_fast64_t prior)
        : X(key), Y(prior), left(nullptr), right(nullptr) {
    }
    // number of nodes
    uint_fast64_t count{};
};
using PElement = Element*;
void DeleteTreap(PElement node) {
    if (node == nullptr)
        return;

    /* first delete both subtrees */
    DeleteTreap(node->left);
    DeleteTreap(node->right);

    /* then delete the node */
    delete node;
}
uint_fast64_t Count(PElement tree) {
    if (tree != nullptr) {
        return tree->count;
    }
    return 0;
}

void UpdateCount(PElement tree) {
    if (tree != nullptr) {
        tree->count = 1 + Count(tree->left) + Count(tree->right);
    }
}
uint_fast64_t Sum(PElement tree) {
    if (tree != nullptr) {
        return tree->sum;  // Return sum if tree is not nullptr
    }
    else {
        return 0;  // or return a specific value depending on your requirements
    }
}
void UpdateSum(PElement tree) {
    if (tree != nullptr) {
        tree->sum = tree->X + Sum(tree->left) + Sum(tree->right);
    }
}

void Split(
    PElement tree, uint_fast64_t key, PElement& left, PElement& right, uint_fast64_t add = 0
) {
    // this should do in O(log(n))
    // should return L and R
    // if the value of the root node < X
    if (tree == nullptr) {
        return void(left = right = nullptr);
    }
    //   Push(tree);
    uint_fast64_t imp_key = add + Count(tree->left);
    if (imp_key < key) {
        // our X is larger than the root node. Hence, also larger than its left side. We split on the
        // right
        // we keep L' in t->r so that t now has the old t->l on the left and L' on the right
        // L' is necissarily larger than t->l because it used to be in the right side of the root
        Split(tree->right, key, tree->right, right, imp_key + 1), left = tree;

    }
    else {
        Split(tree->left, key, left, tree->left, add), right = tree;
    }
    UpdateCount(tree);
    UpdateSum(tree);
}

void Merge(PElement tree1, PElement tree2, PElement& tree) {
    if ((tree1 == nullptr) || (tree2 == nullptr)) {
        tree = (tree1 != nullptr) ? tree1 : tree2;
    }
    else if (tree1->Y > tree2->Y) {
        Merge(tree1->right, tree2, tree1->right), tree = tree1;
    }
    else {
        Merge(tree2->left, tree1, tree2->left), tree = tree2;
    }
    UpdateCount(tree);
    UpdateSum(tree);
}
void Insert(PElement& tree, PElement new_element) {
    if (tree == nullptr) {
        tree = new_element;
    }
    else if (tree->Y < new_element->Y) {
        Split(tree, new_element->X, new_element->left, new_element->right), tree = new_element;

    }
    else {
        Insert(tree->X <= new_element->X ? tree->right : tree->left, new_element);
    }
}

uint_fast64_t CalSum(PElement root, uint_fast64_t left, uint_fast64_t right) {
    PElement tree1 = nullptr;
    PElement tree2 = nullptr;
    PElement tree3 = nullptr;

    Split(root, right + 1, tree2, tree3);
    Split(tree2, left, tree1, tree2);
    uint_fast64_t result = tree2->sum;
    Merge(tree1, tree2, root);
    Merge(root, tree3, root);
    // now T2 should have all the elements from left, right inclusive
    return result;
}
uint_fast64_t ToOddIndex(uint_fast64_t odd_index) {
    uint_fast64_t result = ((odd_index + 1) / 2) - 1;
    return result;
}
uint_fast64_t ToEvenIndex(uint_fast64_t even_index) {
    uint_fast64_t result = ((even_index) / 2);
    return result;
}
uint_fast64_t HandleOdd(
    PElement odd_root, uint_fast64_t left, uint_fast64_t right, uint_fast64_t array_size
) {
    uint_fast64_t last_odd = array_size % 2 == 0 ? array_size - 1 : array_size;
    uint_fast64_t right_odd = right % 2 == 0 ? right - 1 : right;
    uint_fast64_t left_odd = left % 2 == 0 ? left + 1 : left;
    if (left > last_odd || right < 1 || left_odd > right_odd) {
        return 0;
    }

    return CalSum(odd_root, ToOddIndex(left_odd), ToOddIndex(right_odd));
}
uint_fast64_t HandleEven(
    PElement even_root, uint_fast64_t left, uint_fast64_t right, uint_fast64_t array_size
) {
    uint_fast64_t last_even = array_size % 2 == 0 ? array_size : array_size - 1;
    uint_fast64_t right_even = right % 2 == 0 ? right : right - 1;
    uint_fast64_t left_even = left % 2 == 0 ? left : left + 1;
    if (left > last_even || left_even > right_even) {
        return 0;
    }

    return CalSum(even_root, ToEvenIndex(left_even), ToEvenIndex(right_even));
}
void Swap(
    PElement& even_root,
    PElement& odd_root,
    uint_fast64_t left,
    uint_fast64_t right,
    uint_fast64_t array_size
) {
    PElement L1 = nullptr;
    PElement M1 = nullptr;
    PElement K1 = nullptr;
    PElement L2 = nullptr;
    PElement M2 = nullptr;
    PElement K2 = nullptr;

    uint_fast64_t last_odd = array_size % 2 == 0 ? array_size - 1 : array_size;
    //   output_file<<last_odd<<" "<<array_size<<std::endl;

    if (left < last_odd && right > 1) {
        uint_fast64_t right_odd = right % 2 == 0 ? right - 1 : right;
        uint_fast64_t left_odd = left % 2 == 0 ? left + 1 : left;
        Split(odd_root, ToOddIndex(right_odd) + 1, M1, K1);
        Split(M1, ToOddIndex(left_odd), L1, M1);
    }
    uint_fast64_t last_even = array_size % 2 == 0 ? array_size : array_size - 1;
    uint_fast64_t right_even = right % 2 == 0 ? right : right - 1;
    if (left < last_even && right > 0) {
        uint_fast64_t left_even = left % 2 == 0 ? left : left + 1;
        Split(even_root, ToEvenIndex(right_even) + 1, M2, K2);
        Split(M2, ToEvenIndex(left_even), L2, M2);
        // Cal_Sum(even_root, to_even_index(left_even), to_even_index(right_even));
    }
    Merge(L1, M2, odd_root);
    Merge(odd_root, K1, odd_root);
    Merge(L2, M1, even_root);
    Merge(even_root, K2, even_root);
}

int main() {
    // std::ios::sync_with_stdio(false);
    // std::cin.tie(nullptr);
    std::ifstream input_file("input.txt");
    std::ofstream output_file("output.txt");

    uint_fast64_t array_size = 0;
    uint_fast64_t queries = 0;
    uint_fast64_t curr = 0;

    input_file >> array_size >> queries;
    input_file >> curr;

    PElement rootEven = new Element(curr);

    uint_fast64_t operation = 1;
    uint_fast64_t left = 0;
    uint_fast64_t right = array_size;
    uint_fast64_t saved_size = array_size;
    array_size--;
    output_file << "Suite 1:" << std::endl;
    if (array_size == 0) {
        while (queries > 0) {
            input_file >> operation >> left >> right;

            if (operation == 2) {
                output_file << curr;
            }
            queries--;
        }
        DeleteTreap(rootEven);
        return 0;
    }
    input_file >> curr;
    PElement rootOdd = new Element(curr);
    array_size--;
    bool is_odd = false;
    while (array_size > 0) {
        input_file >> curr;
        PElement new_element = new Element(curr);
        if (is_odd) {
            Insert(rootOdd, new_element);
            UpdateCount(rootOdd);
            UpdateSum(rootOdd);
            is_odd = false;
        }
        else {
            Insert(rootEven, new_element);
            UpdateCount(rootEven);
            UpdateSum(rootEven);
            is_odd = true;
        }
        array_size--;
    }
    while (operation != 0) {
        input_file >> operation >> left >> right;
        left--;
        right--;

        if (operation == 2) {
            uint_fast64_t result = HandleEven(rootEven, left, right, saved_size);
            result += HandleOdd(rootOdd, left, right, saved_size);
            output_file << result << std::endl;
        }
        else {
            Swap(rootEven, rootOdd, left, right, saved_size);
        }

        queries--;
    }

    DeleteTreap(rootEven);
    DeleteTreap(rootOdd);
    return 0;
}