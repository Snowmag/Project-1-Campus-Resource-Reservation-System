#include "Resource.h"
#include <alogorithm>

//Linear search by the Resource ID
Resource Resources::findResource(string ID){
    for(int i = 0; i < resourceList.size(); i++){
        if(resourceList[i].getID() == ID){
            return resourceList[i];
        }
    }
    
    //If the resource is not found, return an empty Resource
    Resource notFound;
    notFound.ID = "";
    return notFound;
}

//Merge Sort
void merge(vector<Resource>& v, int left, int mid, int right){
    vector<Resource> temp;
    int i = left, j = mid + 1;

    while(i <= mid && j <= right){
        if(v[i].Name <= v[j].Name){
            temp.push_back(v[i]);
            i++;
        } else {
            temp.push_back(v[j]);
            j++;
        }
    }
    while(i <= mid){ temp.push_back(v[i]); i++; }
    while(j <= right){ temp.push_back(v[j]); j++; }

    for(int k = 0; k < temp.size(); k++){
        v[left + k] = temp[k];
    }
}

void mergeSortHelper(vector<Resource>& v, int left, int right){
    if(left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortHelper(v, left, mid);
    mergeSortHelper(v, mid + 1, right);
    merge(v, left, mid, right);
}

list<Resource> Resources::SortList(string Type){
    vector<Resource> filtered;

    for(int i = 0; i < resourceList.size(); i++){
        if(resourceList[i].Type == Type){
            filtered.push_back(resourceList[i]);
        }
    }
    mergeSortHelper(filtered, 0, filtered.size() - 1);

    //Convert back to list<Resource> to match return type
    list<Resource> result(filtered.begin(), filtered.end());
    return result;
}