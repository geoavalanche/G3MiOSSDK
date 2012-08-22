//
//  TileKey.h
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 09/08/12.
//
//

#ifndef __G3MiOSSDK__TileKey__
#define __G3MiOSSDK__TileKey__

#include <string>


class TileKey {
private:
  const int    _level;
  const int    _row;
  const int    _column;
  
  
public:
  TileKey(int level,
          int row,
          int column):
  _level(level),
  _row(row),
  _column(column)
  {
    
  }
  
  TileKey(const TileKey& that):
  _level(that._level),
  _row(that._row),
  _column(that._column)
  {
    
  }
  
  
  int getLevel() const {
    return _level;
  }
  
  int getRow() const {
    return _row;
  }
  
  int getColumn() const {
    return _column;
  }
  
  bool operator<(const TileKey& that) const {
    if (_level < that._level) {
      return true;
    }
    else if (_level > that._level) {
      return false;
    }

    if (_column < that._column) {
      return true;
    }
    else if (_column > that._column) {
      return false;
    }

    return (_row < that._row);
  }
  
  const std::string description() const;
  
  const std::string tinyDescription() const;

};

#endif
