#ifndef __SCYTHE_LRU_CACHE_H__
#define __SCYTHE_LRU_CACHE_H__

#include <list>
#include <unordered_set>
#include <unordered_map>

namespace scythe {

template<typename Key, typename Value>
class LruCache {
public:
	typedef typename std::pair<Key, Value> Pair;
	typedef std::list<Pair> List;
	typedef std::unordered_map<Key, std::list<Pair>::iterator> Map;

	explicit LruCache(std::size_t max_size)
	: max_size_(max_size)
	{
	}
	
	void Put(const Key& key, const Value& value)
	{
		Map::iterator it = map_.find(key);
		if (it != map_.end())
		{
			list_.splice(list_.begin(), list_, it->second);
		}
		else
		{
			list_.push_front(Pair(key, value));
			map_[key] = list_.begin();
		}
		
		if (map_.size() > max_size_)
		{
			List::iterator last = list_.end();
			--last;
			map_.erase(last->first);
			list_.pop_back();
		}
	}
	
	bool Get(const Key& key, Value& value)
	{
		Map::iterator it = map_.find(key);
		if (it == map_.end())
		{
			return false;
		}
		else
		{
			list_.splice(list_.begin(), list_, it->second);
			value = it->second->second;
			return true;
		}
	}

	void Clear()
	{
		list_.clear();
		map_.clear();
	}
	
	bool Exists(const Key& key) const
	{
		return map_.find(key) != map_.end();
	}
	
	std::size_t size() const
	{
		return map_.size();
	}
	
private:
	List list_;
	Map map_;
	std::size_t max_size_;
};

template<typename Value>
class LruCacheSet {
public:
	typedef std::list<Value> List;
	typedef std::unordered_set<std::list<Value>::iterator> Set;

	explicit LruCacheSet(std::size_t max_size)
	: max_size_(max_size)
	{
	}
	
	void Put(const Value& value)
	{
		Set::iterator it = set_.find(value);
		if (it != set_.end())
		{
			list_.splice(list_.begin(), list_, *it);
		}
		else
		{
			list_.push_front(value);
			set_.insert(list_.begin());
		}
		
		if (set_.size() > max_size_)
		{
			List::iterator last = list_.end();
			--last;
			set_.erase(*last);
			list_.pop_back();
		}
	}

	void Clear()
	{
		list_.clear();
		set_.clear();
	}
	
	std::size_t size() const
	{
		return set_.size();
	}
	
private:
	List list_;
	Set set_;
	std::size_t max_size_;
};

} // namespace scythe

#endif