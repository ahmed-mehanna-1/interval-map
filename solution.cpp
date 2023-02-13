#include <iostream>
#include <map>
#include <vector>
#include <limits>


void print_lower_upper(std::map<int, char> m, int k) {
    auto it = m.lower_bound(k);
    std::cout << "Lower bound of " << k << ": " << it->first << std::endl;
    it = m.upper_bound(k);
    std::cout << "Upper bound of " << k << ": " << (--it)->first << std::endl << std::endl;
}

template<typename K, typename V>
class interval_map {
	friend void IntervalMapTest();
	V m_valBegin;
	std::map<K,V> m_map;

public:
	interval_map(V const& val) :m_valBegin(val)
	{}

    void print_map() {
        for (const std::pair<K, V>& data : m_map)
            std::cout << "Key: " << data.first << "  Val: " << char(data.second) << std::endl;
        std::cout << std::endl;
    }

	void assign( K const& keyBegin, K const& keyEnd, V const& val ) {
        /*
            - Functions used and their time complexity
                1- size() -> O(1)
                2- insert() -> O(log(n))
                3- lower_bound() -> O(log(n))
                4- upper_bound() -> O(log(n))
                5- prev() -> O(1)
                6- next() -> O(1)
                7- insert_or_assign() -> O(log(n))
                8- begin() -> O(1)
                9- end() -> O(1)
                10- erase() -> O(log(n))
            
            - Logic:
                1- Check if the keyBegin is NOT less than keyEnd
                2- Check if the map size is equal to zero which means this is the first interval
                3- Create 4 pointers to insert a new interval
                    - pointer to point on the lower bound of keyBegin
                    - pointer to point on the previous element of the pointer of keyBegin
                    - pointer to point on the upper bound of keyEnd
                    - pointer to point on the previous element of the pointer of keyEnd
                4- Check if the interval is before the first element and its value is equal to the default value
                    - Don't do anything, just leave
                5- Check if the value of the prevEnd is not equal to the value we want to insert
                    - Check if the keyEnd is equal to the key of the prevEnd
                        - As the only allowed operator is the < so we can check the equality by two conditions
                            - if NOT x < y AND NOT y < x so this means that x == y
                        - Now just we will update the ptrEnd to be prevEnd
                    - Check if the ptrEnd is the begining of the map
                        - Just insert the default value
                    - Otherwise, insert the prevEnd value
                6- Check if the ptrBegin is not pointing on the beingin of the map
                    - Check if the prevBegin value is not equal to the value we want to insert
                        - Just insert the val
                    - Otherwise, just update ptrBegin to be prevBegin
                7- Check if the value we want to insert is not equal to the default value
                    - Just insert the new value
                8- Check if the ptrBegin is not pointing on the end of the map
                    - Erase all of the elements between ptrBegin and ptrEnd
        */

        // Check the validity of the interval
        if (!(keyBegin < keyEnd))
            return;
        
        // As the map is empty you can just insert without doing anything else
        if (m_map.size() == 0) {
            m_map.insert(std::make_pair(keyBegin, val));
            m_map.insert(std::make_pair(keyEnd, m_valBegin));
            return;
        }

        // Create the 4 pointers these will help us while inserting and deleting
        // Make sure that we you use 'prev' or 'next' instead of use lower_bound or upper_bound to avoid unnecessary operation that may take O(log(n))
        typename std::map<K, V>::iterator ptrBegin = m_map.lower_bound(keyBegin);
        typename std::map<K, V>::iterator prevBegin = std::prev(ptrBegin);
        typename std::map<K, V>::iterator ptrEnd = m_map.upper_bound(keyEnd);
        typename std::map<K, V>::iterator prevEnd = std::prev(ptrEnd);

        // Check if the end of the interval at begining of the map and the value is the default value, then just don't do anything
        if (val == m_valBegin && prevBegin == prevEnd && prevBegin == m_map.end())
            return;
        
        // Insert the end of the interval
        // Check if the place that we will insert at end is not equal to the same value we want to insert
        if (!(prevEnd->second == val)) {
            // If the key of the place is equal to the keyEnd, then just update the ptrEnd to be used in the erase operation
            if (!(prevEnd->first < keyEnd) && !(keyEnd < prevEnd->first))
                ptrEnd = prevEnd;
            // If the ptrEnd is equal to the begin of the map then we have to insert the default value and update ptrEnd
            else if (ptrEnd == m_map.begin())
                ptrEnd = m_map.insert_or_assign(ptrEnd, keyEnd, m_valBegin);
            // Otherwise, just insert the value of the prevEnd and update ptrEnd
            else
                ptrEnd = m_map.insert_or_assign(ptrEnd, keyEnd, prevEnd->second);
        }

        // Insert the begin of the interval
        // Check if the place that we will insert at begin is not equal to the begin of the map
        if (ptrBegin != m_map.begin()) {
            // If the prevBegin value is not equal to the value we want to insert, just insert the new value and update ptrBegin
            if (!(prevBegin->second == val))
                ptrBegin = m_map.insert_or_assign(ptrBegin, keyBegin, val);
            // Otherwise, just update the ptrBegin to be used in the erase operation
            else
                ptrBegin = prevBegin;
            // Change the ptrBegin to be pointing on the next element after insert to avoid remove it in the erase
            ptrBegin = std::next(ptrBegin);
        }
        // If the value is not equal to the default value, then just insert the new value and update the ptrBegin as described above
        else if (!(val == m_valBegin)) {
            ptrBegin = m_map.insert_or_assign(ptrBegin, keyBegin, val);
            ptrBegin = std::next(ptrBegin);
        }
        
        // Do the erase operation if the ptrBegin is not pointing at the end of the map
        if (ptrBegin != m_map.end())
            m_map.erase(ptrBegin, ptrEnd);

    }

	// look-up of the value associated with key
	V const& operator[]( K const& key ) const {
		auto it=m_map.upper_bound(key);
		if(it==m_map.begin()) {
			return m_valBegin;
		} else {
			return (--it)->second;
		}
	}
};

class TestKey {     
    public:
    int key;
    TestKey(int const& val) :key(val)
	{}
    friend bool operator< ( TestKey const& a, TestKey const& b);
};

class TestVal {
    public:
    int val;
    TestVal(int const& val) :val(val)
    {}
    friend bool operator== (TestVal const& a, TestVal const& b);
};

bool operator< (TestKey const& lhs, TestKey const& rhs) {
    return lhs.key < rhs.key;
}

bool operator== (TestVal const& a, TestVal const& b) {
    return a.val == b.val;
}

int main() {
    // interval_map<TestKey, TestVal> M('A');
    // M = test(M, TestKey(0), TestKey(3), TestVal('B'));
    // M = test(M, TestKey(3), TestKey(8), TestVal('C'));

    interval_map<int, char> m('A');
    m.assign(0, 6, 'B');
    m.assign(2, 5, 'C');
    m.assign(4, 7, 'A');

    m.print_map();
    std::cout << std::endl;
    for(int i = -5; i <= 12; i++)
        std::cout << "Key: " << i << "  Value: " << char(m[i]) << std::endl;
    std::cout << std::endl;


    std::cout << "End of code" << std::endl;

    return 0;
}