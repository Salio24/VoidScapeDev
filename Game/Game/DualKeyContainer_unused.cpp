//void DualKeyContainer::insert(int k1, GLenum k2, int v1, int v2) {
//    // Check and remove existing entry with key1
//    auto it1 = key1Map.find(k1);
//    if (it1 != key1Map.end()) {
//        removeEntry(it1->second);
//    }
//
//    // Check and remove existing entry with key2
//    auto it2 = key2Map.find(k2);
//    if (it2 != key2Map.end()) {
//        removeEntry(it2->second);
//    }
//
//    // Add new entry
//    entries.emplace_back(Entry{ k1, k2, v1, v2 });
//    auto newEntryIt = std::prev(entries.end());
//    key1Map[k1] = newEntryIt;
//    key2Map[k2] = newEntryIt;
//}
//
//DualKeyContainer::Entry* DualKeyContainer::find(int key) {
//    auto it = key1Map.find(key);
//    return (it != key1Map.end()) ? &(*it->second) : nullptr;
//}
//
//// Finds an entry by the enum key
//DualKeyContainer::Entry* DualKeyContainer::find(GLenum key) {
//    auto it = key2Map.find(key);
//    return (it != key2Map.end()) ? &(*it->second) : nullptr;
//}
//
//void DualKeyContainer::removeEntry(EntryList::iterator entryIt) {
//    key1Map.erase(entryIt->key1);
//    key2Map.erase(entryIt->key2);
//    entries.erase(entryIt);
//}

//class DualKeyContainer {
//public:
//	struct Entry {
//		int key1;
//		GLenum key2;
//		int value1;
//		int value2;
//	};
//
//	void insert(int k1, GLenum k2, int v1, int v2);
//
//	Entry* find(int key);
//
//	Entry* find(GLenum key);
//
//private:
//	using EntryList = std::list<Entry>;
//	EntryList entries;
//
//	std::unordered_map<int, EntryList::iterator> key1Map;
//	std::unordered_map<GLenum, EntryList::iterator> key2Map;
//
//	void removeEntry(EntryList::iterator entryIt);
//
//};