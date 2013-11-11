#if !defined(SPLR_MAP_H_)
#define SPLR_MAP_H_

#include <vcl_map.h>
#include <vcl_cassert.h>

/*: \brief wrapper around some kind of map (currently a vcl_map)
    \author P. N. Klein
*/


template <class K, class V>
class splr_map {

 public: //should be private--this is for io, which should be friend
    typedef typename vcl_map<K, V> underlying_map;
    underlying_map map_;
  public:
    splr_map() {}    
    void insert(K key, V value) {
     map_[key] = value;
    }
    int has(K key) const{
      typename underlying_map::const_iterator it = map_.find(key);
      return map_.end() != it;
    }
    const V & lookup(K key) const{
      typename underlying_map::const_iterator it = map_.find(key);
      assert(map_.end() != it);
      return it->second;
     }
    void forget(K key){
      map_.erase(key);
      /*      typename underlying_map::const_iterator it = map_.find(key);
      if (map_.end() != it){
        map_.erase(it);
        }*/
    }
};

#endif
