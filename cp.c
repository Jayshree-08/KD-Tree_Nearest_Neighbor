#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define k 2

struct Node {
    int point[k];
    struct Node *left, *right;
};

struct Node* newNode(int arr[])
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));

    for(int i = 0; i < k; i++){
        temp->point[i] = arr[i];
    }

    temp->left = temp->right = NULL;

    return temp;
}

struct Node* insert(struct Node* root, int point[], int depth)
{
    if(root == NULL){
        return newNode(point);
    }

    int cd = depth % k;

    if(point[cd] < root->point[cd]){
        root->left = insert(root->left, point, depth + 1);
    }
    else{

        root->right = insert(root->right, point, depth + 1);
    }

    return root;
}

int distanceSquared(int p1[], int p2[])
{
    int dist = 0;

    for(int i = 0; i < k; i++){

        dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }

    return dist;
}

void nearestNeighbor(struct Node* root, int target[], int depth,
                     struct Node** best, int* bestDist)
{
    if(root == NULL){

        return;
    }

    int d = distanceSquared(root->point, target);

    if(d < *bestDist)
    {
        *bestDist = d;
        *best = root;
    }

    int cd = depth % k;

    struct Node *nextBranch = NULL;
    struct Node *otherBranch = NULL;

    if(target[cd] < root->point[cd])
    {
        nextBranch = root->left;
        otherBranch = root->right;
    }
    else
    {
        nextBranch = root->right;
        otherBranch = root->left;
    }

    nearestNeighbor(nextBranch, target, depth+1, best, bestDist);

    int diff = target[cd] - root->point[cd];

    if(diff * diff < *bestDist){

        nearestNeighbor(otherBranch, target, depth+1, best, bestDist);
    }
}

int main()
{
    struct Node* root = NULL;

    int points[][2] = {
        {7,2},
        {5,4},
        {9,6},
        {2,3},
        {4,7},
        {8,1},
        {10,5}
    };

    int n = sizeof(points)/sizeof(points[0]);

    for(int i = 0; i < n; i++){

        root = insert(root, points[i], 0);
    }

    int target[2] = {9,2};

    struct Node* best = NULL;
    int bestDist = INT_MAX;

    nearestNeighbor(root, target, 0, &best, &bestDist);

    printf("Nearest Neighbor: (%d, %d)\n", best->point[0], best->point[1]);

    return 0;
}