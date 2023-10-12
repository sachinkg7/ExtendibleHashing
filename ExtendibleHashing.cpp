/*

DBMS Extendible Hashing

Name - Sachin Kumar Gupta
Branch - CSE(BTech)
Roll No - 21075074

*/
#include <iostream>
#include <algorithm>
#include <vector>
#include <bits/stdc++.h>
using namespace std;
// A structure(bucket) to hold the keys values values
class Bucket
{
public:
    int local_depth, capacity;
    set<int> data;
    // to store the keys in particular bucket

    Bucket(int local_depth, int capacity)
    {
        this->capacity = capacity;
        this->local_depth = local_depth;
    }
    /*
    Inserting already present values in the bucket will return -1
    or if splitting would be required 0 will be returned,
    1 will be returned if the operation is successful without splitting
    */
    int insert(int key)
    {
        if (data.count(key))
            return -1;
        if ((int)data.size() - capacity == 0)
            return 0;
        data.insert(key);
        return 1;
    }
    // to check if the size of the Bucket is full or not
    bool isFull()
    {
        return (data.size() == capacity);
    }
    // Removing elements
    int remove(int key)
    {
        auto itr = data.lower_bound(key);
        if (itr != data.end())
        {
            data.erase(data.find(key));
            return 1;
        }
        return -1;
    }
};
// Directory Data Structure to keep all the buckets
class Directory
{
private:
    int global_depth, capacity;
    deque<Bucket *> bucket;

public:
    Directory(int depth, int capacity)
    {
        this->capacity = capacity;
        this->global_depth = depth;
        for (int j = 0; j < (1 << global_depth); j++)
            bucket.emplace_back(new Bucket(1, capacity));
    }
    // hash function to hash the keys
    int hash(int number_of_bits, int key)
    {
        return (key & ((1ll << number_of_bits) - 1));
    }
    void insert(int key)
    {
        int index = hash(global_depth, key);
        int local_depth = bucket[index]->local_depth;
        int status = bucket[index]->insert(key);
        if (status == 0)
        {
            // bucket size full so
            // splitting needs to be done
            if (global_depth == local_depth)
            {
                // double the directory size
                for (int i = 0; i < (1 << global_depth); i++)
                    bucket.emplace_back(bucket[i]);
                global_depth = global_depth + 1;
            }
            // new indices of the bucket that will be created after splitting
            auto newIndex1 = hash(local_depth + 1, index);
            auto newIndex2 = newIndex1 ^ (1 << local_depth);
            Bucket *temp = bucket[newIndex1];
            Bucket *temp1 = new Bucket(local_depth + 1, capacity);
            Bucket *temp2 = new Bucket(local_depth + 1, capacity);
            int size = (1 << global_depth);
            int i = 0;
            while (i < size)
            {
                int mask = (i & ((1 << (local_depth + 1)) - 1));
                bucket[i] = (mask == newIndex1) ? temp1 : bucket[i];
                bucket[i] = (mask == newIndex2) ? temp2 : bucket[i];
                i++;
            }
            // inserted the keys of that bucket that got splitted
            for (auto keys : temp->data)
                insert(keys);
            // clearing the memory occupied by irrelevant pointers
            delete temp;
            // insert the key as it was not inserted due to splitting
            insert(key);
        }
        else if (status == -1)
            printf("Key already exits!\n");
    }
    void remove(int key, bool deleted = 0)
    {
        int index = hash(global_depth, key);
        int status = 0;
        if (!deleted) // if not already deleted then delete it
            status = bucket[index]->remove(key);
        if (status == -1)
        {
            printf("Error: Key is not present!\n");
            return;
        }
        // merging of the buckets if the size of two small buckets
        // is less than that of bucket capacity
        // also decreasing the local depth
        int local_depth = bucket[index]->local_depth;
        int corresponding_index = index ^ (1 << global_depth - 1);
        // if merging is not possible, do nothing
        if (bucket[index]->data.size() + bucket[corresponding_index]->data.size() > capacity)
            return;
        Bucket *temp1 = bucket[index], *temp2 = bucket[corresponding_index];
        // merging the buckets into a bucket with less local depth
        Bucket *merged_bucket = new Bucket(local_depth - 1, capacity);
        for (auto keys : temp1->data)
            merged_bucket->insert(keys);
        for (auto keys : temp2->data)
            merged_bucket->insert(keys);
        // clearing the memory occupied by irrelevant pointers
        delete temp1, temp2;
        bucket[index] = merged_bucket;
        bucket[corresponding_index] = merged_bucket;
        // Shrinking of the buckets
        // if all the buckets have the local_depth less than global depth,
        // we can reduce the global depth
        bool all_are_less_than_global_depth = 1;
        int size = global_depth;
        int i = 0;
        while (i < size)
        {
            if (bucket[i]->local_depth >= global_depth)
                all_are_less_than_global_depth = 0;
            i++;
        }
        if (!all_are_less_than_global_depth)
            return;
        global_depth--;
        // reducting the directory size as global depth is decreased
        i = 0;
        while (i < size)
        {
            bucket.pop_back();
            i++;
        }
        // recursive shrinking and merging
        if (global_depth > 0)
            remove(key, 1);
        return;
    }
    void Display_the_data()
    {
        printf("*********************************************************\n");
        printf("DIRECTORY WITH GLOBAL DEPTH: %d\n\n", global_depth);
        for (int i = 0; i < (1 << global_depth); i++)
        {
            printf("---------------------------------\n");
            printf("\tLOCAL DEPTH: %d\n", bucket[i]->local_depth);
            printf("\t");
            for (int j = global_depth - 1; j >= 0; j--)
            {
                int mask = (1 << j);
                if (mask & i)
                    printf("1");
                else
                    printf("0");
            }
            printf("\n\tValues are: [ ");
            for (auto &values : bucket[i]->data)
                printf("%d ", values);
            printf("]\n");
        }
        printf("*********************************************************\n");
    }
    ~Directory()
    {
        bucket.clear();
    }
};
void menu() // Menu
{
    printf(">>> Enter 1 for Insertion\n");
    printf(">>> Enter 2 for Deletion\n");
    printf(">>> Enter 3 to Display the stored data\n");
    printf(">>> Enter 4 to Exit\n");
    return;
}
int Extendible_Hashing(int global_depth, int bucket_capacity)
{
    string val;
    auto dir = new Directory(global_depth, bucket_capacity);
    // int insertion[] = {45, 56, 60, 72, 12, 31, 29, 57, 11};
    // for (auto x : insertion)
    //     dir->insert(x);
    // int deletion[] = {45, 56, 12, 31, 11};
    // for (auto x : deletion)
    //     dir->remove(x, 0);
    while (true)
    {
        menu();
        cin >> val;
        if (val == "1") // Insertion
        {
            printf("Enter the key to be inserted\n");
            int ins;
            cin >> ins;
            dir->insert(ins);
        }
        else if (val == "2") // Deletion
        {
            printf("Enter the key to be deleted\n");
            int del;
            cin >> del;
            dir->remove(del, 0);
        }
        else if (val == "3") // Display the Data
            dir->Display_the_data();
        else if (val == "4") // exit
        {
            printf("Bye!\n");
            break;
        }
        else
            printf("Wrong Choice! Please Try Again!\n");
    }
    return 0;
}
signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int global_depth = 1, bucket_capacity = 2;
    printf("Enter the Global Depth:\n");
    cin >> global_depth;
    printf("Enter the Bucket Capacity:\n");
    cin >> bucket_capacity;
    Extendible_Hashing(global_depth, bucket_capacity);
    return 0;
}