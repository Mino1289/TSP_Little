#include <stdio.h>
#include <stdbool.h>

#define NBR_TOWNS 6

bool createsSubTour(int next_town[], int start_index, int start_value) {
    bool visited[NBR_TOWNS] = {false};
    int current = start_index;
    
    // First check if there's already a cycle starting from start_index
    while (current != -1 && !visited[current]) {
        visited[current] = true;
        current = next_town[current];
    }
    if (current != -1) {
        return true;
    }

    // Reset visited array
    for (int i = 0; i < NBR_TOWNS; i++) {
        visited[i] = false;
    }
    
    // Check if adding the connection start_index -> start_value creates a cycle
    visited[start_index] = true;
    current = start_value;
    while (current != -1 && !visited[current]) {
        visited[current] = true;
        current = next_town[current];
    }
    return current != -1;
}

int main() {
    int next_town[NBR_TOWNS] = {2, -1, -1, -1, 3, 4};

    int index = 2; // Example index
    int value = 5; // Example value

    if (createsSubTour(next_town, index, value)) {
        printf("Sub-tour created starting at index %d with value %d\n", index, value);
    } else {
        printf("No sub-tour created starting at index %d with value %d\n", index, value);
    }

    return 0;
}