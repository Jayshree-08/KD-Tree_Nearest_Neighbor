# KD-Tree K-Nearest Neighbors: Complete Study Guide
*A line-by-line explanation guide for your ADS Course Project Viva*

## 1. The Big Picture: What are we doing?
We are building a **Machine Learning Classifier** using an **Advanced Data Structure**.
* **The Goal:** Take 150 flowers (from `iris.txt`) with 4 measurements each. When given a *new* flower, the program must guess its species.
* **The Algorithm (K-NN):** Find the `K` (e.g., 5) closest flowers to our new flower and take a majority vote.
* **The Data Structure (KD-Tree):** Searching an array of 150 items takes $O(N)$ time. A KD-Tree organizes the data by splitting it in different dimensions (like a 4D binary tree), dropping the search time to $O(\log N)$ because we don't have to check every single point.

---

## 2. Core Structures (Lines 8-17)

```c
struct Node {
    float point[K_DIM];      // The 4 features of the flower (e.g. sepal length, etc.)
    char class_name[30];     // The species name (e.g. "Iris-setosa")
    struct Node *left, *right; // Pointers to left and right children
};
```
This is the building block of the tree. Just like a Binary Tree node, but it holds a 4D array instead of a single integer.

```c
struct Neighbor {
    float distance;          // Distance from the target point
    struct Node* node;       // Pointer to the actual flower node in the tree
};
```
We use this later to keep track of the "Top 5" closest flowers we find while searching.

---

## 3. Building the Tree (Lines 29-41)
```c
struct Node* insert(struct Node* root, float point[], const char* c_name, int depth)
```
**How it works:** A KD-tree is just a Binary Search Tree that cycles through dimensions. 
* **The Key Line:** `int cd = depth % K_DIM;`
* **Explanation:** At depth 0, we compare dimension 0. At depth 1, we compare dimension 1. At depth 4, `4 % 4 = 0`, so we wrap around back to dimension 0. We go left if our point's value is smaller than the root's value at that specific dimension, otherwise right.

---

## 4. The Distance Calculation (Lines 43-49)
```c
float distanceSquared(float p1[], float p2[])
```
**Viva Question:** *"Why use distance squared instead of square root?"*
**Answer:** Calculating square roots using `sqrt()` is very slow for the CPU. Because we only need to compare distances (to see which is smaller), $A < B$ implies $A^2 < B^2$. By leaving the distance squared, we get the exact same relative ordering but much faster execution.

---

## 5. Keeping Track of Neighbors (Lines 51-88)
```c
void addNeighbor(struct Neighbor neighbors[], int k_neighbors, int *count, struct Node* node, float dist)
```
**How it works:** We have an array of size `K`. 
1. If the array isn't full, we put the new neighbor in and use a simple **Bubble Sort** to sort the array from closest to furthest.
2. If the array IS full, we look at the last element (the furthest one in our top K). If the new node's distance is *smaller* than that last element, we overwrite the last element and re-sort.

---

## 6. The Heart of the Project: KD-Tree Search (Lines 90-123)
```c
void kNearestNeighbors(...)
```
This is a recursive function and the hardest part of the code. Let's break it down:
1. `(*nodes_visited)++;` -> We track efficiency.
2. `distanceSquared()` -> We calculate how far the current node is from our target, and try to add it to our Top K list using `addNeighbor()`.
3. We decide which way to go down the tree (left or right) exactly like we did in `insert()`. We recursively call `kNearestNeighbors()` on that `nextBranch`.

**THE TRICKY PART (The Backtracking):**
```c
float diff = target[cd] - root->point[cd];
if(diff*diff < current_worst_dist) {
    kNearestNeighbors(otherBranch, target, depth+1, neighbors, k_neighbors, count, nodes_visited);
}
```
**Viva Question:** *"Why do you check the other branch sometimes?"*
**Answer:** Think of the KD-Tree as drawing a fence line on a map. We are standing on the left side of the fence, so we searched the left side. However, our target point might be standing right next to the fence. Even though it's technically on the left, a point on the *right* side of the fence might actually be physically closer! 
* `diff*diff` calculates the straight-line perpendicular distance to that fence.
* If the distance to the fence is smaller than the worst distance in our Top-K list (`current_worst_dist`), our search radius overlaps the fence, so we **must** check the `otherBranch` to be accurate.

---

## 7. The Double Pointer Mystery (Line 186)
```c
void loadIrisData(struct Node **root)
```
Called in main as: `loadIrisData(&root);`

**Viva Question:** *"Why is this a double pointer (`**`)?"*
**Answer:** In C, all variables are 'pass-by-value'. The original `root` in `main()` is a pointer set to `NULL`. If we pass it as a normal pointer `*root`, the function only receives a *copy* of that pointer. When the function builds the tree, it attaches it to the copy, and the `root` in `main()` remains `NULL`. 
By passing the **address** of the pointer (`&root`), making it a double pointer (`**root`), the function has direct access to the actual `root` variable sitting in `main()`, allowing it to modify it permanently.

---

## 8. Making the Final Decision (Line 125)
```c
void classifyPoint(...)
```
1. It calls `kNearestNeighbors` to get the Top K list.
2. It loops through the Top K list and uses arrays to count how many times each species (`class_name`) appears.
3. It finds the highest count (Majority Voting) and declares that species as the winner!

---

## Quick Cheat Sheet for Viva
* **Time Complexity (Search):** $O(\log N)$ average.
* **Time Complexity (Insert):** $O(\log N)$ average.
* **Space Complexity:** $O(N)$ to store the tree.
* **Algorithm Paradigm:** Divide and Conquer.
