#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <bits/stdc++.h>

using namespace std;

long long getSet(int byteSize){
	int sets = byteSize * 1024;
	sets = sets / 32; //32==byte size of each line
	return sets;
}
bool isValid(vector<unsigned long long>& c, long long index){
	long long sigBit = c[index] >> 63;
	if(sigBit == 1){
		return true;
	}
	return false;
}

bool directMapping(string behavior, unsigned long long target, int byteSize, vector<unsigned long long>& c){
	int sets = getSet(byteSize);
	int totalBytes = sets * 32;
	bool hit;
	long long c_index = (target / 32) % sets;
	long long tag = target / totalBytes;
	long long c_tag = c[c_index] & ~(1ULL << 63);
	
	
	if((isValid(c, c_index)) && (tag == c_tag)){ //tests to see if it's a hit
		hit = true;
	}
	else{ //if not hit
		c[c_index] = (tag & ~(1ULL << 63)) | (1ULL << 63);
		hit = false;
	}
	return hit;	
}

bool setAssociative(string behavior, unsigned long long target, int associativity, vector<vector<unsigned long long>>& c, vector<vector<unsigned long long>>& lru) {
	//REMEMBER: ASSOCIATIVITY = LINES PER SET
	long long sets = 16 * 1024 / (32 * associativity);
	int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logSets = log2(sets);
	long long set_index = tar & ((1ULL << logSets) - 1);
	long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	long long i = 0;
	long long j = 0;
	while(i < c.at(set_index).size()){
		if(c.at(set_index).at(i) == tag) { //if tag FOUND
			hit = true;
			//updating way using lru
			while(j < lru.at(set_index).size()){
				if(lru.at(set_index).at(j) == i) {
					way = lru.at(set_index).at(j);
					lru.at(set_index).erase(lru.at(set_index).begin() + j);
					
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if(hit == false){
		//if tag NOT FOUND, set way as lru.back()
		way = lru.at(set_index).back();
		c.at(set_index).at(way) = tag;
		lru.at(set_index).pop_back();
	}

	//move way to front of lru
	lru.at(set_index).insert(lru.at(set_index).begin(), way);

	return hit;

	
	/*
	long long sets = 16 * 1024 / (32 * associativity);
	int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	long long logSets = log2(sets);
	long long set_index = target & ((1ULL << logSets) - 1);
	long long tag = target >> logSets;
	
	unsigned long long found = -1;
	int i;
	for(i = 0; i < c.size(); i++){
		if(c[i] == tag){
			found = c[i];
			hit = true;
		}
	}
	cout << "hii" << endl;
	if(hit == true){
		unsigned long long found1 = -1;
		for(int j = 0; i < lru[set_index].size(); j++){
			if(lru[set_index][j] == i){
				found1 = j;
			}
		}
		lru[set_index].erase(lru[set_index].begin()+found1);
		lru[set_index].insert(lru[set_index].begin(), i);
	}
	else{
		i = lru[set_index].size()-1;
		c[set_index*associativity+i] = tag;
		lru[set_index].pop_back();
		lru[set_index].insert(lru[set_index].begin(), i);
	}
	
	return hit;
	*/
}


bool fullyAssociative(string behavior, unsigned long long target, int cacheSize, vector<unsigned long long>& c, vector<unsigned long long>& lru) {
	//REMEMBER: ASSOCIATIVITY = LINES PER SET
	long long associativity = 16 * 1024 / (32);
	long long sets = 16 * 1024 / (32 * associativity);
	int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logSets = log2(sets);
	//long long set_index = tar & ((1ULL << logSets) - 1);
	long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	long long i = 0;
	long long j = 0;
	while(i < c.size()){
		if(c.at(i) == tag) { //if tag FOUND
			hit = true;
			//updating way using lru
			while(j < lru.size()){
				if(lru.at(j) == i) {
					way = lru.at(j);
					lru.erase(lru.begin() + j);
					
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if(hit == false) {
		//if tag NOT FOUND, set way as lru.back()
		way = lru.back();
		c.at(way) = tag;
		lru.pop_back();
	}

	//move way to front of lru
	lru.insert(lru.begin(), way);

	return hit;
}


bool fullyAssociativeHC(string behavior, unsigned long long target, int cacheSize, vector<unsigned long long>& c, vector<unsigned long long>& lru, vector<int>& hotCold) {

	long long associativity = 16 * 1024 / (32);
	//long long sets = 16 * 1024 / (32 * associativity);
	//int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logAssociativity = log2(associativity);
	//long long set_index = tar & ((1ULL << logSets) - 1);
	//long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	int i = 0;
	int j = 0;
	
	while(i < c.size()){
		int index = 0;
		if(c.at(i) == tar) { //if tag FOUND
	//cout<<"hit: "<<i<<endl;
			hit = true;
			way = i;
			index = way + associativity - 1;
			while(index >= 0){
				if(index % 2 == 1){ //check to see if left child
					if(index-1 < 0){
						break;
					}
					index = (index - 1) / 2;
					//cout<<index<<endl;
					hotCold.at(index) = 1;
				}
				else if(index % 2 == 0){ //check to see if right child
					if(index-2 < 0){
						break;
					} 
					index = (index - 2) / 2;
					hotCold.at(index) = 0;
				}
				//cout<<"while"<<endl;
				//break;
			}
			break;
		}
		i++;
	}

	if(hit == false) {
		// Cache miss
		unsigned long long replace = -1;
		long long index = 0;
		
		int a = 0;
		while(a < logAssociativity){
			if(hotCold.at(index) == 1){ //check to see if left child
				hotCold.at(index) = 0;
				index = index * 2 + 2;
				
			}
			else if(hotCold.at(index) == 0){ //check to see if right child
				hotCold.at(index) = 1;
				index = index * 2 + 1;
			}
			a++;
		}
		replace = index - (associativity - 1);
		c.at(replace) = tar;
	}
	return hit;
	
}
/*
bool fullyAssociativeHC(string behavior, unsigned long long target, int cacheSize, vector<unsigned long long>& c, vector<unsigned long long>& lru, vector<int>& hotCold) {

	long long associativity = 16 * 1024 / (32);
	//long long sets = 16 * 1024 / (32 * associativity);
	//int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logAssociativity = log2(associativity);
	//long long set_index = tar & ((1ULL << logSets) - 1);
	//long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	int i = 0;
	int j = 0;
	
	
	while(i < c.size()){
		unsigned long long index = 0;
		if(c.at(i) == tar) { //if tag FOUND
	cout<<"hit: "<<i<<endl;
			hit = true;
			way = i;
			
			
			index = way + associativity - 1;
			while(index >= 0){
				if(index % 2 == 1){ //check to see if left child
					index = (index - 1) / 2;
					hotCold.at(index) = 1;
				}
				else if(index % 2 == 0){ //check to see if right child
					index = (index - 2) / 2;
					hotCold.at(index) = 0;
				}
			
			break;
			}
		break;
		}
		i++;
	}
		
	if(hit == false) {
		unsigned long long replace=-1;
		unsigned long long index = 0;
		int a = 0;
		while(a < logAssociativity){
			if(hotCold.at(index) == 1){ //check to see if left child
				hotCold.at(index) = 0;
				hotCold.at(index) = 2 * 2 + 2;
			}
			else if(hotCold.at(index) == 0){ //check to see if right child
				hotCold.at(index) = 1;
				hotCold.at(index) = 2 * 2 + 1;
			}
			a++;
		}
		replace = index - (associativity - 1);
		c.at(replace) = tar;
	}

	//move way to front of lru
	//lru.insert(lru.begin(), way);

	return hit;
	
}
*/

bool setAssociativeWM(string behavior, unsigned long long target, int associativity, vector<vector<unsigned long long>>& c, vector<vector<unsigned long long>>& lru) {
	//REMEMBER: ASSOCIATIVITY = LINES PER SET
	long long sets = 16 * 1024 / (32 * associativity);
	int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logSets = log2(sets);
	long long set_index = tar & ((1ULL << logSets) - 1);
	long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	long long i = 0;
	long long j = 0;
	while(i < c.at(set_index).size()){
		if(c.at(set_index).at(i) == tag) { //if tag FOUND
			hit = true;
			//updating way using lru
			while(j < lru.at(set_index).size()){
				if(lru.at(set_index).at(j) == i) {
					way = lru.at(set_index).at(j);
					lru.at(set_index).erase(lru.at(set_index).begin() + j);
					lru.at(set_index).insert(lru.at(set_index).begin(), way);
					
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if((hit == false) && (behavior == "L")){
		//if tag NOT FOUND, set way as lru.back()
		way = lru.at(set_index).back();
		c.at(set_index).at(way) = tag;
		lru.at(set_index).pop_back();
		//move way to front of lru
		lru.at(set_index).insert(lru.at(set_index).begin(), way);
	}

	

	return hit;
}

bool setAssociativePF(string behavior, unsigned long long target, int associativity, vector<vector<unsigned long long>>& c, vector<vector<unsigned long long>>& lru) {
	//REMEMBER: ASSOCIATIVITY = LINES PER SET
	long long sets = 16 * 1024 / (32 * associativity);
	int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logSets = log2(sets);
	long long set_index = tar & ((1ULL << logSets) - 1);
	long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	long long i = 0;
	long long j = 0;
	while(i < c.at(set_index).size()){
		if(c.at(set_index).at(i) == tag) { //if tag FOUND
			hit = true;
			//updating way using lru
			while(j < lru.at(set_index).size()){
				if(lru.at(set_index).at(j) == i) {
					way = lru.at(set_index).at(j);
					lru.at(set_index).erase(lru.at(set_index).begin() + j);
					
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if(hit == false){
		//if tag NOT FOUND, set way as lru.back()
		way = lru.at(set_index).back();
		c.at(set_index).at(way) = tag;
		lru.at(set_index).pop_back();
	}

	//move way to front of lru
	lru.at(set_index).insert(lru.at(set_index).begin(), way);

	
	//prefetch part
	long long prefetch_address = target + 32;
	long long prefetch_index = (prefetch_address >> 5) & ((1ULL << logSets) - 1);
	long long prefetch_tag = (prefetch_address >> 5) >> logSets;
	
	long long prefetch_way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	i = 0;
	j = 0;
	bool check = false;
	while(i < c.at(prefetch_index).size()){
		if(c.at(prefetch_index).at(i) == prefetch_tag) { //if tag FOUND
			check = true;
			//updating way using lru
			while(j < lru.at(prefetch_index).size()){
				if(lru.at(prefetch_index).at(j) == i) {
					prefetch_way = lru.at(prefetch_index).at(j);
					lru.at(prefetch_index).erase(lru.at(prefetch_index).begin() + j);
					lru.at(prefetch_index).insert(lru.at(prefetch_index).begin(), prefetch_way);
					
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if(check == false){
		//if tag NOT FOUND, set way as lru.back()
		prefetch_way = lru.at(prefetch_index).back();
		c.at(prefetch_index).at(prefetch_way) = prefetch_tag;
		lru.at(prefetch_index).pop_back();
		lru.at(prefetch_index).insert(lru.at(prefetch_index).begin(), prefetch_way);
	}

	//move way to front of lru
	//lru.at(prefetch_index).insert(lru.at(prefetch_index).begin(), way);
	
	return hit;
}

bool setAssociativePFM(string behavior, unsigned long long target, int associativity, vector<vector<unsigned long long>>& c, vector<vector<unsigned long long>>& lru) {
	//REMEMBER: ASSOCIATIVITY = LINES PER SET
	long long sets = 16 * 1024 / (32 * associativity);
	int totalBytes = sets * associativity * 32;
	bool hit = false;
	
	unsigned long long tar = target >> 5;
	long long logSets = log2(sets);
	long long set_index = tar & ((1ULL << logSets) - 1);
	long long tag = tar >> logSets;
	
	long long way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	long long i = 0;
	long long j = 0;
	while(i < c.at(set_index).size()){
		if(c.at(set_index).at(i) == tag) { //if tag FOUND
			hit = true;
			//updating way using lru
			while(j < lru.at(set_index).size()){
				if(lru.at(set_index).at(j) == i) {
					way = lru.at(set_index).at(j);
					lru.at(set_index).erase(lru.at(set_index).begin() + j);
					lru.at(set_index).insert(lru.at(set_index).begin(), way);
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if(hit == false){
		//if tag NOT FOUND, set way as lru.back()
		way = lru.at(set_index).back();
		c.at(set_index).at(way) = tag;
		lru.at(set_index).pop_back();
		//move way to front of lru
		lru.at(set_index).insert(lru.at(set_index).begin(), way);
		
		//prefetch part
	long long prefetch_address = target + 32;
	long long prefetch_index = (prefetch_address >> 5) & ((1ULL << logSets) - 1);
	long long prefetch_tag = (prefetch_address >> 5) >> logSets;
	
	long long prefetch_way; //represents the line that should be updated
	//find tag in vector of c[set_index]
	i = 0;
	j = 0;
	bool check = false;
	while(i < c.at(prefetch_index).size()){
		if(c.at(prefetch_index).at(i) == prefetch_tag) { //if tag FOUND
			check = true;
			//updating way using lru
			while(j < lru.at(prefetch_index).size()){
				if(lru.at(prefetch_index).at(j) == i) {
					prefetch_way = lru.at(prefetch_index).at(j);
					lru.at(prefetch_index).erase(lru.at(prefetch_index).begin() + j);
					lru.at(prefetch_index).insert(lru.at(prefetch_index).begin(), prefetch_way);
					
					break;
				}
				j++;
			}
			break;
		}
		i++;
	}
		
	if(check == false){
		//if tag NOT FOUND, set way as lru.back()
		prefetch_way = lru.at(prefetch_index).back();
		c.at(prefetch_index).at(prefetch_way) = prefetch_tag;
		lru.at(prefetch_index).pop_back();
		lru.at(prefetch_index).insert(lru.at(prefetch_index).begin(), prefetch_way);
	}
	}


	//move way to front of lru
	//lru.at(prefetch_index).insert(lru.at(prefetch_index).begin(), way);
	
	return hit;
}


int main(int argc, char *argv[]) {
	// Temporary variables
	//unsigned long long addr;
	string behavior, line;
	unsigned long long target;

	// Open file for reading
	ifstream infile(argv[1]);
	ofstream outfile(argv[2]);
	
	//1
	long long total = 0;
	
	long long directMappingHit1 = 0;
	long long directMappingHit4 = 0;
	long long directMappingHit16 = 0;
	long long directMappingHit32 = 0;
	vector<unsigned long long> directMapping1(1*1024, 0);
	vector<unsigned long long> directMapping4(4*1024, 0);
	vector<unsigned long long> directMapping16(16*1024, 0);
	vector<unsigned long long> directMapping32(32*1024, 0);
	
	
	
	//2
	long long setAssociativeHit2 = 0;
	long long setAssociativeHit4 = 0;
	long long setAssociativeHit8 = 0;
	long long setAssociativeHit16 = 0;
	vector<vector<unsigned long long>> setAssociative2((16*1024/(32*2)), vector<unsigned long long>(2, 0));	//1024 = 512 * 2
	vector<vector<unsigned long long>> setAssociative4((16*1024/(32*2)), vector<unsigned long long>(4, 0));	//1024 = 256 * 4
	vector<vector<unsigned long long>> setAssociative8((16*1024/(32*2)), vector<unsigned long long>(8, 0));	//1024 = 128 * 8
	vector<vector<unsigned long long>> setAssociative16((16*1024/(32*2)), vector<unsigned long long>(16, 0));	//1024 = 64 * 16
	
	vector<vector<unsigned long long>> setAssociativeLRU2((16 * 1024 / (32 * 2)), vector<unsigned long long>(2, 0)); //2 lines per set
	vector<vector<unsigned long long>> setAssociativeLRU4((16 * 1024 / (32 * 4)), vector<unsigned long long>(4, 0)); //4 lines per set
	vector<vector<unsigned long long>> setAssociativeLRU8((16 * 1024 / (32 * 8)), vector<unsigned long long>(8, 0)); //8 lines per set
	vector<vector<unsigned long long>> setAssociativeLRU16((16 * 1024 / (32 * 16)), vector<unsigned long long>(16, 0)); //16 lines per set
	
	for(int i = 1; i <= 16; i*=2){
		if(i = 2){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRU2.at(a).at(b) = setAssociativeLRU2.at(a).size() - b - 1;
				}
			}
		}
		if(i = 4){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRU4.at(a).at(b) = setAssociativeLRU4.at(a).size() - b - 1;
				}
			}
		}
		if(i = 8){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRU8.at(a).at(b) = setAssociativeLRU8.at(a).size() - b - 1;
				}
			}
		}
		if(i = 16){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRU16.at(a).at(b) = setAssociativeLRU16.at(a).size() - b - 1;
				}
			}
		}
	}
	
	//3
	long long fullyAssociativeHit = 0;
	vector<unsigned long long> fullyAssociativeVector((16*1024/(32)));
	vector<unsigned long long> fullyAssociativeLRU((16 * 1024 / (32)));
	for(int a = 0; a < fullyAssociativeLRU.size(); a++){
		fullyAssociativeLRU.at(a) = fullyAssociativeLRU.size() - a - 1;
	}
	
	
	//3b
	long long fullyAssociativeHCHit = 0;
	vector<unsigned long long> fullyAssociativeHCVector((16*1024/(32)),0);
	vector<unsigned long long> fullyAssociativeHCLRU((16 * 1024 / (32)));
	vector<int> hotCold((16*1024/(32)), 0);
	for(int a = 0; a < fullyAssociativeHCLRU.size(); a++){
		fullyAssociativeHCLRU.at(a) = fullyAssociativeHCLRU.size() - a - 1;
	}
	
	//4
	long long setAssociativeHitWM2 = 0;
	long long setAssociativeHitWM4 = 0;
	long long setAssociativeHitWM8 = 0;
	long long setAssociativeHitWM16 = 0;
	vector<vector<unsigned long long>> setAssociativeWM2((16*1024/(32*2)), vector<unsigned long long>(2, 0));	//1024 = 512 * 2
	vector<vector<unsigned long long>> setAssociativeWM4((16*1024/(32*2)), vector<unsigned long long>(4, 0));	//1024 = 256 * 4
	vector<vector<unsigned long long>> setAssociativeWM8((16*1024/(32*2)), vector<unsigned long long>(8, 0));	//1024 = 128 * 8
	vector<vector<unsigned long long>> setAssociativeWM16((16*1024/(32*2)), vector<unsigned long long>(16, 0));	//1024 = 64 * 16
	
	vector<vector<unsigned long long>> setAssociativeLRUWM2((16 * 1024 / (32 * 2)), vector<unsigned long long>(2, 0)); //2 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUWM4((16 * 1024 / (32 * 4)), vector<unsigned long long>(4, 0)); //4 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUWM8((16 * 1024 / (32 * 8)), vector<unsigned long long>(8, 0)); //8 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUWM16((16 * 1024 / (32 * 16)), vector<unsigned long long>(16, 0)); //16 lines per set
	
	for(int i = 1; i <= 16; i*=2){
		if(i = 2){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUWM2.at(a).at(b) = setAssociativeLRUWM2.at(a).size() - b - 1;
				}
			}
		}
		if(i = 4){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUWM4.at(a).at(b) = setAssociativeLRUWM4.at(a).size() - b - 1;
				}
			}
		}
		if(i = 8){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUWM8.at(a).at(b) = setAssociativeLRUWM8.at(a).size() - b - 1;
				}
			}
		}
		if(i = 16){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUWM16.at(a).at(b) = setAssociativeLRUWM16.at(a).size() - b - 1;
				}
			}
		}
	}
	
	//5
	long long setAssociativeHitPF2 = 0;
	long long setAssociativeHitPF4 = 0;
	long long setAssociativeHitPF8 = 0;
	long long setAssociativeHitPF16 = 0;
	vector<vector<unsigned long long>> setAssociativePF2((16*1024/(32*2)), vector<unsigned long long>(2, 0));	//1024 = 512 * 2
	vector<vector<unsigned long long>> setAssociativePF4((16*1024/(32*2)), vector<unsigned long long>(4, 0));	//1024 = 256 * 4
	vector<vector<unsigned long long>> setAssociativePF8((16*1024/(32*2)), vector<unsigned long long>(8, 0));	//1024 = 128 * 8
	vector<vector<unsigned long long>> setAssociativePF16((16*1024/(32*2)), vector<unsigned long long>(16, 0));	//1024 = 64 * 16
	
	vector<vector<unsigned long long>> setAssociativeLRUPF2((16 * 1024 / (32 * 2)), vector<unsigned long long>(2, 0)); //2 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUPF4((16 * 1024 / (32 * 4)), vector<unsigned long long>(4, 0)); //4 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUPF8((16 * 1024 / (32 * 8)), vector<unsigned long long>(8, 0)); //8 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUPF16((16 * 1024 / (32 * 16)), vector<unsigned long long>(16, 0)); //16 lines per set
	
	for(int i = 1; i <= 16; i*=2){
		if(i = 2){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPF2.at(a).at(b) = setAssociativeLRUPF2.at(a).size() - b - 1;
				}
			}
		}
		if(i = 4){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPF4.at(a).at(b) = setAssociativeLRUPF4.at(a).size() - b - 1;
				}
			}
		}
		if(i = 8){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPF8.at(a).at(b) = setAssociativeLRUPF8.at(a).size() - b - 1;
				}
			}
		}
		if(i = 16){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPF16.at(a).at(b) = setAssociativeLRUPF16.at(a).size() - b - 1;
				}
			}
		}
	}
	
	//6
	long long setAssociativeHitPFM2 = 0;
	long long setAssociativeHitPFM4 = 0;
	long long setAssociativeHitPFM8 = 0;
	long long setAssociativeHitPFM16 = 0;
	vector<vector<unsigned long long>> setAssociativePFM2((16*1024/(32*2)), vector<unsigned long long>(2, 0));	//1024 = 512 * 2
	vector<vector<unsigned long long>> setAssociativePFM4((16*1024/(32*2)), vector<unsigned long long>(4, 0));	//1024 = 256 * 4
	vector<vector<unsigned long long>> setAssociativePFM8((16*1024/(32*2)), vector<unsigned long long>(8, 0));	//1024 = 128 * 8
	vector<vector<unsigned long long>> setAssociativePFM16((16*1024/(32*2)), vector<unsigned long long>(16, 0));	//1024 = 64 * 16
	
	vector<vector<unsigned long long>> setAssociativeLRUPFM2((16 * 1024 / (32 * 2)), vector<unsigned long long>(2, 0)); //2 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUPFM4((16 * 1024 / (32 * 4)), vector<unsigned long long>(4, 0)); //4 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUPFM8((16 * 1024 / (32 * 8)), vector<unsigned long long>(8, 0)); //8 lines per set
	vector<vector<unsigned long long>> setAssociativeLRUPFM16((16 * 1024 / (32 * 16)), vector<unsigned long long>(16, 0)); //16 lines per set
	
	for(int i = 1; i <= 16; i*=2){
		if(i = 2){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPFM2.at(a).at(b) = setAssociativeLRUPFM2.at(a).size() - b - 1;
				}
			}
		}
		if(i = 4){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPFM4.at(a).at(b) = setAssociativeLRUPFM4.at(a).size() - b - 1;
				}
			}
		}
		if(i = 8){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPFM8.at(a).at(b) = setAssociativeLRUPFM8.at(a).size() - b - 1;
				}
			}
		}
		if(i = 16){
			for(int a = 0; a < (16 * 1024 / (32 * i)); a++){
				for(int b = 0; b < i; b++){
					setAssociativeLRUPFM16.at(a).at(b) = setAssociativeLRUPFM16.at(a).size() - b - 1;
				}
			}
		}
	}
	
	
	
	// The following loop will read a line at a time
	while(getline(infile, line)) {
		// Now we have to parse the line into it's two pieces
		stringstream s(line);
		s >> behavior >> std::hex >> target;
		// Now we can output it
		//cout << addr;
		/*
		if(behavior == "L") {
		cout << " -> load, ";
		}else {
		cout << " -> store, ";
		}
		cout << "target=" << target << endl;
		*/
		
		//1
		if(directMapping(behavior, target, 1, directMapping1) == true){
			directMappingHit1++;
		}
		if(directMapping(behavior, target, 4, directMapping4) == true){
			directMappingHit4++;
		}
		if(directMapping(behavior, target, 16, directMapping16) == true){
			directMappingHit16++;
		}
		if(directMapping(behavior, target, 32, directMapping32) == true){
			directMappingHit32++;
		}
		
		
		//2
		if(setAssociative(behavior, target, 2, setAssociative2, setAssociativeLRU2) == true){
			setAssociativeHit2++;
		}
		if(setAssociative(behavior, target, 4, setAssociative4, setAssociativeLRU4) == true){
			setAssociativeHit4++;
		}
		if(setAssociative(behavior, target, 8, setAssociative8, setAssociativeLRU8) == true){
			setAssociativeHit8++;
		}
		if(setAssociative(behavior, target, 16, setAssociative16, setAssociativeLRU16) == true){
			setAssociativeHit16++;
		}
		
		//3
		if(fullyAssociative(behavior, target, 2, fullyAssociativeVector, fullyAssociativeLRU) == true){
			fullyAssociativeHit++;
		}
		
		//3b
		if(fullyAssociativeHC(behavior, target, 2, fullyAssociativeHCVector, fullyAssociativeLRU, hotCold) == true){
			fullyAssociativeHCHit++;
		}
		//4
		if(setAssociativeWM(behavior, target, 2, setAssociativeWM2, setAssociativeLRUWM2) == true){
			setAssociativeHitWM2++;
		}
		if(setAssociativeWM(behavior, target, 4, setAssociativeWM4, setAssociativeLRUWM4) == true){
			setAssociativeHitWM4++;
		}
		if(setAssociativeWM(behavior, target, 8, setAssociativeWM8, setAssociativeLRUWM8) == true){
			setAssociativeHitWM8++;
		}
		if(setAssociativeWM(behavior, target, 16, setAssociativeWM16, setAssociativeLRUWM16) == true){
			setAssociativeHitWM16++;
		}
		//5
		if(setAssociativePF(behavior, target, 2, setAssociativePF2, setAssociativeLRUPF2) == true){
			setAssociativeHitPF2++;
		}
		if(setAssociativePF(behavior, target, 4, setAssociativePF4, setAssociativeLRUPF4) == true){
			setAssociativeHitPF4++;
		}
		if(setAssociativePF(behavior, target, 8, setAssociativePF8, setAssociativeLRUPF8) == true){
			setAssociativeHitPF8++;
		}
		if(setAssociativePF(behavior, target, 16, setAssociativePF16, setAssociativeLRUPF16) == true){
			setAssociativeHitPF16++;
		}
		//6
		if(setAssociativePFM(behavior, target, 2, setAssociativePFM2, setAssociativeLRUPFM2) == true){
			setAssociativeHitPFM2++;
		}
		if(setAssociativePFM(behavior, target, 4, setAssociativePFM4, setAssociativeLRUPFM4) == true){
			setAssociativeHitPFM4++;
		}
		if(setAssociativePFM(behavior, target, 8, setAssociativePFM8, setAssociativeLRUPFM8) == true){
			setAssociativeHitPFM8++;
		}
		if(setAssociativePFM(behavior, target, 16, setAssociativePFM16, setAssociativeLRUPFM16) == true){
			setAssociativeHitPFM16++;
		}
		
		total++;
	
	}
	
	//1
	outfile << directMappingHit1 << "," << total << "; " << directMappingHit4 << "," << total << "; " << directMappingHit16 << "," << total << "; " << directMappingHit32 << "," << total << ";" << endl;
	//2
	outfile << setAssociativeHit2 << "," << total << "; " << setAssociativeHit4 << "," << total << "; " << setAssociativeHit8 << "," << total << "; " << setAssociativeHit16 << "," << total << ";" << endl;
	//3
	outfile << fullyAssociativeHit << "," << total << "; " << endl;
	//3b
	outfile << fullyAssociativeHCHit << "," << total << "; " << endl;
	//4
	outfile << setAssociativeHitWM2 << "," << total << "; " << setAssociativeHitWM4 << "," << total << "; " << setAssociativeHitWM8 << "," << total << "; " << setAssociativeHitWM16 << "," << total << ";" << endl;
	//5
	outfile << setAssociativeHitPF2 << "," << total << "; " << setAssociativeHitPF4 << "," << total << "; " << setAssociativeHitPF8 << "," << total << "; " << setAssociativeHitPF16 << "," << total << ";" << endl;
	//6
	outfile << setAssociativeHitPFM2 << "," << total << "; " << setAssociativeHitPFM4 << "," << total << "; " << setAssociativeHitPFM8 << "," << total << "; " << setAssociativeHitPFM16 << "," << total << ";" << endl;
	

	return 0;
}
